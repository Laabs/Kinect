#include "stdafx.h"
#include "included.h"
#include <wchar.h>

static const float c_JointThickness = 3.0f;
static const float c_TrackedBoneThickness = 6.0f;
static const float c_InferredBoneThickness = 1.0f;
static const float c_HandSize = 40.0f;

// ImageRender类构造函数
ImageRenderer::ImageRenderer() {
	// 创建资源
	m_hrInit = CreateDeviceIndependentResources();
	m_timer.Start();
}


// 创建设备无关资源
HRESULT ImageRenderer::CreateDeviceIndependentResources() {
	HRESULT hr = S_OK;

	// 创建 Direct2D 工厂.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	if (SUCCEEDED(hr))
	{
		// 创建 WIC 工厂.
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			reinterpret_cast<void **>(&m_pWICFactory)
			);
	}

	if (SUCCEEDED(hr))
	{
		// 创建 DirectWrite 工厂.
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
			);
	}

	if (SUCCEEDED(hr))
	{
		// 创建正文文本格式.
		hr = m_pDWriteFactory->CreateTextFormat(
			TEXT("微软黑雅"),
			nullptr,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			30.f,
			TEXT(""), //locale
			&m_pTextFormatMain
			);
	}

	return hr;
}

// 从文件读取位图
HRESULT ImageRenderer::LoadBitmapFromFile(
	ID2D1RenderTarget *pRenderTarget,
	IWICImagingFactory *pIWICFactory,
	PCWSTR uri,
	UINT destinationWidth,
	UINT destinationHeight,
	ID2D1Bitmap **ppBitmap
	)
{
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;
	IWICBitmapScaler *pScaler = NULL;

	HRESULT hr = pIWICFactory->CreateDecoderFromFilename(
		uri,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
		);

	if (SUCCEEDED(hr))
	{
		hr = pDecoder->GetFrame(0, &pSource);
	}
	if (SUCCEEDED(hr))
	{
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}


	if (SUCCEEDED(hr))
	{
		if (destinationWidth != 0 || destinationHeight != 0)
		{
			UINT originalWidth, originalHeight;
			hr = pSource->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr))
			{
				if (destinationWidth == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
					destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
				}
				else if (destinationHeight == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
					destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				hr = pIWICFactory->CreateBitmapScaler(&pScaler);
				if (SUCCEEDED(hr))
				{
					hr = pScaler->Initialize(
						pSource,
						destinationWidth,
						destinationHeight,
						WICBitmapInterpolationModeCubic
						);
				}
				if (SUCCEEDED(hr))
				{
					hr = pConverter->Initialize(
						pScaler,
						GUID_WICPixelFormat32bppPBGRA,
						WICBitmapDitherTypeNone,
						NULL,
						0.f,
						WICBitmapPaletteTypeMedianCut
						);
				}
			}
		}
		else
		{
			hr = pConverter->Initialize(
				pSource,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				NULL,
				0.f,
				WICBitmapPaletteTypeMedianCut
				);
		}
	}
	if (SUCCEEDED(hr))
	{
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
			);
	}

	SafeRelease(pDecoder);
	SafeRelease(pSource);
	SafeRelease(pStream);
	SafeRelease(pConverter);
	SafeRelease(pScaler);

	return hr;
}

