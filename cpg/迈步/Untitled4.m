k1=equation(t(ii),ym(ii,:),omiga1);
k2=equation(t(ii)+h/2,ym(ii,:)+h*k1/2,omiga1);
k3=equation(t(ii)+h/2,ym(ii,:)+h*k2/2,omiga1);
k4=equation(t(ii)+h,ym(ii,:)+h*k3,omiga1);
ym(ii+1,:)=ym(ii,:)+h*(k1+2*k2+2*k3+k4)/6;