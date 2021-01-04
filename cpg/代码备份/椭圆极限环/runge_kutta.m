%% 龙格库塔法求解微分方程组的解
% ufunc:微分方程组名称
% y0：初始值
% h：步长
% [a,b]：时间起点和终点

function [t,yy1,yy2,yy3,yy4]=runge_kutta(equation,y0,h,a,b)
n=floor((b-a)/h);%求步数
t(1)=a;          %时间起点

yp1(1,:)=y0;        %赋初值，可以是向量，但是要注意维数
yf1(1,:)=y0;

yp2(1,:)=y0;        %赋初值，可以是向量，但是要注意维数
yf2(1,:)=y0;

yp3(1,:)=y0;        %赋初值，可以是向量，但是要注意维数
yf3(1,:)=y0;

yp4(1,:)=y0;        %赋初值，可以是向量，但是要注意维数
yf4(1,:)=y0;

beta=0.75;  %占空比(支撑相) == Df
% 四条腿相位
phi_LF=0;
phi_RF=0.5;
phi_LH=beta;
phi_RH=beta-phi_RF;

%各腿相对于第一条腿的相位差1-4(LF、RF、LH、RH)
phi_ij=[2*pi*phi_LF,2*pi*phi_RF,2*pi*phi_LH,2*pi*phi_RH];

for ii=1:n
% 更新数据
ym=[yp1(ii,:);yf1(ii,:);yp2(ii,:);yf2(ii,:);yp3(ii,:);yf3(ii,:);yp4(ii,:);yf4(ii,:)];

t(ii+1)=t(ii)+h;
k1=equation(t(ii),ym,phi_ij,beta);
k2=equation(t(ii)+h/2,ym+h*k1/2,phi_ij,beta);
k3=equation(t(ii)+h/2,ym+h*k2/2,phi_ij,beta);
k4=equation(t(ii)+h,ym+h*k3,phi_ij,beta);
% yp(ii+1,:)=ym(ii,:)+h*(k1+2*k2+2*k3+k4)/6;

yp1(ii+1,:)=ym(1,:)+h*(k1(1,:)+2*k2(1,:)+2*k3(1,:)+k4(1,:))/6;
yf1(ii+1,:)=ym(2,:)+h*(k1(2,:)+2*k2(2,:)+2*k3(2,:)+k4(2,:))/6;

yp2(ii+1,:)=ym(3,:)+h*(k1(3,:)+2*k2(3,:)+2*k3(3,:)+k4(3,:))/6;
yf2(ii+1,:)=ym(4,:)+h*(k1(4,:)+2*k2(4,:)+2*k3(4,:)+k4(4,:))/6;

yp3(ii+1,:)=ym(5,:)+h*(k1(5,:)+2*k2(5,:)+2*k3(5,:)+k4(5,:))/6;
yf3(ii+1,:)=ym(6,:)+h*(k1(6,:)+2*k2(6,:)+2*k3(6,:)+k4(6,:))/6;

yp4(ii+1,:)=ym(7,:)+h*(k1(7,:)+2*k2(7,:)+2*k3(7,:)+k4(7,:))/6;
yf4(ii+1,:)=ym(8,:)+h*(k1(8,:)+2*k2(8,:)+2*k3(8,:)+k4(8,:))/6;
%按照龙格库塔方法进行数值求解 

end

yy1=[yp1,yf1];
yy2=[yp2,yf2];
yy3=[yp3,yf3];
yy4=[yp4,yf4];


% aa=[1,2;3,4]
% bb=[5,6;7,8]
% cc=[aa,bb]

end

