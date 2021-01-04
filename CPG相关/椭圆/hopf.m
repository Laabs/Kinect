% 改进椭圆方程Hopf

function dydt=hopf(t,x_y)
x=x_y(1);
y=x_y(2);
alpha=5;%x轴方向收敛速度
beta=10;%y轴方向收敛速度
L_s=150;%椭圆长轴
H_s=100;%椭圆短轴
omiga=pi;% 振荡器频率，越小收敛的越快
b_p=2; %摆动相和支撑相的切换速度

D_f=0.75;
V_f=15;
% omiga=pi*V_f/L_s*(D_f/(1-D_f)*)
omiga_swing=pi*(1-D_f)*V_f/L_s;
omiga_stance=pi*D_f*V_f/L_s;

dydt = [alpha*(1-x*x/L_s/L_s-y*y/H_s/H_s)*x+omiga*L_s/H_s*y;...
    beta*(1-x*x/L_s/L_s-y*y/H_s/H_s)*y-omiga*H_s/L_s*x]; 

omiga=omiga_stance/(1+exp(-b_p*y))+omiga_swing/(1+exp(b_p*y));

end