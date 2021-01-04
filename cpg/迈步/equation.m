%% 函数定义

%此处函数为CPG震荡模型
function [yy]=equation(t,value,phi_ij,omiga_m)
    alpha=0.004;%x轴方向收敛速度
    u=10000; %圆的半径
    
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
%             if j~=k
%               thetax(j)=thetax(j)+phi_ij(1,k)*xxx(k)-phi_ij(2,k)*yyy(k);
%               thetay(j)=thetay(j)+phi_ij(2,k)*xxx(k)+phi_ij(1,k)*yyy(k);
                thetax(j)=thetax(j)+cos(phi_ij(j)-phi_ij(k))*xxx(k)-sin(phi_ij(j)-phi_ij(k))*yyy(k);
                thetay(j)=thetay(j)+sin(phi_ij(j)-phi_ij(k))*xxx(k)+cos(phi_ij(j)-phi_ij(k))*yyy(k);
%             end
        end
    end
    
%     for j=1:1:4
%         thetax(1)=thetax(1)+phi_ij(1,j)*xxx(j)-sin(phi_ij(j)-phi_ij(1))*yyy(j);
%         thetax(2)=thetax(2)+cos(phi_ij(j)-phi_ij(2))*xxx(j)-sin(phi_ij(j)-phi_ij(2))*yyy(j);
%         thetax(3)=thetax(3)+cos(phi_ij(j)-phi_ij(3))*xxx(j)-sin(phi_ij(j)-phi_ij(3))*yyy(j);
%         thetax(4)=thetax(4)+cos(phi_ij(j)-phi_ij(4))*xxx(j)-sin(phi_ij(j)-phi_ij(4))*yyy(j);
%         
%         thetay(1)=thetay(1)+sin(phi_ij(j)-phi_ij(1))*xxx(j)+cos(phi_ij(j)-phi_ij(1))*yyy(j);
%         thetay(2)=thetay(2)+sin(phi_ij(j)-phi_ij(2))*xxx(j)+cos(phi_ij(j)-phi_ij(2))*yyy(j);
%         thetay(3)=thetay(3)+sin(phi_ij(j)-phi_ij(3))*xxx(j)+cos(phi_ij(j)-phi_ij(3))*yyy(j);
%         thetay(4)=thetay(4)+sin(phi_ij(j)-phi_ij(4))*xxx(j)+cos(phi_ij(j)-phi_ij(4))*yyy(j);
%            
%         
%     end

    omiga=omiga_m(1);
    yy1(1,1)=alpha*(u-x1*x1-y1*y1)*x1-omiga*y1 + thetax(1);
    yy1(1,2)=alpha*(u-x1*x1-y1*y1)*y1+omiga*x1 +thetay(1);
    
    omiga=omiga_m(2);
    yy2(1,1)=alpha*(u-x2*x2-y2*y2)*x2-omiga*y2+thetax(2);
    yy2(1,2)=alpha*(u-x2*x2-y2*y2)*y2+omiga*x2+thetay(2);
    
    omiga=omiga_m(3);
    yy3(1,1)=alpha*(u-x3*x3-y3*y3)*x3-omiga*y3+thetax(3);
    yy3(1,2)=alpha*(u-x3*x3-y3*y3)*y3+omiga*x3+thetay(3);
    
    omiga=omiga_m(4);
    yy4(1,1)=alpha*(u-x4*x4-y4*y4)*x4-omiga*y4+thetax(4);
    yy4(1,2)=alpha*(u-x4*x4-y4*y4)*y4+omiga*x4+thetay(4);
    
    yy=[yy1;yy2;yy3;yy4];
end



%%调用举例
% [T1,F1]=runge_kutta(@equation,[0;0.2],0.01,0,10);
% plot(F1(:,1),F1(:,2),'.')



