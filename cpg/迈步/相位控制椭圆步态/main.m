
clc;
clear;

%-----------------------------------------------------相关参数------------------------------------
% 1.步态参数
lamda=0.25; %腿在空中占用的时间
zo=0;xo=0;zd=80;xd=150;yo=0;yd=0;
z_high=0;
%左下角起点位置
zm=-440;
xm=-80;
ym=40;

T=0.4;


beta=0.5;  %占空比(支撑相)

% 四条腿相位
phi_LF=0;
phi_RF=0.5;
phi_LH=beta;
phi_RH=beta-phi_RF;

%各腿相对于第一条腿的相位差1-4(LF、RF、LH、RH)
phi_ij=[2*pi*phi_LF,2*pi*phi_RF,2*pi*phi_LH,2*pi*phi_RH];


% 迈步开始坐标（相对于足端起点）
P1=[-0.2,0];
P2=[-2,0];
P3=[-4,0];
P4=[-6,0];
% 中心点坐标（相对于髋关节）
P1_m=[20,-440];
P2_m=[20,-440];
P3_m=[20,-440];
P4_m=[20,-440];


P1_plot=[];
P2_plot=[];
P3_plot=[];
P4_plot=[];

% 时间步长
h=0.01;

% 步态相关
Hs=0;% y落地点离地高
H=80/100;%步高
L=150/200;%步长


%---------------------------------------------------VREP初始化相关----------------------------------
vrep=remApi('remoteApi'); % using the prototype file (remoteApiProto.m)

vrep.simxFinish(-1); % 断开其他的连接

while 1
    clientID=vrep.simxStart('127.0.0.1',19999,true,true,5000,5);
    if (clientID>-1)  
        disp('连接成功');
        
        break;    
    else
         disp('连接失败') ;  
    end
    
     pause(0.05);  
end
 
%----------------------------------------------------------------------------------------

%--------------------------------------------------获取关节句柄------------------------------------
    vrep.simxAddStatusbarMessage(clientID ,'hello vrep , i am matlab ',vrep.simx_opmode_blocking);
    % 腿1
    [returncode1,motor11]=vrep.simxGetObjectHandle(clientID,'motor11',vrep.simx_opmode_blocking);
    [~,motor12]=vrep.simxGetObjectHandle(clientID,'motor12',vrep.simx_opmode_blocking);
    [~,motor13]=vrep.simxGetObjectHandle(clientID,'motor13',vrep.simx_opmode_blocking);
    % 腿2
    [~,motor21]=vrep.simxGetObjectHandle(clientID,'motor21',vrep.simx_opmode_blocking);
    [~,motor22]=vrep.simxGetObjectHandle(clientID,'motor22',vrep.simx_opmode_blocking);
    [~,motor23]=vrep.simxGetObjectHandle(clientID,'motor23',vrep.simx_opmode_blocking);
    % 腿3
    [~,motor31]=vrep.simxGetObjectHandle(clientID,'motor31',vrep.simx_opmode_blocking);
    [~,motor32]=vrep.simxGetObjectHandle(clientID,'motor32',vrep.simx_opmode_blocking);
    [~,motor33]=vrep.simxGetObjectHandle(clientID,'motor33',vrep.simx_opmode_blocking);
    % 腿4
    [~,motor41]=vrep.simxGetObjectHandle(clientID,'motor41',vrep.simx_opmode_blocking);
    [~,motor42]=vrep.simxGetObjectHandle(clientID,'motor42',vrep.simx_opmode_blocking);
    [~,motor43]=vrep.simxGetObjectHandle(clientID,'motor43',vrep.simx_opmode_blocking);
    pause(0.1);
  %-----------------------------------------------------------------------------------------    
    
  if (returncode1==vrep.simx_return_ok)
    disp('获取操作对象句柄');
    vrep.simxStartSimulation(clientID,vrep.simx_opmode_oneshot);
     while 1
                tstep = 0.01;  % 设置模拟的步长为5ms/次,与verp中的设置为一样，实现同步控制
                vrep.simxSetFloatingParameter(clientID,vrep.sim_floatparam_simulation_time_step,tstep,vrep.simx_opmode_oneshot);%设置模拟步长
                recode=vrep.simxSynchronous(clientID,true);
                if(recode==vrep.simx_return_ok)
                    disp('使能同步成功');
    %                 vrep.simxSetJointPosition (clientID,motor1,-0.5,vrep.simx_opmode_oneshot );                   vrep.simxSetJointPosition (clientID,motor2,-0.2,vrep.simx_opmode_oneshot );
                    disp('电机初始化.....');
                     pause(2);
                   vrep.simxSynchronousTrigger(clientID);
                    break;
                else continue;
                end
     end

      %---------------------------------------------初始化复位---------------------------