// 创建设备相关资源
HRESULT ImageRenderer::CreateDeviceResources()
{
	HRESULT hr = S_OK;
	if (!m_pRenderTarget)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
			);

		// 创建 Direct2D RenderTarget.
		hr = m_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRenderTarget
			);

		// 创建白色笔刷,这里的作用是在左上角显示FPS以及放大率
		if (SUCCEEDED(hr)) {
			ID2D1SolidColorBrush* pSolidColorBrush = nullptr;
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pSolidColorBrush);
			m_pWhiteBrush = pSolidColorBrush;
		}
		//创建深绿色笔刷，这里的作用是绘制被追踪关节的颜色
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.27f, 0.75f, 0.27f), &m_pBrushJointTracked);
		}
		//创建黄色笔刷，这里的作用是绘制被Kinect推断出的关节的颜色
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow, 1.0f), &m_pBrushJointInferred);
		}
		//创建绿色笔刷，这里的作用是绘制被追踪骨骼的颜色
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green, 1.0f), &m_pBrushBoneTracked);
			Brushcolor[0] = m_pBrushBoneTracked;
		}
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Purple), &m_pBrushBoneTrackedPurple);
			Brushcolor[1] = m_pBrushBoneTrackedPurple;
		}
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_pBrushBoneTrackedRed);
			Brushcolor[2] = m_pBrushBoneTrackedRed;
		}
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBrushBoneTrackedBlack);
			Brushcolor[3] = m_pBrushBoneTrackedBlack;
		}
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &m_pBrushBoneTrackedYellow);
			Brushcolor[4] = m_pBrushBoneTrackedYellow;
		}
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pBrushBoneTrackedWhite);
			Brushcolor[5] = m_pBrushBoneTrackedWhite;
		}

		//创建Gray笔刷，这里的作用是绘制骨骼被推断的颜色
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray, 1.0f), &m_pBrushBoneInferred);
		}
		//创建红色笔刷，这里的作用是绘制手打开时的颜色
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 0.5f), &m_pBrushHandClosed);
		}
		//创建绿色笔刷，这里的作用是绘制握拳时的颜色
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green, 0.5f), &m_pBrushHandOpen);
		}
		//创建蓝色笔刷，这里的作用是绘制手掌介于握拳和张开之间的颜色
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue, 0.5f), &m_pBrushHandLasso);
		}
	}

	return hr;
}

// ImageRender析构函数
ImageRenderer::~ImageRenderer() {
	DiscardDeviceResources();
	SafeRelease(m_pD2DFactory);
	SafeRelease(m_pWICFactory);
	SafeRelease(m_pDWriteFactory);

	for (int i = 0; i < 6; i++)
		SafeRelease(Brushcolor[0]);
}

// 丢弃设备相关资源
void ImageRenderer::DiscardDeviceResources() {
	// 清空位图缓存
	for (BitmapCacheMap::iterator itr = m_mapBitmapCache.begin(); itr != m_mapBitmapCache.end(); ++itr) {
		SafeRelease(itr->second);
	}
	m_mapBitmapCache.clear();
	SafeRelease(m_pWhiteBrush);
	SafeRelease(m_pRenderTarget);
}


// 获取图片
// bitmapName	[in] : 文件名
// 返回: NULL表示失败 其余的则为位图的指针
ID2D1Bitmap* ImageRenderer::GetBitmap(std::wstring& bitmapName) {
	ID2D1Bitmap* pBitmap;
	// 缓存中没有的话，从文件中读取
	BitmapCacheMap::iterator itr = m_mapBitmapCache.find(bitmapName);
	if (itr == m_mapBitmapCache.end()) {
		// 读取成功的话
		if (SUCCEEDED(LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, bitmapName.c_str(), 0, 0, &pBitmap)))
			return m_mapBitmapCache[bitmapName] = pBitmap;
		else
			return m_mapBitmapCache[bitmapName] = NULL;
	}
	else
		return itr->second;
}
// 渲染图形图像
HRESULT ImageRenderer::OnRender(int TrackedNum) {
	HRESULT hr = S_OK;
	WCHAR buffer[1024];
	D2D1_RECT_F rect;
	// 尝试创建资源
	hr = CreateDeviceResources();
	if (SUCCEEDED(hr)) {
		// 开始
		m_pRenderTarget->BeginDraw();
		// 重置转换
		m_pRenderTarget->SetTransform(this->matrix);
		// 清屏
		m_pRenderTarget->Clear(D2D1::ColorF(0xFF66CC00));
		// 刻画骨骼
		DrawBody();
		// 复位显示FPS
		auto length = swprintf_s(buffer,
			TEXT("帧率: %2.2f\tTrackedNum:%d\n放大率x: %2.2f\n放大率y: %2.2f"),
			m_fFPS, TrackedNum, this->matrix._11, this->matrix._22);
		auto size = m_pRenderTarget->GetSize();
		rect.left = 0.f; rect.right = size.width;
		rect.top = 0.f; rect.bottom = size.height;
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->DrawText(buffer, length, m_pTextFormatMain, &rect, Brushcolor[TrackedNum]);//可能会有问题
		hr = m_pRenderTarget->EndDraw();// 结束刻画
										// 收到重建消息时，释放资源，等待下次自动创建
		if (hr == D2DERR_RECREATE_TARGET) {
			DiscardDeviceResources();
			hr = S_OK;
		}
	}
	return hr;
}


