clear all;
clc;

c=clock;
fix(c);
data=c;

hour=data(4);
minute = data(5);
second = fix(data(6));
time = num2str(hour)+":"+num2str(minute)+":"+num2str(second); 

% for i=1:10
%    t = clock;
%    fix(t);
%    data = [data; t];
%    pause(1);
% end
% hours = data(:,4);
% minutes = data(:,5);
% seconds = data(:,6);
% hours
% minutes
% seconds
