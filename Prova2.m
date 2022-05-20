clear
clc


% ciao1 = char("154.000");
% ciao2 = char("4561.2004");
% data = ciao1+ " " + ciao2;
% data1 = char(data);

ciao1 = [5 6 7 898 54 654 0 0 0 56 54];
ciao2 = uint8(ciao1(2:5));
Afloat = typecast( ciao2, 'single');