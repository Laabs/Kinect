function [s1,v1,a1]=fh_avmax(sb,v_max,a_max,h,s,v)
a=fhan(s-sb,v,a_max,h);
if v>=v_max
    v=v_max;
    if(a>0)
        a=0;
    end
end

if v<=-v_max
    v=-v_max;
    if a<0
        a=0;
    end
end
s1=s+h*v;
v1=v+h*a;
a1=a;
end