// 设置骨骼数据
void ImageRenderer::SetBodyInfo(int i, BodyInfo* info) {//第一个参数人体编号，第二个参数代表此人的信息
														//此函数的作用是把资源内存拷贝到目标内存中，拷贝多少个由一个size变量控制
	memcpy(m_bodyInfo + i, info, sizeof(BodyInfo));//memcpy的三个参数：目标图像，原始图像，大小
	m_bodyInfo[i].ok = TRUE;                      //此人被追踪
}


// 刻画骨骼
void ImageRenderer::DrawBody() {
	m_fFPS = 1000.f / m_timer.DeltaF_ms();//计算帧率
	m_timer.MovStartEnd();//起始时间赋值为结束时间，以便进行下一帧的计算
	for (int i = 0; i < BODY_COUNT; ++i) {
		if (!m_bodyInfo[i].ok) continue;
		m_bodyInfo[i].ok = FALSE;
		DrawBody(m_bodyInfo[i].joints, m_bodyInfo[i].jointPoints, i);//刻画人体
		DrawHand(m_bodyInfo[i].leftHandState, m_bodyInfo[i].jointPoints[JointType_HandLeft]);//刻画左手
		DrawHand(m_bodyInfo[i].rightHandState, m_bodyInfo[i].jointPoints[JointType_HandRight]);//刻画右手
	}
}



//刻画人体骨架

