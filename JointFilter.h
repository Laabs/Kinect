#ifndef _FUNCTION_H
#define _FUNCTION_H
#include <Windows.h>
#include <WinBase.h>
#include <fstream>
#include <Kinect.h>

#define N  5	//��ֵ�˲���Nȡ������

/* �����ṹ�� vector����ʾһ����ά���������ԱΪ float �� */
struct vector {
	float x;
	float y;
};

//�����ṹ�� vector3D����//--------------------------------------------------------------------------------------
// KinectJointFilter.h
//
// This file contains Holt Double Exponential Smoothing filter for filtering Joints
//
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

#include <DirectXMath.h>
#include <queue>

namespace Sample
{
	typedef struct _TRANSFORM_SMOOTH_PARAMETERS
	{
		FLOAT   fSmoothing;             // [0..1], lower values closer to raw data
		FLOAT   fCorrection;            // [0..1], lower values slower to correct towards the raw data
		FLOAT   fPrediction;            // [0..n], the number of frames to predict into the future
		FLOAT   fJitterRadius;          // The radius in meters for jitter reduction
		FLOAT   fMaxDeviationRadius;    // The maximum radius in meters that filtered positions are allowed to deviate from raw data
	} TRANSFORM_SMOOTH_PARAMETERS;

	// Holt Double Exponential Smoothing filter ��������˫ָ��ƽ���˲�����
	class FilterDoubleExponentialData
	{
	public:
		DirectX::XMVECTOR m_vRawPosition;		//_m128���͵ı������Զ�����Ϊ16���ֽڵ��ֳ�
		DirectX::XMVECTOR m_vFilteredPosition;
		DirectX::XMVECTOR m_vTrend;
		DWORD    m_dwFrameCount;
	};

	class FilterDoubleExponential
	{
	public:
		FilterDoubleExponential() { Init(); }
		~FilterDoubleExponential() { Shutdown(); }
//�޸��˲���������{0.7f, 0.3f, 1.0f, 1.0f, 1.0f}
		//defaultParams :{0.5f, 0.5f, 0.5f, 0.05f, 0.04f};
		//VOID Init(FLOAT fSmoothing = 0.5f, FLOAT fCorrection = 0.5f, FLOAT fPrediction = 0.5f, FLOAT fJitterRadius = 0.05f, FLOAT fMaxDeviationRadius = 0.04f)
		//somewhatLatentParams : {0.5f, 0.1f, 0.5f, 0.1f, 0.1f}
		VOID Init(FLOAT fSmoothing = 0.5f, FLOAT fCorrection = 0.1f, FLOAT fPrediction = 0.5f, FLOAT fJitterRadius = 0.1f, FLOAT fMaxDeviationRadius = 0.1f)
		//verySmoothParams : {0.7f, 0.3f, 1.0f, 1.0f, 1.0f};
		//VOID Init(FLOAT fSmoothing = 0.7f, FLOAT fCorrection = 0.3f, FLOAT fPrediction = 1.0f, FLOAT fJitterRadius = 1.0f, FLOAT fMaxDeviationRadius = 1.0f)

		//VOID Init(FLOAT fSmoothing = 0.999f, FLOAT fCorrection = 1.0f, FLOAT fPrediction = 1.0f, FLOAT fJitterRadius = 0.1f, FLOAT fMaxDeviationRadius = 0.1f)

		//VOID Init(FLOAT fSmoothing = 0.25f, FLOAT fCorrection = 0.25f, FLOAT fPrediction = 0.25f, FLOAT fJitterRadius = 0.03f, FLOAT fMaxDeviationRadius = 0.05f)		
		{
			Reset(fSmoothing, fCorrection, fPrediction, fJitterRadius, fMaxDeviationRadius);
		}

		VOID Shutdown()
		{
		}

		VOID Reset(FLOAT fSmoothing = 0.25f, FLOAT fCorrection = 0.25f, FLOAT fPrediction = 0.25f, FLOAT fJitterRadius = 0.03f, FLOAT fMaxDeviationRadius = 0.05f)
		{
			assert(m_pFilteredJoints);			//�� m_pFilteredJoints Ϊ FALSE��assert() ���ڱ�׼���� stderr ����ʾ������Ϣ������ֹ����
			assert(m_pHistory);

			m_fMaxDeviationRadius = fMaxDeviationRadius; // Size of the max prediction radius Can snap back to noisy data when too high
			m_fSmoothing = fSmoothing;                   // How much smothing will occur.  Will lag when too high
			m_fCorrection = fCorrection;                 // How much to correct back from prediction.  Can make things springy
			m_fPrediction = fPrediction;                 // Amount of prediction into the future to use. Can over shoot when too high
			m_fJitterRadius = fJitterRadius;             // Size of the radius where jitter is removed. Can do too much smoothing when too high

			memset(m_pFilteredJoints, 0, sizeof(DirectX::XMVECTOR) * JointType_Count);		//���� m_pFilteredJoints ������ȫΪ0  (16 * 25)
			memset(m_pHistory, 0, sizeof(FilterDoubleExponentialData)* JointType_Count);	//���� m_pHistory ������ȫΪ0
		}

		void Update(IBody* const pBody);
		void Update(Joint joints[]);

		inline const DirectX::XMVECTOR* GetFilteredJoints() const { return &m_pFilteredJoints[0]; }

	public:
		DirectX::XMVECTOR m_pFilteredJoints[JointType_Count];
	private:
		FilterDoubleExponentialData m_pHistory[JointType_Count];
		FLOAT m_fSmoothing;
		FLOAT m_fCorrection;
		FLOAT m_fPrediction;
		FLOAT m_fJitterRadius;
		FLOAT m_fMaxDeviationRadius;

		void Update(Joint joints[], UINT JointID, TRANSFORM_SMOOTH_PARAMETERS smoothingParams);
	};
}
//ʾһ����ά���������ԱΪ float�� 
struct vector3D {
	float X;
	float Y;
	float Z;
};

void init_Medianfilter(CameraSpacePoint Jointstart, int f_num);//����ֵ�˲������г�ʼ��
CameraSpacePoint Medianfilter(CameraSpacePoint JointBuff, int num);		//��һ�������������ֵ�˲�

void get_localtime(SYSTEMTIME* current_systemtime);      //�õ��������ǰ��ʱ��
void export_data_to_txtfile(int data);                   //�������ݵ�txt�ļ���
float cal_dot_product(vector A, vector B);         //�����ά����A��B�ĵ������ȻҲ��������������ģ
float cal_dot_product3D(vector3D A, vector3D B);   //������ά����A��B�ĵ��
BOOL set_console_color(WORD attributes); //���ÿ���̨�������ɫ, ����ǰ��ɫ�ͱ���ɫ
#endif
