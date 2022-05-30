function [ciao1,timestamp] = ciao(src,evt)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
[ciao1, timestamp] = read(src, "oldest");
% data = char(ciao1);
% elemento = split(data);
% data =(elemento(1))+ " " + (elemento(2));% stacco cmq non va bene dobbiamo mandare la lunghezza o settare da firmware che devono essere mandati spazi fino alla chiusura della stringa
% data = char (data);
ciao2 = uint8(ciao1(2:5));
ciao3 = uint8(ciao1(6:9));
ciao4 = uint8(ciao1(10:13));
Afloat = typecast( ciao2, 'single');
Bfloat = typecast( ciao3, 'single');
Cfloat = typecast( ciao4, 'single');
disp([fix(Afloat) fix(Bfloat) fix(Cfloat)])

end