void ImageRenderer::DrawBody(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, int i) {//第一个参数人体关节数据，第二个参数骨骼坐标映射到屏幕坐标的坐标值
	D2D1_RECT_F rect1;
	WCHAR buffer1[1024];

	DrawBone(pJoints, pJointPoints, JointType_Head, JointType_Neck, i);
	DrawBone(pJoints, pJointPoints, JointType_Neck, JointType_SpineShoulder, i);
	DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_SpineMid, i);
	DrawBone(pJoints, pJointPoints, JointType_SpineMid, JointType_SpineBase, i);
	DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_ShoulderRight, i);
	DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_ShoulderLeft, i);
	DrawBone(pJoints, pJointPoints, JointType_SpineBase, JointType_HipRight, i);
	DrawBone(pJoints, pJointPoints, JointType_SpineBase, JointType_HipLeft, i);

	DrawBone(pJoints, pJointPoints, JointType_ShoulderRight, JointType_ElbowRight, i);
	DrawBone(pJoints, pJointPoints, JointType_ElbowRight, JointType_WristRight, i);
	DrawBone(pJoints, pJointPoints, JointType_WristRight, JointType_HandRight, i);
	DrawBone(pJoints, pJointPoints, JointType_HandRight, JointType_HandTipRight, i);
	DrawBone(pJoints, pJointPoints, JointType_WristRight, JointType_ThumbRight, i);

	DrawBone(pJoints, pJointPoints, JointType_ShoulderLeft, JointType_ElbowLeft, i);
	DrawBone(pJoints, pJointPoints, JointType_ElbowLeft, JointType_WristLeft, i);
	DrawBone(pJoints, pJointPoints, JointType_WristLeft, JointType_HandLeft, i);
	DrawBone(pJoints, pJointPoints, JointType_HandLeft, JointType_HandTipLeft, i);
	DrawBone(pJoints, pJointPoints, JointType_WristLeft, JointType_ThumbLeft, i);

	DrawBone(pJoints, pJointPoints, JointType_HipRight, JointType_KneeRight, i);
	DrawBone(pJoints, pJointPoints, JointType_KneeRight, JointType_AnkleRight, i);
	DrawBone(pJoints, pJointPoints, JointType_AnkleRight, JointType_FootRight, i);

	DrawBone(pJoints, pJointPoints, JointType_HipLeft, JointType_KneeLeft, i);
	DrawBone(pJoints, pJointPoints, JointType_KneeLeft, JointType_AnkleLeft, i);
	DrawBone(pJoints, pJointPoints, JointType_AnkleLeft, JointType_FootLeft, i);

	//进行人体序号的刻画
	D2D1_ELLIPSE ellipseHead = D2D1::Ellipse(pJointPoints[JointType_Head], 30, 30);//对圆进行定义
	m_pRenderTarget->DrawEllipse(ellipseHead, Brushcolor[i], 4);//对头部画圆
	auto legth1 = swprintf_s(buffer1, TEXT(" %d"), i);
	rect1.left = -25 + pJointPoints[JointType_Head].x;//矩形左上角坐标             
	rect1.top = -35 + pJointPoints[JointType_Head].y;
	rect1.right = 0 + pJointPoints[JointType_Head].x;   //矩形右下角坐标
	rect1.bottom = 0 + pJointPoints[JointType_Head].y;
	m_pRenderTarget->DrawText(buffer1, legth1, m_pTextFormatMain, &rect1, Brushcolor[i]);//绘制序号

	for (int i = 0; i < JointType_Count; ++i)
	{
		D2D1_ELLIPSE ellipse = D2D1::Ellipse(pJointPoints[i], c_JointThickness, c_JointThickness);

		if (pJoints[i].TrackingState == TrackingState_Inferred)
		{
			m_pRenderTarget->FillEllipse(ellipse, m_pBrushJointInferred);
		}
		else if (pJoints[i].TrackingState == TrackingState_Tracked)
		{
			m_pRenderTarget->FillEllipse(ellipse, m_pBrushJointTracked);
		}
	}
}

//
inline void ImageRenderer::DrawBone(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, JointType joint0, JointType joint1, int ii)
{
	TrackingState joint0State = pJoints[joint0].TrackingState;
	TrackingState joint1State = pJoints[joint1].TrackingState;

	if ((joint0State == TrackingState_NotTracked) || (joint1State == TrackingState_NotTracked))
	{
		return;
	}

	if ((joint0State == TrackingState_Inferred) && (joint1State == TrackingState_Inferred))
	{
		return;
	}

	if ((joint0State == TrackingState_Tracked) && (joint1State == TrackingState_Tracked))
	{
		m_pRenderTarget->DrawLine(pJointPoints[joint0], pJointPoints[joint1], Brushcolor[ii], c_TrackedBoneThickness);
	}
	else
	{
		m_pRenderTarget->DrawLine(pJointPoints[joint0], pJointPoints[joint1], m_pBrushBoneInferred, c_InferredBoneThickness);
	}
}

//刻画手（左、右手）
void ImageRenderer::DrawHand(HandState handState, const D2D1_POINT_2F& handPosition)//状态，手的位置
{
	D2D1_ELLIPSE ellipse = D2D1::Ellipse(handPosition, c_HandSize, c_HandSize);//后面的参数椭圆的a、b参数

	switch (handState)
	{
	case HandState_Closed:
		m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandClosed);//刻画圆
		break;

	case HandState_Open:
		m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandOpen);
		break;

	case HandState_Lasso:
		m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandLasso);
		break;
	}
}
