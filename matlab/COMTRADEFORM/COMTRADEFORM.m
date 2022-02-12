function COMTRADEFORM(FileName , VoltageUnits , CurrentUnits , SIGA , SIGB , BusCreatorName , DAT)
%1.������ ������������� ����� � �� Simulink ��� ������ � ������ COMTRADE:
%1.1.������ ������ � ������ �� Simulink ������ �������� � ��������������
%    ������ "BusCreator" � "To Workspace"
%1.2.������ ������������ ����� � �������� �������������� ������� 
%  "Sample Time"
%1.3.������ ��������� �������� �������� � ������������ �������
%1.4.���������� ���������� � ������� ������ ���� ���������� � ������
%    "double"
%1.5.��� �������, ���������� ������ "Bus Creator" ������ ����� ��������,
%    ������ �������� ������ ����

%2.������ ����������� ���������� ������� ��� �� ������ � ������ COMTRADE:
%2.1.�������� ���������� �������� ���������� � ������� ������������� "_"

%2.2.�������� ���������� ���������� � U, ����� ������� ������������ ���� �
%  ��������� �����, ����� - ����� �������: "Ua" - ���������� ���� �.

%2.3.�������� ����� ���������� � I, ����� ������� ������������ ���� �
%  ��������� �����, ����� - ����� �������: "Ia" - ��� ���� �.


%FileName='Trial_8_1';
%Signal(SIGID,SIGPH,SIGCC,SIGUU,SIGA,SIGB,SIGSKEW,SIGM,SIGTYPE,SIGDATA,TIME)
ModelName = bdroot(gcb);
directory=strcat(ModelName,'/',BusCreatorName);
SigNames_cell    =get_param(directory,'InputSignalNames');
SigNames		 =string(SigNames_cell);
SigNames_char    =char(SigNames_cell);

%�������� ��������:
SigTypes=zeros(1,1);%��� ������� (�������/������)
SigUnits=zeros(1,4);%������� ��������� ������� �/�
SigPh   =zeros(1,1);%���� ������� (�,�,�)
[lng, wdt] = size(DAT);

for i=1:wdt
    %������ ��� �������:
    if SigNames_char(i,1)=='_'
	SigTypes(i,1) = 'D';
	else
	SigTypes(i,1) = 'A';
    end
    
    %������ ������� ��������� �������(������ ��� ��������!)
    if SigTypes(i,1)=='A' && SigNames_char(i,1)=='U'
    SigUnits(i,1:length(VoltageUnits))=VoltageUnits;
    else
        if SigTypes(i,1)=='A' && SigNames_char(i,1)=='I'
        SigUnits(i,1:length(CurrentUnits))=CurrentUnits;
        else
        SigUnits(i,:)=' ';
        end
    end
    
    %������ ���� ������� (������ ��� ��������!)    
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
SIG(:,1)=[];%������� ������ �������, �.�. ��� �����
COMTRADE(50,FileName,SIG);

end

