function [in_signal , coeffs , osc_duration] = COMTRADE_TO_MATFILE(filename , directory)

% %Исходные данные:
% filename       = '01';
% directory      = 'E:\RelayProtection\DiffBus\osc';

%Задаем имена cfg и dat файлов;
cfg_file_name = strcat(directory,'\',filename,'.cfg');
dat_file_name = strcat(directory,'\',filename,'.dat');

%Работа с cfg - файлом:

%Читаем cfg - файл:
fileID = fopen(cfg_file_name);
cfg_file = fread(fileID,'*char')';
fclose(fileID);

%Начальные значения количества строк, аналоговых и дискретных сигналов:
str_num  = 1;
com_num  = 0;
NA       = '';
ND       = '';
asig_num = 0;
a        = '';
b        = '';

for n = 1:1:length(cfg_file)
    
    %Считаем строчки текста в cfg - файле:
    if regexp(cfg_file(1,n), '\n', 'once')
    str_num = str_num + 1;
    end
    
    %По номеру строки находим строку с количеством аналогов и дискретов:
    if str_num == 2
        
        %Определяем количество аналоговых сигналов в файле:
        if cfg_file(1,n) == 'A'
        NA = strcat( cfg_file(1,n-2) , cfg_file(1,n-1) );
        NA = str2num(NA);
        end
        
        %Определяем количество дискретных сигналов в файле:
        if cfg_file(1,n) == 'D'
        ND = strcat( cfg_file(1,n-2) , cfg_file(1,n-1) );
        ND = str2num(ND);
        end
    end
    
    if str_num > 2
        
        start = n;
        break
        
    end
    
end

    coeff = zeros(NA,2);
    
    for n = start:1:length(cfg_file)
                
        if  str_num <= NA + 4

            if regexp(cfg_file(1,n), '\n', 'once')
            str_num = str_num + 1;
            com_num              = 0;
            a                    = '';
            b                    = '';
            asig_num = asig_num + 1;
            end  
            
            if cfg_file(1,n) == ','
                com_num = com_num + 1;
            end

            if com_num == 5 && cfg_file(1,n) ~= ','
                a = strcat( a , cfg_file(1,n) );
            end

            if com_num == 6 && cfg_file(1,n) ~= ','
            b = strcat( b , cfg_file(1,n) );
            end
            
            if com_num == 7
            coeff(asig_num , 1)  = str2double(a);
            coeff(asig_num , 2)  = str2double(b);
            end
            
        end
    
    end
    
    
%Работа с dat - файлом:
%Читаем dat - файл:
fileID         = fopen( dat_file_name );
dat_file       = fread(fileID,'*char')';
dat_file       = str2num(dat_file);
fclose(fileID);

%Определяем частоту дискретизации выборки:
Fs   = abs ( 1/( dat_file(4,2)*1e-6 - dat_file(5,2)*1e-6 ) );

%Формируем заново массив времени:
time = zeros(length(dat_file) , 1);

for n = 1:1:length(dat_file)
    
    time(n,1) = 0 + n/Fs;
    
end

%Урезание dat - файла:
dat_file = dat_file( : , 3:NA+2 );

%Корректируем выборку аналоговых сигналов:
for n = 1:1:NA
    
    if coeff(n,1)~=0 || coeff(n,2)~=0
    dat_file(:,n) = dat_file(:,n).*coeff(n,1) + coeff(n,2);
    end
    
end

osc_duration = time(length(time));

in_signal = [time , dat_file];
coeffs    = coeff;
disp(strcat('Частота дискретизации АЦП равна' ,'_', num2str(Fs),'_','Гц') );
disp(strcat('Длительность осциллограммы равна','_', num2str(time(length(time))), '_секунд' ) );

    

end

