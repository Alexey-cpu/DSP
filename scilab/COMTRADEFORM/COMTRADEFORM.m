function COMTRADEFORM(FileName , VoltageUnits , CurrentUnits , SIGA , SIGB , BusCreatorName , DAT)
%1.Формат записываемого файла в ПК Simulink для записи в формат COMTRADE:
%1.1.Запись данных в модели ПК Simulink должна делаться с использованием
%    блоков "BusCreator" и "To Workspace"
%1.2.Первым записывается время с заданной дискретизацией выборки 
%  "Sample Time"
%1.3.Запись остальных сигналов делается в произвольном порядке
%1.4.Дискретные переменные и сигналы должны быть переведены в формат
%    "double"
%1.5.Все сигналы, собираемые блоком "Bus Creator" должны иметь название,
%    формат которого описан ниже

%2.Формат наименованя измеряемых величин для их записи в формат COMTRADE:
%2.1.Названия дискретных сигналов начинаются с нижнего подчеркивания "_"

%2.2.Названия напряжений начинаются с U, далее следует наименование фазы с
%  маленькой буквы, далее - любые символы: "Ua" - напряжение фазы А.

%2.3.Названия токов начинаются с I, далее следует наименование фазы с
%  маленькой буквы, далее - любые символы: "Ia" - ток фазы А.


%FileName='Trial_8_1';
%Signal(SIGID,SIGPH,SIGCC,SIGUU,SIGA,SIGB,SIGSKEW,SIGM,SIGTYPE,SIGDATA,TIME)
ModelName = bdroot(gcb);
directory=strcat(ModelName,'/',BusCreatorName);
SigNames_cell    =get_param(directory,'InputSignalNames');
SigNames		 =string(SigNames_cell);
SigNames_char    =char(SigNames_cell);

%Атрибуты сигналов:
SigTypes=zeros(1,1);%тип сигнала (дискрет/аналог)
SigUnits=zeros(1,4);%единицы измерения сигнала В/А
SigPh   =zeros(1,1);%фаза сигнала (А,В,С)
[lng, wdt] = size(DAT);

for i=1:wdt
    %Узнаем тип сигнала:
    if SigNames_char(i,1)=='_'
	SigTypes(i,1) = 'D';
	else
	SigTypes(i,1) = 'A';
    end
    
    %Узнаем единицы измерения сигнала(только для аналогов!)
    if SigTypes(i,1)=='A' && SigNames_char(i,1)=='U'
    SigUnits(i,1:length(VoltageUnits))=VoltageUnits;
    else
        if SigTypes(i,1)=='A' && SigNames_char(i,1)=='I'
        SigUnits(i,1:length(CurrentUnits))=CurrentUnits;
        else
        SigUnits(i,:)=' ';
        end
    end
    
    %Узнаем фазу сигнала (только для аналогов!)    
    if SigTypes(i,1)=='A' && SigNames_char(i,2)=='a' && (SigNames_char(i,1)=='U' || SigNames_char(i,1)=='I')
    SigPh(i,1)='A';
    else
        if SigTypes(i,1)=='A' && SigNames_char(i,2)=='b'
        SigPh(i,1)='B';
        else
            if SigTypes(i,1)=='A' && SigNames_char(i,2)=='c'
            SigPh(i,1)='C';
            else
                SigPh(i,1)=' ';
            end
        end   
    end 
end
SigTypes=char(SigTypes);
SigUnits=char(SigUnits);
SigPh   =char(SigPh);
%Signal(SIGID,SIGPH,SIGCC,SIGUU,SIGA,SIGB,SIGSKEW,SIGM,SIGTYPE,SIGDATA,TIME)
TIME=DAT(:,1);
SIG=Signal(' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ');

for i=1:wdt
    if SigTypes(i,:)=='A'
    SIG(1,i)=Signal(SigNames(1,i) , SigPh(i,:) , ' ' , SigUnits(i,:) , SIGA , SIGB ,'0','0','A',DAT(:,i),TIME);
    end
    
    if SigTypes(i,:)=='D' && DAT(1,i)==0
    SIG(1,i)=Signal(SigNames(1,i) , SigPh(i,:) , ' ' , SigUnits(i,:) , '1' , '0' , '0' , '0' , 'D',DAT(:,i),TIME);
    end
    
    if SigTypes(i,:)=='D' && DAT(1,i)==1
    SIG(1,i)=Signal(SigNames(1,i),SigPh(i,:),' ',SigUnits(i,:),'1','0','0','1','D',DAT(:,i),TIME);
    end
    
end
SIG(:,1)=[];%Удяляем первый столбец, т.к. это время
COMTRADE(50,FileName,SIG);

end

