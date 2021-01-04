% 绘制位移时间图
% tt=0.05*(0:1:length(F1(:,1))-1);
% plot(tt,F1(:,1),'r.',tt,F1(:,2),'g.')
% xlabel('时间t(ms)');
% ylabel('位移s(cm)');
% legend('x','z');

clc;
clear;

[T1,y1,y2,y3,y4]=runge_kutta(@equation,[0;0.5],0.01,0,80);
 plot(y1(:,1),y1(:,2),'r.')
 hold on;
 plot(y2(:,1),y2(:,2),'k.')
 hold on;
 plot(y3(:,1),y3(:,2),'b.')
 hold on;
 plot(y4(:,1),y4(:,2),'g.')

 plot(T1,y1(:,1),'.',T1,y2(:,1),'.',T1,y3(:,1),'.',T1,y4(:,1),'.')
 plot(T1,y1(:,2),'.',T1,y2(:,2),'.',T1,y3(:,2),'.',T1,y4(:,2),'.')
 
 for ii=1:1:length(y1(:,2))
     if y1(ii,2)>0
         theta_ki(ii)=3.3/9.8*y1(ii,2);
     else
         theta_ki(ii)=0;
     end
 end
 
 
 
 