%  %1  
       disp('初始化复位');
       %获取句柄
       [~,init_angle(1)]=vrep.simxGetJointPosition(clientID,motor11,vrep.simx_opmode_streaming);
       [~,init_angle(2)]=vrep.simxGetJointPosition(clientID,motor12,vrep.simx_opmode_streaming);
       [~,init_angle(3)]=vrep.simxGetJointPosition(clientID,motor13,vrep.simx_opmode_streaming);
%  %2  
       [~,init_angle(4)]=vrep.simxGetJointPosition(clientID,motor21,vrep.simx_opmode_streaming);
       [~,init_angle(5)]=vrep.simxGetJointPosition(clientID,motor22,vrep.simx_opmode_streaming);
       [~,init_angle(6)]=vrep.simxGetJointPosition(clientID,motor23,vrep.simx_opmode_streaming);

%  %3  
       [~,init_angle(7)]=vrep.simxGetJointPosition(clientID,motor31,vrep.simx_opmode_streaming);
       [~,init_angle(8)]=vrep.simxGetJointPosition(clientID,motor32,vrep.simx_opmode_streaming);
       [~,init_angle(9)]=vrep.simxGetJointPosition(clientID,motor33,vrep.simx_opmode_streaming);
%  %4
       [~,init_angle(10)]=vrep.simxGetJointPosition(clientID,motor41,vrep.simx_opmode_streaming);
       [~,init_angle(11)]=vrep.simxGetJointPosition(clientID,motor42,vrep.simx_opmode_streaming);
       [~,init_angle(12)]=vrep.simxGetJointPosition(clientID,motor43,vrep.simx_opmode_streaming);
       
       disp('11,12,13,21,22,23,31,32,33,41,42,43');
       disp(init_angle*180/pi); 
       disp('初始化复位完成');
%------------------------------------------------------------------------------------------------------------------------
  

        %引用步态规划函数
%         [Gx,Gz]=G(xo,xd,zo,zd,z_high,lamda,zm,xm); %0，60,0,50,0,0.25,-120,0
        
        
        % i参数和d参数
        % hdtheta=[0,0,0,0];
        % idtheta=[0,0,0,0];
        % hdphi=[0,0,0,0];
        % idphi=[0,0,0,0];

        % 初始位置差值
        angletheta=[-2.63,-2.63,-2.63,-2.63]*pi/180; % 向外为正 motor1
        anglephi=[10.3,10.3,10.3,10.3]*pi/180;% 向前为负 motor2
        anglegama=[-6.15,-6.15,-6.15,-6.15]*pi/180; %向下为正 motor3 -10.3


        % 句柄编号
        motor1=[motor11,motor21,motor31,motor41]; %
        motor2=[motor12,motor22,motor32,motor42]; %
        motor3=[motor13,motor23,motor33,motor43]; %
        %获取关节位置 v2 phi motor1
        for j=0:100
            for num=1:4
                [~,jointPositiontheta(num)]=vrep.simxGetJointPosition(clientID,motor1(num),vrep.simx_opmode_buffer);
                [~,jointPositionphi(num)]=vrep.simxGetJointPosition(clientID,motor2(num),vrep.simx_opmode_buffer);
                [~,jointPositiongama(num)]=vrep.simxGetJointPosition(clientID,motor3(num),vrep.simx_opmode_buffer);
            end
            disp(jointPositiontheta);
            disp(jointPositionphi);
            disp(jointPositiongama);
            pause(0.5);
            if(jointPositiontheta(1)<pi)
%                 if num==3 ||num==4
%                     jointPositiontheta=-jointPositiontheta;
%                 end    
                init_angle=[jointPositiontheta,jointPositionphi,jointPositiongama];
                break;
            end
        end

%         angle1=[0:0.05:pi/2,pi/2:-0.05:-pi/8,-pi/8:0.05:0]; % pid调节专用
        
