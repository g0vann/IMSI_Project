close all; clear all;

% serial object
s = serial('COM3', 'BaudRate', 115200);
% open the serial port
fopen(s);
% read data
    data = str2double(fscanf(s)); %potremmo utilizzare poi un ciclo while per plottare di continuo
    plot(data);
    
    
% close the serial port
fclose(s);
    
    
    

