%% Import the data
[~, ~, raw] = xlsread('C:\Users\Александр\YandexDisk\Защиты_генераторов\Осциллограммы\Model_10 2002_3002.xlsm','Лист1','A1:Z502');
raw(cellfun(@(x) ~isempty(x) && isnumeric(x) && isnan(x),raw)) = {''};
%% Replace non-numeric cells with NaN
R = cellfun(@(x) ~isnumeric(x) && ~islogical(x),raw); % Find non-numeric cells
raw(R) = {NaN}; % Replace non-numeric cells
data = reshape([raw{:}],size(raw)); %% Create output variable

%% Allocate imported array to column variable names
N = size(data,1);%Колличество точек
UsA.time = data(2:N,1);
UsA.signals.values = data(2:N,2);
UsB.time = data(2:N,1);
UsB.signals.values = data(2:N,3);
UsC.time = data(2:N,1);
UsC.signals.values = data(2:N,4); 
UA.time = data(2:N,1);
UA.signals.values = data(2:N,5);
UB.time = data(2:N,1);
UB.signals.values = data(2:N,6);
UC.time = data(2:N,1);
UC.signals.values = data(2:N,7); 
Ia.time = data(2:N,1);
Ia.signals.values = data(2:N,8); 
Ib.time = data(2:N,1);
Ib.signals.values = data(2:N,9); 
Ic.time = data(2:N,1);
Ic.signals.values = data(2:N,10);
If.time = data(2:N,1);
If.signals.values = data(2:N,11);
Ikz.time = data(2:N,1);
Ikz.signals.values = data(2:N,12);
Ina.time = data(2:N,1);
Ina.signals.values = data(2:N,13);
Inb.time = data(2:N,1);
Inb.signals.values = data(2:N,14);
Inc.time = data(2:N,1);
Inc.signals.values = data(2:N,15);
In1n2.time = data(2:N,1);
In1n2.signals.values = data(2:N,16);
Ig.time = data(2:N,1);
Ig.signals.values = data(2:N,17);
Un.time = data(2:N,1);
Un.signals.values = data(2:N,18);
%% Clear temporary variables
clearvars data raw R;