%         ttt=1:1:length(angle1);
%         plot(ttt,angle1);
        inum=1;
        % motor1—theta，motor2—phi，motor3—gama
        num_cpg=1;
        
    while 1
       
        % 更新xi、yi
        ym=[P1;P2;P3;P4];
        % 更新xi、yi
        P1_plot=[P1_plot;L*P1(1)+P1_m(1),-H*P1(2)*(P1(2)<Hs)+P1_m(2)];
        P2_plot=[P2_plot;L*P2(1)+P2_m(1),-H*P2(2)*(P2(2)<Hs)+P2_m(2)];
        P3_plot=[P3_plot;L*P3(1)+P3_m(1),-H*P3(2)*(P3(2)<Hs)+P3_m(2)];
        P4_plot=[P4_plot;L*P4(1)+P4_m(1),-H*P4(2)*(P4(2)<Hs)+P4_m(2)];
        % 龙格库塔法更新[x,y]
        k1=equation(ym,phi_ij);
        k2=equation(ym+h*k1/2,phi_ij);
        k3=equation(ym+h*k2/2,phi_ij);
        k4=equation(ym+h*k3,phi_ij);
        P1=P1+h*(k1(1,:)+2*k2(1,:)+2*k3(1,:)+k4(1,:))/6;
        P2=P2+h*(k1(2,:)+2*k2(2,:)+2*k3(2,:)+k4(2,:))/6;
        P3=P3+h*(k1(3,:)+2*k2(3,:)+2*k3(3,:)+k4(3,:))/6;
        P4=P4+h*(k1(4,:)+2*k2(4,:)+2*k3(4,:)+k4(4,:))/6;
% 
%          [theta(1),phi(1),gama(1)]=anglesolve_Online(P1_plot(num_cpg,1),-0,P1_plot(num_cpg,2)); %motor 1
%          [theta(4),phi(4),gama(4)]=anglesolve_Online(P2_plot(num_cpg,1),-0,P2_plot(num_cpg,2)); %motor 4
%          [theta(2),phi(2),gama(2)]=anglesolve_Online(P3_plot(num_cpg,1),0,P3_plot(num_cpg,2)); %motor 2
%          [theta(3),phi(3),gama(3)]=anglesolve_Online(P4_plot(num_cpg,1),0,P4_plot(num_cpg,2)); %motor 3
         
         [theta(3),phi(3),gama(3)]=anglesolve_Online(P1_plot(num_cpg,1),-0,P1_plot(num_cpg,2)); %motor 1
         [theta(2),phi(2),gama(2)]=anglesolve_Online(P2_plot(num_cpg,1),-0,P2_plot(num_cpg,2)); %motor 4
         [theta(4),phi(4),gama(4)]=anglesolve_Online(P3_plot(num_cpg,1),0,P3_plot(num_cpg,2)); %motor 2
         [theta(1),phi(1),gama(1)]=anglesolve_Online(P4_plot(num_cpg,1),0,P4_plot(num_cpg,2)); %motor 3
        
         
         num_cpg=num_cpg+1;
         if num_cpg==300
               beta=0.5;  %占空比(支撑相)
                % 四条腿相位
                phi_LF=0;
                phi_RF=0.5;
                phi_LH=beta;
                phi_RH=beta-phi_RF;
                %各腿相对于第一条腿的相位差1-4(LF、RF、LH、RH)
                phi_ij=[2*pi*phi_LF,2*pi*phi_RF,2*pi*phi_LH,2*pi*phi_RH];
                disp('步态切换【0.75-0.5】');
                
         end
