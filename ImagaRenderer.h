// ImageRender类 主管图形图像渲染

#pragma once

typedef std::map<std::wstring, ID2D1Bitmap*> BitmapCacheMap;
#define DEPTH_WIDTH 511//深度传感器分辨率-1，参照官方文档
#define DEPTH_HEIGHT 423


struct BodyInfo {
	BOOL ok = FALSE;
	Joint joints[JointType_Count];//记录关节点位置（关节种类、摄像机点坐标、是否被追踪）
	JointOrientation jointorientations[JointType_Count];//记录关节点方向（关节种类，方向）
														//骨骼坐标映射到屏幕坐标
	D2D1_POINT_2F jointPoints[JointType_Count];//D2D1_POINT_2F: Represents an x-coordinate and y-coordinate pair in two-dimensional space.
	HandState leftHandState = HandState_Unknown;//左手状态
	HandState rightHandState = HandState_Unknown;//右手状态
};

class ImageRenderer {
public:
	// 构造函数
	ImageRenderer();
	// 析构函数
	~ImageRenderer();
	// 渲染,用于刻画骨骼
	HRESULT OnRender(int);
	// 设置窗口句柄
	void SetHwnd(HWND hwnd) { m_hwnd = hwnd; }
	// 返回初始化结果
	operator HRESULT() const { return m_hrInit; }
	// 设置骨骼数据，
	void SetBodyInfo(int, BodyInfo*);
private:
	// 获取图片
	// bitmapName	[in] : 文件名
	// 返回: NULL表示失败 其余的则为位图的指针
	ID2D1Bitmap* GetBitmap(std::wstring& bitmapName);
	// 创建设备无关资源
	HRESULT CreateDeviceIndependentResources();
	// 创建设备有关资源
	HRESULT CreateDeviceResources();
	// 丢弃设备有关资源
	void DiscardDeviceResources();
	// 从文件读取位图
	HRESULT LoadBitmapFromFile(ID2D1RenderTarget*, IWICImagingFactory *, PCWSTR uri, UINT, UINT, ID2D1Bitmap **);
	// 刻画身体
	void DrawBody();
	// 刻画身体
	void DrawBody(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, int i);
	// 刻画手
	void DrawHand(HandState handState, const D2D1_POINT_2F& handPosition);
	// 刻画骨骼
	void DrawBone(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, JointType joint0, JointType joint1, int ii);
public:
	// D2D转换矩阵，平移、旋转、缩放、倾斜
	D2D1_MATRIX_3X2_F                   matrix = D2D1::Matrix3x2F::Identity();
private:
	// 初始化结果
	HRESULT								m_hrInit = E_FAIL;
	// 窗口句柄
	HWND								m_hwnd = nullptr;
	// D2D 工厂
	ID2D1Factory*						m_pD2DFactory = nullptr;
	// WIC 工厂
	IWICImagingFactory*					m_pWICFactory = nullptr;
	// DWrite工厂
	IDWriteFactory*						m_pDWriteFactory = nullptr;
	// 正文文本渲染格式
	IDWriteTextFormat*					m_pTextFormatMain = nullptr;
	// D2D渲染目标
	ID2D1HwndRenderTarget*				m_pRenderTarget = nullptr;
	// 白色笔刷
	ID2D1Brush*                         m_pWhiteBrush = nullptr;
	// 关节被追踪的颜色
	ID2D1SolidColorBrush*               m_pBrushJointTracked = nullptr;
	// 关节被推断的颜色
	ID2D1SolidColorBrush*               m_pBrushJointInferred = nullptr;
	// 骨骼被追踪的颜色
	ID2D1SolidColorBrush*               m_pBrushBoneTracked = nullptr;//绿色，绘制第一个人的骨骼
	ID2D1SolidColorBrush*				m_pBrushBoneTrackedRed = nullptr;//红色，绘制第二个人的骨骼
	ID2D1SolidColorBrush*				m_pBrushBoneTrackedYellow = nullptr;//黄色，绘制第三个人的骨骼
	ID2D1SolidColorBrush*				m_pBrushBoneTrackedWhite = nullptr;//白色，绘制第四个人的骨骼
	ID2D1SolidColorBrush*				m_pBrushBoneTrackedBlack = nullptr;//黑色，绘制第五个人的骨骼
	ID2D1SolidColorBrush*				m_pBrushBoneTrackedPurple = nullptr;//紫色，绘制第六个人的骨骼
																			// 骨骼被推断的颜色
	ID2D1SolidColorBrush*               m_pBrushBoneInferred = nullptr;
	// 手打开的颜色
	ID2D1SolidColorBrush*               m_pBrushHandClosed = nullptr;
	// 握拳的颜色
	ID2D1SolidColorBrush*               m_pBrushHandOpen = nullptr;
	// 部分手指伸开的颜色
	ID2D1SolidColorBrush*               m_pBrushHandLasso = nullptr;
	// 计时器
	PrecisionTimer                      m_timer;
	// FPS，即每秒传输帧数
	FLOAT                               m_fFPS = 0.f;
	// 骨骼数据，BODY_COUNT代表的是人体数量为6
	BodyInfo                            m_bodyInfo[BODY_COUNT];
	// 图像缓存
	BitmapCacheMap						m_mapBitmapCache;
	ID2D1SolidColorBrush*				Brushcolor[7];
	int TrackedNum;
};
