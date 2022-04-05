% function save_data(s,~)
% %UNTITLED5 Summary of this function goes here
% global time
% global mag
% global sample_period
% %   Detailed explanation goes here
% 
% data = fscanf(s);
% data_mag=str2double(char(data(1:10)));
% disp(data_mag);
% if isempty(time)
%     time = [time 0];
% else
%     time = [time (time(end) + sample_period)];
% end
% mag=[mag data_mag];
% 
% if length(time) > 1
%     plot(time, mag);
% end
% end

