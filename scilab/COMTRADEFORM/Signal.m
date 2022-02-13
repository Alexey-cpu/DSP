classdef Signal
    %   ����� - ������, �������� ���������� �� ������������������ �������
    properties(GetAccess=public, SetAccess=public)
        SIGID;  %��� �������
        SIGPH;  %���� �������
        SIGCC;  %�������� ������� � �������, ��������, ����� ����-2
        SIGUU;  %������� ��������� �������
        SIGA;   %����������� ���������� �������
        SIGB;   %����������� �������� �������
        SIGSKEW;%����������� ����������� ������� �������� ���
        SIGDATA;%������� �������� �������
        TIME;   %������� ������� ������ �������� ������� �������
        SIGMIN; %����������� �������� �������
        SIGMAX; %������������ �������� �������
        SIGTYPE;%��� �������
        SIGM;   %���������� ��������� ����������� ������� (0 ��� 1)
    end

    methods (Static)
        %����������� ������� ���� "Signal"
        %========================================================================================
        function obj = Signal(SIGID,SIGPH,SIGCC,SIGUU,SIGA,SIGB,SIGSKEW,SIGM,SIGTYPE,SIGDATA,TIME)
            %���� ����� ������� ���������� �� ����� ����, ��:
            if (nargin>0)
                obj.SIGID   = num2str(SIGID);
                obj.SIGPH   = num2str(SIGPH);
                obj.SIGCC   = num2str(SIGCC);
                obj.SIGUU   = num2str(SIGUU);
                obj.SIGA    = num2str(SIGA);
                obj.SIGB    = num2str(SIGB);
                obj.SIGSKEW = num2str(SIGSKEW);
                obj.SIGDATA = SIGDATA;
                obj.TIME    = TIME*1000000;
                obj.SIGMIN  = num2str(min(SIGDATA));
                obj.SIGMAX  = num2str(max(SIGDATA));
                obj.SIGTYPE = num2str(SIGTYPE);
                obj.SIGM    = num2str(SIGM);
            end
        end
       %==========================================================================================
    end

end

