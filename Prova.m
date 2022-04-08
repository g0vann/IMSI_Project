y = [];
x = [];
k = 1;
for h=1:150
    k = k + 1;
    y = [y 3];
    x = [x k];
    
    plot(x,y);
    hold on;
    if((mod(5,k))==0)
        plot(x(end), y(end), '-o');
    end
    pause(0.5);
end
