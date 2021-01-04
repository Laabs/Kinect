%% 龙格库塔法求解微分方程组的解
% ufunc:微分方程组名称
% y0：初始值
% h：步长
% [a,b]：时间起点和终点

function [t,y]=runge_kutta(equation,y0,h,a,b)
n=floor((b-a)/h);%求步数
t(1)=a;          %时间起点
y(1,:)=y0;        %赋初值，可以是向量，但是要注意维数
beta=0.25;
wsw=pi;
b_p=-0;

for ii=1:n 
w_y=y(ii,2); 
omiga=(1-beta)/beta*wsw/(1+exp(-b_p*w_y))+wsw/(1+exp(b_p*w_y));

t(ii+1)=t(ii)+h;
k1=equation(t(ii),y(ii,:),omiga);
k2=equation(t(ii)+h/2,y(ii,:)+h*k1/2,omiga);
k3=equation(t(ii)+h/2,y(ii,:)+h*k2/2,omiga);
k4=equation(t(ii)+h,y(ii,:)+h*k3,omiga);
y(ii+1,:)=y(ii,:)+h*(k1+2*k2+2*k3+k4)/6;
%按照龙格库塔方法进行数值求解
end




