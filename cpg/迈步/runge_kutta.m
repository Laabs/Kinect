%% 龙格库塔法求解微分方程组的解
% ufunc:微分方程组名称
% y0：初始值
% h：步长
% [a,b]：时间起点和终点
% 返回值：t 时间步长
%         y1 - y4,4个CPG震荡器的值(x,y)

function [t,y1,y2,y3,y4]=runge_kutta(equation,y0,h,a,b)
n=floor((b-a)/h);%求步数
t(1)=a;          %时间起点

y1(1,:)=y0;        %赋初值，可以是向量，但是要注意维数
y2(1,:)=y0;
y3(1,:)=y0;
y4(1,:)=y0;

beta=0.75;  %占空比(支撑相)
w_sw=0.2*pi;     %角频率(摆动相)
b_p=0;     %决定支撑相、摆动相间的切换速度（设置为很大或者0）

% 四条腿相位
phi_LF=0;
phi_RF=0.5;
phi_LH=beta;
phi_RH=beta-phi_RF;

%各腿相对于第一条腿的相位差1-4(LF、RF、LH、RH)
phi_ij=[2*pi*phi_LF,2*pi*phi_RF,2*pi*phi_LH,2*pi*phi_RH];


% for j=1:1:4
%     phi_cos(j)=0;
%     phi_sin(j)=0;
%     for k=1:1:4
%         if j~=k
%             phi_cos(j)=phi_cos(j)+cos(phi_ij(j)-phi_ij(k));
%             phi_sin(j)=phi_sin(j)+sin(phi_ij(j)-phi_ij(k));
%         end
%     end
% end


% phi=[phi_cos;phi_sin];

for ii=1:n 
%上一次的y值
w_y1=y1(ii,2); 
w_y2=y2(ii,2);
w_y3=y3(ii,2);
w_y4=y4(ii,2);
% 更新xi、yi
ym=[y1(ii,:);y2(ii,:);y3(ii,:);y4(ii,:)];
%更新wi
omiga1=(1-beta)/beta*w_sw/(1+exp(-b_p*w_y1))+w_sw/(1+exp(b_p*w_y1));
omiga2=(1-beta)/beta*w_sw/(1+exp(-b_p*w_y2))+w_sw/(1+exp(b_p*w_y2));
omiga3=(1-beta)/beta*w_sw/(1+exp(-b_p*w_y3))+w_sw/(1+exp(b_p*w_y3));
omiga4=(1-beta)/beta*w_sw/(1+exp(-b_p*w_y4))+w_sw/(1+exp(b_p*w_y4));
omiga_m=[omiga1,omiga2,omiga3,omiga4];
%更新步长
t(ii+1)=t(ii)+h;

% y1
k1=equation(t(ii),ym,phi_ij,omiga_m);
k2=equation(t(ii)+h/2,ym+h*k1/2,phi_ij,omiga_m);
k3=equation(t(ii)+h/2,ym+h*k2/2,phi_ij,omiga_m);
k4=equation(t(ii)+h,ym+h*k3,phi_ij,omiga_m);

y1(ii+1,:)=ym(1,:)+h*(k1(1,:)+2*k2(1,:)+2*k3(1,:)+k4(1,:))/6;
y2(ii+1,:)=ym(2,:)+h*(k1(2,:)+2*k2(2,:)+2*k3(2,:)+k4(2,:))/6;
y3(ii+1,:)=ym(3,:)+h*(k1(3,:)+2*k2(3,:)+2*k3(3,:)+k4(3,:))/6;
y4(ii+1,:)=ym(4,:)+h*(k1(4,:)+2*k2(4,:)+2*k3(4,:)+k4(4,:))/6;

% ym(ii+1,:)=ym(ii,:)+h*(k1+2*k2+2*k3+k4)/6;
if ii==floor(3*n/5)
    beta=0.5;  %占空比(支撑相)
    w_sw=0.2*pi;     %角频率(摆动相)
    b_p=0;     %决定支撑相、摆动相间的切换速度（设置为很大或者0）

    % 四条腿相位
    phi_LF=0;
    phi_RF=0.5;
    phi_LH=beta;
    phi_RH=beta-phi_RF;

    %各腿相对于第一条腿的相位差1-4(LF、RF、LH、RH)
    phi_ij=[2*pi*phi_LF,2*pi*phi_RF,2*pi*phi_LH,2*pi*phi_RH];
end

end






