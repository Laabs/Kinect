





data:2017/5/12

Configuration requirements:
opencv3.0 beta
Armadillo//D:.........

function:
	1.霍尔特指数平滑
	2.特定人识别
	3.angle3问题修复（带角度的正反向问题）
	4.限幅预处理

bug:
	1.未经过机器检验
	2.霍尔特参数固定，所有关节权值相同

next aim：
	1.霍尔特参数线性化，不同关节参数设置灵活化
	2.利用joint oriention 正向运动学求解关节角度，目的是为了使得模仿模型更加完善

joint oriention connection

1.别人的代码，还没有看，考试完后重点看
https://github.com/IntStarFoo/KinectJointVisualizer

2.msdn相关链接
https://social.msdn.microsoft.com/Forums/en-US/a87049b5-7842-4c17-b776-3f6f4260c801/how-to-interpret-jointorientation-data

3.无法下载的代码（网站中有很多源代码）
http://www.codeforge.cn/

