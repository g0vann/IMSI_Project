close all; clear all;

% serial object
s = serial('COM3', 'BaudRate', 230400);
% open the serial port
fopen(s);
% read data
%     data = str2double(fscanf(s)); %potremmo utilizzare poi un ciclo while per plottare di continuo
%     plot(data);
while(1)
    fgets(s) 
end
% close the serial port
fclose(s);
    
    
    
fwrite(s,10)
