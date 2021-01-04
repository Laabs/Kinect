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
//boolֵ�����ж��Ƿ������ߵ�ǰ�ĸ�ֵ
bool init_bessel = true;
//���ڼ������ִ�д���
static int num_bessel = 0;
//ż������½������
static int Bessel_cacular = 0;

int PeopleTrackedNum = 0;

//��ȡ�ĸ����Ƶ㣬ԭʼ����
float anglepoint1[4], anglepoint2[4], anglepoint3[4], anglepoint4[4];
float anglepoint1_[4], anglepoint2_[4], anglepoint3_[4], anglepoint4_[4];

//��ȡ�ĸ����Ƶ㣬ȡ�������������
float anglesave1[4], anglesave2[4], anglesave3[4], anglesave4[4];
float anglesave1_[4], anglesave2_[4], anglesave3_[4], anglesave4_[4];

//��ȡÿ�������ֵ���������е���
float anglebessel1[4] = { 0 }, anglebessel2[4] = { 0 }, anglebessel3[4] = { 0 }, anglebessel4[4] = { 0 };
float anglebessel1_[4] = { 0 }, anglebessel2_[4] = { 0 }, anglebessel3_[4] = { 0 }, anglebessel4_[4] = { 0 };


//���ڴ洢bessel���ĸ�����
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
* �������ƣ�norm_3D
* �������ܣ�������ά���� A �ĵ�λ����
* ���������A   �� �Զ��� vector3D �ṹ������
* �����������
* �� �� ֵ��norm(A)    �� vector3D����
* �� �� �ã���
* ����ʱ�䣺2017�� 4�� 27��
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
//����任����
inline CameraSpacePoint tran_Kinect2Man(CameraSpacePoint RightPoint, CameraSpacePoint LeftPoint, CameraSpacePoint PrePoint)
{
	CameraSpacePoint TranPoint;
	double temp = sqrt(powf((RightPoint.X - LeftPoint.X), 2.0) + powf((RightPoint.Y - LeftPoint.Y), 2.0) + powf((RightPoint.Z - LeftPoint.Z), 2.0));
	TranPoint.X = ((RightPoint.X + LeftPoint.X - 2 * PrePoint.X)*(RightPoint.X - LeftPoint.X) + (RightPoint.Y + LeftPoint.Y - 2 * PrePoint.Y)*(RightPoint.Y - LeftPoint.Y) + (RightPoint.Z + LeftPoint.Z - 2 * PrePoint.Z)*(RightPoint.Z - LeftPoint.Z)) / (2 * temp);
	TranPoint.Y = PrePoint.Y - (RightPoint.Y + LeftPoint.Y) / 2;
	TranPoint.Z = ((RightPoint.Z + LeftPoint.Z - 2 * PrePoint.Z)*(RightPoint.X - LeftPoint.X) - (RightPoint.X + LeftPoint.X - 2 * PrePoint.X)*(RightPoint.Z - LeftPoint.Z)) / (2 * temp);
	return TranPoint;
}
// ThisApp���캯��
ThisApp::ThisApp() {

}

// ThisApp��������
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
	// �����¼�
	if (m_hBodyFrameArrived && m_pBodyFrameReader) {
		m_pBodyFrameReader->UnsubscribeFrameArrived(m_hBodyFrameArrived);
		m_hBodyFrameArrived = 0;
	}
	// �ͷ�BodyFrameReader
	SafeRelease(m_pBodyFrameReader);
	// �ر�Kinect
	if (m_pKinect) {
		m_pKinect->Close();
	}
	SafeRelease(m_pKinect);
}

// ��ʼ��
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
		// ע�ᴰ��
		RegisterClassEx(&wcex);
		// ��������
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
			// ���ô��ھ��
			m_ImagaRenderer.SetHwnd(m_hwnd);
			// ��ʾ����
			ShowWindow(m_hwnd, nCmdShow);
			UpdateWindow(m_hwnd);
		}
	}

	return hr;
}



// ��Ϣѭ��
void ThisApp::RunMessageLoop()
{
	MSG msg;//MSG structure: Contains message information from a thread's message queue.
	HANDLE events[] = { reinterpret_cast<HANDLE>(m_hBodyFrameArrived) };
	while (true) {
		// ��Ϣ����
		if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		// �����¼�
		// �¼�0: ��ɫ��֡�¼�
		events[0] = reinterpret_cast<HANDLE>(m_hBodyFrameArrived);
		// ����¼�
		switch (MsgWaitForMultipleObjects(lengthof(events), events, FALSE, INFINITE, QS_ALLINPUT))
		{
			// events[0]
		case WAIT_OBJECT_0 + 0:
			this->check_body_frame();
			break;
		default:
			break;
		}
		// �˳�
		if (msg.message == WM_QUIT) {
			break;
		}
	}
}


