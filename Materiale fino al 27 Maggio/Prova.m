% %% Media e Deviazione Standard di un vettore di k vettori di n elementi ciascuno
% %% Metodo 1
% a = [1 2 4 8 6 2 4 5 2 2 4 11]; %12 numeri
% 
% k = 3;      %numero cicli
% n = 4;      %numero punti per cliclo
% m = [];
% %mi aspetto il vettore media m =[3 2 4 8]
% for i = 1:(length(a)/k)
%     h = 0;
%     for j = 0 : (k-1)
%         h = h + a(i+(n*j));
%     end
%     h = h/k;
%     m = [m h];
%     
% end
% 
% 
% %% Media e Deviazione Standard di un vettore di k vettori di n elementi ciascuno
% %% Metodo 2
% a = [1 2 4 8 6 2 4 5 2 2 4 11]; %12 numeri
% 
% k = 3;      %numero cicli
% n = 4;      %numero punti per cliclo
% b = [];
% c = [];
% h = [];
% %mi aspetto il vettore media m =[3 2 4 8]
% for i = 1:(length(a)/k)
%     h = [];
%     for j = 0 : (k-1)
%         h = [h  a(i+(n*j))];
%     end
%     x = mean(h);
%     z = std(h);
%     b = [b x]; %vettore delle medie per ogni punto
%     c = [c z]; %vettore della deviazione standard punto per punto
%     v = -c;
%     
% end


%% Prova utilizzo ricezione/invio tramite bluetooth
% b = ble("FrancOlino_BLE");
% car = characteristic(b,"00005301-0000-0041-4C50-574953450000","00005303-0000-0041-4C50-574953450000");
% car2 = characteristic(b,"00005301-0000-0041-4C50-574953450000","00005302-0000-0041-4C50-574953450000"); 
% subscribe(car,"notification");
% car.DataAvailableFcn = @ciao;

%%Prova Boundline
% x = linspace(0, 2*pi, 50);
% y1 = sin(x);
% y2 = cos(x);
% e1 = rand(size(y1))*.5+.5;
% e2 = [.25 .5];
% 
% ax(1) = subplot(2,2,1);
% [l,p] = boundedline(x, y1, e1, '-b*', x, y2, e2, '--ro');
% outlinebounds(l,p);
% title('Opaque bounds, with outline');
% axis tight;
% 
% ax(2) = subplot(2,2,2);
% boundedline(x, [y1;y2], rand(length(y1),2,2)*.5+.5, 'alpha');
% title('Transparent bounds');
% axis tight;

x=[1 2 3 4 5 6 7 8 9 ]; %asse delle x
mean=[4 6 5 3 7 8 10 12 9]; %media
std1 = [1 2 1 1 2 2 3 1 2]; %deviazione standard
std2 = [-1 -2 -1 -1 -2 -2 -3 -1 -2];

boundedline(x, mean, std1,'-ro','transparency', 0.1);


















