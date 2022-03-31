close all; clear all; clc;
% 
% % serial object
% s = serial('COM3', 'BaudRate', 230400);
% % open the serial port
% fopen(s);
% % read data
% %     data = str2double(fscanf(s)); %potremmo utilizzare poi un ciclo while per plottare di continuo
% %     plot(data);
% while(1)
%     fgets(s) 
% end
% % close the serial port
% fclose(s);
%     
%     
%     
% fwrite(s,10)
% a=[13 10];
% caratteri=char(a);
% c=double(caratteri)

% char * temp;


space=' ';
mess="allora";
mess1=convertStringsToChars(mess); %%Provare semplicemente a convertire prima dell'invio
len=strlength(mess1);

 for i=1:len
    if i == 1
        temp = mess1(1);
    elseif i == len
        temp = [temp ' ' mess1(i) ' '];
    else
        temp = [temp ' ' mess1(i)];
    end
end
string=convertCharsToStrings(temp);


