function AUTOMATIC_MODEL_RUNNING(Directory, RegistratorName, dt, T)

%���������� �������� ������:
cd(Directory);
file_attributes = dir('*t*.slx');
num_of_files    = length(file_attributes);

clc
%������� � ����� ��� ������ � ��������� ����������� � ������ COMTRADE:
for i = 1:num_of_files
cd(Directory);
file = strcat(file_attributes(i).folder,'\',file_attributes(i).name);
load_system(file);
disp(strcat('��������� ������ - ',file_attributes(i).name));
simOut = sim(file,'SimulationMode','accelerator','AbsTol', num2str(dt),...
            'StopTime', num2str(T) );
disp(strcat('����� ������ ������ - ',file_attributes(i).name));		

%�������� ����������� � ������ COMTRADE:
COMTRADEFORM(file_attributes(i).name,'kV','A',RegistratorName , simOut.DAT);
disp(strcat('�������� ������ � �������� ����������� ������� � ������ COMTRADE ������ - ',file_attributes(i).name));
%==================================================================================================

%��������� ����������� ��������:
bdclose(file);
disp(strcat('������ - ',file_attributes(i).name,'_','��������� �� ������'));

%������� ������ ������� �� ��������� ������ ������:
clearvars -except ALARModel_Directory COMTRADES_Directory1 COMTRADES_Directory2 file_attributes i num_of_files

disp(strcat('�������� - ',num2str(num_of_files-i),'_��_',num2str(num_of_files)));
end

disp('������ ��������!')

end

