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
    

t = linspace(350, 750);                                             % Create Data
y = sin(2*pi*t/200 + 2) .* cos(2*pi*t/300)*0.2 + 0.2;               % Create Data
ptchidx = (t >= 500) & (t <= 650);                                  % Area To Shade
figure(1)
plot(t, y)
hold on
patch([500 650 650 500], [ 0 0 [1 1]*max(ylim)], [0.6 0.4 0.9], 'FaceAlpha',0.3, 'EdgeColor','none')
%patch([t(ptchidx) fliplr(t(ptchidx))], [y(ptchidx) zeros(size(y(ptchidx)))], [0.6 0.4 0.9], 'FaceAlpha',0.3, 'EdgeColor','none')
hold off
grid


    



