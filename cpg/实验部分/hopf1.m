%% ±ê×¼Hopf

function dydt=hopf(t,x_y)
x=x_y(1);
y=x_y(2);
alpha=5;
beta=10;
omiga=pi;


dydt = [alpha*(21-sqrt(x*x+y*y))*x+omiga*y;...
    beta*(21-sqrt(x*x+y*y))*y-omiga*x]; 
end