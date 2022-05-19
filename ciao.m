function [data,timestamp] = ciao(src,evt)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
[ciao1, timestamp] = read(src, "oldest");
data = char(ciao1);
elemento = split(data);
data =(elemento(1))+ " " + (elemento(2));% stacco cmq non va bene dobbiamo mandare la lunghezza o settare da firmware che devono essere mandati spazi fino alla chiusura della stringa
data = char (data);
disp(data)
end

