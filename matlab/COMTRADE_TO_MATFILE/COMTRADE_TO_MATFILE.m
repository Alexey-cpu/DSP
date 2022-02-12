function [in_signal , coeffs , osc_duration] = COMTRADE_TO_MATFILE(filename , directory)

% %�������� ������:
% filename       = '01';
% directory      = 'E:\RelayProtection\DiffBus\osc';

%������ ����� cfg � dat ������;
cfg_file_name = strcat(directory,'\',filename,'.cfg');
dat_file_name = strcat(directory,'\',filename,'.dat');

%������ � cfg - ������:

%������ cfg - ����:
fileID = fopen(cfg_file_name);
cfg_file = fread(fileID,'*char')';
fclose(fileID);

%��������� �������� ���������� �����, ���������� � ���������� ��������:
str_num  = 1;
com_num  = 0;
NA       = '';
ND       = '';
asig_num = 0;
a        = '';
b        = '';

for n = 1:1:length(cfg_file)
    
    %������� ������� ������ � cfg - �����:
    if regexp(cfg_file(1,n), '\n', 'once')
    str_num = str_num + 1;
    end
    
    %�� ������ ������ ������� ������ � ����������� �������� � ���������:
    if str_num == 2
        
        %���������� ���������� ���������� �������� � �����:
        if cfg_file(1,n) == 'A'
        NA = strcat( cfg_file(1,n-2) , cfg_file(1,n-1) );
        NA = str2num(NA);
        end
        
        %���������� ���������� ���������� �������� � �����:
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
    
    
%������ � dat - ������:
%������ dat - ����:
fileID         = fopen( dat_file_name );
dat_file       = fread(fileID,'*char')';
dat_file       = str2num(dat_file);
fclose(fileID);

%���������� ������� ������������� �������:
Fs   = abs ( 1/( dat_file(4,2)*1e-6 - dat_file(5,2)*1e-6 ) );

%��������� ������ ������ �������:
time = zeros(length(dat_file) , 1);

for n = 1:1:length(dat_file)
    
    time(n,1) = 0 + n/Fs;
    
end

%�������� dat - �����:
dat_file = dat_file( : , 3:NA+2 );

%������������ ������� ���������� ��������:
for n = 1:1:NA
    
    if coeff(n,1)~=0 || coeff(n,2)~=0
    dat_file(:,n) = dat_file(:,n).*coeff(n,1) + coeff(n,2);
    end
    
end

osc_duration = time(length(time));

in_signal = [time , dat_file];
coeffs    = coeff;
disp(strcat('������� ������������� ��� �����' ,'_', num2str(Fs),'_','��') );
disp(strcat('������������ ������������� �����','_', num2str(time(length(time))), '_������' ) );

    

end

