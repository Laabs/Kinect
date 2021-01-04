function dydt=hopf1(t,x_y)
x=x_y(1);
y=x_y(2);
alpha=5 ;% x轴方向收敛速度
omiga=pi;% 振荡器频率，越小收敛的越快
u=100;


dydt = [alpha*(u-x*x-y*y)*x-omiga*y;...
    alpha*(u-x*x-y*y)*y+omiga*x]; 
end