




 while 1
clc;
clear;

%-----------------------------------------------------相关参数------------------------------------
% 1.步态参数
lamda=0.25; %腿在空中占用的时间
zo=0;xo=0;zd=80;xd=150;
z_high=0;
%左下角起点位置
zm=-440;
xm=-80;

% 2.连杆参数
a_link=265;
b_link=75;
d_link=240;
e_link=80;
f_link=45;
g_link=sqrt(e_link*e_link-f_link*f_link);
gama_link=147/180*pi;
Q=[-f_link;0;-g_link];
t=1;
[Gx,Gz]=G(xo,xd,zo,zd,z_high,lamda,zm,xm); %0，60,0,50,0,0.25,-120,0

for i=1:1:length(Gx)
    % 末端相对于世界坐标系的坐标
%      A_x=-50+50*cos(i);
%      A_y=0;
%      A_z=-450+50*sin(i);
       A_x=Gx(i);
       A_y=40;
       A_z=Gz(i);
     
     rox=atan2(-A_y,-A_z); %【电机转角】向外为负，向里为正
     Rx=[1 0 0;0 cos(rox) -sin(rox);0 sin(rox) cos(rox)];%旋转矩阵
     P_A=[A_x;A_y;A_z];
     P_B=Rx'*P_A; %P_A=Rx*P_B;
     x=P_B(1);
     y=P_B(2);
     z=P_B(3);
 

QD=sqrt((x-Q(1))^2+(z-Q(3))^2);

alpha_link=atan2(-(z-Q(3)),x-Q(1))+acos((a_link*a_link+QD*QD-d_link*d_link)/(2*a_link*QD)); %【电机转角】恒为正
C=[Q(1)+a_link*cos(alpha_link);0;Q(3)-a_link*sin(alpha_link)];
beta_link=gama_link-acos((C(1)-x)/d_link);                        %【电机转角】向上为正，向下为负

A=[Q(1)-b_link*cos(beta_link);0;Q(3)+b_link*sin(beta_link)];
B=[Q(1)+a_link*cos(alpha_link)-b_link*cos(beta_link);0;Q(3)-a_link*sin(alpha_link)+b_link*sin(beta_link)];
D=[x;0;z];


A=Rx*A;
B=Rx*B;
C=Rx*C;
D=Rx*D;
A_Q=Rx*Q;

xx(t)=D(1);
yy(t)=D(2);
zz(t)=D(3);
t=t+1;

plot3([A_Q(1),0],[A_Q(2),0],[A_Q(3),0]);
hold on;
plot3([A_Q(1),A(1)],[A_Q(2),A(2)],[A_Q(3),A(3)]);
hold on;
plot3([A(1),B(1)],[A(2),B(2)],[A(3),B(3)]);
hold on;
plot3([B(1),C(1)],[B(2),C(2)],[B(3),C(3)]);
hold on;
plot3([C(1),D(1)],[C(2),D(2)],[C(3),D(3)]);
hold on;
plot3([A_Q(1),C(1)],[A_Q(2),C(2)],[A_Q(3),C(3)]);
hold on;
%%---------
plot3([0,0],[-200,200],[0,0]);
hold on;
plot3([-200,200],[0,0],[0,0]);
hold on;
plot3([0,0],[0,0],[-200,200]);
hold on;
%%---------
plot3(xx,yy,zz,'.');
hold on;
axis([-600,400,-600,400,-600,400])
grid on;
hold off;
pause(0.04);
%%% 求模验证答案是否正确
 bbb1(t-1)=norm(A-Q,2);
 bbb2(t-1)=norm(B-C,2);
 aaa1(t-1)=norm(A-B,2);
 aaa2(t-1)=norm(C-Q,2);
 ddd(t-1)=norm(D-C,2);
end
end





