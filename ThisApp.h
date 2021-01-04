// ThisApp�� ������ĳ���

#pragma once

#include <DirectXMath.h>
#include <queue>
#include "JointFilter.h"

using namespace Sample;

// ThisApp��
class ThisApp {
public:
	// ���캯��
	ThisApp();
	// ��������
	~ThisApp();
	// ��ʼ��
	HRESULT Initialize(HINSTANCE hInstance, int nCmdShow);
	// ��Ϣѭ��
	void RunMessageLoop();
	// ����ӳ��
	D2D1_POINT_2F BodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height); //CameraSpacePoint: Represents a 3D point in camera space (in meters). 
																						  //The origin point (0,0,0), of the coordinate system is the camera position.
private:
	// ���ڹ��̺���
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	// ��ʼ��Kinect
	HRESULT init_kinect();
	// ����ɫ֡
	void check_body_frame();
private:
	// ���ھ��
	HWND			        m_hwnd = nullptr;
	// Kinect v2 ������
	IKinectSensor*          m_pKinect = nullptr;
	// ����֡��ȡ��
	IBodyFrameReader*       m_pBodyFrameReader = nullptr;
	// ������֡�¼� ������nullptr��ʼ�� ����
	WAITABLE_HANDLE         m_hBodyFrameArrived = 0;
	// ����ӳ����
	ICoordinateMapper*      m_pCoordinateMapper = nullptr;
	// ��Ⱦ��
	ImageRenderer	        m_ImagaRenderer;

	//������˫ָ��ƽ���˲�
	FilterDoubleExponential   m_FilterDoubleExponential;
};
