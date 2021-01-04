

% ufunc:微分方程组名称
% y0：初始值
% h：步长
% [a,b]：时间起点和终点

clc;
clear;

[T1,F1]=runge_kutta(@equation,[0;0.5],0.01,0,10);
 plot(F1(:,1),F1(:,2),'.')

[T2,F2]=runge_kutta(@equation,[0;0.5],0.01,0,5);
% plot(F2(:,1),F2(:,2),'.')

[T3,F3]=runge_kutta(@equation,[0;0.5],0.01,0,2);




tt=0.05*(0:1:length(F1(:,1))-1);
% plot(tt,F1(:,1),'r.',tt,F2(:,1),'g.',tt,F3(:,1),'b.')

subplot(3,1,1)
plot(tt,F1(:,2),'r.')
legend('beta=0.25');
subplot(3,1,2)
plot(tt,F2(:,2),'g.')
legend('beta=0.5');
subplot(3,1,3)
plot(tt,F3(:,2),'b.')
legend('beta=0.75');


xlabel('时间t(ms)');
ylabel('位移s(cm)');




legend('w=0.5pi','w=pi','w=2pi');

% 绘制位移时间图
% tt=0.05*(0:1:length(F1(:,1))-1);
% plot(tt,F1(:,1),'r.',tt,F1(:,2),'g.')
% xlabel('时间t(ms)');
% ylabel('位移s(cm)');
% legend('x','z');


% 四条腿相位
phi_LF=0;
phi_RF=0.5;
phi_LH=beta;
phi_RH=beta-phi_RF;

%各腿相对于第一条腿的相位差1-4(LF、RF、LH、RH)
phi_ij=[2*pi*phi_LF,2*pi*phi_RF,2*pi*phi_LH,2*pi*phi_RH];



clc
clear


[T1,y1,y2,y3,y4]=runge_kutta(@equation,[0;0.5],0.01,0,110);
 plot(y1(:,1),y1(:,2),'r.')
 hold on;
 plot(y2(:,1),y2(:,2),'k.')
 hold on;
 plot(y3(:,1),y3(:,2),'b.')
 hold on;
 plot(y4(:,1),y4(:,2),'g.')
 
 theta_hi=y1(:,1);
 
 for ii=1:1:length(y1(:,2))
     if y1(ii,2)>0
         theta_ki(ii)=3.3/9.8*y1(ii,2);
     else
         theta_ki(ii)=0;
     end
 end
 
 
 tt=0.05*(0:1:length(y1(:,1))-1);
 plot(tt,y1(:,1),'.',tt,y2(:,1),'.',tt,y3(:,1),'.',tt,y4(:,1),'.')

 

 plot(tt,theta_hi)
 hold on













