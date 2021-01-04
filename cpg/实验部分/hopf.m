% 改进椭圆方程Hopf

function dydt=hopf(t,x_y)
x=x_y(1);
y=x_y(2);
alpha=5;%x轴方向收敛速度
beta=10;%y轴方向收敛速度
a=150;%椭圆长轴
b=100;%椭圆短轴
omiga=pi;% 振荡器频率，越小收敛的越快


dydt = [alpha*(1-x*x/a/a-y*y/b/b)*x+omiga*a/b*y;...
    beta*(1-x*x/a/a-y*y/b/b)*y-omiga*b/a*x]; 
end