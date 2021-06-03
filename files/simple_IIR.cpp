#include <iostream>
#include "simple_IIR.hpp"

// ПРОСТЕЙШИЕ ФИЛЬТРЫ

// конструктор:
//------------------------------------------------------------------------------
simple_IIR::simple_IIR() 
{
	// стандратные значения системных переменных класса:
	m_Fs = 4000;
	m_Fn = 50;
	m_T1 = 0.01;
	m_T2 = 0.01;
	m_Kd = 0.70;
	m_Fc = 120;
	m_N  = 2;
	m_out = 0;

	// инициализация буфферов:
	m_BUFF_SX.BuffInit(m_N + 1);
	m_BUFF_SY.BuffInit(m_N + 1);
	m_BUFF_CX.BuffInit(m_N);
	m_BUFF_CY.BuffInit(m_N);
}
//------------------------------------------------------------------------------
// деструктор:
//------------------------------------------------------------------------------
simple_IIR::~simple_IIR() {}
//------------------------------------------------------------------------------
// функция инициализации реального дифференцирующего звена:
int simple_IIR::DIF_Init(double Fs, double Fn, double Td)
{
	// Fs - частота дискретизации
	// Fn - номинальная частота сети
	// Td - постоянная времени реального дифференцирующего звена

	// инициализация системных переменных:
	m_Fs        = Fs;
	m_Ts        = 1 / Fs;
	m_Fn        = Fn;
	m_T1        = Td;
	m_N         = 2;
	m_Nx        = m_N;
	m_Ny        = m_N - 1;
	m_filt_type = 1;

	// инициализация буфферов:
	m_BUFF_SX.BuffInit(m_N + 1);
	m_BUFF_SY.BuffInit(m_N + 1);
	m_BUFF_CX.BuffInit(m_N);
	m_BUFF_CY.BuffInit(m_N);

	return 0;
}
//------------------------------------------------------------------------------
// функция инициализации апериодического звена:
int simple_IIR::EXP_Init(double Fs, double Fn, double Ta)
{
	// Fs - частота дискретизации
	// Fn - номинальная частота сети
	// Td - постоянная времени апериодического звена

	// инициализация системных переменных:
	m_Fs        = Fs;
	m_Ts		= 1 / Fs;
	m_Fn        = Fn;
	m_T1        = Ta;
	m_N         = 2;
	m_Nx        = m_N;
	m_Ny        = m_N - 1;
	m_filt_type = 2;

	// инициализация буфферов:
	m_BUFF_SX.BuffInit(m_N + 1);
	m_BUFF_SY.BuffInit(m_N + 1);
	m_BUFF_CX.BuffInit(m_N);
	m_BUFF_CY.BuffInit(m_N);

	return 0;
}
//------------------------------------------------------------------------------
// функция инициализации фазосдвигающего звена:
int simple_IIR::LEG_Init(double Fs, double Fn, double T1, double T2)
{
	// Fs - частота дискретизации
	// Fn - номинальная частота сети
	// T1 - постоянная времени числителя фазосдвигающего звена
	// T2 - постоянная времени знаменателя фазосдвигающего звена

	// инициализация системных переменных:
	m_Fs        = Fs;
	m_Ts		= 1 / Fs;
	m_Fn        = Fn;
	m_T1        = T1;
	m_T2        = T2;
	m_N         = 2;
	m_Nx        = m_N;
	m_Ny        = m_N - 1;
	m_filt_type = 3;

	// инициализация буфферов:
	m_BUFF_SX.BuffInit(m_N + 1);
	m_BUFF_SY.BuffInit(m_N + 1);
	m_BUFF_CX.BuffInit(m_N);
	m_BUFF_CY.BuffInit(m_N);

	return 0;
}
//------------------------------------------------------------------------------
// функция инициализации интегрирующего звена:
int simple_IIR::INT_Init(double Fs, double Fn)
{
	// Fs - частота дискретизации
	// Fn - номинальная частота сети

	// инициализация системных переменных:
	m_Fs        = Fs;
	m_Ts        = 1 / Fs;
	m_Fn        = Fn;
	m_N         = 2;
	m_Nx        = m_N;
	m_Ny        = m_N - 1;
	m_filt_type = 4;

	// инициализация буфферов:
	m_BUFF_SX.BuffInit(m_N + 1);
	m_BUFF_SY.BuffInit(m_N + 1);
	m_BUFF_CX.BuffInit(m_N);
	m_BUFF_CY.BuffInit(m_N);

	return 0;
}
//------------------------------------------------------------------------------
// функция инициализации ФНЧ второго порядка:
int simple_IIR::LP_Init(double Fs, double Fn, double Kd, double Fc)
{
	// Fs - частота дискретизации
	// Fn - номинальная частота сети
	// Kd - коэффициент демпфирования
	// Fc - частота среза ( резонансная частота )

	// инициализация системных переменных:
	m_Fs        = Fs;
	m_Ts        = 1 / Fs;
	m_Fn        = Fn;
	m_Fc        = Fc;
	m_Kd        = Kd;
	m_N         = 3;
	m_Nx        = m_N;
	m_Ny        = m_N - 1;
	m_filt_type = 5;

	// инициализация буфферов:
	m_BUFF_SX.BuffInit(m_N + 1);
	m_BUFF_SY.BuffInit(m_N + 1);
	m_BUFF_CX.BuffInit(m_N);
	m_BUFF_CY.BuffInit(m_N);

	return 0;
}
//------------------------------------------------------------------------------
// функция ФВЧ второго порядка:
int simple_IIR::HP_Init(double Fs, double Fn, double Kd, double Fc)
{
	// Fs - частота дискретизации
	// Fn - номинальная частота сети
	// Kd - коэффициент демпфирования
	// Fc - частота среза ( резонансная частота )

	// инициализация системных переменных:
	m_Fs = Fs;
	m_Ts = 1 / Fs;
	m_Fn = Fn;
	m_Fc = Fc;
	m_Kd = Kd;
	m_N = 3;
	m_Nx = m_N;
	m_Ny = m_N - 1;
	m_filt_type = 6;

	// инициализация буфферов:
	m_BUFF_SX.BuffInit(m_N + 1);
	m_BUFF_SY.BuffInit(m_N + 1);
	m_BUFF_CX.BuffInit(m_N);
	m_BUFF_CY.BuffInit(m_N);

	return 0;
}
//------------------------------------------------------------------------------
// функция ПФ второго порядка:
int simple_IIR::BP_Init(double Fs, double Fn, double Kd, double Fc)
{
	// Fs - частота дискретизации
	// Fn - номинальная частота сети
	// Kd - коэффициент демпфирования
	// Fc - частота среза ( резонансная частота )

	// инициализация системных переменных:
	m_Fs = Fs;
	m_Ts = 1 / Fs;
	m_Fn = Fn;
	m_Fc = Fc;
	m_Kd = Kd;
	m_N = 3;
	m_Nx = m_N;
	m_Ny = m_N - 1;
	m_filt_type = 7;

	// инициализация буфферов:
	m_BUFF_SX.BuffInit(m_N + 1);
	m_BUFF_SY.BuffInit(m_N + 1);
	m_BUFF_CX.BuffInit(m_N);
	m_BUFF_CY.BuffInit(m_N);

	return 0;
}
//------------------------------------------------------------------------------
// функция РФ второго порядка 1:
int simple_IIR::BS1_Init(double Fs, double Fn, double Kd, double Fc)
{
	// Fs - частота дискретизации
	// Fn - номинальная частота сети
	// Kd - коэффициент демпфирования
	// Fc - частота среза ( резонансная частота )

	// инициализация системных переменных:
	m_Fs = Fs;
	m_Ts = 1 / Fs;
	m_Fn = Fn;
	m_Fc = Fc;
	m_Kd = Kd;
	m_N = 3;
	m_Nx = m_N;
	m_Ny = m_N - 1;
	m_filt_type = 8;

	// инициализация буфферов:
	m_BUFF_SX.BuffInit(m_N + 1);
	m_BUFF_SY.BuffInit(m_N + 1);
	m_BUFF_CX.BuffInit(m_N);
	m_BUFF_CY.BuffInit(m_N);

	return 0;
}
//------------------------------------------------------------------------------
// функция РФ второго порядка 2:
int simple_IIR::BS2_Init(double Fs, double Fn, double Fc, double Fb)
{
	// Fs - частота дискретизации
	// Fn - номинальная частота сети
	// Fc - частота среза ( резонансная частота )
	// Fb - ширина полосы режекции

	// инициализация системных переменных:
	m_Fs = Fs;
	m_Ts = 1 / Fs;
	m_Fn = Fn;
	m_Fc = Fc;
	m_Fb = Fb;
	m_N = 3;
	m_Nx = m_N;
	m_Ny = m_N - 1;
	m_filt_type = 9;

	// инициализация буфферов:
	m_BUFF_SX.BuffInit(m_N + 1);
	m_BUFF_SY.BuffInit(m_N + 1);
	m_BUFF_CX.BuffInit(m_N);
	m_BUFF_CY.BuffInit(m_N);

	return 0;
}
//------------------------------------------------------------------------------
// функция инициализации рекурсивного сглаживающего фильтра с линейной ФЧХ:
int simple_IIR::SM_Init(double Fs, double Fn, int order)
{
	// Fs - частота дискретизации
	// Fn - номинальная частота сети
	// Fc - частота среза ( резонансная частота )
	// Fb - ширина полосы режекции

	// инициализация системных переменных:
	m_Fs = Fs;
	m_Ts = 1 / Fs;
	m_Fn = Fn;
	m_N = order;
	m_ElemNum = m_N + 1;

	m_filt_type = 10;

	// инициализация буфферов:
	m_BUFF_SX.BuffInit(m_N + 1);

	return 0;
}
//------------------------------------------------------------------------------
// функция инициализации дифференцирующего фильтра второго порядка:
int simple_IIR::DIFF2_init( double Fs , double Fn )
{
	// Fs - частота дискретизации
	// Fn - номинальная частота сети
	// Fc - частота среза ( резонансная частота )
	// Fb - ширина полосы режекции

	// инициализация системных переменных:
	m_Fs        = Fs;
	m_Ts        = 1 / Fs;
	m_Fn        = Fn;
	m_N         = 2;
	m_ElemNum   = m_N + 1;
	m_filt_type = 11;

	// инициализация буфферов:
	m_BUFF_SX.BuffInit(m_N + 1);
	
	return 0;
}
//------------------------------------------------------------------------------
int simple_IIR::CoeffCalc() 
{
	// расчет реального дифференцирующего звена:
	if (m_filt_type == 1) 
	{
		// расчет коэффициента усиления фильтра на нулевой частоте:
		m_Gain = 2 / ( m_Ts * ( 1 + 2 * m_T1 / m_Ts ) );

		// расчета коэффициентов передаточной функции числителя:
		m_BUFF_CX.m_buff[0] = 1;
		m_BUFF_CX.m_buff[1] = -1;

		// расчета коэффициентов передаточной функции знаменателя:
		m_BUFF_CY.m_buff[0] = 1;
		m_BUFF_CY.m_buff[1] = (1 - 2 * m_T1 / m_Ts) / (1 + 2 * m_T1 / m_Ts);
	}

	// расчет апериодического звена:
	if (m_filt_type == 2) 
	{
		// расчет коэффициента усиления фильтра на нулевой частоте:
		m_Gain = 1 / (1 + 2 * m_T1 / m_Ts);

		// расчета коэффициентов передаточной функции числителя:
		m_BUFF_CX.m_buff[0] = 1;
		m_BUFF_CX.m_buff[1] = 1;

		// расчета коэффициентов передаточной функции знаменателя:
		m_BUFF_CY.m_buff[0] = 1;
		m_BUFF_CY.m_buff[1] = (1 - 2 * m_T1 / m_Ts) / (1 + 2 * m_T1 / m_Ts);
	}

	// расчет фазосдвигающего звена:
	if (m_filt_type == 3) 
	{
		// расчет коэффициента усиления фильтра на нулевой частоте:
		m_Gain = (1 + 2 * m_T1 / m_Ts) / (1 + 2 * m_T2 / m_Ts);

		// расчета коэффициентов передаточной функции числителя:
		m_BUFF_CX.m_buff[0] = 1;
		m_BUFF_CX.m_buff[1] = (1 - 2 * m_T1 / m_Ts) / (1 + 2 * m_T1 / m_Ts);

		// расчета коэффициентов передаточной функции знаменателя:
		m_BUFF_CY.m_buff[0] = 1;
		m_BUFF_CY.m_buff[1] = (1 - 2 * m_T2 / m_Ts) / (1 + 2 * m_T2 / m_Ts);
	}

	// расчет интегрирующего звена:
	if (m_filt_type == 4) 
	{
		// расчет коэффициента усиления фильтра на нулевой частоте:
		m_Gain = 0.5 * m_Ts;

		// расчета коэффициентов передаточной функции числителя:
		m_BUFF_CX.m_buff[0] = 1;
		m_BUFF_CX.m_buff[1] = 1;

		// расчета коэффициентов передаточной функции знаменателя:
		m_BUFF_CY.m_buff[0] = 1;
		m_BUFF_CY.m_buff[1] = -1;
	}

	// расчет ФНЧ второго порядка:
	if (m_filt_type == 5) 
	{
		// расчет круговой частоты среза (рад/c):
		double omega = PI2 * m_Fc;

		// коррекция круговой частоты среза при переходе из непрерывного в дискретное время:
		omega = tan(omega * m_Ts / 2);

		// расчет коэффициентов полинома числителя:
		m_BUFF_CX.m_buff[0] = 1;
		m_BUFF_CX.m_buff[1] = 2;
		m_BUFF_CX.m_buff[2] = 1;

		// расчет коэффициентов полинома знаменателя:

		double a = 1;
		double b = omega / m_Kd;
		double c = omega * omega;

		double k1 = a + b + c;
		double k2 = 2 * c - 2 * a;
		double k3 = a - b + c;

		m_BUFF_CY.m_buff[0] = 1;
		m_BUFF_CY.m_buff[1] = k2 / k1;
		m_BUFF_CY.m_buff[2] = k3 / k1;

		// расчет коэффициента усиления фильтра на нулевой частоте:
		m_Gain = omega * omega / k1;
	}

	// расчет ФВЧ второго порядка:
	if (m_filt_type == 6)
	{
		// расчет круговой частоты среза (рад/c):
		double omega = PI2 * m_Fc;

		// коррекция круговой частоты среза при переходе из непрерывного в дискретное время:
		omega = tan(omega * m_Ts / 2);

		// расчет коэффициентов полинома числителя:
		m_BUFF_CX.m_buff[0] =  1;
		m_BUFF_CX.m_buff[1] = -2;
		m_BUFF_CX.m_buff[2] =  1;

		// расчет коэффициентов полинома знаменателя:

		double a = 1;
		double b = omega / m_Kd;
		double c = omega * omega;

		double k1 = a + b + c;
		double k2 = 2 * c - 2 * a;
		double k3 = a - b + c;

		m_BUFF_CY.m_buff[0] = 1;
		m_BUFF_CY.m_buff[1] = k2 / k1;
		m_BUFF_CY.m_buff[2] = k3 / k1;

		// расчет коэффициента усиления фильтра на нулевой частоте:
		m_Gain = 1 / k1;
	}

	// расчет ПФ второго порядка:
	if (m_filt_type == 7)
	{
		// расчет круговой частоты среза (рад/c):
		double omega = PI2 * m_Fc;

		// коррекция круговой частоты среза при переходе из непрерывного в дискретное время:
		omega = tan(omega * m_Ts / 2);

		// расчет коэффициентов полинома числителя:
		m_BUFF_CX.m_buff[0] =  1;
		m_BUFF_CX.m_buff[1] =  0;
		m_BUFF_CX.m_buff[2] = -1;

		// расчет коэффициентов полинома знаменателя:

		double a = 1;
		double b = omega / m_Kd;
		double c = omega * omega;

		double k1 = a + b + c;
		double k2 = 2 * c - 2 * a;
		double k3 = a - b + c;

		m_BUFF_CY.m_buff[0] = 1;
		m_BUFF_CY.m_buff[1] = k2 / k1;
		m_BUFF_CY.m_buff[2] = k3 / k1;

		// расчет коэффициента усиления фильтра на нулевой частоте:
		m_Gain = omega / m_Kd / k1;
	}

	// расчет РФ второго порядка 1:
	if (m_filt_type == 8)
	{
		// расчет круговой частоты среза (рад/c):
		double omega = PI2 * m_Fc;

		// коррекция круговой частоты среза при переходе из непрерывного в дискретное время:
		omega = tan(omega * m_Ts / 2);

		// расчет коэффициентов полинома числителя:
		m_BUFF_CX.m_buff[0] = 1 + omega * omega;
		m_BUFF_CX.m_buff[1] = 2 * omega*omega - 2;
		m_BUFF_CX.m_buff[2] = 1 + omega * omega;

		// расчет коэффициентов полинома знаменателя:

		double a = 1;
		double b = omega / m_Kd;
		double c = omega * omega;

		double k1 = a + b + c;
		double k2 = 2 * c - 2 * a;
		double k3 = a - b + c;

		m_BUFF_CY.m_buff[0] = 1;
		m_BUFF_CY.m_buff[1] = k2 / k1;
		m_BUFF_CY.m_buff[2] = k3 / k1;

		// расчет коэффициента усиления фильтра на нулевой частоте:
		m_Gain = 1 / k1;
	}

	// расчет РФ второго порядка 1:
	if (m_filt_type == 9)
	{
		// Расчет вспомогательных коэффициентов:
		double k1 = -cos(2 * PI0 * m_Fc * m_Ts);
		double k2 = (1 - tan(PI0 * m_Fb * m_Ts)) / (1 + tan(PI0 * m_Fb * m_Ts));

		// расчет коэффициентов полинома числителя:
		m_BUFF_CX.m_buff[0] = 1;
		m_BUFF_CX.m_buff[1] = 2 * k1;
		m_BUFF_CX.m_buff[2] = 1;

		// расчет коэффициентов полинома знаменателя:
		m_BUFF_CY.m_buff[0] = 1;
		m_BUFF_CY.m_buff[1] = k1 * (1 + k2);
		m_BUFF_CY.m_buff[2] = k2;

		// расчет коэффициента усиления фильтра на нулевой частоте:
		m_Gain = 0.5 * (1 + k2);
	}

	// расчет рекурсивного сглаживающего фильтра:
	if (m_filt_type == 10) 
	{
		m_Gain = 1 / (double)m_N;
	}
	
	// расчет дифференцирующего фильтра второго порядка:
	if (m_filt_type == 11) 
	{
		m_Kd      = m_Fs / ( PI2 * m_Fn );
	}

	return 0;
}
//------------------------------------------------------------------------------
// функция выделения памяти:
int simple_IIR::allocate() 
{
	// выделение памяти под внутренние буфферы фильтра:

	if ( (m_filt_type == 10) || (m_filt_type == 11) )  // выделение памяти под сглаживающий рекурсинвый фильтр или под дифф. фильтр 2 порядка
	{
		m_BUFF_SX.allocate(true);
	}
	else 
	{
		m_BUFF_SX.allocate(true);
		m_BUFF_SY.allocate(true);
		m_BUFF_CX.allocate(false);
		m_BUFF_CY.allocate(false);
	}

	// расчет коэффициентов фильтра:
	CoeffCalc();

	return 0;
}
//------------------------------------------------------------------------------
// функция освобождения памяти:
int simple_IIR::deallocate()
{
	// освобождение памяти из-под внутренних буфферов:
	// ( функции очистки памяти предусматривают условную очистку, т.е. очистку только в случае, если память была выделена )
	m_BUFF_SX.deallocate();
	m_BUFF_SY.deallocate();
	m_BUFF_CX.deallocate();
	m_BUFF_CY.deallocate();

	return 0;
}
//------------------------------------------------------------------------------
//функция расчета АЧХ и ФЧХ фильтра:
int simple_IIR::FreqCharacteristics()
{
	// расчет коэффициентов комплексного коэффициента передачи:
	double Re_nom = 0;
	double Im_nom = 0;
	double Re_den = 0;
	double Im_den = 0;
	double Re	  = 0;
	double Im	  = 0;


	switch (m_filt_type) // расчет АЧХ и ФЧХ в зависимости от типа фильтра
	{

	  case 10: // расет АЧХ и ФЧХ сглаживающего рекурсивного фильтра:
	      Re_nom = 1 - cos(-PI2 * m_in_F * m_N * m_Ts);
	      Im_nom = 0 - sin(-PI2 * m_in_F * m_N * m_Ts);
	      Re_den = 1 - cos(-PI2 * m_in_F * m_Ts);
	      Im_den = 0 - sin(-PI2 * m_in_F * m_Ts);
	      // расчет коэффициентов АЧХ и ФЧХ фильтра:
	      Re   = (Re_nom * Re_den + Im_nom * Im_den) / (Re_den * Re_den + Im_den * Im_den);
	      Im   = (Re_den * Im_nom - Re_nom * Im_den) / (Re_den * Re_den + Im_den * Im_den);
	      m_Km = m_Gain * sqrt(Re * Re + Im * Im);
	      m_pH = atan2(Im , Re);
	    break;

	  case 11: // расет АЧХ и ФЧХ дифференцирующего фильтра 2 порядка:
	    Re = 1 - cos(-PI2 * m_in_F * 1 * m_Ts);
	    Im = 0 - sin(-PI2 * m_in_F * 1 * m_Ts);

	    // расчет коэффициентов АЧХ и ФЧХ фильтра:
	    m_Km = m_Gain * sqrt(Re * Re + Im * Im);
	    m_pH = atan2(Im , Re);
	    break;

	  default: // расчет АЧХ и ФЧХ остальных фильтров:
	      // расчет комплекного числа числителя и знаменателя:
	      for (int cnt = 0; cnt < m_N; cnt++)
	      {
		      Re_nom = Re_nom + cos(-PI2 * m_in_F * cnt * m_Ts) * m_BUFF_CX.m_buff[cnt];
		      Im_nom = Im_nom + sin(-PI2 * m_in_F * cnt * m_Ts) * m_BUFF_CX.m_buff[cnt];
		      Re_den = Re_den + cos(-PI2 * m_in_F * cnt * m_Ts) * m_BUFF_CY.m_buff[cnt];
		      Im_den = Im_den + sin(-PI2 * m_in_F * cnt * m_Ts) * m_BUFF_CY.m_buff[cnt];
	      }
	      // расчет коэффициентов АЧХ и ФЧХ фильтра:
	      Re   = (Re_nom * Re_den + Im_nom * Im_den) / (Re_den * Re_den + Im_den * Im_den);
	      Im   = (Re_den * Im_nom - Re_nom * Im_den) / (Re_den * Re_den + Im_den * Im_den);
	      m_Km = m_Gain * sqrt(Re * Re + Im * Im);
	      m_pH = atan2(Im, Re);

            break;
        }

	return 0;
};


