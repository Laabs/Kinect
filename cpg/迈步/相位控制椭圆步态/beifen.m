A_x=P1_plot(num,1);
         A_y=40;
         A_z=P1_plot(num,2);

         % 1.连杆参数
        a_link=265;
        b_link=75;
        d_link=240;
        e_link=80;
        f_link=45;
        g_link=sqrt(e_link*e_link-f_link*f_link);
        gama_link=147/180*pi;
        Q=[-f_link;0;-g_link];

         
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

        A=[Q(1)-b_link*cos(beta_link);0;Q(3)+b_link*sin(beta_link)];
        B=[Q(1)+a_link*cos(alpha_link)-b_link*cos(beta_link);0;Q(3)-a_link*sin(alpha_link)+b_link*sin(beta_link)];
        D=[x;0;z];


        A=Rx*A;
        B=Rx*B;
        C=Rx*C;
        D=Rx*D;
        A_Q=Rx*Q;

        xx(num)=D(1);
        yy(num)=D(2);
        zz(num)=D(3);

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
        num=num+1;