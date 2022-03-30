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
stringa="ciao";
len=strlength(stringa);
len=len+len;

% for i=0 to len
%     if i==0
%         temp=stringa();
%     end

