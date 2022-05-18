function [data,timestamp] = ciao(src,evt)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
[ciao1, timestamp] = read(src, "oldest");
data = char(ciao1);
data = data(3:20);
disp(data)
end

