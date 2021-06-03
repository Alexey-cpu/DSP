// A.M. Tykvinsky, 28.11.2020
//--------------------------------------------------------------------------------------------------
// ОДНОФАЗНЫЕ ОРТОГОНАЛЬНЫЕ ПРЕОБРАЗОВАНИЯ:
//--------------------------------------------------------------------------------------------------

#include "orto_transformations_1ph.hpp"

//--------------------------------------------------------------------------------------------------
// конструктор по умолчанию
orto_transformations_1ph::orto_transformations_1ph()
{
	// инициализация системных перменных по умолчанию:
	m_Fs        = 4000;
	m_Fn        = 50;
	m_Ts        = 1 / m_Fs;
	m_order     = m_Fs / m_Fn;
	m_Ns        = m_order;
	m_filt_type = 1;
	m_Gain      = 1;
	
	// инициализация промежуточных переменных по умолчанию:
	m_a	       = 0;
	m_b        = 0;
	m_d        = 0;
	m_q        = 0;
	m_cnt      = 0;
	m_ElemNum1  = m_order + 1;
	m_ElemNum2  = m_order - 1;

	// Инициализация входов/выходов по умолчанию:
	m_in_F = 50;
	m_Re   = 0;
	m_Im   = 0;
	m_Km   = 0;
	m_pH   = 0;
	
	// инициализация оконной функции по умолчанию:
	m_WIND_FCN.windInit(m_order);

	// Инициализация оконного буффера входной выборки по умолчанию:
	
	m_BUFF_WIND_SX.BuffInit(m_order + 1);  // буффер под выборку
	m_BUFF_WIND_SY.BuffInit(m_order + 1);  // буффер под выборку
	m_BUFF_WIND_CX.BuffInit(m_order);      // буффер под коэффициенты косинусного фильтра
	m_BUFF_WIND_CY.BuffInit(m_order);      // буффер под коэффициенты синусного   фильтра
}
//--------------------------------------------------------------------------------------------------
// деструктор
orto_transformations_1ph::~orto_transformations_1ph(){}
//--------------------------------------------------------------------------------------------------
// функция инициализации дискретного преобразования Фурье:
int orto_transformations_1ph::DFT_Init(float Fs, float Fn)
{
	// инициализация системных перменных:
	m_Fs        = Fs;
	m_Fn        = Fn;
	m_Ts        = 1 / m_Fs;
	m_order     = m_Fs / m_Fn;
	m_Ns        = m_order;
	m_filt_type = 1;
	m_Gain      = 1;
	
	// инициализация промежуточных переменных:
	m_a	       = 0;
	m_b        = 0;
	m_d        = 0;
	m_q        = 0;
	m_cnt      = 0;
	m_ElemNum1  = m_order + 1;
	m_ElemNum2  = m_order - 1;

	// Инициализация входов/выходов:
	m_in_F = 50;
	m_Re   = 0;
	m_Im   = 0;
	m_Km   = 0;
	m_pH   = 0;
	
	// инициализация оконной функции:
	m_WIND_FCN.windInit(m_order);

	// Инициализация оконного буффера входной выборки:
	m_BUFF_WIND_SX.BuffInit(m_order + 1);  // буффер под выборку
	m_BUFF_WIND_SY.BuffInit(m_order + 1);  // буффер под выборку
	m_BUFF_WIND_CX.BuffInit(m_order);      // буффер под коэффициенты косинусного фильтра
	m_BUFF_WIND_CY.BuffInit(m_order);      // буффер под коэффициенты синусного   фильтра

	return 0;
}
//--------------------------------------------------------------------------------------------------
// функция инициализации дискретного преобразования Фурье с заданием ширины окна наблюдения:
int orto_transformations_1ph::DFT_Init(float Fs, float Fn, int order)
{
	// инициализация системных перменных:
	m_Fs        = Fs;
	m_Fn        = Fn;
	m_Ts        = 1 / m_Fs;
	m_order     = order;
	m_Ns        = m_Fs / m_Fn;
	m_filt_type = 1;
	m_Gain      = 1;

	// Инициализация входов/выходов:
	m_in_F = 50;
	m_Re   = 0;
	m_Im   = 0;
	m_Km   = 0;
	m_pH   = 0;
	
	// инициализация оконной функции:
	m_WIND_FCN.windInit(m_order);

	// Инициализация оконного буффера входной выборки:
	m_BUFF_WIND_SX.BuffInit(m_order + 1);  // буффер под выборку
	m_BUFF_WIND_SY.BuffInit(m_order + 1);  // буффер под выборку

	m_BUFF_WIND_CX.BuffInit(m_order);     // буффер под коэффициенты косинусного фильтра
	m_BUFF_WIND_CY.BuffInit(m_order);    // буффер под коэффициенты синусного   фильтра

	return 0;
}
//--------------------------------------------------------------------------------------------------
// функция инициализации дискретного преобразования Хартли:
int orto_transformations_1ph::DHT_Init(float Fs, float Fn)
{
	// инициализация системных перменных:
	m_Fs        = Fs;
	m_Fn        = Fn;
	m_Ts        = 1 / m_Fs;
	m_order     = m_Fs / m_Fn;
	m_Ns        = m_order;
	m_filt_type = 2;
	m_Gain      = 1;

	// Инициализация входов/выходов:
	m_in_F = 50;
	m_Re   = 0;
	m_Im   = 0;
	m_Km   = 0;
	m_pH   = 0;
	
	// инициализация оконной функции:
	m_WIND_FCN.windInit(m_order);

	// Инициализация оконного буффера входной выборки:
	m_BUFF_WIND_SX.BuffInit(m_order + 1);  // буффер под выборку
	m_BUFF_WIND_SY.BuffInit(m_order + 1);  // буффер под выборку

	m_BUFF_WIND_CX.BuffInit(m_order);     // буффер под коэффициенты косинусного фильтра
	m_BUFF_WIND_CY.BuffInit(m_order);    // буффер под коэффициенты синусного   фильтра

	return 0;
}
//--------------------------------------------------------------------------------------------------
// функция инициализации дискретного преобразования Хартли с заданием ширины окна наблюдения:
int orto_transformations_1ph::DHT_Init(float Fs, float Fn , int order)
{
	// инициализация системных перменных:
	m_Fs        = Fs;
	m_Fn        = Fn;
	m_Ts        = 1 / m_Fs;
	m_order     = order;
	m_Ns        = m_Fs / m_Fn;
	m_filt_type = 2;
	m_Gain      = 1;

	// Инициализация входов/выходов:
	m_in_F = 50;
	m_Re   = 0;
	m_Im   = 0;
	m_Km   = 0;
	m_pH   = 0;
	
	// инициализация оконной функции:
	m_WIND_FCN.windInit(m_order);

	// Инициализация оконного буффера входной выборки:
	m_BUFF_WIND_SX.BuffInit(m_order + 1);  // буффер под выборку
	m_BUFF_WIND_SY.BuffInit(m_order + 1);  // буффер под выборку

	m_BUFF_WIND_CX.BuffInit(m_order    );     // буффер под коэффициенты косинусного фильтра
	m_BUFF_WIND_CY.BuffInit(m_order    );    // буффер под коэффициенты синусного   фильтра

	return 0;
}
//--------------------------------------------------------------------------------------------------
// функция инициализации квадратуры:
int orto_transformations_1ph::DQT_Init(float Fs, float Fn , bool mode)
{
	
	// инициализация системных перменных:
	m_Fs		= Fs;
	m_Fn		= Fn;
	m_Ts		= 1 / m_Fs;
	m_order		= m_Fs / m_Fn;
	m_Ns		= m_order;
	m_filt_type = 3;
	m_Gain      = 1;
	m_mode      = mode;
	
	// инициализация промежуточных переменных:
	m_a	  		= 0;
	m_b   		= 0;
	m_d   	    = 0;
	m_q         = 0;
	m_cnt       = 0;
	m_ElemNum1  = m_order + 1;
	m_ElemNum2  = m_order - 1;

	// Инициализация входов/выходов:
	m_in_F  = 50;
	m_Re	= 0;
	m_Im	= 0;
	m_Km	= 0;
	m_pH	= 0;

	// инициализация оконной функции:
	m_WIND_FCN.windInit(m_order);

	// Инициализация оконного буффера входной выборки:
	m_BUFF_WIND_SX.BuffInit(m_ElemNum1 + 1);  // буффер под выборку
	m_BUFF_WIND_SY.BuffInit(m_ElemNum1 + 1);  // буффер под выборку

	m_BUFF_WIND_CX.BuffInit(m_order);     // буффер под коэффициенты косинусного фильтра
	m_BUFF_WIND_CY.BuffInit(m_order);    // буффер под коэффициенты синусного   фильтра

	return 0;
}
//--------------------------------------------------------------------------------------------------
// функция расчета коэффициентов:
int orto_transformations_1ph::CoeffCalc() 
{
	// проверяем, спроектированно ли окно:
	if (m_WIND_FCN.m_wind_ready == false) // если окно не было предварительно спроектировано пользователем, то по умолчанию будет спроектированно прямоугольное окно
	{
		m_WIND_FCN.Rectangular();
	}

	switch (m_filt_type)
	{

	case 1: // расчет коэффициентов фильтра дискретного преобразования Фурье

		m_Gain = 2 / ((float)m_order);

		for (int n = 0; n < m_order; n++)
		{
			m_BUFF_WIND_CX.m_buff[n] = m_Gain * cos(-PI2 * (float)n / (float)m_Ns) * m_WIND_FCN.m_BUFF_WIND[n]; // коэффициенты косинусного фильтра
			m_BUFF_WIND_CY.m_buff[n] = m_Gain * sin(-PI2 * (float)n / (float)m_Ns) * m_WIND_FCN.m_BUFF_WIND[n]; // коэффициенты синусного фильтра
		}

		break;

	case 2: // расчет коэффициентов фильтра дискретного преобразования Хартли

		m_Gain = 2 / ((float)m_order);

		for (int n = 0; n < m_order; n++)
		{
			m_BUFF_WIND_CX.m_buff[n] = m_Gain  * (0.5 )*(cos(-PI2 * (float)n / (float)m_Ns) + sin(-PI2 * (float)n / (float)m_Ns)) + 0.5*(cos(-PI2 * (float)(m_Ns - n) / (float)m_Ns) + sin(-PI2 * (float)(m_Ns - n) / (float)m_Ns)) * m_WIND_FCN.m_BUFF_WIND[n];
			m_BUFF_WIND_CY.m_buff[n] = m_Gain  * (-0.5)*(cos(-PI2 * (float)n / (float)m_Ns) + sin(-PI2 * (float)n / (float)m_Ns)) + 0.5*(cos(-PI2 * (float)(m_Ns - n) / (float)m_Ns) + sin(-PI2 * (float)(m_Ns - n) / (float)m_Ns)) * m_WIND_FCN.m_BUFF_WIND[n];
		}

	    break;

	case 3:
		
		m_Gain = 2 / ((float)m_order);

		for (int n = 0; n < m_order ; n++) 
		{
			m_BUFF_WIND_CX.m_buff[n] = cos(PI2 * (float)n / (float)m_Ns); // коэффициенты косинусного фильтра
			m_BUFF_WIND_CY.m_buff[n] = sin(PI2 * (float)n / (float)m_Ns); // коэффициенты синусного фильтра
		}

		break;
	}
	
	// освобождение памяти из-под коэффициентов оконной функции:
	m_WIND_FCN.deallocate();

	return 0;
}
//------------------------------------------------------------------------------
//функция расчета АЧХ и ФЧХ фильтра:
int orto_transformations_1ph::FreqCharacteristics()
{
	// Начальные значения комплекного коэффициента передачи:
	float Re1 = 0;
	float Im1 = 0;
	float Re2 = 0;
	float Im2 = 0;

	//Расчет комплексного коэффициента передачи:
	for (int n = 0; n < m_order; n++)
	{
		//Для частоты m_in_F Гц:
		Re1 = Re1 + cos(-PI2 * n * m_in_F * m_Ts) * m_BUFF_WIND_CX.m_buff[n];
		Im1 = Im1 + sin(-PI2 * n * m_in_F * m_Ts) * m_BUFF_WIND_CX.m_buff[n];
		Re2 = Re2 + cos(-PI2 * n * m_in_F * m_Ts) * m_BUFF_WIND_CY.m_buff[n];
		Im2 = Im2 + sin(-PI2 * n * m_in_F * m_Ts) * m_BUFF_WIND_CY.m_buff[n];
	}
	
	// Расчет ФЧХ:
	// (в качестве ФЧХ берется разница ФЧХ синусного и косинусного фильтров)
	m_pH = atan2( Im1 , Re1) - atan2(Re1, Im2);

	// Расчет АЧХ:
	// ( в качестве Re берется сигнал на выходе косинусного фильтра, а в качестве Im - сигнал на выходе синусного фильтра )
	m_Km = sqrt (Re1 * Re1 + Im1 * Im1 + Re2 * Re2 + Im2 * Im2) * m_Gain * 0.5;

	return 0;
};
//--------------------------------------------------------------------------------------------------
// функция выделения памяти:
int orto_transformations_1ph::allocate()
{
	// выделение памяти под коэффициенты синусного и косинусного фильтра:
	
	/*switch (m_filt_type)
	{
	case 3:
		m_BUFF_WIND_SX.allocate(true);  // буффер под выборку
		m_BUFF_WIND_SY.allocate(true);  // буффер под выборку
		break;

	default:
		m_BUFF_WIND_SX.allocate(true);  // буффер под выборку
		break;
	}*/

	m_BUFF_WIND_SX.allocate(true);  // буффер под выборку
	m_BUFF_WIND_SY.allocate(true);  // буффер под выборку
	m_BUFF_WIND_CX.allocate(false);
	m_BUFF_WIND_CY.allocate(false);

	// расчет коэффициентов фильтра:
	CoeffCalc();

	return 0;
}
//--------------------------------------------------------------------------------------------------
// Освобождение памяти:
int orto_transformations_1ph::deallocate()
{
	// освобождение памяти из под буфферов с выборкой и коэффициентами синусного и косинусного фильтров:
	/*switch (m_filt_type)
	{
	case 3:
		m_BUFF_WIND_SX.deallocate();  // буффер под выборку
		m_BUFF_WIND_SY.deallocate();  // буффер под выборку
		break;

	default:
		m_BUFF_WIND_SX.deallocate();  // буффер под выборку
		break;
	}*/
	
	m_BUFF_WIND_SX.deallocate();  // буффер под выборку
	m_BUFF_WIND_SY.deallocate();  // буффер под выборку
	m_BUFF_WIND_CX.deallocate();
	m_BUFF_WIND_CY.deallocate();

	return 0;
}
