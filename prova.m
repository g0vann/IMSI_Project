% mag=[11 1 3 4 16 6 7 8 9 10];
% time = [10 9 8 7 6 5 4 3 2 1];
%    
% % [max, index] = min(mag)
% 
% if(isempty(mag) == 0)
%     x=100
% end


% %ciao = magic(5);
% 
% ciao(1,1) = "prova";
% ciao(1,2) = "ooo";
% ciao(2,1) = 1111;
% ciao(2,2) = 234;
% 
% 
% 
% 
% writetable(ciao,'C:\Users\giann\Desktop\Giovanni\01_Università\Magistrale\SecondoAnno\IMSI\IMSI_Project\Ciao.xlsx','Sheet',1);

 ciao = struct('N',0,'Lable'," ",'Freq',0);

ciao(1).N = 1;
ciao(1).Lable = "primo";
ciao(1).Freq = 123.23;

ciao(2).N = 2;
ciao(2).Lable = "secondo";
ciao(2).Freq = 123.23;

tb = struct2table(ciao);
tb.Properties.VariableNames = ["Numero","CIAO","Aor"];

% colonna1 = ["luigi"; "mario"];
% colonna2 = [20; 30];
% ciao=table(colonna1,colonna2,'VariableNames',{'Nome','Età'});