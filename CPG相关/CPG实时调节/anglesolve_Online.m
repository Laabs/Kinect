% 在线步态（一个步长）
function [angle1,angle2,angle3]=anglesolve_Online(Gx,Gy,Gz)
        
% 1.连杆参数
        a_link=265;
        b_link=75;
        d_link=240;
        e_link=80;
        f_link=45;
        g_link=sqrt(e_link*e_link-f_link*f_link);
        gama_link=147/180*pi;
        Q=[-f_link;0;-g_link];
         
% 2.赋值
         A_x=Gx;
         A_y=Gy;
         A_z=Gz;
         
         rox=atan2(A_y,-A_z); %【电机转角1】向外为负，向里为正
         Rx=[1 0 0;0 cos(rox) -sin(rox);0 sin(rox) cos(rox)];%旋转矩阵
         P_A=[A_x;A_y;A_z];
         P_B=Rx'*P_A; %P_A=Rx*P_B;
         x=P_B(1);
         y=P_B(2);
         z=P_B(3);
         
         QD=sqrt((x-Q(1))^2+(z-Q(3))^2);
         alpha_link=atan2(-(z-Q(3)),x-Q(1))+acos((a_link*a_link+QD*QD-d_link*d_link)/(2*a_link*QD)); %【电机转角2】恒为正
         C=[Q(1)+a_link*cos(alpha_link);0;Q(3)-a_link*sin(alpha_link)];
         beta_link=gama_link-acos((C(1)-x)/d_link);                         %【电机转角3】向上为正，向下为负
         angle1=rox;
         angle2=(alpha_link-pi/2);
         angle3=-beta_link;
end
