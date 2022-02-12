classdef COMTRADE
    %   ����� COMTRADE, �������� ��� �������� ������ .cfg � .dat
    properties
        Fnetwork;   %������� ������� ����
        FileName;   %��� �����
    end
    methods (Static)
            %������� ������������ .cfg-�����
            function CfgFilePrint(FileName,Fnetwork,Fsampl,Nsampl,SIGNALS)
                %������� ������ ��������� ���� � ����������� .cfg
                dlmwrite(strcat(FileName,'.cfg'),'');
                if (nargin>0)%���� ����� ������� ���������� �� ����� ����, ��:
                NASIG=0;%����� ���������� ��������
                NDSIG=0;%����� ���������� ��������
                n    =0;%������� ������� ��������
                NUM  =zeros(length(SIGNALS),1);
                %������ ���������� �������� � ���������:
                for i=1:length(SIGNALS)
                    if SIGNALS(1,i).SIGTYPE=='A'%���� ������ ����������, ��:
                        NASIG=NASIG+1;
                        n=n+1;
                        NUM(i,1)=n;
                    end
                    
                    if SIGNALS(1,i).SIGTYPE=='D'%���� ������ ����������, ��:
                        NDSIG=NDSIG+1;
                        n=n+1;
                        NUM(i,1)=n;
                    end
                end
                %���������� ����� ����� ��������, ����� ���������� � �����
                %���������� ��������:
                Ntotal = num2str(NASIG+NDSIG);
                NASIG  = strcat(num2str(NASIG),'A');
                NDSIG  = strcat(num2str(NDSIG),'D');
                %����� �������� ������������ (������ ��������+�����)�
                %������ ��������� ���� ������� .cfg:
                MatLabVersion=ver('Simulink');
                StationID=strcat(MatLabVersion.Name,'_',MatLabVersion.Version,...
                    MatLabVersion.Release);
                file=fopen(strcat(FileName,'.cfg'),'wt');
                fprintf(file,'%s\n',strcat(StationID , ','  ,'1'));
                fprintf(file,'%s\n',strcat(Ntotal,',',NASIG,',',NDSIG));
                %���������� � ������ �������� ���� � ����������� .cfg
                %���������� �� ��������
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
                %������� �������������, ���������� �������, ����� �.�.�
                fprintf(file,Fnetwork);%������� ����
                fprintf(file,'\n');
                fprintf(file,'1');%����������� ������ �������������
                fprintf(file,'\n');
                fprintf(file,strcat(Fsampl,',',Nsampl));%������ �������� ������� �������������
                fprintf(file,'\n');
                fprintf(file,'19/10/2016,10:41:04.000');%����� ������ ������
                fprintf(file,'\n');
                fprintf(file,'19/10/2016,10:41:05.000');%����� ��������� ������
                fprintf(file,'\n');
                fprintf(file,'ASCII');%�������� �������
                fclose('all');%��������� ��� �����
                end
            end
         %������������ .dat-�����:
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
        %����������� ������ COMTRADE:
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