// ���ڹ��̺���
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

		// ����ʼ��Kinect
		if (FAILED(pOurApp->init_kinect())) {
			::MessageBoxW(hwnd, L"��ʼ��Kinect v2ʧ��", L"��ĺ��ź�", MB_ICONERROR);
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
				pOurApp->m_ImagaRenderer.OnRender(PeopleTrackedNum);//��仰������ûʲô�ã���Ϊ������̴�Kinect�ղ������ݻ���˵���ݲ�����
				break;
			case WM_KEYDOWN://������Ӧ����
				//the lefe ley�� VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
				//the little(right) key��VK_0 - VK_9 are the same as ASCII '0' - '9' (0x60 - 0x69)
				//the following code function��test any tey's ASCII:
			    //default:
				//_cwprintf(L"ACSII��%#X\n", wParam); //#X funciton is printf the standard ASCII
				//break;
				switch (wParam)
				{
//right key
				case 0x30://0
					PeopleTrackedNum = 0;
					_cwprintf(L"��ǰ׷��Ŀ�꣺%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x31://1
					PeopleTrackedNum = 1;
					_cwprintf(L"��ǰ׷��Ŀ�꣺%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x32://2
					PeopleTrackedNum = 2;
					_cwprintf(L"��ǰ׷��Ŀ�꣺%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x33://3
					PeopleTrackedNum = 3;
					_cwprintf(L"��ǰ׷��Ŀ�꣺%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x34://4
					PeopleTrackedNum = 4;
					_cwprintf(L"��ǰ׷��Ŀ�꣺%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x35://5
					PeopleTrackedNum = 5;
					_cwprintf(L"��ǰ׷��Ŀ�꣺%d", PeopleTrackedNum);
					Sleep(10);
					break;
//little key
				case 0x60://0
					PeopleTrackedNum = 0;
					_cwprintf(L"��ǰ׷��Ŀ�꣺%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x61://1
					PeopleTrackedNum = 1;
					_cwprintf(L"��ǰ׷��Ŀ�꣺%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x62://2
					PeopleTrackedNum = 2;
					_cwprintf(L"��ǰ׷��Ŀ�꣺%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x63://3
					PeopleTrackedNum = 3;
					_cwprintf(L"��ǰ׷��Ŀ�꣺%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x64://4
					PeopleTrackedNum = 4;
					_cwprintf(L"��ǰ׷��Ŀ�꣺%d", PeopleTrackedNum);
					Sleep(10);
					break;
				case 0x65://5
					PeopleTrackedNum = 5;
					_cwprintf(L"��ǰ׷��Ŀ�꣺%d", PeopleTrackedNum);
					Sleep(10);
					break;
				default:
					_cwprintf(L"������Ч,��Ч������0 - 5 \n");
					break;
				}
				pOurApp->m_ImagaRenderer.OnRender(PeopleTrackedNum);
				break;
				
			case WM_CLOSE:
				// ����β����(�����ȫ�����߳�)��������
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


// ��ʼ��Kinect
HRESULT ThisApp::init_kinect() {
	IBodyFrameSource* pBodyFrameSource = nullptr;
	// ���ҵ�ǰĬ��Kinect
	HRESULT hr = ::GetDefaultKinectSensor(&m_pKinect);
	// ��ʿ�ش�Kinect
	if (SUCCEEDED(hr)) {
		hr = m_pKinect->Open();
	}
	// ��ȡ��ɫ֡Դ(BodyFrameSource)
	if (SUCCEEDED(hr)) {
		hr = m_pKinect->get_BodyFrameSource(&pBodyFrameSource);
	}
	// �ٻ�ȡ��ɫ֡��ȡ��
	if (SUCCEEDED(hr)) {
#ifdef _DEBUG
		// ����Ƿ��Ѵ���
		if (m_pBodyFrameReader)
			::MessageBoxW(m_hwnd, L"��Ա����:m_pBodyFrameReader ֵ�Ѵ���", L"<ThisApp::init_kinect>", MB_ICONERROR);
#endif
		hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
	}
	// ע����֡�¼�
	if (SUCCEEDED(hr)) {
#ifdef _DEBUG
		// ����Ƿ��Ѵ���
		if (m_hBodyFrameArrived)
			::MessageBoxW(m_hwnd, L"��Ա����:m_hBodyFrameArrived ֵ�Ѵ���", L"<ThisApp::init_kinect>", MB_ICONERROR);
#endif
		m_pBodyFrameReader->SubscribeFrameArrived(&m_hBodyFrameArrived);
	}
	// ��ȡ����ӳ����
	if (SUCCEEDED(hr)) {
		hr = m_pKinect->get_CoordinateMapper(&m_pCoordinateMapper);
	}
	SafeRelease(pBodyFrameSource);
	return hr;
}


// ������֡
void ThisApp::check_body_frame() {
	if (!m_pBodyFrameReader) return;
#ifdef _DEBUG
	static int frame_num = 0;
	++frame_num;
	_cwprintf(L"<ThisApp::check_color_frame>Frame@%8d ", frame_num);
#endif 
	// ������֡�¼�����
	IBodyFrameArrivedEventArgs* pArgs = nullptr;  //IBodyFrameArrivedEventArgs: Arguments for the body FrameReady events. 
												  // ����֡����
	IBodyFrameReference* pBFrameRef = nullptr;
	// ����֡
	IBodyFrame* pBodyFrame = nullptr;
	// ����
	IBody*  ppBody[BODY_COUNT] = { 0 };
	// ��ȡ����
	HRESULT hr = m_pBodyFrameReader->GetFrameArrivedEventData(m_hBodyFrameArrived, &pArgs); //GetFrameArrivedEventData: Gets the event data when a new frame arrives.
																							//Return value
																							//Type : HRESULT
																							//Returns S_OK if successful; otherwise, returns a failure code.
																							// ��ȡ����
	if (SUCCEEDED(hr)) {
		hr = pArgs->get_FrameReference(&pBFrameRef);
	}
	// ��ȡ����֡
	if (SUCCEEDED(hr)) {
		hr = pBFrameRef->AcquireFrame(&pBodyFrame);
	}
	// ��ȡ��������
	if (SUCCEEDED(hr)) {
		hr = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, ppBody);
	}


	{//��ȡʱ��
		SYSTEMTIME systime1;
		get_localtime(&systime1);
	//	_cwprintf(L"time1 is %d: %d: %d   %d\n", systime1.wHour, systime1.wMinute, systime1.wSecond, systime1.wMilliseconds);
	}

	// ��������
	if (SUCCEEDED(hr)) {
		double Distance = 4.5;		//��kinect�ľ���,���ֵ
		int index = 10;				//index��ֵ����5���ɣ���Ϊ�˵��������Ϊ��0��5��
		CameraSpacePoint _SpineBase_;//��λm
									 // ��������
		BodyInfo infoDraw;
		for (int i = 0; i < BODY_COUNT; ++i)
		{
			// ��֤���ݣ���֤��׷��
			BOOLEAN bTracked = false;
			hr = ppBody[i]->get_IsTracked(&bTracked);

			if (SUCCEEDED(hr) && bTracked && ppBody[i]) {
				IBody* pNowBodyDraw = ppBody[i];
				// ��ȡ������״̬
				pNowBodyDraw->get_HandLeftState(&infoDraw.leftHandState);
				pNowBodyDraw->get_HandRightState(&infoDraw.rightHandState);
				hr = pNowBodyDraw->GetJoints(JointType_Count, infoDraw.joints);

				Joint _joints[JointType_Count];
				hr = ppBody[i]->GetJoints(JointType_Count, _joints);//���ڻ�ȡ�˵����йؽ�λ����Ϣ
				if (SUCCEEDED(hr)&&i != PeopleTrackedNum)
				{
					for (int j = 0; j < JointType_Count; ++j)
						infoDraw.jointPoints[j] = BodyToScreen(infoDraw.joints[j].Position, WNDWIDTH, WNDHEIGHT);
					m_ImagaRenderer.SetBodyInfo(i, &infoDraw);
				}

				if (SUCCEEDED(hr))
				{
					//��SpineBase��ZֵΪ��׼���ҳ���kinect���������
					CameraSpacePoint SpineBase = _joints[0].Position;
					if (SpineBase.Z < Distance)
					{
						Distance = SpineBase.Z;
						index = i;
						_SpineBase_.X = SpineBase.X;
						//_cwprintf(TEXT("\nX:%f\n"), i);//�������Ҿ��룬��λm���Ҿ��벻������
					}
				}
			}
		}

		if (index < 6 && abs(_SpineBase_.X < 0.9) && flag1)//��¼׷���˱��
		{
			PeopleTrackedNum = index;
			flag1 = false;

		}

		/*ȷ�������ڷ�Χ�ڣ�����ʶ��*/
		if (flag1 == false)		//  ����(_SpineBase_.X)<1.1
		{
			IBody* pNowBody = ppBody[PeopleTrackedNum];
			BodyInfo info;
			// ��ȡ������״̬
			pNowBody->get_HandLeftState(&info.leftHandState);
			pNowBody->get_HandRightState(&info.rightHandState);
			// ��ȡ�����ռ�������Ϣ
			hr = pNowBody->GetJoints(JointType_Count, info.joints);
//�޷���ʼ
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
			//	//����һ֡����
			//	last_info.joints[i].Position.X = info.joints[i].Position.X;
			//	last_info.joints[i].Position.Y = info.joints[i].Position.Y;
			//	last_info.joints[i].Position.Z = info.joints[i].Position.Z;
			//}
//�����޷�
			if (SUCCEEDED(hr)) {
				//��������ָ����  ������׷�ٵ������壬0��11����������˲�������ȡ�˲���Ĺ�������
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
				*Ŀ��������Χ��380-389
				*TraverseResult��ʾ׷�ٽ��(�������Z�������Ϊ0��ʾû��׷�ٵ�)�����������⣬����ֻ��Ҫ3��2��20��1��
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
					// ����ת��
					info.jointPoints[j] = BodyToScreen(info.joints[j].Position, WNDWIDTH, WNDHEIGHT);
				}
				m_ImagaRenderer.SetBodyInfo(PeopleTrackedNum, &info);
//��������任
					CameraSpacePoint LeftPoint = info.joints[12].Position;
					CameraSpacePoint RightPoint = info.joints[16].Position;
					info.joints[4].Position = tran_Kinect2Man(RightPoint, LeftPoint, info.joints[4].Position);
					info.joints[5].Position = tran_Kinect2Man(RightPoint, LeftPoint, info.joints[5].Position);
					info.joints[6].Position = tran_Kinect2Man(RightPoint, LeftPoint, info.joints[6].Position);

					info.joints[8].Position = tran_Kinect2Man(RightPoint, LeftPoint, info.joints[8].Position);
					info.joints[9].Position = tran_Kinect2Man(RightPoint, LeftPoint, info.joints[9].Position);
					info.joints[10].Position = tran_Kinect2Man(RightPoint, LeftPoint, info.joints[10].Position);
			
			
				
				//���ּ粿�Ƕȵļ���
				/*˵�������ּ粿�Ƕȵļ���
				* ˼·�� 1>�����ڼ粿�ؽ�����ϵ�е�YOZ���ϵ�ͶӰ����粿�ؽ�����ϵ�е�Y�Ḻ����ļн�Ϊ��1��
				*		 2>����ؽ�����粿�ؽ�����ϵ��X��������нǵ����Ϊ��2��
				*		 3>���⴦���ڼ���ؽ���X�᷽���غ�ʱ�������ڼ粿�ؽ�����ϵ�е�YOZ���ϵ�ͶӰ�����ڣ�һ���㣩����ʱȡ����ؽ���粿�ؽ�����ϵ�е�Y�Ḻ����ļн�Ϊ��1��
				*/
				int  angle1 = 0, angle2 = 0, angle3 = 0, angle4 = 0;	//��1 ,��2����3����4
				/*��������������Y0���������
				*DATA:5.29
				*AUTHOR:TS
				*vectorY0.X = info.joints[0].Position.X - info.joints[20].Position.X;
				*vectorY0.Y = info.joints[0].Position.Y - info.joints[20].Position.Y;
				*vectorY0.Z = info.joints[0].Position.Z - info.joints[20].Position.Z;
				*/
				vector3D  vectorS_E, vectorS_E_proj, norm_vectorS_E, norm_vectorS_E_proj, vectorX0, vectorY0 = { 0, 1.0, 0 };
				//����X0������X0ȡ��λ�����Ҽ絽�����XOZƽ���ͶӰ������ȡ��XOZƽ���ͶӰ������ԭ���Ǽ粿�ؽڵ�������ֲ������ƶ��������ƶ�������ɦ�2ƫС
				vectorX0.X = info.joints[4].Position.X - info.joints[8].Position.X;
				vectorX0.Y = 0.0;//�˴���Ϊ�˱���粿�ƶ���Ӱ��
				vectorX0.Z = info.joints[4].Position.Z - info.joints[8].Position.Z;
				vectorX0 = norm_3D(vectorX0);//��λ��X0����
											 //�����������SE����
				vectorS_E.X = info.joints[5].Position.X - info.joints[4].Position.X;
				vectorS_E.Y = info.joints[5].Position.Y - info.joints[4].Position.Y;
				vectorS_E.Z = info.joints[5].Position.Z - info.joints[4].Position.Z;
				//�������������YOZƽ���ͶӰ����SE_proj����
				vectorS_E_proj.X = 0.0;
				vectorS_E_proj.Y = info.joints[5].Position.Y - info.joints[4].Position.Y;
				vectorS_E_proj.Z = info.joints[5].Position.Z - info.joints[4].Position.Z;
				//�ڼ���ؽ���X�᷽���غ�ʱ�����⴦��ʹ��SE��������SE_proj��������֤����λ�÷�ĸ��Ϊ0
				if (abs(vectorS_E_proj.Z)<0.01&&abs(vectorS_E_proj.Y) < 0.01)
				{
					vectorS_E_proj.X = vectorS_E.X;
				}
				//��λ��SE������SE_proj����
				norm_vectorS_E = norm_3D(vectorS_E);
				norm_vectorS_E_proj = norm_3D(vectorS_E_proj);
				//���S_E_proj������Y0�����ĵ��
				float dot_S_E_proj_Y0 = cal_dot_product3D(norm_vectorS_E_proj, vectorY0);
				//���S_E_������X0�����ĵ��
				float dot_S_E_X0 = cal_dot_product3D(norm_vectorS_E, vectorX0);
				//����1
				angle1 = (int)(acosf(-dot_S_E_proj_Y0) * 180 / PI);
				//����'2���˴���15������Ϊ(acosf(dot_S_E_X0_) * 180 / PI)�ļ���ƫС15�����ң�Ϊ��ʹ���������ƣ���Ϊ�������
				angle2 = 90 - (int)(acosf(dot_S_E_X0) * 180 / PI) - 15;
				//�ؽ�1����������
				if (vectorS_E.Z<0)
				{
					angle1 = -angle1;//���ⲿ�ڼ粿����ʱ����'1�ķ���ת
									 //�˴���Ϊ�˵��ⲿ�Ǵ�����Խ���粿ƽ��ʱ��ʹ��'1����������룬���Ǵ�180��360�ȵķ�����룬������һ��������-180���ҡ�
					if (angle1 < -110)
						angle1 += 360;
				}


				//�����ⲿ�Ƕȵļ���
				/*˵���� �����ⲿ�Ƕȵļ���
				* ˼·�� 1>����ؽ�����SE����������ؽ�����EW�����ļн�Ϊ��4��
				*		 2>EW������SE�����ϵ�ͶӰΪE_W_proj������EW�����ڴ�ֱ��SE������ƽ���ϵ�ͶӰΪElbowWrist_projXZ������ElbowWrist_projXZ�����뾭��ؽ�1��ת���Z�����ļнǼ�Ϊ��3��
				*
				*/
				vector3D  vectorE_W, norm_vectorE_W, vectorE_W_proj, ElbowWrist_projXZ, norm_ElbowWrist_projXZ;
				//����E_W����
				vectorE_W.X = info.joints[6].Position.X - info.joints[5].Position.X;
				vectorE_W.Y = info.joints[6].Position.Y - info.joints[5].Position.Y;
				vectorE_W.Z = info.joints[6].Position.Z - info.joints[5].Position.Z;
				//��λ��E_W����
				norm_vectorE_W = norm_3D(vectorE_W);
				//����SE������EW�����ĵ��
				float dot_S_E_E_W = cal_dot_product3D(norm_vectorS_E, norm_vectorE_W);
				//����4>90��ʱ�����4=90�ȣ��ܻ�е�ṹ���ƣ�
				if (dot_S_E_E_W<0) {
					dot_S_E_E_W = 0;
				}
				//�����4
				angle4 = (int)(acosf(dot_S_E_E_W) * 180 / PI);

				//�����⴦��:������紦��һ��ֱ��ʱ����3Ϊ�������
				if (abs(angle4)<10)
				{
					angle3 = 0;

				}
				else
				{
					vector3D Z_vect, vector_N;
					//Z_vectΪ�粿��ת��z��ķ��������� Z_vect = (0, sin��1, cos��1)
					Z_vect.X = 0;
					Z_vect.Y = sinf(angle1 * PI / 180);
					Z_vect.Z = cosf(angle1 * PI / 180);
					//N�����������б��3����ĸ�����������EW�������SE��������
					vector_N = cross_3D(vectorE_W, vectorS_E);
					//����E_W_proj������EW������SE�����ϵ�ͶӰ����
					float ElbowWrist_proj_f = cal_dot_product3D(vectorE_W, norm_vectorS_E);
					vectorE_W_proj.X = ElbowWrist_proj_f * norm_vectorS_E.X;
					vectorE_W_proj.Y = ElbowWrist_proj_f * norm_vectorS_E.Y;
					vectorE_W_proj.Z = ElbowWrist_proj_f * norm_vectorS_E.Z;
					//����E_W_projXZ������EW�����ڴ�ֱ��SE������ƽ���ϵ�ͶӰ����
					ElbowWrist_projXZ.X = vectorE_W.X - vectorE_W_proj.X;
					ElbowWrist_projXZ.Y = vectorE_W.Y - vectorE_W_proj.Y;
					ElbowWrist_projXZ.Z = vectorE_W.Z - vectorE_W_proj.Z;
					//��λ��E_W_projXZ����
					norm_ElbowWrist_projXZ = norm_3D(ElbowWrist_projXZ);
					//��3Ϊ���� Z_vect��ElbowWrist_projXZ �ļн�;
					angle3 = (int)(acosf(cal_dot_product3D(norm_ElbowWrist_projXZ, Z_vect)) * 180 / PI);
					//����N������Z�����жϦ�3������������תʱΪ��
					if (cal_dot_product3D(vector_N, Z_vect) > 0)
					{
						/*if (Z_vect.Z > 0)
							angle3 = -angle3;*/
						//else
						//	angle3 = 360 - angle3;//�˴���Ϊ�˦�3��180�ȷ�����룬������ͻȻ��ת��-180�ȣ�����ʱ��3>180��
						//������ԭ���ģ�������Ϊ��ע��
						angle3 = -angle3;

						/*���ؽ�1�ĽǶ�Ϊ���ŵ�ʱ�ؽ�3������Ĵ���
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

				//�����⴦��:���ֱ���ֱ��ֱ��ǰʱ������������ڵ���������ڵ���������⴦��
				if (abs(angle4)<30 && abs(vectorE_W.X)<0.07)
					angle3 = 0;

				////���ٹؽ�3�Ķ���
				//if (abs(angle3) < 10) {
				//	angle3 = 0;
				//}
				//ofstream fileoutb;
				//fileoutb.open("E:\\���ڽ���\\����\\����ǰ.dat", ios::app);
				//fileoutb << angle1 << "\t" << angle3 << endl;
				//fileoutb.close();


				//���ּ粿�Ƕȵļ���
				/*˵�������ּ粿�Ƕȵļ���
				*˼·��1>�����ڼ粿�ؽ�����ϵ�е�YOZ���ϵ�ͶӰ����粿�ؽ�����ϵ�е�Y�Ḻ����ļн�Ϊ��1��
				*		2>����ؽ�����粿�ؽ�����ϵ��X��������нǵ����Ϊ��2��
				3>���⴦���ڼ���ؽ���X�᷽���غ�ʱ�������ڼ粿�ؽ�����ϵ�е�YOZ���ϵ�ͶӰ�����ڣ�һ���㣩����ʱȡ����ؽ���粿�ؽ�����ϵ�е�Y�Ḻ����ļн�Ϊ��1��
				*
				*/
				int  angle1_ = 0, angle2_ = 0, angle3_ = 0, angle4_ = 0;	//��'1 ,��'2����'3����'4
				/*��������������Y0���������
				*DATA:5.29
				*AUTHOR:TS
				*vectorY0_.X = info.joints[0].Position.X - info.joints[20].Position.X;
				*vectorY0_.Y = info.joints[0].Position.Y - info.joints[20].Position.Y;
				*vectorY0_.Z = info.joints[0].Position.Z - info.joints[20].Position.Z;
				*/
				vector3D  vectorS_E_, vectorS_E_proj_, norm_vectorS_E_, norm_vectorS_E_proj_, vectorX0_, vectorY0_ = { 0, 1.0, 0 };
				//����X0������X0ȡ��λ�����Ҽ絽�����XOZƽ���ͶӰ������ȡ��XOZƽ���ͶӰ������ԭ���Ǽ粿�ؽڵ�������ֲ������ƶ��������ƶ�������ɦ�2ƫС
				vectorX0_.X = info.joints[8].Position.X - info.joints[4].Position.X;
				vectorX0_.Y = 0.0;//�˴���Ϊ�˱���粿�ƶ���Ӱ��
				vectorX0_.Z = info.joints[8].Position.Z - info.joints[4].Position.Z;
				vectorX0_ = norm_3D(vectorX0_);//��λ��X0����


											   //�����������SE����
				vectorS_E_.X = info.joints[9].Position.X - info.joints[8].Position.X;
				vectorS_E_.Y = info.joints[9].Position.Y - info.joints[8].Position.Y;
				vectorS_E_.Z = info.joints[9].Position.Z - info.joints[8].Position.Z;
				//�������������YOZƽ���ͶӰ����SE_proj����
				vectorS_E_proj_.X = 0.0;
				vectorS_E_proj_.Y = info.joints[9].Position.Y - info.joints[8].Position.Y;
				vectorS_E_proj_.Z = info.joints[9].Position.Z - info.joints[8].Position.Z;
				//�ڼ���ؽ���X�᷽���غ�ʱ�����⴦��ʹ��SE��������SE_proj��������֤����λ�÷�ĸ��Ϊ0
				if (abs(vectorS_E_proj_.Z)<0.01&&abs(vectorS_E_proj_.Y) < 0.01)
				{
					vectorS_E_proj_.X = vectorS_E_.X;
				}
				//��λ��SE������SE_proj����
				norm_vectorS_E_ = norm_3D(vectorS_E_);
				norm_vectorS_E_proj_ = norm_3D(vectorS_E_proj_);
				//���S_E_proj������Y0�����ĵ��
				float dot_S_E_proj_Y0_ = cal_dot_product3D(norm_vectorS_E_proj_, vectorY0_);
				//���S_E_������X0�����ĵ��
				float dot_S_E_X0_ = cal_dot_product3D(norm_vectorS_E_, vectorX0_);
				//����'1
				angle1_ = (int)(acosf(-dot_S_E_proj_Y0_) * 180 / PI);
				//����'2���˴���15������Ϊ(acosf(dot_S_E_X0_) * 180 / PI)�ļ���ƫС15�����ң�Ϊ��ʹ���������ƣ���Ϊ�������
				angle2_ = 90 - (int)(acosf(dot_S_E_X0_) * 180 / PI) - 15;




				//�ؽ�1����������
				if (vectorS_E_.Z<0)
				{
					angle1_ = -angle1_;//���ⲿ�ڼ粿����ʱ����'1�ķ���ת
									   //�˴���Ϊ�˵��ⲿ�Ǵ�����Խ���粿ƽ��ʱ��ʹ��'1����������룬���Ǵ�180��360�ȵķ�����룬������һ��������-180���ҡ�
					if (angle1_ < -110)
						angle1_ += 360;
				}


				//�����ⲿ�Ƕȵļ���
				/*˵���������ⲿ�Ƕȵļ���
				*˼·��1>����ؽ�����SE����������ؽ�����EW�����ļн�Ϊ��4��
				*	   2>EW������SE�����ϵ�ͶӰΪE_W_proj������EW�����ڴ�ֱ��SE������ƽ���ϵ�ͶӰΪElbowWrist_projXZ������ElbowWrist_projXZ�����뾭��ؽ�1��ת���Z�����ļнǼ�Ϊ��3��

				*
				*/
				vector3D  vectorE_W_, norm_vectorE_W_, vectorE_W_proj_, ElbowWrist_projXZ_, norm_ElbowWrist_projXZ_;
				//����E_W����
				vectorE_W_.X = info.joints[10].Position.X - info.joints[9].Position.X;
				vectorE_W_.Y = info.joints[10].Position.Y - info.joints[9].Position.Y;
				vectorE_W_.Z = info.joints[10].Position.Z - info.joints[9].Position.Z;
				//��λ��E_W����
				norm_vectorE_W_ = norm_3D(vectorE_W_);
				//����SE������EW�����ĵ��
				float dot_S_E_E_W_ = cal_dot_product3D(norm_vectorS_E_, norm_vectorE_W_);
				//����4>90��ʱ�����4=90�ȣ��ܻ�е�ṹ���ƣ�
				if (dot_S_E_E_W_<0) {
					dot_S_E_E_W_ = 0;
				}
				//�����4
				angle4_ = (int)(acosf(dot_S_E_E_W_) * 180 / PI);

				//�����⴦��:������紦��һ��ֱ��ʱ����3Ϊ�������
				if (abs(angle4_)<10)
				{
					angle3_ = 0;

				}
				else
				{
					vector3D Z_vect_, vector_N_;
					//Z_vectΪ�粿��ת��z��ķ��������� Z_vect = (0, sin��1, cos��1)
					Z_vect_.X = 0;
					Z_vect_.Y = sinf(angle1_ * PI / 180);
					Z_vect_.Z = cosf(angle1_ * PI / 180);

					//N�����������б��3����ĸ�����������EW�������SE��������
					vector_N_ = cross_3D(vectorE_W_, vectorS_E_);
					//����E_W_proj������EW������SE�����ϵ�ͶӰ����
					float ElbowWrist_proj_f_ = cal_dot_product3D(vectorE_W_, norm_vectorS_E_);
					vectorE_W_proj_.X = ElbowWrist_proj_f_ * norm_vectorS_E_.X;
					vectorE_W_proj_.Y = ElbowWrist_proj_f_ * norm_vectorS_E_.Y;
					vectorE_W_proj_.Z = ElbowWrist_proj_f_ * norm_vectorS_E_.Z;
					//����E_W_projXZ������EW�����ڴ�ֱ��SE������ƽ���ϵ�ͶӰ����
					ElbowWrist_projXZ_.X = vectorE_W_.X - vectorE_W_proj_.X;
					ElbowWrist_projXZ_.Y = vectorE_W_.Y - vectorE_W_proj_.Y;
					ElbowWrist_projXZ_.Z = vectorE_W_.Z - vectorE_W_proj_.Z;
					//��λ��E_W_projXZ����
					norm_ElbowWrist_projXZ_ = norm_3D(ElbowWrist_projXZ_);


					//��3Ϊ���� Z_vect��ElbowWrist_projXZ �ļн�;
					angle3_ = (int)(acosf(cal_dot_product3D(norm_ElbowWrist_projXZ_, Z_vect_)) * 180 / PI);
					//����N������Z�����жϦ�3������������תʱΪ��
					if (cal_dot_product3D(vector_N_, Z_vect_) > 0)
					{
						if (Z_vect_.Z > 0)
							angle3_ = -angle3_;
						else
							angle3_ = 360 - angle3_;//�˴���Ϊ�˦�3��180�ȷ�����룬������ͻȻ��ת��-180�ȣ�����ʱ��3>180��

					}
					/*���ؽ�1�ĽǶ�Ϊ���ŵ�ʱ�ؽ�3������Ĵ���
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

				//�����⴦��:���ֱ���ֱ��ֱ��ǰʱ������������ڵ���������ڵ���������⴦��
				if (abs(angle4_)<30 && abs(vectorE_W_.X)<0.07)
					angle3_ = 0;
				
				////���ٹؽ�3�Ķ���
				//if (abs(angle3_) < 10) {
				//	angle3_ = 0;
				//}
				//�Ƕȱ���
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

				//bessel�˲���

				//�洢�ĸ����Ƶ㣬ԭʼ���� start
				if (init_bessel)//��ʼ��
				{
					anglepoint1[num_bessel] = angle1;//��������
					anglepoint2[num_bessel] = angle2;
					anglepoint3[num_bessel] = angle3;
					anglepoint4[num_bessel] = angle4;

					anglepoint1_[num_bessel] = angle1_;//��������
					anglepoint2_[num_bessel] = angle2_;
					anglepoint3_[num_bessel] = angle3_;
					anglepoint4_[num_bessel] = angle4_;

					//���������� 1
					num_bessel++;

					if (num_bessel == 4)//����ӿ�ʼ���е������ĸ����ݣ����ǰ�����ߣ�֮������ if ����������ִ��
					{
						//�Ӵ�ȡ���ĸ����Ƶ���ȡǰ�������Ƶ㣬ȡ������������� start 
						for (int i = 0; i<3; i++)
						{
							anglesave1[i] = anglepoint1[i];//����ǰ��������
							anglesave2[i] = anglepoint2[i];
							anglesave3[i] = anglepoint3[i];
							anglesave4[i] = anglepoint4[i];

							anglesave1_[i] = anglepoint1_[i];//����ǰ��������
							anglesave2_[i] = anglepoint2_[i];
							anglesave3_[i] = anglepoint3_[i];
							anglesave4_[i] = anglepoint4_[i];
						}

						//���ֵ��ĸ����ݵ�ļ���
						anglesave1[3] = (anglepoint1[2] + anglepoint1[3]) / 2.0;
						anglesave2[3] = (anglepoint2[2] + anglepoint2[3]) / 2.0;
						anglesave3[3] = (anglepoint3[2] + anglepoint3[3]) / 2.0;
						anglesave4[3] = (anglepoint4[2] + anglepoint4[3]) / 2.0;
						//���ֵ��ĸ����ݵļ���
						anglesave1_[3] = (anglepoint1_[2] + anglepoint1_[3]) / 2.0;
						anglesave2_[3] = (anglepoint2_[2] + anglepoint2_[3]) / 2.0;
						anglesave3_[3] = (anglepoint3_[2] + anglepoint3_[3]) / 2.0;
						anglesave4_[3] = (anglepoint4_[2] + anglepoint4_[3]) / 2.0;
						//��ȡ�ĸ����Ƶ㣬ȡ������������� end 

						init_bessel = false;
					}
				}
				else //�洢�������ݣ��ǳ�ʼ�����
				{
					static int j = 0;
					//�������ݴ洢
					anglepoint1[j] = anglepoint1[j + 2];
					anglepoint1[j + 2] = angle1;

					anglepoint2[j] = anglepoint2[j + 2];
					anglepoint2[j + 2] = angle2;

					anglepoint3[j] = anglepoint3[j + 2];
					anglepoint3[j + 2] = angle3;

					anglepoint4[j] = anglepoint4[j + 2];
					anglepoint4[j + 2] = angle4;

					//�������ݴ洢
					anglepoint1_[j] = anglepoint1_[j + 2];
					anglepoint1_[j + 2] = angle1_;

					anglepoint2_[j] = anglepoint2_[j + 2];
					anglepoint2_[j + 2] = angle2_;

					anglepoint3_[j] = anglepoint3_[j + 2];
					anglepoint3_[j + 2] = angle3_;

					anglepoint4_[j] = anglepoint4_[j + 2];
					anglepoint4_[j + 2] = angle4_;

					j++; // ���������ۼ�1

					if (j == 2)
					{
						//��ȡ�ĸ����Ƶ㣬ȡ������������� start
						for (int i = 1; i<3; i++)
						{
							anglesave1[i] = anglepoint1[i];//����ǰ��������
							anglesave2[i] = anglepoint2[i];
							anglesave3[i] = anglepoint3[i];
							anglesave4[i] = anglepoint4[i];

							anglesave1_[i] = anglepoint1_[i];//����ǰ��������
							anglesave2_[i] = anglepoint2_[i];
							anglesave3_[i] = anglepoint3_[i];
							anglesave4_[i] = anglepoint4_[i];
						}

						anglesave1[0] = (anglepoint1[0] + anglepoint1[1]) / 2.0;//���ֵ�һ������
						anglesave2[0] = (anglepoint2[0] + anglepoint2[1]) / 2.0;
						anglesave3[0] = (anglepoint3[0] + anglepoint3[1]) / 2.0;
						anglesave4[0] = (anglepoint4[0] + anglepoint4[1]) / 2.0;

						anglesave1_[0] = (anglepoint1_[0] + anglepoint1_[1]) / 2.0;//���ֵ�һ������
						anglesave2_[0] = (anglepoint2_[0] + anglepoint2_[1]) / 2.0;
						anglesave3_[0] = (anglepoint3_[0] + anglepoint3_[1]) / 2.0;
						anglesave4_[0] = (anglepoint4_[0] + anglepoint4_[1]) / 2.0;


						anglesave1[3] = (anglepoint1[2] + anglepoint1[3]) / 2.0;//���ֵ��ĸ�����
						anglesave2[3] = (anglepoint2[2] + anglepoint2[3]) / 2.0;
						anglesave3[3] = (anglepoint3[2] + anglepoint3[3]) / 2.0;
						anglesave4[3] = (anglepoint4[2] + anglepoint4[3]) / 2.0;

						anglesave1_[3] = (anglepoint1_[2] + anglepoint1_[3]) / 2.0;//���ֵ��ĸ�����
						anglesave2_[3] = (anglepoint2_[2] + anglepoint2_[3]) / 2.0;
						anglesave3_[3] = (anglepoint3_[2] + anglepoint3_[3]) / 2.0;
						anglesave4_[3] = (anglepoint4_[2] + anglepoint4_[3]) / 2.0;
						//��ȡ�ĸ����Ƶ㣬ȡ������������� end

						j = 0;//������������
					}
				}
				//�洢�ĸ����Ƶ㣬ԭʼ���� end


				//������ϽǶȵļ���
				if (!init_bessel)
				{
					
					float t = 0.0;

					//_cwprintf(L"%d\n", angle1_);
				
					if (Bessel_cacular % 2 == 0 )
					{
						for (int m = 0; m < 4; m++)
						{
							t = m * 1.0 / 4;

							anglebessel1[m] = 0;//���ָ� 0
							anglebessel2[m] = 0;
							anglebessel3[m] = 0;
							anglebessel4[m] = 0;

							anglebessel1_[m] = 0;//���ָ� 0
							anglebessel2_[m] = 0;
							anglebessel3_[m] = 0;
							anglebessel4_[m] = 0;

							//���ߵĻ�����
							T_Bessel[0] = (1 - t)*(1 - t)*(1 - t);
							T_Bessel[1] = 3 * t*(1 - t)*(1 - t);
							T_Bessel[2] = 3 * t*t*(1 - t);
							T_Bessel[3] = t*t*t;

							for (int i = 0; i<4; i++)
							{
								anglebessel1[m] = anglebessel1[m] + T_Bessel[i] * anglesave1[i];//���ּ�������
								anglebessel2[m] = anglebessel2[m] + T_Bessel[i] * anglesave2[i];
								anglebessel3[m] = anglebessel3[m] + T_Bessel[i] * anglesave3[i];
								anglebessel4[m] = anglebessel4[m] + T_Bessel[i] * anglesave4[i];

								anglebessel1_[m] = anglebessel1_[m] + T_Bessel[i] * anglesave1_[i];//���ּ�������
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
					/*******����λ����***************************************************************/
					/*******����λ����***************************************************************/
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
					//������ʱ����˳��ȷ����Ӧ��һ�����
					union {
						int n;
						unsigned char arr[4];
					} data1_, data1, data2, data3_, data3, data4, data5_, data5, data6, data7_, data7, data8;	//�ֱ��Ӧ�� ��1��������1 ,��2����3��������3����4����5��������5����6����7��������7����8
																												//datan_.n=1;��ʾ�����ת�Ƕ�Ϊ����	datan_.n=0;��ʾ�����ת�Ƕ�Ϊ����

																												//data1_.n = Symbol_bit[0];
																												////data2_.n = Symbol_bit[1];
																												//data3_.n = Symbol_bit[1];
																												//data5_.n = Symbol_bit[2];
																												////data6_.n = Symbol_bit[4];
																												//data7_.n = Symbol_bit[3];
																												//Ԥ���趨����Ƕ�ֵ  data1, data2, data3, data4��
																												//�� i+1 ֡�õ��ĽǶ��� i ֡�õ������Ӧ�ĽǶȽ��бȽϣ�
																												//����ֵ������ֵ��������ֵ��Χ��ʱ����Ϊi+1 ֡�õ��ĽǶ���Ч���� i+1 ֡�õ��ĽǶ���� i ֡�õ��ĽǶȣ�����Ƕ�ֵ������
																												//���Ͱ˸��Ƕ� data1~data8 ���Ƶ��
																												//ʵ��Ӧ��ʱ�������Ƕȡ�+����-�����ж� (����1 ����5)



																												/*����ӭ�������˵��ӽǣ���
																												���֣��������š�  	1		2		3		4
																												������		  ��ǰ	   ����	  ˳ʱ��  ��ǰ
																												���֣��������š�		1		2		3		4
																												������		  ��ǰ	   ����	  ��ʱ��  ��ǰ
																												*/
					data1.n = abs(angle1);	 //angle1	abs(angle1)
					data2.n = abs(angle2);	//angle2	abs(angle2)
					data3.n = abs(angle3);   //data3.n = angle3 / 4;	//angle3/4	abs(angle3)
					data4.n = abs(angle4);	//data4.n = (angle4 * 3) / 2;	//(angle4 * 3)/2	abs(angle4)
					data5.n = abs(angle1_);// abs(angle1_);	//angle1_
					data6.n = abs(angle2_);	//angle2_	abs(angle2_)
					data7.n = abs(angle3_);	//angle3_	abs(angle3_)
					data8.n = abs(angle4_);	//angle4_	abs(angle4_)
											//����λ
					data1_.n = Symbol_bit[0];
					//data2_.n = Symbol_bit[1];
					data3_.n = Symbol_bit[1];
					data5_.n = Symbol_bit[2];
					//data6_.n = Symbol_bit[4];
					data7_.n = Symbol_bit[3];

					//cout << abs(angle1) << "    " << abs(angle2) << "    " << abs(angle3) << "    " << abs(angle4) << endl;
					//cout << abs(angle1_) << "    " << abs(angle2_) << "    " << abs(angle3_) << "    " << abs(angle4_) << endl;
					//Sleep(500);
 
					//ӭ�������������� USART1
					unsigned char array[14] = { 0xa5, data1.arr[0], data2.arr[0], data3.arr[0], data4.arr[0], data5.arr[0], data6.arr[0], data7.arr[0], data8.arr[0], data1_.arr[0], data3_.arr[0], data5_.arr[0], data7_.arr[0], 0xaa };
					//��λ������if (datan(n=1,3,5,7).n=1){datan.n=0-datan.n/*���ת-datan.n��*/}
					/******���޸Ĳ���***********************************************************/
					CSerialPort myport;
					myport.InitPort(3, CBR_115200, 'N', 8, 1, EV_RXCHAR);
					myport.OpenListenThread();
					myport.WriteData(array, 14);
					myport.CloseListenTread();
					myport.closePort();
					Sleep(15);//�д�����ȷ��
					_cwprintf(L" %d:��%d:��%d��%d\n", angle1_,angle2_,angle3_, angle4_);
					//ofstream file_Bessel;
					//file_Bessel.open("E:\\Bessel.txt", ios::app);
					//file_Bessel << angle3_ << endl;
					//file_Bessel.close();

				}
				
			    

				{//��ȡʱ��
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
		// ��Ⱦ
		if (SUCCEEDED(hr)) m_ImagaRenderer.OnRender(PeopleTrackedNum);
		// ��ȫ�ͷ�
		for (int i = 0; i < BODY_COUNT; ++i) SafeRelease(ppBody[i]);
		SafeRelease(pBodyFrame);
		SafeRelease(pBFrameRef);
		SafeRelease(pArgs);
#ifdef _DEBUG
		if (SUCCEEDED(hr))
			_cwprintf(L" �ɹ�\n");
		else
			_cwprintf(L" ʧ��\n");
#endif
	}
}


// ��������ӳ����Ļ����
D2D1_POINT_2F ThisApp::BodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height) {

	DepthSpacePoint depthPoint = { 0 };
	m_pCoordinateMapper->MapCameraPointToDepthSpace(bodyPoint, &depthPoint);

	float screenPointX = static_cast<float>(depthPoint.X * width) / DEPTH_WIDTH;
	float screenPointY = static_cast<float>(depthPoint.Y * height) / DEPTH_HEIGHT;

	return D2D1::Point2F(screenPointX, screenPointY);
}

