function AUTOMATIC_MODEL_RUNNING(Directory, RegistratorName, dt, T)

%Подгружаем атрибуты файлов:
cd(Directory);
file_attributes = dir('*t*.slx');
num_of_files    = length(file_attributes);

clc
%Считаем в цикле все модели с выгрузкой результатов в формат COMTRADE:
for i = 1:num_of_files
cd(Directory);
file = strcat(file_attributes(i).folder,'\',file_attributes(i).name);
load_system(file);
disp(strcat('Загружена модель - ',file_attributes(i).name));
simOut = sim(file,'SimulationMode','accelerator','AbsTol', num2str(dt),...
            'StopTime', num2str(T) );
disp(strcat('Начат расчет модели - ',file_attributes(i).name));		

%Выгрузка результатов в формат COMTRADE:
COMTRADEFORM(file_attributes(i).name,'kV','A',RegistratorName , simOut.DAT);
disp(strcat('Закончен расчет и выгрузка резальтатов расчета в формат COMTRADE модели - ',file_attributes(i).name));
%==================================================================================================

%Закрываем посчитанную модельку:
bdclose(file);
disp(strcat('Модель - ',file_attributes(i).name,'_','выгружена из памяти'));

%Удаляем лишние массивы во избежание утечек памяти:
clearvars -except ALARModel_Directory COMTRADES_Directory1 COMTRADES_Directory2 file_attributes i num_of_files

disp(strcat('Осталось - ',num2str(num_of_files-i),'_из·_',num2str(num_of_files)));
end

disp('Расчет завершен!')

end

