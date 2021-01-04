// ImageRender�� ����ͼ��ͼ����Ⱦ

#pragma once

typedef std::map<std::wstring, ID2D1Bitmap*> BitmapCacheMap;
#define DEPTH_WIDTH 511//��ȴ������ֱ���-1�����չٷ��ĵ�
#define DEPTH_HEIGHT 423


struct BodyInfo {
	BOOL ok = FALSE;
	Joint joints[JointType_Count];//��¼�ؽڵ�λ�ã��ؽ����ࡢ����������ꡢ�Ƿ�׷�٣�
	JointOrientation jointorientations[JointType_Count];//��¼�ؽڵ㷽�򣨹ؽ����࣬����
														//��������ӳ�䵽��Ļ����
	D2D1_POINT_2F jointPoints[JointType_Count];//D2D1_POINT_2F: Represents an x-coordinate and y-coordinate pair in two-dimensional space.
	HandState leftHandState = HandState_Unknown;//����״̬
	HandState rightHandState = HandState_Unknown;//����״̬
};

class ImageRenderer {
public:
	// ���캯��
	ImageRenderer();
	// ��������
	~ImageRenderer();
	// ��Ⱦ,���ڿ̻�����
	HRESULT OnRender(int);
	// ���ô��ھ��
	void SetHwnd(HWND hwnd) { m_hwnd = hwnd; }
	// ���س�ʼ�����
	operator HRESULT() const { return m_hrInit; }
	// ���ù������ݣ�
	void SetBodyInfo(int, BodyInfo*);
private:
	// ��ȡͼƬ
	// bitmapName	[in] : �ļ���
	// ����: NULL��ʾʧ�� �������Ϊλͼ��ָ��
	ID2D1Bitmap* GetBitmap(std::wstring& bitmapName);
	// �����豸�޹���Դ
	HRESULT CreateDeviceIndependentResources();
	// �����豸�й���Դ
	HRESULT CreateDeviceResources();
	// �����豸�й���Դ
	void DiscardDeviceResources();
	// ���ļ���ȡλͼ
	HRESULT LoadBitmapFromFile(ID2D1RenderTarget*, IWICImagingFactory *, PCWSTR uri, UINT, UINT, ID2D1Bitmap **);
	// �̻�����
	void DrawBody();
	// �̻�����
	void DrawBody(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, int i);
	// �̻���
	void DrawHand(HandState handState, const D2D1_POINT_2F& handPosition);
	// �̻�����
	void DrawBone(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, JointType joint0, JointType joint1, int ii);
public:
	// D2Dת������ƽ�ơ���ת�����š���б
	D2D1_MATRIX_3X2_F                   matrix = D2D1::Matrix3x2F::Identity();
private:
	// ��ʼ�����
	HRESULT								m_hrInit = E_FAIL;
	// ���ھ��
	HWND								m_hwnd = nullptr;
	// D2D ����
	ID2D1Factory*						m_pD2DFactory = nullptr;
	// WIC ����
	IWICImagingFactory*					m_pWICFactory = nullptr;
	// DWrite����
	IDWriteFactory*						m_pDWriteFactory = nullptr;
	// �����ı���Ⱦ��ʽ
	IDWriteTextFormat*					m_pTextFormatMain = nullptr;
	// D2D��ȾĿ��
	ID2D1HwndRenderTarget*				m_pRenderTarget = nullptr;
	// ��ɫ��ˢ
	ID2D1Brush*                         m_pWhiteBrush = nullptr;
	// �ؽڱ�׷�ٵ���ɫ
	ID2D1SolidColorBrush*               m_pBrushJointTracked = nullptr;
	// �ؽڱ��ƶϵ���ɫ
	ID2D1SolidColorBrush*               m_pBrushJointInferred = nullptr;
	// ������׷�ٵ���ɫ
	ID2D1SolidColorBrush*               m_pBrushBoneTracked = nullptr;//��ɫ�����Ƶ�һ���˵Ĺ���
	ID2D1SolidColorBrush*				m_pBrushBoneTrackedRed = nullptr;//��ɫ�����Ƶڶ����˵Ĺ���
	ID2D1SolidColorBrush*				m_pBrushBoneTrackedYellow = nullptr;//��ɫ�����Ƶ������˵Ĺ���
	ID2D1SolidColorBrush*				m_pBrushBoneTrackedWhite = nullptr;//��ɫ�����Ƶ��ĸ��˵Ĺ���
	ID2D1SolidColorBrush*				m_pBrushBoneTrackedBlack = nullptr;//��ɫ�����Ƶ�����˵Ĺ���
	ID2D1SolidColorBrush*				m_pBrushBoneTrackedPurple = nullptr;//��ɫ�����Ƶ������˵Ĺ���
																			// �������ƶϵ���ɫ
	ID2D1SolidColorBrush*               m_pBrushBoneInferred = nullptr;
	// �ִ򿪵���ɫ
	ID2D1SolidColorBrush*               m_pBrushHandClosed = nullptr;
	// ��ȭ����ɫ
	ID2D1SolidColorBrush*               m_pBrushHandOpen = nullptr;
	// ������ָ�쿪����ɫ
	ID2D1SolidColorBrush*               m_pBrushHandLasso = nullptr;
	// ��ʱ��
	PrecisionTimer                      m_timer;
	// FPS����ÿ�봫��֡��
	FLOAT                               m_fFPS = 0.f;
	// �������ݣ�BODY_COUNT���������������Ϊ6
	BodyInfo                            m_bodyInfo[BODY_COUNT];
	// ͼ�񻺴�
	BitmapCacheMap						m_mapBitmapCache;
	ID2D1SolidColorBrush*				Brushcolor[7];
	int TrackedNum;
};
