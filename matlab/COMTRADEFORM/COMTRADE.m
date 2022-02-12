classdef COMTRADE
    %   Класс COMTRADE, служащий для создания файлов .cfg и .dat
    properties
        Fnetwork;   %базовая частота сети
        FileName;   %имя файла
    end
    methods (Static)
            %Функция формирования .cfg-файла
            function CfgFilePrint(FileName,Fnetwork,Fsampl,Nsampl,SIGNALS)
                %Создаем пустой текстовый файл с расширением .cfg
                dlmwrite(strcat(FileName,'.cfg'),'');
                if (nargin>0)%если число входных переменных не равно нулю, то:
                NASIG=0;%число аналоговых сигналов
                NDSIG=0;%число дискретных сигналов
                n    =0;%счетчик номеров сигналов
                NUM  =zeros(length(SIGNALS),1);
                %Расчет количества аналогов и дискретов:
                for i=1:length(SIGNALS)
                    if SIGNALS(1,i).SIGTYPE=='A'%если сигнал аналоговый, то:
                        NASIG=NASIG+1;
                        n=n+1;
                        NUM(i,1)=n;
                    end
                    
                    if SIGNALS(1,i).SIGTYPE=='D'%если сигнал дискретный, то:
                        NDSIG=NDSIG+1;
                        n=n+1;
                        NUM(i,1)=n;
                    end
                end
                %Определяем общее число сигналов, число дискретных и число
                %аналоговых сигналов:
                Ntotal = num2str(NASIG+NDSIG);
                NASIG  = strcat(num2str(NASIG),'A');
                NDSIG  = strcat(num2str(NDSIG),'D');
                %Пишем название регистратора (версия симулинк+релиз)в
                %пустой текстовый файл формата .cfg:
                MatLabVersion=ver('Simulink');
                StationID=strcat(MatLabVersion.Name,'_',MatLabVersion.Version,...
                    MatLabVersion.Release);
                file=fopen(strcat(FileName,'.cfg'),'wt');
                fprintf(file,'%s\n',strcat(StationID , ','  ,'1'));
                fprintf(file,'%s\n',strcat(Ntotal,',',NASIG,',',NDSIG));
                %Записываем в пустой тектовый файл с расширением .cfg
                %информацию по сигналам
                for i=1:length(SIGNALS)
                    if SIGNALS(1,i).SIGTYPE=='A'
                    fprintf(file,strcat(num2str(NUM(i,1)) , ',')); 
                    fprintf(file,strcat(SIGNALS(1,i).SIGID , ','));
                    fprintf(file,strcat(SIGNALS(1,i).SIGPH , ','));
                    fprintf(file,strcat(SIGNALS(1,i).SIGCC , ','));
                    fprintf(file,strcat(SIGNALS(1,i).SIGUU , ','));
                    fprintf(file,strcat(SIGNALS(1,i).SIGA , ','));
                    fprintf(file,strcat(SIGNALS(1,i).SIGB , ','));
                    fprintf(file,strcat(SIGNALS(1,i).SIGSKEW, ','));
                    fprintf(file,strcat(SIGNALS(1,i).SIGMIN), ',');
                    fprintf(file,strcat(',',SIGNALS(1,i).SIGMAX));
                    fprintf(file,'\n');
                    end
                    if SIGNALS(1,i).SIGTYPE=='D'
                    fprintf(file,strcat(num2str(NUM(i,1)) , ','));
                    fprintf(file,strcat(SIGNALS(1,i).SIGID , ','));
                    fprintf(file,strcat(SIGNALS(1,i).SIGM));
                    fprintf(file,'\n');
                    end
                end
                %Частота дискретизации, количество замеров, время и.т.д
                fprintf(file,Fnetwork);%частота сети
                fprintf(file,'\n');
                fprintf(file,'1');%колличество частот дискретизации
                fprintf(file,'\n');
                fprintf(file,strcat(Fsampl,',',Nsampl));%запись значений частоты дискретизации
                fprintf(file,'\n');
                fprintf(file,'19/10/2016,10:41:04.000');%время начала записи
                fprintf(file,'\n');
                fprintf(file,'19/10/2016,10:41:05.000');%время окончания записи
                fprintf(file,'\n');
                fprintf(file,'ASCII');%описание формата
                fclose('all');%закрываем все файлы
                end
            end
         %Формирование .dat-файла:
        function DatFilePrint(SIGNALS,Time,FileName)
           
            dlmwrite(strcat(FileName,'.dat'),'');

            if (nargin>0)
            wdt  = length(SIGNALS);
            lng  = length(SIGNALS(1).SIGDATA);
            Data = zeros(lng, wdt);
            
            for i=1:wdt
                Data(:,i) = SIGNALS(i).SIGDATA;  
            end
            
            Number = transpose(1:lng);

            datfile = [Number, Time, Data];
            
            format = '';
            n = 1;
            
            for i=1:wdt+2
                
                if i==wdt+2 && SIGNALS(n).SIGTYPE == 'A'
                format = strcat(format,'%.6f','\n');
                end
                
                if i==wdt+2 && SIGNALS(n).SIGTYPE == 'D'
                format = strcat(format,'%.0f','\n');
                end
                
                if i==1
                format = strcat(format,'%.0f',',');
                end
                
                if i==2
                format = strcat(format,'%.0f',',');
                end
                
                if i>2 && i < wdt+2 && n < wdt
                    
                    if SIGNALS(n).SIGTYPE == 'A'
                    format = strcat(format,'%.6f',',');
                    n = n + 1;
                    end
                    
                    if SIGNALS(n).SIGTYPE == 'D'
                    format = strcat(format,'%.0f',',');
                    n = n + 1;
                    end
                    
                end
                
            end
            file = fopen(strcat(FileName,'.dat'),'wt');
            fprintf(file, format, transpose(datfile));
            fclose('all');
            end
            
        end
        %Конструктор класса COMTRADE:
        function obj = COMTRADE(Fnetwork,FileName,SIGNALS)
            if (nargin>0)
            obj.Fnetwork = num2str(Fnetwork);
            obj.FileName = FileName;
            Fsampl = num2str(1000000/(SIGNALS(1).TIME(2,1)-SIGNALS(1).TIME(1,1)));
            Nsampl = num2str(length(SIGNALS(1).TIME));
            Time   = SIGNALS(1).TIME;
            COMTRADE.CfgFilePrint(FileName,obj.Fnetwork,Fsampl,Nsampl,SIGNALS)
            COMTRADE.DatFilePrint(SIGNALS,Time,FileName)
            end
        end
    end
end

