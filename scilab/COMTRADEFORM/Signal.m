classdef Signal
    %   Класс - сигнал, хранящий информацию об осциллографируемом сигнале
    properties(GetAccess=public, SetAccess=public)
        SIGID;  %имя сигнала
        SIGPH;  %фаза сигнала
        SIGCC;  %привязка сигнала к объекту, например, линии МКПА-2
        SIGUU;  %единицы измерения сигнала
        SIGA;   %коэффициент калибровки сигнала
        SIGB;   %коэффициент смещения сигнала
        SIGSKEW;%коэффициент компенсации фазовой задержки АЦП
        SIGDATA;%выборка значений сигнала
        TIME;   %моменты времени снятия значений выборки сигнала
        SIGMIN; %минимальное значение сигнала
        SIGMAX; %максимальное значение сигнала
        SIGTYPE;%тип сигнала
        SIGM;   %нормальное состояние дискретного сигнала (0 или 1)
    end

    methods (Static)
        %Конструктор объекта типа "Signal"
        %========================================================================================
        function obj = Signal(SIGID,SIGPH,SIGCC,SIGUU,SIGA,SIGB,SIGSKEW,SIGM,SIGTYPE,SIGDATA,TIME)
            %Если число входных аргументов не равно нулю, то:
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

