function [xx,yy]=Gait_between(t,value,phi,n,xy_j)

    alpha=5;%x轴方向收敛速度
    beta=10;%y轴方向收敛速度
    L_s=150; %椭圆长轴，迈步步长
    H_s=100; %椭圆短轴，最大抬腿高度
    
    
    b_p=20; %摆动相和支撑相的切换速度

    
    D_f=0.75;%占地系数
    V_f=15;

    % 四条腿相位
    phi_LF=0;
    phi_RF=0.5;
    phi_LH=beta;
    phi_RH=beta-phi_RF;

    %各腿相对于第一条腿的相位差1-4(LF、RF、LH、RH)
    phi1=2*pi*phi_LF;
    phi2=2*pi*phi_RF;
    phi3=2*pi*phi_LH;
    phi4=2*pi*phi_RH;

    phi_ij=0;

    % 不同脚的步态
    Rphi_ij=[cos(phi_ij),-sin(phi_ij);sin(phi_ij),cos(phi_ij)];

    x_j=xy_j(1);%参考腿x坐标
    y_j=xy_j(2);%参考腿y坐标
    x=value(1);%相对于中心 x_p0 的坐标
    y=value(2);%相对于中心 y_p0 的坐标
    
    omiga=pi*V_f/L_s*(D_f/(1-D_f)/(1+exp(-b_p*y))+1/(1+exp(b_p*y)));%震荡频率(角)
    yy(1,1)=alpha*(1-4*x*x/L_s/L_s-y*y/H_s/H_s)*x+omiga*L_s/H_s/2*y;
    yy(1,2)=beta*(1-4*x*x/L_s/L_s-y*y/H_s/H_s)*y-omiga*2*H_s/L_s*x;
    

end