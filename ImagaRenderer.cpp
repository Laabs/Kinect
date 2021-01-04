#include "stdafx.h"
#include "included.h"
#include <wchar.h>

static const float c_JointThickness = 3.0f;
static const float c_TrackedBoneThickness = 6.0f;
static const float c_InferredBoneThickness = 1.0f;
static const float c_HandSize = 40.0f;

// ImageRender�๹�캯��
ImageRenderer::ImageRenderer() {
	// ������Դ
	m_hrInit = CreateDeviceIndependentResources();
	m_timer.Start();
}


// �����豸�޹���Դ
HRESULT ImageRenderer::CreateDeviceIndependentResources() {
	HRESULT hr = S_OK;

	// ���� Direct2D ����.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	if (SUCCEEDED(hr))
	{
		// ���� WIC ����.
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
		// ���� DirectWrite ����.
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
			);
	}

	if (SUCCEEDED(hr))
	{
		// ���������ı���ʽ.
		hr = m_pDWriteFactory->CreateTextFormat(
			TEXT("΢�����"),
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

// ���ļ���ȡλͼ
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

// �����豸�����Դ
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

		// ���� Direct2D RenderTarget.
		hr = m_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRenderTarget
			);

		// ������ɫ��ˢ,����������������Ͻ���ʾFPS�Լ��Ŵ���
		if (SUCCEEDED(hr)) {
			ID2D1SolidColorBrush* pSolidColorBrush = nullptr;
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pSolidColorBrush);
			m_pWhiteBrush = pSolidColorBrush;
		}
		//��������ɫ��ˢ������������ǻ��Ʊ�׷�ٹؽڵ���ɫ
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.27f, 0.75f, 0.27f), &m_pBrushJointTracked);
		}
		//������ɫ��ˢ������������ǻ��Ʊ�Kinect�ƶϳ��Ĺؽڵ���ɫ
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow, 1.0f), &m_pBrushJointInferred);
		}
		//������ɫ��ˢ������������ǻ��Ʊ�׷�ٹ�������ɫ
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

		//����Gray��ˢ������������ǻ��ƹ������ƶϵ���ɫ
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray, 1.0f), &m_pBrushBoneInferred);
		}
		//������ɫ��ˢ������������ǻ����ִ�ʱ����ɫ
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 0.5f), &m_pBrushHandClosed);
		}
		//������ɫ��ˢ������������ǻ�����ȭʱ����ɫ
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green, 0.5f), &m_pBrushHandOpen);
		}
		//������ɫ��ˢ������������ǻ������ƽ�����ȭ���ſ�֮�����ɫ
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue, 0.5f), &m_pBrushHandLasso);
		}
	}

	return hr;
}

// ImageRender��������
ImageRenderer::~ImageRenderer() {
	DiscardDeviceResources();
	SafeRelease(m_pD2DFactory);
	SafeRelease(m_pWICFactory);
	SafeRelease(m_pDWriteFactory);

	for (int i = 0; i < 6; i++)
		SafeRelease(Brushcolor[0]);
}

// �����豸�����Դ
void ImageRenderer::DiscardDeviceResources() {
	// ���λͼ����
	for (BitmapCacheMap::iterator itr = m_mapBitmapCache.begin(); itr != m_mapBitmapCache.end(); ++itr) {
		SafeRelease(itr->second);
	}
	m_mapBitmapCache.clear();
	SafeRelease(m_pWhiteBrush);
	SafeRelease(m_pRenderTarget);
}


