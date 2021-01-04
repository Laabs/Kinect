%% 函数定义

%此处函数为CPG震荡模型
function [yy]=equation(t,value,omiga)
    alpha=0.003;%x轴方向收敛速度
    x=value(1);%相对于中心 x_p0 的坐标
    y=value(2);%相对于中心 y_p0 的坐标
    u=10000; %圆的半径
    beta=0.75;  %占空比(支撑相)
    w_sw=12.5*pi;     %角频率(摆动相)
    b_p=50;     %决定支撑相、摆动相间的切换速度（设置为很大或者0）+
    
    %更新wi
    omiga=(1-beta)/beta*w_sw/(1+exp(-b_p*y))+w_sw/(1+exp(b_p*y));
    yy(1,1)=alpha*(u-x*x-y*y)*x+omiga*y;
    yy(1,2)=alpha*(u-x*x-y*y)*y-omiga*x;
    
end



%%调用举例
% [T1,F1]=runge_kutta(@equation,[0;0.2],0.01,0,10);
% plot(F1(:,1),F1(:,2),'.')