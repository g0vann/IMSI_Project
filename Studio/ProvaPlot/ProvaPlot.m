% Prova

clear all; clc;

s = serial('COM7','BaudRate',230400);


set(s,'FlowControl','Software'); 
% s.Terminator='CR';
s.BytesAvailableFcn = {@save_data};

global time
global mag
global sample_period
time = [];
mag = [];
sample_period = 1./10000;
fopen(s);

