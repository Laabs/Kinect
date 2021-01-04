%% 龙格库塔法求解微分方程组的解
% ufunc:微分方程组名称
% y0：初始值
% h：步长
% [a,b]：时间起点和终点

function [t,yp,yf]=runge_kutta(equation,y0,h,a,b)
n=floor((b-a)/h);%求步数
t(1)=a;          %时间起点
yp(1,:)=y0;        %赋初值，可以是向量，但是要注意维数
yf(1,:)=y0;
for ii=1:n
% 更新数据
ym=[yp(ii,:);yf(ii,:)];

t(ii+1)=t(ii)+h;
k1=equation(t(ii),ym);
k2=equation(t(ii)+h/2,ym+h*k1/2);
k3=equation(t(ii)+h/2,ym+h*k2/2);
k4=equation(t(ii)+h,ym+h*k3);
% yp(ii+1,:)=ym(ii,:)+h*(k1+2*k2+2*k3+k4)/6;
yp(ii+1,:)=ym(1,:)+h*(k1(1,:)+2*k2(1,:)+2*k3(1,:)+k4(1,:))/6;
yf(ii+1,:)=ym(2,:)+h*(k1(2,:)+2*k2(2,:)+2*k3(2,:)+k4(2,:))/6;

%按照龙格库塔方法进行数值求解 
end





