#pragma once

#include "stdafx.h"
#include "serialport.h"
#include "included.h"
#include <math.h>
#include <fstream>
#include <iostream>
#include"function.h"

using namespace std;
using namespace Sample;

#define TITLE L"Title"
#define WNDWIDTH 900
#define WNDHEIGHT 500
#define PI 3.1415926
#define pi 3.1415926
#define ABESSEL 4

bool flag1 = true;
//bool值用于判断是否是曲线的前四个值
bool init_bessel = true;
//用于计算程序执行次数
static int num_bessel = 0;
//偶数情况下进行拟合
static int Bessel_cacular = 0;

int PeopleTrackedNum = 0;

//存取四个控制点，原始数据
float anglepoint1[4], anglepoint2[4], anglepoint3[4], anglepoint4[4];
float anglepoint1_[4], anglepoint2_[4], anglepoint3_[4], anglepoint4_[4];

//存取四个控制点，取出来计算的数据
float anglesave1[4], anglesave2[4], anglesave3[4], anglesave4[4];
float anglesave1_[4], anglesave2_[4], anglesave3_[4], anglesave4_[4];

//存取每次算出的值，用来进行迭代
float anglebessel1[4] = { 0 }, anglebessel2[4] = { 0 }, anglebessel3[4] = { 0 }, anglebessel4[4] = { 0 };
float anglebessel1_[4] = { 0 }, anglebessel2_[4] = { 0 }, anglebessel3_[4] = { 0 }, anglebessel4_[4] = { 0 };


//用于存储bessel的四个方程
float T_Bessel[4];



#define lengthof(a) sizeof(a)/sizeof(*a)
extern DirectX::XMVECTOR m_pFilteredJoints;
inline vector3D cross_3D(struct vector3D A, struct vector3D B)
{
	vector3D C;
	C.X = A.Y*B.Z - B.Y*A.Z;
	C.Y = B.X*A.Z - A.X*B.Z;
	C.Z = A.X*B.Y - A.Y*B.X;
	return C;
}
/*
* 函数名称：norm_3D
* 函数功能：计算三维向量 A 的单位向量
* 输入参数：A   ： 自定义 vector3D 结构体类型
* 输出参数：无
* 返 回 值：norm(A)    ： vector3D类型
* 副 作 用：无
* 创建时间：2017年 4月 27日
*/
inline vector3D norm_3D(struct vector3D A)
{
	vector3D norm_A;
	float length = sqrt(powf(A.X, 2.0) + powf(A.Y, 2.0) + powf(A.Z, 2.0));
	norm_A.X = A.X / length;
	norm_A.Y = A.Y / length;
	norm_A.Z = A.Z / length;
	return norm_A;
}
//坐标变换函数
inline CameraSpacePoint tran_Kinect2Man(CameraSpacePoint RightPoint, CameraSpacePoint LeftPoint, CameraSpacePoint PrePoint)
{
	CameraSpacePoint TranPoint;
	double temp = sqrt(powf((RightPoint.X - LeftPoint.X), 2.0) + powf((RightPoint.Y - LeftPoint.Y), 2.0) + powf((RightPoint.Z - LeftPoint.Z), 2.0));
	TranPoint.X = ((RightPoint.X + LeftPoint.X - 2 * PrePoint.X)*(RightPoint.X - LeftPoint.X) + (RightPoint.Y + LeftPoint.Y - 2 * PrePoint.Y)*(RightPoint.Y - LeftPoint.Y) + (RightPoint.Z + LeftPoint.Z - 2 * PrePoint.Z)*(RightPoint.Z - LeftPoint.Z)) / (2 * temp);
	TranPoint.Y = PrePoint.Y - (RightPoint.Y + LeftPoint.Y) / 2;
	TranPoint.Z = ((RightPoint.Z + LeftPoint.Z - 2 * PrePoint.Z)*(RightPoint.X - LeftPoint.X) - (RightPoint.X + LeftPoint.X - 2 * PrePoint.X)*(RightPoint.Z - LeftPoint.Z)) / (2 * temp);
	return TranPoint;
}
// ThisApp构造函数
ThisApp::ThisApp() {

}

// ThisApp析构函数
ThisApp::~ThisApp() {
	SafeRelease(m_pCoordinateMapper);
	/*Many of the code examples in this documentation use the following function to release COM interface pointers.
	This function releases the pointer ppT and sets it equal to NULL.
	template <class T> void SafeRelease(T **ppT)
	{
	if (*ppT)
	{
	(*ppT)->Release();
	*ppT = NULL;
	}
	}
	*/
	// 销毁事件
	if (m_hBodyFrameArrived && m_pBodyFrameReader) {
		m_pBodyFrameReader->UnsubscribeFrameArrived(m_hBodyFrameArrived);
		m_hBodyFrameArrived = 0;
	}
	// 释放BodyFrameReader
	SafeRelease(m_pBodyFrameReader);
	// 关闭Kinect
	if (m_pKinect) {
		m_pKinect->Close();
	}
	SafeRelease(m_pKinect);
}

// 初始化
HRESULT ThisApp::Initialize(HINSTANCE hInstance, int nCmdShow) { //Initialize function: Initializes a thread to use Windows Runtime APIs.
	HRESULT hr = E_FAIL;
	if (SUCCEEDED(static_cast<HRESULT>(m_ImagaRenderer)))
	{
		//register window class
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = ThisApp::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = hInstance;
		wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		wcex.hbrBackground = nullptr;
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = L"Direct2DTemplate";
		wcex.hIcon = nullptr;
		// 注册窗口
		RegisterClassEx(&wcex);
		// 创建窗口
		RECT window_rect = { 0, 0, WNDWIDTH, WNDHEIGHT };
		DWORD window_style = WS_CAPTION | WS_VISIBLE | WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX;
		AdjustWindowRect(&window_rect, window_style, FALSE);
		AdjustWindowRect(&window_rect, window_style, FALSE);
		window_rect.right -= window_rect.left;
		window_rect.bottom -= window_rect.top;
		window_rect.left = (GetSystemMetrics(SM_CXFULLSCREEN) - window_rect.right) / 2;
		window_rect.top = (GetSystemMetrics(SM_CYFULLSCREEN) - window_rect.bottom) / 2;

		m_hwnd = CreateWindowExW(0, wcex.lpszClassName, TITLE, window_style,
			window_rect.left, window_rect.top, window_rect.right, window_rect.bottom, 0, 0, hInstance, this);
		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			// 设置窗口句柄
			m_ImagaRenderer.SetHwnd(m_hwnd);
			// 显示窗口
			ShowWindow(m_hwnd, nCmdShow);
			UpdateWindow(m_hwnd);
		}
	}

	return hr;
}



