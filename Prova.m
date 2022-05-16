% marker_temp = struct('N',0,'Lable', " ", 'Mag', 0, 'Phase', 0, 'Sample', 0);
% marker_saved = struct('N',0,'Lable', " ", 'Mag', 0, 'Phase', 0, 'Sample', 0);
% marker_temp.N = 1;
% marker_temp.Lable = "Ciao";
% marker_temp.Mag = 10;
% marker_temp.Phase = 2;
% marker_temp.Sample = 0.1;
% marker_2.N = 2;
% marker_2.Lable = "Allora";
% marker_2.Mag = 120;
% marker_2.Phase = 5;
% marker_2.Sample = 0.2;
% 
% N = marker_saved.N;
% Lable = marker_saved.Lable;
% Magnitude = marker_saved.Mag;
% Phase = marker_saved.Phase;
% Sample = marker_saved.Sample;
% 
% %marker_saved(1) = [];
% marker_saved = [marker_saved marker_temp]
% T = struct2table(marker_saved)
% %%marker_saved = [marker_saved marker_2];
% if((marker_saved(end).N) == 0 )
%    marker_saved(1) = []; 
% end
% 
% T = struct2table(marker_saved)
%     
% 
% t = linspace(350, 750);                                             % Create Data
% y = sin(2*pi*t/200 + 2) .* cos(2*pi*t/300)*0.2 + 0.2;               % Create Data
% ptchidx = (t >= 500) & (t <= 650);                                  % Area To Shade
% temp = (t >= 675) & (t <=725);
% ptchidx = [ptchidx; temp];
% figure(1)
% plot(t, y)
% hold on
% %patch([500 650 650 500], [ 0 0 [1 1]*max(ylim)], [0.6 0.4 0.9], 'FaceAlpha',0.3, 'EdgeColor','none')
% patch([t(ptchidx(0)) fliplr(t(ptchidx(1)))], [y(ptchidx(1)) zeros(size(y(ptchidx(1))))], [0.6 0.4 0.9], 'FaceAlpha',0.3, 'EdgeColor','none')
% hold off
% grid

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


