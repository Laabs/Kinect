// ThisApp类 本程序的抽象

#pragma once

#include <DirectXMath.h>
#include <queue>
#include "JointFilter.h"

using namespace Sample;

// ThisApp类
class ThisApp {
public:
	// 构造函数
	ThisApp();
	// 析构函数
	~ThisApp();
	// 初始化
	HRESULT Initialize(HINSTANCE hInstance, int nCmdShow);
	// 消息循环
	void RunMessageLoop();
	// 坐标映射
	D2D1_POINT_2F BodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height); //CameraSpacePoint: Represents a 3D point in camera space (in meters). 
																						  //The origin point (0,0,0), of the coordinate system is the camera position.
private:
	// 窗口过程函数
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	// 初始化Kinect
	HRESULT init_kinect();
	// 检查彩色帧
	void check_body_frame();
private:
	// 窗口句柄
	HWND			        m_hwnd = nullptr;
	// Kinect v2 传感器
	IKinectSensor*          m_pKinect = nullptr;
	// 骨骼帧读取器
	IBodyFrameReader*       m_pBodyFrameReader = nullptr;
	// 骨骼临帧事件 不能用nullptr初始化 蛋疼
	WAITABLE_HANDLE         m_hBodyFrameArrived = 0;
	// 坐标映射器
	ICoordinateMapper*      m_pCoordinateMapper = nullptr;
	// 渲染器
	ImageRenderer	        m_ImagaRenderer;

	//霍尔特双指数平滑滤波
	FilterDoubleExponential   m_FilterDoubleExponential;
};
