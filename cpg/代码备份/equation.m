%% 函数定义

%此处函数为CPG震荡模型
function [yy]=equation(t,value)
    alpha=5;%x轴方向收敛速度
    beta=10;%y轴方向收敛速度
    L_s=150; %椭圆长轴，迈步步长
    H_s=100; %椭圆短轴，最大抬腿高度
    %omiga=pi;% 振荡器频率，越小收敛的越快
    
    b_p=20; %摆动相和支撑相的切换速度
%     b_f=20;
%     ztd=0;
%     Kc=0;
    
    D_f=0.75;%占地系数
    V_f=15;
    % omiga=pi*V_f/L_s*(D_f/(1-D_f)*)
%     omiga_swing=pi*(1-D_f)*V_f/L_s;
%     omiga_stance=pi*D_f*V_f/L_s;

    
    x=value(1);%相对于中心 x_p0 的坐标
    y=value(2);%相对于中心 y_p0 的坐标
    
%     omiga=omiga_stance/(1+exp(-b_p*y))+omiga_swing/(1+exp(b_p*y));
    
    omiga=pi*V_f/L_s*(D_f/(1-D_f)/(1+exp(-b_p*y))+1/(1+exp(b_p*y)));%震荡频率(角)
    
    yy(1,1)=alpha*(1-4*x*x/L_s/L_s-y*y/H_s/H_s)*x+omiga*L_s/H_s/2*y;
    yy(1,2)=beta*(1-4*x*x/L_s/L_s-y*y/H_s/H_s)*y-omiga*2*H_s/L_s*x;
    
%     dxp=alpha*(1-4*x*x/L_s/L_s-y*y/H_s/H_s)*x+omiga*L_s/H_s/2*y;
%     dy=beta*(1-4*x*x/L_s/L_s-y*y/H_s/H_s)*y-omiga*2*H_s/L_s*x;
%     xf=(dxp)/(1+exp(-b_f*(dy-ztd)))-V_f/(1+exp(-b_f*(dy-ztd)));
%     
%     yy(1,1)=xf;
%     yy(1,2)=dy;
end



%%调用举例
% [T1,F1]=runge_kutta(@equation,[0;0.2],0.01,0,10);
% plot(F1(:,1),F1(:,2),'.')