// ��ȡͼƬ
// bitmapName	[in] : �ļ���
// ����: NULL��ʾʧ�� �������Ϊλͼ��ָ��
ID2D1Bitmap* ImageRenderer::GetBitmap(std::wstring& bitmapName) {
	ID2D1Bitmap* pBitmap;
	// ������û�еĻ������ļ��ж�ȡ
	BitmapCacheMap::iterator itr = m_mapBitmapCache.find(bitmapName);
	if (itr == m_mapBitmapCache.end()) {
		// ��ȡ�ɹ��Ļ�
		if (SUCCEEDED(LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, bitmapName.c_str(), 0, 0, &pBitmap)))
			return m_mapBitmapCache[bitmapName] = pBitmap;
		else
			return m_mapBitmapCache[bitmapName] = NULL;
	}
	else
		return itr->second;
}
// ��Ⱦͼ��ͼ��
HRESULT ImageRenderer::OnRender(int TrackedNum) {
	HRESULT hr = S_OK;
	WCHAR buffer[1024];
	D2D1_RECT_F rect;
	// ���Դ�����Դ
	hr = CreateDeviceResources();
	if (SUCCEEDED(hr)) {
		// ��ʼ
		m_pRenderTarget->BeginDraw();
		// ����ת��
		m_pRenderTarget->SetTransform(this->matrix);
		// ����
		m_pRenderTarget->Clear(D2D1::ColorF(0xFF66CC00));
		// �̻�����
		DrawBody();
		// ��λ��ʾFPS
		auto length = swprintf_s(buffer,
			TEXT("֡��: %2.2f\tTrackedNum:%d\n�Ŵ���x: %2.2f\n�Ŵ���y: %2.2f"),
			m_fFPS, TrackedNum, this->matrix._11, this->matrix._22);
		auto size = m_pRenderTarget->GetSize();
		rect.left = 0.f; rect.right = size.width;
		rect.top = 0.f; rect.bottom = size.height;
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->DrawText(buffer, length, m_pTextFormatMain, &rect, Brushcolor[TrackedNum]);//���ܻ�������
		hr = m_pRenderTarget->EndDraw();// �����̻�
										// �յ��ؽ���Ϣʱ���ͷ���Դ���ȴ��´��Զ�����
		if (hr == D2DERR_RECREATE_TARGET) {
			DiscardDeviceResources();
			hr = S_OK;
		}
	}
	return hr;
}


// ���ù�������
void ImageRenderer::SetBodyInfo(int i, BodyInfo* info) {//��һ�����������ţ��ڶ�������������˵���Ϣ
														//�˺����������ǰ���Դ�ڴ濽����Ŀ���ڴ��У��������ٸ���һ��size��������
	memcpy(m_bodyInfo + i, info, sizeof(BodyInfo));//memcpy������������Ŀ��ͼ��ԭʼͼ�񣬴�С
	m_bodyInfo[i].ok = TRUE;                      //���˱�׷��
}


// �̻�����
void ImageRenderer::DrawBody() {
	m_fFPS = 1000.f / m_timer.DeltaF_ms();//����֡��
	m_timer.MovStartEnd();//��ʼʱ�丳ֵΪ����ʱ�䣬�Ա������һ֡�ļ���
	for (int i = 0; i < BODY_COUNT; ++i) {
		if (!m_bodyInfo[i].ok) continue;
		m_bodyInfo[i].ok = FALSE;
		DrawBody(m_bodyInfo[i].joints, m_bodyInfo[i].jointPoints, i);//�̻�����
		DrawHand(m_bodyInfo[i].leftHandState, m_bodyInfo[i].jointPoints[JointType_HandLeft]);//�̻�����
		DrawHand(m_bodyInfo[i].rightHandState, m_bodyInfo[i].jointPoints[JointType_HandRight]);//�̻�����
	}
}



//�̻�����Ǽ�

void ImageRenderer::DrawBody(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, int i) {//��һ����������ؽ����ݣ��ڶ���������������ӳ�䵽��Ļ���������ֵ
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

	//����������ŵĿ̻�
	D2D1_ELLIPSE ellipseHead = D2D1::Ellipse(pJointPoints[JointType_Head], 30, 30);//��Բ���ж���
	m_pRenderTarget->DrawEllipse(ellipseHead, Brushcolor[i], 4);//��ͷ����Բ
	auto legth1 = swprintf_s(buffer1, TEXT(" %d"), i);
	rect1.left = -25 + pJointPoints[JointType_Head].x;//�������Ͻ�����             
	rect1.top = -35 + pJointPoints[JointType_Head].y;
	rect1.right = 0 + pJointPoints[JointType_Head].x;   //�������½�����
	rect1.bottom = 0 + pJointPoints[JointType_Head].y;
	m_pRenderTarget->DrawText(buffer1, legth1, m_pTextFormatMain, &rect1, Brushcolor[i]);//�������

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

//�̻��֣������֣�
void ImageRenderer::DrawHand(HandState handState, const D2D1_POINT_2F& handPosition)//״̬���ֵ�λ��
{
	D2D1_ELLIPSE ellipse = D2D1::Ellipse(handPosition, c_HandSize, c_HandSize);//����Ĳ�����Բ��a��b����

	switch (handState)
	{
	case HandState_Closed:
		m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandClosed);//�̻�Բ
		break;

	case HandState_Open:
		m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandOpen);
		break;

	case HandState_Lasso:
		m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandLasso);
		break;
	}
}
