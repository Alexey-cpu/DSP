// A.M. Tykvinsky, 19.10.2020

#ifndef _QUAD_MLTPX_HPP
#define _QUAD_MLTPX_HPP

// фильтры:
#include "aperiodic_filt.hpp"
#include "FIR_filt.hpp"

// функциональные блоки:
#include "cplx_operations.hpp"
#include "mirror_ring_buff_x32.hpp"
#include "special_functions.hpp"

// ВЫЧИСЛИТЕЛЬ РЗиА НА БАЗЕ КВАДРАТУРНОГО УМНОЖИТЕЛЯ

class quad_mltpx
{

private:

	double m_Fs;                    // частота дискретизации выборки (внутри шага устройства 1/0,004 = 250 Гц)
	double m_Ts;                   // период дискретизации выборки
	double m_Fn;                  // номинальная частота
	int    m_HBuffSize;          // размер входного полубуффера
	int    m_SamplesPerPeriod;  // к - во точек на период входного сигнала

	// промежуточные переменные:
	float m_a;     // промужуточная переменная для расчета квадратурной компоненты по оси X
	float m_b;     // промужуточная переменная для расчета квадратурной компоненты по оси Y
	float m_K;     // промежуточный коэффициент для расчета скольжения
	float m_Km;    // коэффициент компенсации амплитудного искажения
	
	// коэффициенты компенсации фазового и амплитудного искажения фильтра:
	
	/* АЧХ входного фильтра и фильтра квадратурных компонент в рабочем диапазоне частот 
	   аппроксимируются параболой y(x) = A*x*x + B*x + C, коэффициенты которой рассчитываются
	   автоматически ( см. реализацию функции allocate() в файле .cpp ) */
	
	// коэффициенты кривой компенсации амплитудного искажения входного фильтра апериодики:
	float m_A1;
	float m_B1;
	float m_C1;
	
	// коэффициенты кривой компенсации амплитудного искажения фильтра квадратурных компонент:
	float m_A2;
	float m_B2;
	float m_C2;	

	// переменные опорных сигналов:
	int      m_cnt;      // счетчик отсчетов опорных сигналов
	int      m_ElemNum; // число отсчетов на период опорного сигнала минус 1 отсчет (см. cpp файл )
	
	int      m_order_eq;

public:

	special_functions SPEC_FCN;

	// указатели на массивы с опорным синусом и косинусом:
	float *m_BUFF_COS;
	float *m_BUFF_SIN;

	// буфферы:
	mirror_ring_buff m_BUFF_Re;
	mirror_ring_buff m_BUFF_Im;

	mirror_ring_buff m_BUFF_out1;
	mirror_ring_buff m_BUFF_out2;
	
	// фильтры:
	aperiodic_filt m_APFILT; // фильтр апериодической слагающей
	FIR_filt       m_FIR_Re; // фильтр квадратурной компоненты по оси X
	FIR_filt       m_FIR_Im; // фильтр квадратурной компоненты по оси Y
	FIR_filt       m_FIR_dF; // фильтр для сглаживания скольжения
	
	// блок дл¤ операций с комплексными числами:
	cplx_operations m_CPLX_OPERATIONS;

	// выходные переменные блока:
	double m_Re;      // текущее значение орт.составл¤ющей по оси X
	double m_Im;     // текущее значение орт.составл¤ющей по оси Y
	double m_F;     // текущее значение частоты
	double m_dF;   // текущее значение скольжения
	
	// функция вычисления:
	int process(double *input , double intChanDelay);

	// функци¤ инициализации вычислетел¤:
	int procInit(double Fs, double Fn, double SideLobeAtten, int FiltWindowWidth , int CycleWidth);

	// конструктор по умолчанию
	quad_mltpx();

	// деструктор
	~quad_mltpx();

	// функци¤ выделени¤ пам¤ти:
	int allocate();

	// функци¤ освобождени¤ пам¤ти:
	int deallocate();

};

#endif
