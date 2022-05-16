%% Media e Deviazione Standard di un vettore di k vettori di n elementi ciascuno
%% Metodo 1
a = [1 2 4 8 6 2 4 5 2 2 4 11]; %12 numeri

k = 3;      %numero cicli
n = 4;      %numero punti per cliclo
m = [];
%mi aspetto il vettore media m =[3 2 4 8]
for i = 1:(length(a)/k)
    h = 0;
    for j = 0 : (k-1)
        h = h + a(i+(n*j));
    end
    h = h/k;
    m = [m h];
    
end


%% Media e Deviazione Standard di un vettore di k vettori di n elementi ciascuno
%% Metodo 2
a = [1 2 4 8 6 2 4 5 2 2 4 11]; %12 numeri

k = 3;      %numero cicli
n = 4;      %numero punti per cliclo
b = [];
c = [];
h = [];
%mi aspetto il vettore media m =[3 2 4 8]
for i = 1:(length(a)/k)
    h = [];
    for j = 0 : (k-1)
        h = [h  a(i+(n*j))];
    end
    x = mean(h);
    z = std(h);
    b = [b x]; %vettore delle medie per ogni punto
    c = [c z]; %vettore della deviazione standard punto per punto
    v = -c;
    
end