%         if inum > length(angle1)  % pid调节绘图
%             inum=1;
%             xxx=1:1:length(gama_plot1(:,1));
%             
%             % gama[1 - 4]
%             figure(1);
%             subplot(2,2,1);
%             plot(xxx,gama_plot1(:,1),'r',xxx,gama_plot2(:,1),'k');
%             
%             subplot(2,2,2);
%             plot(xxx,gama_plot1(:,2),'r',xxx,gama_plot2(:,2),'k');
%             
%             subplot(2,2,3);
%             plot(xxx,gama_plot1(:,3),'r',xxx,gama_plot2(:,3),'k');
%             
%             subplot(2,2,4);
%             plot(xxx,gama_plot1(:,4),'r',xxx,gama_plot2(:,4),'k');
%             
%             %phi[1 - 4] 
%             yyy=1:1:length(phi_plot1(:,1));
%             figure(2);
%             subplot(2,2,1);
%             plot(yyy,phi_plot1(:,1),'r',yyy,phi_plot2(:,1),'k');
%             
%             subplot(2,2,2);
%             plot(yyy,phi_plot1(:,2),'r',yyy,phi_plot2(:,2),'k');
%             
%             subplot(2,2,3);
%             plot(yyy,phi_plot1(:,3),'r',yyy,phi_plot2(:,3),'k');
%             
%             subplot(2,2,4);
%             plot(yyy,phi_plot1(:,4),'r',yyy,phi_plot2(:,4),'k');
%           
%               %theta[1 - 4]
%               figure(3);
%               zzz=1:1:length(theta_plot1(:,4));
%               subplot(2,2,1);
%               plot(xxx,theta_plot1(:,1),'r',xxx,theta_plot2(:,1),'k');
%               
%               subplot(2,2,2);
%               plot(xxx,theta_plot1(:,2),'r',xxx,theta_plot2(:,2),'k');
%               
%               subplot(2,2,3);
%               plot(xxx,theta_plot1(:,3),'r',xxx,theta_plot2(:,3),'k');
%               
%               subplot(2,2,4);
%               plot(xxx,theta_plot1(:,4),'r',xxx,theta_plot2(:,4),'k');
% 
%         end
        
            % 1.当前实际角度 ，motor1—theta，motor2—phi，motor3—gama，
            for num=1:4
                [~,jointPositiontheta(num)]=vrep.simxGetJointPosition(clientID,motor1(num),vrep.simx_opmode_buffer);
                [~,jointPositionphi(num)]=vrep.simxGetJointPosition(clientID,motor2(num),vrep.simx_opmode_buffer);
                [~,jointPositiongama(num)]=vrep.simxGetJointPosition(clientID,motor3(num),vrep.simx_opmode_buffer);
            end

%  pid 调参专用
%             theta=[-angle1(inum),-angle1(inum),angle1(inum),angle1(inum)]; %向右为正，电机向上为正
%             phi=[angle1(inum),angle1(inum),angle1(inum),angle1(inum)]; %向右为正，电机向上为正
%             gama=[angle1(inum),angle1(inum),angle1(inum),angle1(inum)]; %向下为正，电机向下为正
            
            % 3.计算角度偏差和速度
            for num=1:4
              % 角度偏差
              dtheta=theta(num)+angletheta(num)-jointPositiontheta(num)-init_angle(num);
              dphi=phi(num)-anglephi(num)-jointPositionphi(num)-init_angle(num+4);
              dgama=gama(num)-anglegama(num)-jointPositiongama(num)-init_angle(num+8);
              
              % 速度值
              v1(num)=10*dtheta;%10  【Bullet、Newton 物理引擎】
              v2(num)=4.2*dphi;%4.2  【Bullet、Newton 物理引擎】
              v3(num)=6*dgama;%6     【Bullet、Newton 物理引擎】
              
              % 存取值绘图 pid 调参专用
%               theta_plot1(inum,num)=theta(num);
%               theta_plot2(inum,num)=jointPositiontheta(num)-angletheta(num)+init_angle(num);
%               gama_plot1(inum,num)=gama(num);
%               gama_plot2(inum,num)=jointPositiongama(num)+anglegama(num)+init_angle(num+8);
%               phi_plot1(inum,num)=phi(num);
%               phi_plot2(inum,num)=jointPositionphi(num)+anglephi(num)+init_angle(num+4);
              
            end

            % 4.发送速度
            for num=1:4
              vrep.simxSetJointTargetVelocity(clientID,motor1(num),v1(num),vrep.simx_opmode_oneshot);% 电机1
              vrep.simxSetJointTargetVelocity(clientID,motor2(num),v2(num),vrep.simx_opmode_oneshot);% 电机2
              vrep.simxSetJointTargetVelocity(clientID,motor3(num),v3(num),vrep.simx_opmode_oneshot);% 电机3
            end
            
           inum=inum+1;
%            pause(0.01);
           vrep.simxSynchronousTrigger(clientID);
    end
    
%------------------------------------------------------------------------------------------------------------       
else 
    disp('获取操作对象句柄失败，按任意键退出 ')
    pause; 
end

vrep.simxStopSimulation(clientID,vrep.simx_opmode_oneshot_wait);

vrep.simxFinish(clientID);

vrep.delete(); % call the destructor!