// 消息循环
void ThisApp::RunMessageLoop()
{
	MSG msg;//MSG structure: Contains message information from a thread's message queue.
	HANDLE events[] = { reinterpret_cast<HANDLE>(m_hBodyFrameArrived) };
	while (true) {
		// 消息处理
		if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		// 设置事件
		// 事件0: 彩色临帧事件
		events[0] = reinterpret_cast<HANDLE>(m_hBodyFrameArrived);
		// 检查事件
		switch (MsgWaitForMultipleObjects(lengthof(events), events, FALSE, INFINITE, QS_ALLINPUT))
		{
			// events[0]
		case WAIT_OBJECT_0 + 0:
			this->check_body_frame();
			break;
		default:
			break;
		}
		// 退出
		if (msg.message == WM_QUIT) {
			break;
		}
	}
}


// 窗口过程函数
LRESULT CALLBACK ThisApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		ThisApp *pOurApp = (ThisApp *)pcs->lpCreateParams;

		::SetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA,
			PtrToUlong(pOurApp)
			);

		// 并初始化Kinect
		if (FAILED(pOurApp->init_kinect())) {
			::MessageBoxW(hwnd, L"初始化Kinect v2失败", L"真的很遗憾", MB_ICONERROR);
		}
		result = 1;
	}
	else
	{
		ThisApp *pOurApp = reinterpret_cast<ThisApp *>(static_cast<LONG_PTR>(
			::GetWindowLongPtrW(
				hwnd,
				GWLP_USERDATA
				)));

		bool wasHandled = false;
		if (pOurApp)
		{
			switch (message)
			{
			case WM_DISPLAYCHANGE:
				InvalidateRect(hwnd, NULL, FALSE);
				result = 0;
				wasHandled = true;
				break;
			case WM_MOUSEWHEEL:
				pOurApp->m_ImagaRenderer.matrix._11 += 0.05f * static_cast<float>(static_cast<short>(HIWORD(wParam)))
					/ static_cast<float>(WHEEL_DELTA);
				pOurApp->m_ImagaRenderer.matrix._22 = pOurApp->m_ImagaRenderer.matrix._11;
				pOurApp->m_ImagaRenderer.OnRender(PeopleTrackedNum);
				break;
			case WM_PAINT:
				pOurApp->m_ImagaRenderer.OnRender(PeopleTrackedNum);//这句话在这里没什么用，因为这个过程从Kinect收不到数据或者说数据不完整
				break;
			case WM_KEYDOWN://按键响应函数
				//the lefe ley： VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
				//the little(right) key：VK_0 - VK_9 are the same as ASCII '0' - '9' (0x60 - 0x69)
				//the following code function：test any tey's ASCII:
			    //default:
				//_cwprintf(L"ACSII：%#X\n", wParam); //#X funciton is printf the standard ASCII
				//break;
				switch (wParam)
				{
//right key
				case 0x30://0
					PeopleTrackedNum = 0;
					_cwprintf(L"当前追踪目标：%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x31://1
					PeopleTrackedNum = 1;
					_cwprintf(L"当前追踪目标：%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x32://2
					PeopleTrackedNum = 2;
					_cwprintf(L"当前追踪目标：%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x33://3
					PeopleTrackedNum = 3;
					_cwprintf(L"当前追踪目标：%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x34://4
					PeopleTrackedNum = 4;
					_cwprintf(L"当前追踪目标：%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x35://5
					PeopleTrackedNum = 5;
					_cwprintf(L"当前追踪目标：%d", PeopleTrackedNum);
					Sleep(10);
					break;
//little key
				case 0x60://0
					PeopleTrackedNum = 0;
					_cwprintf(L"当前追踪目标：%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x61://1
					PeopleTrackedNum = 1;
					_cwprintf(L"当前追踪目标：%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x62://2
					PeopleTrackedNum = 2;
					_cwprintf(L"当前追踪目标：%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x63://3
					PeopleTrackedNum = 3;
					_cwprintf(L"当前追踪目标：%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x64://4
					PeopleTrackedNum = 4;
					_cwprintf(L"当前追踪目标：%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x65://5
					PeopleTrackedNum = 5;
					_cwprintf(L"当前追踪目标：%d", PeopleTrackedNum);
					Sleep(10);
					break;
				default:
					_cwprintf(L"按键无效,有效按键：0 - 5 \n");
					break;
				}
				pOurApp->m_ImagaRenderer.OnRender(PeopleTrackedNum);
				break;
				
			case WM_CLOSE:
				// 将收尾操作(如结束全部子线程)放在这里
				DestroyWindow(hwnd);
				result = 1;
				wasHandled = true;
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
				result = 1;
				wasHandled = true;
				break;
			}
		}

		if (!wasHandled)
		{
			result = DefWindowProc(hwnd, message, wParam, lParam);
		}
	}

	return result;
}


// 初始化Kinect
HRESULT ThisApp::init_kinect() {
	IBodyFrameSource* pBodyFrameSource = nullptr;
	// 查找当前默认Kinect
	HRESULT hr = ::GetDefaultKinectSensor(&m_pKinect);
	// 绅士地打开Kinect
	if (SUCCEEDED(hr)) {
		hr = m_pKinect->Open();
	}
	// 获取彩色帧源(BodyFrameSource)
	if (SUCCEEDED(hr)) {
		hr = m_pKinect->get_BodyFrameSource(&pBodyFrameSource);
	}
	// 再获取彩色帧读取器
	if (SUCCEEDED(hr)) {
#ifdef _DEBUG
		// 检查是否已存在
		if (m_pBodyFrameReader)
			::MessageBoxW(m_hwnd, L"成员变量:m_pBodyFrameReader 值已存在", L"<ThisApp::init_kinect>", MB_ICONERROR);
#endif
		hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
	}
	// 注册临帧事件
	if (SUCCEEDED(hr)) {
#ifdef _DEBUG
		// 检查是否已存在
		if (m_hBodyFrameArrived)
			::MessageBoxW(m_hwnd, L"成员变量:m_hBodyFrameArrived 值已存在", L"<ThisApp::init_kinect>", MB_ICONERROR);
#endif
		m_pBodyFrameReader->SubscribeFrameArrived(&m_hBodyFrameArrived);
	}
	// 获取坐标映射器
	if (SUCCEEDED(hr)) {
		hr = m_pKinect->get_CoordinateMapper(&m_pCoordinateMapper);
	}
	SafeRelease(pBodyFrameSource);
	return hr;
}


// 检查骨骼帧
void ThisApp::check_body_frame() {
	if (!m_pBodyFrameReader) return;
#ifdef _DEBUG
	static int frame_num = 0;
	++frame_num;
	_cwprintf(L"<ThisApp::check_color_frame>Frame@%8d ", frame_num);
#endif 
	// 骨骼临帧事件参数
	IBodyFrameArrivedEventArgs* pArgs = nullptr;  //IBodyFrameArrivedEventArgs: Arguments for the body FrameReady events. 
												  // 骨骼帧引用
	IBodyFrameReference* pBFrameRef = nullptr;
	// 骨骼帧
	IBodyFrame* pBodyFrame = nullptr;
	// 骨骼
	IBody*  ppBody[BODY_COUNT] = { 0 };
	// 获取参数
	HRESULT hr = m_pBodyFrameReader->GetFrameArrivedEventData(m_hBodyFrameArrived, &pArgs); //GetFrameArrivedEventData: Gets the event data when a new frame arrives.
																							//Return value
																							//Type : HRESULT
																							//Returns S_OK if successful; otherwise, returns a failure code.
																							// 获取引用
	if (SUCCEEDED(hr)) {
		hr = pArgs->get_FrameReference(&pBFrameRef);
	}
	// 获取骨骼帧
	if (SUCCEEDED(hr)) {
		hr = pBFrameRef->AcquireFrame(&pBodyFrame);
	}
	// 获取骨骼数据
	if (SUCCEEDED(hr)) {
		hr = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, ppBody);
	}


	{//读取时间
		SYSTEMTIME systime1;
		get_localtime(&systime1);
	//	_cwprintf(L"time1 is %d: %d: %d   %d\n", systime1.wHour, systime1.wMinute, systime1.wSecond, systime1.wMilliseconds);
	}

	// 处理数据
	if (SUCCEEDED(hr)) {
		double Distance = 4.5;		//离kinect的距离,最大值
		int index = 10;				//index初值大于5即可，因为人的索引编号为【0—5】
		CameraSpacePoint _SpineBase_;//单位m
									 // 声明数据
		BodyInfo infoDraw;
		for (int i = 0; i < BODY_COUNT; ++i)
		{
			// 保证数据，保证被追踪
			BOOLEAN bTracked = false;
			hr = ppBody[i]->get_IsTracked(&bTracked);

			if (SUCCEEDED(hr) && bTracked && ppBody[i]) {
				IBody* pNowBodyDraw = ppBody[i];
				// 获取左右手状态
				pNowBodyDraw->get_HandLeftState(&infoDraw.leftHandState);
				pNowBodyDraw->get_HandRightState(&infoDraw.rightHandState);
				hr = pNowBodyDraw->GetJoints(JointType_Count, infoDraw.joints);

				Joint _joints[JointType_Count];
				hr = ppBody[i]->GetJoints(JointType_Count, _joints);//用于获取人的所有关节位置信息
				if (SUCCEEDED(hr)&&i != PeopleTrackedNum)
				{
					for (int j = 0; j < JointType_Count; ++j)
						infoDraw.jointPoints[j] = BodyToScreen(infoDraw.joints[j].Position, WNDWIDTH, WNDHEIGHT);
					m_ImagaRenderer.SetBodyInfo(i, &infoDraw);
				}

				if (SUCCEEDED(hr))
				{
					//以SpineBase的Z值为基准，找出离kinect最近的人体
					CameraSpacePoint SpineBase = _joints[0].Position;
					if (SpineBase.Z < Distance)
					{
						Distance = SpineBase.Z;
						index = i;
						_SpineBase_.X = SpineBase.X;
						//_cwprintf(TEXT("\nX:%f\n"), i);//测试左右距离，单位m左右距离不成正比
					}
				}
			}
		}

		if (index < 6 && abs(_SpineBase_.X < 0.9) && flag1)//记录追踪人编号
		{
			PeopleTrackedNum = index;
			flag1 = false;

		}

		/*确保有人在范围内，并被识别到*/
		if (flag1 == false)		//  设置(_SpineBase_.X)<1.1
		{
			IBody* pNowBody = ppBody[PeopleTrackedNum];
			BodyInfo info;
			// 获取左右手状态
			pNowBody->get_HandLeftState(&info.leftHandState);
			pNowBody->get_HandRightState(&info.rightHandState);
			// 获取骨骼空间坐标信息
			hr = pNowBody->GetJoints(JointType_Count, info.joints);
//限幅开始
			//static BodyInfo last_info = info;
			//for (int i = 0; i < 25; i++)
			//{
			//	if (info.joints[i].Position.X - last_info.joints[i].Position.X > 0.04)
			//		info.joints[i].Position.X = last_info.joints[i].Position.X + 0.04;
			//	if (info.joints[i].Position.X - last_info.joints[i].Position.X < -0.04)
			//		info.joints[i].Position.X = last_info.joints[i].Position.X - 0.04;
			//	if (info.joints[i].Position.Y - last_info.joints[i].Position.Y > 0.04)
			//		info.joints[i].Position.Y = last_info.joints[i].Position.Y + 0.04;
			//	if (info.joints[i].Position.Y - last_info.joints[i].Position.Y <-0.04)
			//		info.joints[i].Position.Y = last_info.joints[i].Position.Y - 0.04;
			//	if (info.joints[i].Position.Z - last_info.joints[i].Position.Z > 0.05)
			//		info.joints[i].Position.Z = last_info.joints[i].Position.Z + 0.05;
			//	if (info.joints[i].Position.Z - last_info.joints[i].Position.Z < -0.05)
			//		info.joints[i].Position.Z = last_info.joints[i].Position.Z - 0.05;
			//	//存上一帧数据
			//	last_info.joints[i].Position.X = info.joints[i].Position.X;
			//	last_info.joints[i].Position.Y = info.joints[i].Position.Y;
			//	last_info.joints[i].Position.Z = info.joints[i].Position.Z;
			//}
//结束限幅
			if (SUCCEEDED(hr)) {
				//【霍尔特指数滤  波】对追踪到的人体，0—11骨骼点进行滤波，并读取滤波后的骨骼数据
				m_FilterDoubleExponential.Update(info.joints);
				DirectX::XMFLOAT3 p_FilteredJoints[JointType_Count];
				for (int i = 0; i < JointType_Count; ++i) {
					XMStoreFloat3(&p_FilteredJoints[i], m_FilterDoubleExponential.m_pFilteredJoints[i]);
					info.joints[i].Position.X = p_FilteredJoints[i].x;
					info.joints[i].Position.Y = p_FilteredJoints[i].y;
					info.joints[i].Position.Z = p_FilteredJoints[i].z;
				}
				static int frame_num = 0;
				++frame_num;
				/*
				*目标行数范围：380-389
				*TraverseResult表示追踪结果(如果所有Z坐标相加为0表示没有追踪到)，可能有问题，可能只需要3，2，20，1等
				*/
				int TraverseResult = 0;
				for (int Traverse = 0; Traverse < 24; Traverse++)
				{
					TraverseResult += info.joints[Traverse].Position.Z;
				}
				if (TraverseResult == 0)
				{
					flag1 = true;
					frame_num = 0;
				}
			//	_cwprintf(L"<ThisApp::check_body_frame>Frame@%8d \n", frame_num);
				for (int j = 0; j < JointType_Count; ++j)
				{
					// 坐标转换
					info.jointPoints[j] = BodyToScreen(info.joints[j].Position, WNDWIDTH, WNDHEIGHT);
				}
				m_ImagaRenderer.SetBodyInfo(PeopleTrackedNum, &info);
//进行坐标变换
					CameraSpacePoint LeftPoint = info.joints[12].Position;
					CameraSpacePoint RightPoint = info.joints[16].Position;
					info.joints[4].Position = tran_Kinect2Man(RightPoint, LeftPoint, info.joints[4].Position);
					info.joints[5].Position = tran_Kinect2Man(RightPoint, LeftPoint, info.joints[5].Position);
					info.joints[6].Position = tran_Kinect2Man(RightPoint, LeftPoint, info.joints[6].Position);

					info.joints[8].Position = tran_Kinect2Man(RightPoint, LeftPoint, info.joints[8].Position);
					info.joints[9].Position = tran_Kinect2Man(RightPoint, LeftPoint, info.joints[9].Position);
					info.joints[10].Position = tran_Kinect2Man(RightPoint, LeftPoint, info.joints[10].Position);
			
			
				
				//左手肩部角度的计算
				/*说明：左手肩部角度的计算
				* 思路： 1>肩肘在肩部关节坐标系中的YOZ面上的投影，与肩部关节坐标系中的Y轴负方向的夹角为θ1；
				*		 2>肩肘关节与与肩部关节坐标系的X轴正方向夹角的余角为θ2；
				*		 3>特殊处理：在肩肘关节与X轴方向重合时，肩肘在肩部关节坐标系中的YOZ面上的投影不存在（一个点），此时取肩肘关节与肩部关节坐标系中的Y轴负方向的夹角为θ1；
				*/
				int  angle1 = 0, angle2 = 0, angle3 = 0, angle4 = 0;	//θ1 ,θ2，θ3，θ4
				/*利用躯干来代替Y0向量的求解
				*DATA:5.29
				*AUTHOR:TS
				*vectorY0.X = info.joints[0].Position.X - info.joints[20].Position.X;
				*vectorY0.Y = info.joints[0].Position.Y - info.joints[20].Position.Y;
				*vectorY0.Z = info.joints[0].Position.Z - info.joints[20].Position.Z;
				*/
				vector3D  vectorS_E, vectorS_E_proj, norm_vectorS_E, norm_vectorS_E_proj, vectorX0, vectorY0 = { 0, 1.0, 0 };
				//定义X0向量，X0取单位化的右肩到左肩在XOZ平面的投影向量，取在XOZ平面的投影向量的原因是肩部关节点会随着手部上下移动而上下移动，会造成θ2偏小
				vectorX0.X = info.joints[4].Position.X - info.joints[8].Position.X;
				vectorX0.Y = 0.0;//此处是为了避免肩部移动的影响
				vectorX0.Z = info.joints[4].Position.Z - info.joints[8].Position.Z;
				vectorX0 = norm_3D(vectorX0);//单位化X0向量
											 //定义肩肘向量SE向量
				vectorS_E.X = info.joints[5].Position.X - info.joints[4].Position.X;
				vectorS_E.Y = info.joints[5].Position.Y - info.joints[4].Position.Y;
				vectorS_E.Z = info.joints[5].Position.Z - info.joints[4].Position.Z;
				//定义肩肘向量在YOZ平面的投影向量SE_proj向量
				vectorS_E_proj.X = 0.0;
				vectorS_E_proj.Y = info.joints[5].Position.Y - info.joints[4].Position.Y;
				vectorS_E_proj.Z = info.joints[5].Position.Z - info.joints[4].Position.Z;
				//在肩肘关节与X轴方向重合时的特殊处理，使用SE向量代替SE_proj向量，保证特殊位置分母不为0
				if (abs(vectorS_E_proj.Z)<0.01&&abs(vectorS_E_proj.Y) < 0.01)
				{
					vectorS_E_proj.X = vectorS_E.X;
				}
				//单位化SE向量和SE_proj向量
				norm_vectorS_E = norm_3D(vectorS_E);
				norm_vectorS_E_proj = norm_3D(vectorS_E_proj);
				//求解S_E_proj向量和Y0向量的点积
				float dot_S_E_proj_Y0 = cal_dot_product3D(norm_vectorS_E_proj, vectorY0);
				//求解S_E_向量和X0向量的点积
				float dot_S_E_X0 = cal_dot_product3D(norm_vectorS_E, vectorX0);
				//求解θ1
				angle1 = (int)(acosf(-dot_S_E_proj_Y0) * 180 / PI);
				//求解θ'2，此处减15度是因为(acosf(dot_S_E_X0_) * 180 / PI)的计算偏小15度左右，为了使动作更相似，人为调整结果
				angle2 = 90 - (int)(acosf(dot_S_E_X0) * 180 / PI) - 15;
				//关节1的正反向处理
				if (vectorS_E.Z<0)
				{
					angle1 = -angle1;//当肘部在肩部后面时，θ'1的方向反转
									 //此处是为了当肘部是从上面越过肩部平面时，使θ'1从正方向进入，就是从180到360度的方向进入，而不是一下子跳到-180左右。
					if (angle1 < -110)
						angle1 += 360;
				}


				//左手肘部角度的计算
				/*说明： 右手肘部角度的计算
				* 思路： 1>肩肘关节向量SE向量与肘腕关节向量EW向量的夹角为θ4；
				*		 2>EW向量在SE向量上的投影为E_W_proj向量，EW向量在垂直于SE向量的平面上的投影为ElbowWrist_projXZ向量，ElbowWrist_projXZ向量与经肩关节1旋转后的Z向量的夹角即为θ3；
				*
				*/
				vector3D  vectorE_W, norm_vectorE_W, vectorE_W_proj, ElbowWrist_projXZ, norm_ElbowWrist_projXZ;
				//定义E_W向量
				vectorE_W.X = info.joints[6].Position.X - info.joints[5].Position.X;
				vectorE_W.Y = info.joints[6].Position.Y - info.joints[5].Position.Y;
				vectorE_W.Z = info.joints[6].Position.Z - info.joints[5].Position.Z;
				//单位化E_W向量
				norm_vectorE_W = norm_3D(vectorE_W);
				//计算SE向量和EW向量的点积
				float dot_S_E_E_W = cal_dot_product3D(norm_vectorS_E, norm_vectorE_W);
				//当θ4>90度时，令θ4=90度（受机械结构限制）
				if (dot_S_E_E_W<0) {
					dot_S_E_E_W = 0;
				}
				//计算θ4
				angle4 = (int)(acosf(dot_S_E_E_W) * 180 / PI);

				//【特殊处理】:当手肘肩处于一条直线时，θ3为特殊情况
				if (abs(angle4)<10)
				{
					angle3 = 0;

				}
				else
				{
					vector3D Z_vect, vector_N;
					//Z_vect为肩部旋转后z轴的方向向量； Z_vect = (0, sinθ1, cosθ1)
					Z_vect.X = 0;
					Z_vect.Y = sinf(angle1 * PI / 180);
					Z_vect.Z = cosf(angle1 * PI / 180);
					//N向量是用来判别θ3方向的辅助向量，由EW向量叉乘SE向量而来
					vector_N = cross_3D(vectorE_W, vectorS_E);
					//计算E_W_proj向量，EW向量在SE向量上的投影向量
					float ElbowWrist_proj_f = cal_dot_product3D(vectorE_W, norm_vectorS_E);
					vectorE_W_proj.X = ElbowWrist_proj_f * norm_vectorS_E.X;
					vectorE_W_proj.Y = ElbowWrist_proj_f * norm_vectorS_E.Y;
					vectorE_W_proj.Z = ElbowWrist_proj_f * norm_vectorS_E.Z;
					//计算E_W_projXZ向量，EW向量在垂直于SE向量的平面上的投影向量
					ElbowWrist_projXZ.X = vectorE_W.X - vectorE_W_proj.X;
					ElbowWrist_projXZ.Y = vectorE_W.Y - vectorE_W_proj.Y;
					ElbowWrist_projXZ.Z = vectorE_W.Z - vectorE_W_proj.Z;
					//单位化E_W_projXZ向量
					norm_ElbowWrist_projXZ = norm_3D(ElbowWrist_projXZ);
					//θ3为向量 Z_vect、ElbowWrist_projXZ 的夹角;
					angle3 = (int)(acosf(cal_dot_product3D(norm_ElbowWrist_projXZ, Z_vect)) * 180 / PI);
					//利用N向量与Z向量判断θ3的正负，向下转时为正
					if (cal_dot_product3D(vector_N, Z_vect) > 0)
					{
						/*if (Z_vect.Z > 0)
							angle3 = -angle3;*/
						//else
						//	angle3 = 360 - angle3;//此处是为了θ3从180度方向进入，而不是突然反转到-180度，即此时θ3>180度
						//上面是原来的，现在是为了注释
						angle3 = -angle3;

						/*当关节1的角度为负号的时关节3正反向的处理
						DATA:5.29
						AUTUOR:TS
						if (angle1 < 0 && angle3<0)
						{
							int angle31;
							vector3D vectorMidShoulder, vectorMidShoulderElbow;
							vectorMidShoulder.X = info.joints[4].Position.X - info.joints[20].Position.X;
							vectorMidShoulder.Y = info.joints[4].Position.Y - info.joints[20].Position.Y;
							vectorMidShoulder.Z = info.joints[4].Position.Z - info.joints[20].Position.Z;
							vectorMidShoulderElbow = cross_3D(vectorS_E, vectorMidShoulder);
							angle31 = (int)(acosf(cal_dot_product3D(vectorMidShoulderElbow, vectorE_W)) * 180 / PI) - 90;
							if (abs(angle31) > 10)
								angle3 = angle31;
						}
						*/

					}
					
					
				}

				//【特殊处理】:当手臂伸直垂直向前时会产生骨骼点遮挡，对这个遮挡点进行特殊处理
				if (abs(angle4)<30 && abs(vectorE_W.X)<0.07)
					angle3 = 0;

				////减少关节3的抖动
				//if (abs(angle3) < 10) {
				//	angle3 = 0;
				//}
				//ofstream fileoutb;
				//fileoutb.open("E:\\中期结题\\数据\\反向前.dat", ios::app);
				//fileoutb << angle1 << "\t" << angle3 << endl;
				//fileoutb.close();


				//右手肩部角度的计算
				/*说明：右手肩部角度的计算
				*思路：1>肩肘在肩部关节坐标系中的YOZ面上的投影，与肩部关节坐标系中的Y轴负方向的夹角为θ1；
				*		2>肩肘关节与与肩部关节坐标系的X轴正方向夹角的余角为θ2；
				3>特殊处理：在肩肘关节与X轴方向重合时，肩肘在肩部关节坐标系中的YOZ面上的投影不存在（一个点），此时取肩肘关节与肩部关节坐标系中的Y轴负方向的夹角为θ1；
				*
				*/
				int  angle1_ = 0, angle2_ = 0, angle3_ = 0, angle4_ = 0;	//θ'1 ,θ'2，θ'3，θ'4
				/*利用躯干来代替Y0向量的求解
				*DATA:5.29
				*AUTHOR:TS
				*vectorY0_.X = info.joints[0].Position.X - info.joints[20].Position.X;
				*vectorY0_.Y = info.joints[0].Position.Y - info.joints[20].Position.Y;
				*vectorY0_.Z = info.joints[0].Position.Z - info.joints[20].Position.Z;
				*/
				vector3D  vectorS_E_, vectorS_E_proj_, norm_vectorS_E_, norm_vectorS_E_proj_, vectorX0_, vectorY0_ = { 0, 1.0, 0 };
				//定义X0向量，X0取单位化的右肩到左肩在XOZ平面的投影向量，取在XOZ平面的投影向量的原因是肩部关节点会随着手部上下移动而上下移动，会造成θ2偏小
				vectorX0_.X = info.joints[8].Position.X - info.joints[4].Position.X;
				vectorX0_.Y = 0.0;//此处是为了避免肩部移动的影响
				vectorX0_.Z = info.joints[8].Position.Z - info.joints[4].Position.Z;
				vectorX0_ = norm_3D(vectorX0_);//单位化X0向量


											   //定义肩肘向量SE向量
				vectorS_E_.X = info.joints[9].Position.X - info.joints[8].Position.X;
				vectorS_E_.Y = info.joints[9].Position.Y - info.joints[8].Position.Y;
				vectorS_E_.Z = info.joints[9].Position.Z - info.joints[8].Position.Z;
				//定义肩肘向量在YOZ平面的投影向量SE_proj向量
				vectorS_E_proj_.X = 0.0;
				vectorS_E_proj_.Y = info.joints[9].Position.Y - info.joints[8].Position.Y;
				vectorS_E_proj_.Z = info.joints[9].Position.Z - info.joints[8].Position.Z;
				//在肩肘关节与X轴方向重合时的特殊处理，使用SE向量代替SE_proj向量，保证特殊位置分母不为0
				if (abs(vectorS_E_proj_.Z)<0.01&&abs(vectorS_E_proj_.Y) < 0.01)
				{
					vectorS_E_proj_.X = vectorS_E_.X;
				}
				//单位化SE向量和SE_proj向量
				norm_vectorS_E_ = norm_3D(vectorS_E_);
				norm_vectorS_E_proj_ = norm_3D(vectorS_E_proj_);
				//求解S_E_proj向量和Y0向量的点积
				float dot_S_E_proj_Y0_ = cal_dot_product3D(norm_vectorS_E_proj_, vectorY0_);
				//求解S_E_向量和X0向量的点积
				float dot_S_E_X0_ = cal_dot_product3D(norm_vectorS_E_, vectorX0_);
				//求解θ'1
				angle1_ = (int)(acosf(-dot_S_E_proj_Y0_) * 180 / PI);
				//求解θ'2，此处减15度是因为(acosf(dot_S_E_X0_) * 180 / PI)的计算偏小15度左右，为了使动作更相似，人为调整结果
				angle2_ = 90 - (int)(acosf(dot_S_E_X0_) * 180 / PI) - 15;




				//关节1的正反向处理
				if (vectorS_E_.Z<0)
				{
					angle1_ = -angle1_;//当肘部在肩部后面时，θ'1的方向反转
									   //此处是为了当肘部是从上面越过肩部平面时，使θ'1从正方向进入，就是从180到360度的方向进入，而不是一下子跳到-180左右。
					if (angle1_ < -110)
						angle1_ += 360;
				}


				//右手肘部角度的计算
				/*说明：右手肘部角度的计算
				*思路：1>肩肘关节向量SE向量与肘腕关节向量EW向量的夹角为θ4；
				*	   2>EW向量在SE向量上的投影为E_W_proj向量，EW向量在垂直于SE向量的平面上的投影为ElbowWrist_projXZ向量，ElbowWrist_projXZ向量与经肩关节1旋转后的Z向量的夹角即为θ3；

				*
				*/
				vector3D  vectorE_W_, norm_vectorE_W_, vectorE_W_proj_, ElbowWrist_projXZ_, norm_ElbowWrist_projXZ_;
				//定义E_W向量
				vectorE_W_.X = info.joints[10].Position.X - info.joints[9].Position.X;
				vectorE_W_.Y = info.joints[10].Position.Y - info.joints[9].Position.Y;
				vectorE_W_.Z = info.joints[10].Position.Z - info.joints[9].Position.Z;
				//单位化E_W向量
				norm_vectorE_W_ = norm_3D(vectorE_W_);
				//计算SE向量和EW向量的点积
				float dot_S_E_E_W_ = cal_dot_product3D(norm_vectorS_E_, norm_vectorE_W_);
				//当θ4>90度时，令θ4=90度（受机械结构限制）
				if (dot_S_E_E_W_<0) {
					dot_S_E_E_W_ = 0;
				}
				//计算θ4
				angle4_ = (int)(acosf(dot_S_E_E_W_) * 180 / PI);

				//【特殊处理】:当手肘肩处于一条直线时，θ3为特殊情况
				if (abs(angle4_)<10)
				{
					angle3_ = 0;

				}
				else
				{
					vector3D Z_vect_, vector_N_;
					//Z_vect为肩部旋转后z轴的方向向量； Z_vect = (0, sinθ1, cosθ1)
					Z_vect_.X = 0;
					Z_vect_.Y = sinf(angle1_ * PI / 180);
					Z_vect_.Z = cosf(angle1_ * PI / 180);

					//N向量是用来判别θ3方向的辅助向量，由EW向量叉乘SE向量而来
					vector_N_ = cross_3D(vectorE_W_, vectorS_E_);
					//计算E_W_proj向量，EW向量在SE向量上的投影向量
					float ElbowWrist_proj_f_ = cal_dot_product3D(vectorE_W_, norm_vectorS_E_);
					vectorE_W_proj_.X = ElbowWrist_proj_f_ * norm_vectorS_E_.X;
					vectorE_W_proj_.Y = ElbowWrist_proj_f_ * norm_vectorS_E_.Y;
					vectorE_W_proj_.Z = ElbowWrist_proj_f_ * norm_vectorS_E_.Z;
					//计算E_W_projXZ向量，EW向量在垂直于SE向量的平面上的投影向量
					ElbowWrist_projXZ_.X = vectorE_W_.X - vectorE_W_proj_.X;
					ElbowWrist_projXZ_.Y = vectorE_W_.Y - vectorE_W_proj_.Y;
					ElbowWrist_projXZ_.Z = vectorE_W_.Z - vectorE_W_proj_.Z;
					//单位化E_W_projXZ向量
					norm_ElbowWrist_projXZ_ = norm_3D(ElbowWrist_projXZ_);


					//θ3为向量 Z_vect、ElbowWrist_projXZ 的夹角;
					angle3_ = (int)(acosf(cal_dot_product3D(norm_ElbowWrist_projXZ_, Z_vect_)) * 180 / PI);
					//利用N向量与Z向量判断θ3的正负，向下转时为正
					if (cal_dot_product3D(vector_N_, Z_vect_) > 0)
					{
						if (Z_vect_.Z > 0)
							angle3_ = -angle3_;
						else
							angle3_ = 360 - angle3_;//此处是为了θ3从180度方向进入，而不是突然反转到-180度，即此时θ3>180度

					}
					/*当关节1的角度为负号的时关节3正反向的处理
					DATA:5.29
					AUTUOR:TS
					if (angle1 < 0&&angle3<0)
					{
					int angle31_;
					vector3D vectorMidShoulder_,vectorMidShoulderElbow_;
					vectorMidShoulder_.X = info.joints[4].Position.X - info.joints[20].Position.X;
					vectorMidShoulder_.Y = info.joints[4].Position.Y - info.joints[20].Position.Y;
					vectorMidShoulder_.Z = info.joints[4].Position.Z - info.joints[20].Position.Z;
					vectorMidShoulderElbow_=cross_3D(vectorMidShoulder_,vectorS_E_);
					angle31_ = (int)(acosf(cal_dot_product3D(vectorMidShoulderElbow_, vectorE_W_)) * 180 / PI);
					if (abs(angle31_) > 10)
					angle3_ = angle31_;
					}
					*/
				}

				//【特殊处理】:当手臂伸直垂直向前时会产生骨骼点遮挡，对这个遮挡点进行特殊处理
				if (abs(angle4_)<30 && abs(vectorE_W_.X)<0.07)
					angle3_ = 0;
				
				////减少关节3的抖动
				//if (abs(angle3_) < 10) {
				//	angle3_ = 0;
				//}
				//角度保护
				//if (abs(angle1)>180) {
				//	angle1 = 180*angle1/abs(angle1);
				//}

				if (angle2>180) {
					angle2 = 180;
				}
				if (angle2<0) {
					angle2 = 0;
				}
				//if (abs(angle3)>180) {
				//	angle3 = 180 * angle3 / abs(angle3);
				//}

				if (abs(angle4) >= 82) {
					angle4 = 82 * angle4 / abs(angle4);
				}



				//if (abs(angle1_)>180) {
				//	angle1_ = 180 * angle1_ / abs(angle1_);
				//}

				if (angle2_>180) {
					angle2_ = 180;
				}
				if (angle2_<0) {
					angle2_ = 0;
				}
				//if (abs(angle3_)>180) {
				//	angle3_= 180 * angle3_ / abs(angle3_);
				//}

				if (abs(angle4_) >= 82) 
				{
					angle4_ = 82 * angle4_ / abs(angle4_);
				}


			/*	ofstream file_Original;
				file_Original.open("E:\\Original.txt", ios::app);
				file_Original << angle3_ << endl;
				file_Original.close();*/

				//bessel滤波器

				//存储四个控制点，原始数据 start
				if (init_bessel)//初始化
				{
					anglepoint1[num_bessel] = angle1;//左手数据
					anglepoint2[num_bessel] = angle2;
					anglepoint3[num_bessel] = angle3;
					anglepoint4[num_bessel] = angle4;

					anglepoint1_[num_bessel] = angle1_;//右手数据
					anglepoint2_[num_bessel] = angle2_;
					anglepoint3_[num_bessel] = angle3_;
					anglepoint4_[num_bessel] = angle4_;

					//计数次数加 1
					num_bessel++;

					if (num_bessel == 4)//如果从开始运行到存了四个数据，拟合前段曲线，之后整个 if 函数都不会执行
					{
						//从存取的四个控制点中取前三个控制点，取出来计算的数据 start 
						for (int i = 0; i<3; i++)
						{
							anglesave1[i] = anglepoint1[i];//左手前三个数据
							anglesave2[i] = anglepoint2[i];
							anglesave3[i] = anglepoint3[i];
							anglesave4[i] = anglepoint4[i];

							anglesave1_[i] = anglepoint1_[i];//右手前三个数据
							anglesave2_[i] = anglepoint2_[i];
							anglesave3_[i] = anglepoint3_[i];
							anglesave4_[i] = anglepoint4_[i];
						}

						//右手第四个数据点的计算
						anglesave1[3] = (anglepoint1[2] + anglepoint1[3]) / 2.0;
						anglesave2[3] = (anglepoint2[2] + anglepoint2[3]) / 2.0;
						anglesave3[3] = (anglepoint3[2] + anglepoint3[3]) / 2.0;
						anglesave4[3] = (anglepoint4[2] + anglepoint4[3]) / 2.0;
						//左手第四个数据的计算
						anglesave1_[3] = (anglepoint1_[2] + anglepoint1_[3]) / 2.0;
						anglesave2_[3] = (anglepoint2_[2] + anglepoint2_[3]) / 2.0;
						anglesave3_[3] = (anglepoint3_[2] + anglepoint3_[3]) / 2.0;
						anglesave4_[3] = (anglepoint4_[2] + anglepoint4_[3]) / 2.0;
						//存取四个控制点，取出来计算的数据 end 

						init_bessel = false;
					}
				}
				else //存储两个数据，非初始化情况
				{
					static int j = 0;
					//左手数据存储
					anglepoint1[j] = anglepoint1[j + 2];
					anglepoint1[j + 2] = angle1;

					anglepoint2[j] = anglepoint2[j + 2];
					anglepoint2[j + 2] = angle2;

					anglepoint3[j] = anglepoint3[j + 2];
					anglepoint3[j + 2] = angle3;

					anglepoint4[j] = anglepoint4[j + 2];
					anglepoint4[j + 2] = angle4;

					//右手数据存储
					anglepoint1_[j] = anglepoint1_[j + 2];
					anglepoint1_[j + 2] = angle1_;

					anglepoint2_[j] = anglepoint2_[j + 2];
					anglepoint2_[j + 2] = angle2_;

					anglepoint3_[j] = anglepoint3_[j + 2];
					anglepoint3_[j + 2] = angle3_;

					anglepoint4_[j] = anglepoint4_[j + 2];
					anglepoint4_[j + 2] = angle4_;

					j++; // 计数次数累加1

					if (j == 2)
					{
						//存取四个控制点，取出来计算的数据 start
						for (int i = 1; i<3; i++)
						{
							anglesave1[i] = anglepoint1[i];//左手前三个数据
							anglesave2[i] = anglepoint2[i];
							anglesave3[i] = anglepoint3[i];
							anglesave4[i] = anglepoint4[i];

							anglesave1_[i] = anglepoint1_[i];//右手前三个数据
							anglesave2_[i] = anglepoint2_[i];
							anglesave3_[i] = anglepoint3_[i];
							anglesave4_[i] = anglepoint4_[i];
						}

						anglesave1[0] = (anglepoint1[0] + anglepoint1[1]) / 2.0;//左手第一个数据
						anglesave2[0] = (anglepoint2[0] + anglepoint2[1]) / 2.0;
						anglesave3[0] = (anglepoint3[0] + anglepoint3[1]) / 2.0;
						anglesave4[0] = (anglepoint4[0] + anglepoint4[1]) / 2.0;

						anglesave1_[0] = (anglepoint1_[0] + anglepoint1_[1]) / 2.0;//右手第一个数据
						anglesave2_[0] = (anglepoint2_[0] + anglepoint2_[1]) / 2.0;
						anglesave3_[0] = (anglepoint3_[0] + anglepoint3_[1]) / 2.0;
						anglesave4_[0] = (anglepoint4_[0] + anglepoint4_[1]) / 2.0;


						anglesave1[3] = (anglepoint1[2] + anglepoint1[3]) / 2.0;//左手第四个数据
						anglesave2[3] = (anglepoint2[2] + anglepoint2[3]) / 2.0;
						anglesave3[3] = (anglepoint3[2] + anglepoint3[3]) / 2.0;
						anglesave4[3] = (anglepoint4[2] + anglepoint4[3]) / 2.0;

						anglesave1_[3] = (anglepoint1_[2] + anglepoint1_[3]) / 2.0;//右手第四个数据
						anglesave2_[3] = (anglepoint2_[2] + anglepoint2_[3]) / 2.0;
						anglesave3_[3] = (anglepoint3_[2] + anglepoint3_[3]) / 2.0;
						anglesave4_[3] = (anglepoint4_[2] + anglepoint4_[3]) / 2.0;
						//存取四个控制点，取出来计算的数据 end

						j = 0;//计数次数清零
					}
				}
				//存储四个控制点，原始数据 end


				//进行拟合角度的计算
				if (!init_bessel)
				{
					
					float t = 0.0;

					//_cwprintf(L"%d\n", angle1_);
				
					if (Bessel_cacular % 2 == 0 )
					{
						for (int m = 0; m < 4; m++)
						{
							t = m * 1.0 / 4;

							anglebessel1[m] = 0;//左手赋 0
							anglebessel2[m] = 0;
							anglebessel3[m] = 0;
							anglebessel4[m] = 0;

							anglebessel1_[m] = 0;//右手赋 0
							anglebessel2_[m] = 0;
							anglebessel3_[m] = 0;
							anglebessel4_[m] = 0;

							//曲线的基函数
							T_Bessel[0] = (1 - t)*(1 - t)*(1 - t);
							T_Bessel[1] = 3 * t*(1 - t)*(1 - t);
							T_Bessel[2] = 3 * t*t*(1 - t);
							T_Bessel[3] = t*t*t;

							for (int i = 0; i<4; i++)
							{
								anglebessel1[m] = anglebessel1[m] + T_Bessel[i] * anglesave1[i];//右手计算数据
								anglebessel2[m] = anglebessel2[m] + T_Bessel[i] * anglesave2[i];
								anglebessel3[m] = anglebessel3[m] + T_Bessel[i] * anglesave3[i];
								anglebessel4[m] = anglebessel4[m] + T_Bessel[i] * anglesave4[i];

								anglebessel1_[m] = anglebessel1_[m] + T_Bessel[i] * anglesave1_[i];//左手计算数据
								anglebessel2_[m] = anglebessel2_[m] + T_Bessel[i] * anglesave2_[i];
								anglebessel3_[m] = anglebessel3_[m] + T_Bessel[i] * anglesave3_[i];
								anglebessel4_[m] = anglebessel4_[m] + T_Bessel[i] * anglesave4_[i];
							}
						}
					}
					Bessel_cacular++;

				}

				for (int i = 0; i < 2; i++)
				{
					if (Bessel_cacular % 2 == 1)
					{
						angle1 = anglebessel1[i];
						angle2 = anglebessel2[i];
						angle3 = anglebessel3[i];
						angle4 = anglebessel4[i];

						angle1_ = anglebessel1_[i];
						angle2_ = anglebessel2_[i];
						angle3_ = anglebessel3_[i];
						angle4_ = anglebessel4_[i];
					}
					else
					{
						angle1 = anglebessel1[i+2];
						angle2 = anglebessel2[i+2];
						angle3 = anglebessel3[i+2];
						angle4 = anglebessel4[i+2];

						angle1_ = anglebessel1_[i+2];
						angle2_ = anglebessel2_[i+2];
						angle3_ = anglebessel3_[i+2];
						angle4_ = anglebessel4_[i+2];
					}
					//_cwprintf(L"\nmathcoculat:%d,%d,%d,%d\n", angle1_, angle2_, angle3_, angle4_);
					//cout << angle1 << "    " << angle2 << "    " << angle3 << "    " << angle4 << endl;
					//cout << angle1_ << "    " << angle2_ << "    " << angle3_ << "    " << angle4_ << endl;
					/*******符号位处理***************************************************************/
					/*******符号位处理***************************************************************/
					int Symbol_angle[4] = { angle1, angle3, angle1_, angle3_ };
					//int Symbol_angle[6] = { angle1, angle2, angle3, angle1_, angle2_, angle3_ };
					int Symbol_bit[4], k;
					for (k = 0; k < 4; k++)
					{
						if (Symbol_angle[k] < 0)
						{
							Symbol_bit[k] = 1;
						}
						else
						{
							Symbol_bit[k] = 0;
						}
					}
					//发数据时根据顺序确定对应哪一个电机
					union {
						int n;
						unsigned char arr[4];
					} data1_, data1, data2, data3_, data3, data4, data5_, data5, data6, data7_, data7, data8;	//分别对应于 θ1正负，θ1 ,θ2，θ3正负，θ3，θ4，θ5正负，θ5，θ6，θ7正负，θ7，θ8
																												//datan_.n=1;表示电机旋转角度为负；	datan_.n=0;表示电机旋转角度为正；

																												//data1_.n = Symbol_bit[0];
																												////data2_.n = Symbol_bit[1];
																												//data3_.n = Symbol_bit[1];
																												//data5_.n = Symbol_bit[2];
																												////data6_.n = Symbol_bit[4];
																												//data7_.n = Symbol_bit[3];
																												//预先设定电机角度值  data1, data2, data3, data4；
																												//将 i+1 帧得到的角度与 i 帧得到的相对应的角度进行比较；
																												//当差值（绝对值）处于阈值范围内时，认为i+1 帧得到的角度有效，以 i+1 帧得到的角度替代 i 帧得到的角度，否则角度值不更新
																												//发送八个角度 data1~data8 控制电机
																												//实际应用时还需加入角度“+”“-”的判断 (仅θ1 ，θ5)



																												/*【以迎宾机器人的视角：】
																												左手：【电机编号】  	1		2		3		4
																												【正向】		  向前	   向外	  顺时针  向前
																												右手：【电机编号】		1		2		3		4
																												【正向】		  向前	   向外	  逆时针  向前
																												*/
					data1.n = abs(angle1);	 //angle1	abs(angle1)
					data2.n = abs(angle2);	//angle2	abs(angle2)
					data3.n = abs(angle3);   //data3.n = angle3 / 4;	//angle3/4	abs(angle3)
					data4.n = abs(angle4);	//data4.n = (angle4 * 3) / 2;	//(angle4 * 3)/2	abs(angle4)
					data5.n = abs(angle1_);// abs(angle1_);	//angle1_
					data6.n = abs(angle2_);	//angle2_	abs(angle2_)
					data7.n = abs(angle3_);	//angle3_	abs(angle3_)
					data8.n = abs(angle4_);	//angle4_	abs(angle4_)
											//符号位
					data1_.n = Symbol_bit[0];
					//data2_.n = Symbol_bit[1];
					data3_.n = Symbol_bit[1];
					data5_.n = Symbol_bit[2];
					//data6_.n = Symbol_bit[4];
					data7_.n = Symbol_bit[3];

					//cout << abs(angle1) << "    " << abs(angle2) << "    " << abs(angle3) << "    " << abs(angle4) << endl;
					//cout << abs(angle1_) << "    " << abs(angle2_) << "    " << abs(angle3_) << "    " << abs(angle4_) << endl;
					//Sleep(500);
 
					//迎宾机器人蓝牙接 USART1
					unsigned char array[14] = { 0xa5, data1.arr[0], data2.arr[0], data3.arr[0], data4.arr[0], data5.arr[0], data6.arr[0], data7.arr[0], data8.arr[0], data1_.arr[0], data3_.arr[0], data5_.arr[0], data7_.arr[0], 0xaa };
					//下位机处理：if (datan(n=1,3,5,7).n=1){datan.n=0-datan.n/*电机转-datan.n度*/}
					/******我修改部分***********************************************************/
					CSerialPort myport;
					myport.InitPort(3, CBR_115200, 'N', 8, 1, EV_RXCHAR);
					myport.OpenListenThread();
					myport.WriteData(array, 14);
					myport.CloseListenTread();
					myport.closePort();
					Sleep(15);//有待调试确定
					_cwprintf(L" %d:，%d:，%d，%d\n", angle1_,angle2_,angle3_, angle4_);
					//ofstream file_Bessel;
					//file_Bessel.open("E:\\Bessel.txt", ios::app);
					//file_Bessel << angle3_ << endl;
					//file_Bessel.close();

				}
				
			    

				{//读取时间
					SYSTEMTIME systime2;
					get_localtime(&systime2);
			//		_cwprintf(L"time2 is %d: %d: %d   %d\n", systime2.wHour, systime2.wMinute, systime2.wSecond, systime2.wMilliseconds);
				}
				
				/*hr = pNowBody->GetJointOrientations(JointType_Count, info.jointorientations);
				if (SUCCEEDED(hr)) {
					m_ImagaRenderer.SetBodyInfo(index, &info);
				}*/
			}
		}
		// 渲染
		if (SUCCEEDED(hr)) m_ImagaRenderer.OnRender(PeopleTrackedNum);
		// 安全释放
		for (int i = 0; i < BODY_COUNT; ++i) SafeRelease(ppBody[i]);
		SafeRelease(pBodyFrame);
		SafeRelease(pBFrameRef);
		SafeRelease(pArgs);
#ifdef _DEBUG
		if (SUCCEEDED(hr))
			_cwprintf(L" 成功\n");
		else
			_cwprintf(L" 失败\n");
#endif
	}
}


// 骨骼坐标映射屏幕坐标
D2D1_POINT_2F ThisApp::BodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height) {

	DepthSpacePoint depthPoint = { 0 };
	m_pCoordinateMapper->MapCameraPointToDepthSpace(bodyPoint, &depthPoint);

	float screenPointX = static_cast<float>(depthPoint.X * width) / DEPTH_WIDTH;
	float screenPointY = static_cast<float>(depthPoint.Y * height) / DEPTH_HEIGHT;

	return D2D1::Point2F(screenPointX, screenPointY);
}

