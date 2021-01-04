

% ufunc:微分方程组名称
% y0：初始值
% h：步长
% [a,b]：时间起点和终点

clc;
clear;

[T1,F1]=runge_kutta(@equation,[100;-80],0.01,0,100);
plot(F1(:,1),F1(:,2),'.')
hold on

% 绘制位移时间图
% tt=0.05*(0:1:length(F1(:,1))-1);
% plot(tt,F1(:,1),'r.',tt,F1(:,2),'g.')
% xlabel('时间t(ms)');
% ylabel('位移s(cm)');
% legend('x','z');

beta=0.75;

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






