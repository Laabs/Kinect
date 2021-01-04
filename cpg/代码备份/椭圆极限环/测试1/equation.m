%% 函数定义

%此处函数为CPG震荡模型
function [yy]=equation(t,value,phi_ij,omiga_m)

    
    alpha=10;%x轴方向收敛速度
    beta=5;%y轴方向收敛速度
    L_s=150; %椭圆长轴，迈步步长
    H_s=100; %椭圆短轴，最大抬腿高度
    %omiga=pi;% 振荡器频率，越小收敛的越快
    
    
    x1=value(1,1);%相对于中心 x_p0 的坐标
    y1=value(1,2);%相对于中心 y_p0 的坐标
    x2=value(2,1);%相对于中心 x_p0 的坐标
    y2=value(2,2);%相对于中心 y_p0 的坐标
    x3=value(3,1);%相对于中心 x_p0 的坐标
    y3=value(3,2);%相对于中心 y_p0 的坐标
    x4=value(4,1);%相对于中心 x_p0 的坐标
    y4=value(4,2);%相对于中心 y_p0 的坐标
    
    xxx=[x1,x2,x3,x4];
    yyy=[y1,y2,y3,y4];
    for j=1:1:4
        thetax(j)=0;
        thetay(j)=0;
        for k=1:1:4
            if j~=k
%               thetax(j)=thetax(j)+phi_ij(1,k)*xxx(k)-phi_ij(2,k)*yyy(k);
%               thetay(j)=thetay(j)+phi_ij(2,k)*xxx(k)+phi_ij(1,k)*yyy(k);
                thetax(j)=thetax(j)+cos(phi_ij(j)-phi_ij(k))*xxx(k)-sin(phi_ij(j)-phi_ij(k))*yyy(k);
                thetay(j)=thetay(j)+sin(phi_ij(j)-phi_ij(k))*xxx(k)+cos(phi_ij(j)-phi_ij(k))*yyy(k);
            end
        end
    end
    

C=[0 -1 1 -1;-1 0 -1 1;-1 1 0 -1;1 -1 -1 0]*[y1;y2;y3;y4];

    omiga=omiga_m(1);
%     omiga=omiga1;
    %---------------
    yy1(1,1)=alpha*(1-4*x1*x1/L_s/L_s-y1*y1/H_s/H_s)*x1-omiga*L_s/H_s/2*y1 +thetax(1);
    yy1(1,2)=beta*(1-4*x1*x1/L_s/L_s-y1*y1/H_s/H_s)*y1+omiga*2*H_s/L_s*x1 +thetay(1);
    
    
    omiga=omiga_m(2);
% omiga=omiga2;
    %---------------
    yy2(1,1)=alpha*(1-4*x2*x2/L_s/L_s-y2*y2/H_s/H_s)*x2-omiga*L_s/H_s/2*y2 +thetax(2);
    yy2(1,2)=beta*(1-4*x2*x2/L_s/L_s-y2*y2/H_s/H_s)*y2+omiga*2*H_s/L_s*x2+thetay(2);
   
    omiga=omiga_m(3);
% omiga=omiga3;
    %---------------
    yy3(1,1)=alpha*(1-4*x3*x3/L_s/L_s-y3*y3/H_s/H_s)*x3-omiga*L_s/H_s/2*y3 +thetax(3);
    yy3(1,2)=beta*(1-4*x3*x3/L_s/L_s-y3*y3/H_s/H_s)*y3+omiga*2*H_s/L_s*x3 +thetay(3);
    
    omiga=omiga_m(4);
% omiga=omiga4;
    %---------------
    yy4(1,1)=alpha*(1-4*x4*x4/L_s/L_s-y4*y4/H_s/H_s)*x4-omiga*L_s/H_s/2*y4 +thetax(4);
    yy4(1,2)=beta*(1-4*x4*x4/L_s/L_s-y4*y4/H_s/H_s)*y4+omiga*2*H_s/L_s*x4 +thetay(4);
    
    yy=[yy1;yy2;yy3;yy4];
end



%%调用举例
% [T1,F1]=runge_kutta(@equation,[0;0.2],0.01,0,10);
% plot(F1(:,1),F1(:,2),'.')



