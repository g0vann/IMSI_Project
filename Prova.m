marker_temp = struct('N',0,'Lable', " ", 'Mag', 0, 'Phase', 0, 'Sample', 0);
marker_saved = struct('N',0,'Lable', " ", 'Mag', 0, 'Phase', 0, 'Sample', 0);
marker_temp.N = 1;
marker_temp.Lable = "Ciao";
marker_temp.Mag = 10;
marker_temp.Phase = 2;
marker_temp.Sample = 0.1;
marker_2.N = 2;
marker_2.Lable = "Allora";
marker_2.Mag = 120;
marker_2.Phase = 5;
marker_2.Sample = 0.2;

marker_saved = [marker_saved marker_temp];
marker_saved = [marker_saved marker_2];

N = marker_saved.N;
Lable = marker_saved.Lable;
Magnitude = marker_saved.Mag;
Phase = marker_saved.Phase;
Sample = marker_saved.Sample;

T = struct2table(marker_saved);

% app.marker_table=table(N,Lable,Magnitude,Phase,Sample);