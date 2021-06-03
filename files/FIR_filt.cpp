//--------------------------------------------------------------------------------
//A.M.Tykvinsky, 30.09.2020
//--------------------------------------------------------------------------------
// ФИЛЬТР С КОНЕЧНОЙ ИМПУЛЬСНОЙ ХАРАКТЕРИСТИКОЙ
//--------------------------------------------------------------------------------

#include "FIR_filt.hpp"

// Список использованных для реализации источников:

// 1. L.Thede    ,                   " Practical analog and digital filter design "
// 2. T.W Parks  , C.S. Burrus,      " Digital Filter Design "
// 3. C.Ifeachor , Barrie W. Jervis, " Digital signal processing - a practical approach "

// Общее описание работы класса:
// Расчет КИХ фильтра методом оконного сглаживания состоит из следующих этапов:
// 1. Расчет коэффициентов оконной функции;
// 2. Расчет коэффициентов идеального ФНЧ, ФВЧ, ПФ или РФ;
// 3. Умножение коэффициентов оконной функции на коэффициенты идеального ФНЧ, ФВЧ, ПФ или РФ.

//------------------------------------------------------------------------------
//Конструктор:
FIR_filt::FIR_filt()
{
	// Заполнение полей:
	m_Fs        = 4000;
	m_Fn        = 50;
	m_Ts        = 1 / m_Fs;
	m_order     = 80;
	m_filt_type = 1;
	m_scale     = false; // по умолчанию, считаем ненормированные коэффициенты
	m_ang = 0;

	// инициализация оконной функции:
	m_WIND_FCN.windInit(m_order + 1); // порядок фильтра всегда на 1 болшье из-за симметрии коэффициентов !!!

	// Инициализация входов/выходов:
	m_in_F = 50;
	m_out  = 0;

	// Инициализация оконного буффера входной выборки:
	m_BUFF_WIND_SX.BuffInit(m_order + 2);  // буффер под выборку (+2 из-за особенности зеркального буффера)
	m_BUFF_WIND_CX.BuffInit(m_order + 1); // буффер под коэффициенты

}
//------------------------------------------------------------------------------
//Деструктор:
FIR_filt::~FIR_filt() {};
//------------------------------------------------------------------------------
// функция инициализации ФНЧ:
int FIR_filt::LP_Init(double Fs, double Fn, double F_stop, int order , bool scale)
{
	// Fs     - частота дискретизации АЦП
	// Fn     - номинальная частота сети
	// F_stop - частота среза фильтра
	// order  - порядок фильтра
	// scale  - вкл/выкл нормирование коэффициентов фильтра (true - вкл, false - выкл)

	// Заполнение полей:
	m_Fs        = Fs;
	m_Fn        = Fn;
	m_Ts        = 1 / m_Fs;
	m_order     = order;
	m_Fstop     = F_stop;
	m_filt_type = 1;
	m_scale     = scale;

	// инициализация оконной функции:
	m_WIND_FCN.windInit( m_order + 1);  // порядок фильтра всегда на 1 болшье из-за симметрии коэффициентов !!!

	// Инициализация входов/выходов:
	m_in_F = 50;
	m_out  = 0;

	// Инициализация оконного буффера входной выборки:
	m_BUFF_WIND_SX.BuffInit(m_order + 2);  // буффер под выборку
	m_BUFF_WIND_CX.BuffInit(m_order + 1); // буффер под коэффициенты

	return 0;
}
//------------------------------------------------------------------------------
// функция инициализации ФВЧ:
int FIR_filt::HP_Init(double Fs, double Fn, double F_stop, int order , bool scale)
{
	// Fs     - частота дискретизации АЦП
	// Fn     - номинальная частота сети
	// F_stop - частота среза фильтра
	// order  - порядок фильтра
	// scale  - вкл/выкл нормирование коэффициентов фильтра (true - вкл, false - выкл)

	// Заполнение полей:
	m_Fs		= Fs;
	m_Fn		= Fn;
	m_Ts		= 1 / m_Fs;
	m_order		= order;
	m_Fstop		= F_stop;
	m_filt_type = 2;
	m_scale     = scale;

	// инициализация оконной функции:
	m_WIND_FCN.windInit( m_order + 1 );// порядок фильтра всегда на 1 болшье из-за симметрии коэффициентов !!!

	// Инициализация входов/выходов:
	m_in_F = 50;
	m_out  = 0;

	// Инициализация оконного буффера входной выборки:
	m_BUFF_WIND_SX.BuffInit( m_order + 2 );  // буффер под выборку
	m_BUFF_WIND_CX.BuffInit( m_order + 1 ); // буффер под коэффициенты

	return 0;
}
//------------------------------------------------------------------------------
// функция инициализации ПФ:
int FIR_filt::BP_Init(double Fs, double Fn, double F_stop1, double F_stop2, int order, bool scale)
{
	// Fs      - частота дискретизации АЦП
	// Fn      - номинальная частота сети
	// F_stop1 - начало полосы пропускания фильтра
	// F_stop2 - конец  полосы пропускания фильтра
	// order   - порядок фильтра
	// scale   - вкл/выкл нормирование коэффициентов фильтра (true - вкл, false - выкл)

	// Заполнение полей:
	m_Fs		= Fs;
	m_Fn        = Fn;
	m_Ts        = 1 / m_Fs;
	m_order     = order;
	m_Fstop1    = F_stop1;
	m_Fstop2    = F_stop2;
	m_filt_type = 3;
	m_scale     = scale;

	// инициализация оконной функции:
	m_WIND_FCN.windInit(m_order + 1 );// порядок фильтра всегда на 1 болшье из-за симметрии коэффициентов !!!

	// Инициализация входов/выходов:
	m_in_F = 50;
	m_out  = 0;

	// Инициализация оконного буффера входной выборки:
	m_BUFF_WIND_SX.BuffInit(m_order + 2);  // буффер под выборку
	m_BUFF_WIND_CX.BuffInit(m_order + 1); // буффер под коэффициенты

	return 0;
}
//------------------------------------------------------------------------------
// функция инициализации РФ:
int FIR_filt::BS_Init(double Fs, double Fn, double F_stop1, double F_stop2, int order , bool scale)
{
	// Fs      - частота дискретизации АЦП
	// Fn      - номинальная частота сети
	// F_stop1 - начало полосы подавления фильтра
	// F_stop2 - конец  полосы подавления фильтра
	// order   - порядок фильтра
	// scale   - вкл/выкл нормирование коэффициентов фильтра (true - вкл, false - выкл)

	// Заполнение полей:
	m_Fs        = Fs;
	m_Fn        = Fn;
	m_Ts        = 1 / m_Fs;
	m_order     = order;
	m_Fstop1    = F_stop1;
	m_Fstop2    = F_stop2;
	m_filt_type = 4;
	m_scale     = scale;

	// При нечетном порядке РФ требуется увеличить порядок на 1, т.к. РФ на частоте Найквиста должен иметь коэфф. усиления равный 0 !!!
	if (m_order % 2 != 0) m_order++;

	// инициализация оконной функции:
	m_WIND_FCN.windInit( m_order + 1 );// порядок фильтра всегда на 1 болшье из-за симметрии коэффициентов !!!

	// Инициализация входов/выходов:
	m_in_F = 50;
	m_out  = 0;

	// Инициализация оконного буффера входной выборки:
	m_BUFF_WIND_SX.BuffInit(m_order + 2);  // буффер под выборку
	m_BUFF_WIND_CX.BuffInit(m_order + 1); // буффер под коэффициенты

	return 0;
}
//------------------------------------------------------------------------------
// функция инициализации синусного фильтра:
int FIR_filt::CF_Init(double Fs, double Fn, int order)
{
	// Fs       - частота дискретизации АЦП
	// Fn       - номинальная частота сети
	// order    - порядок фильтра
	// flt_type - тип фильтра (true - синусный , false - косинусный)

	// порядок косинусного/синусного фильтра нужно задавать на 1 меньше расчетного.

	// Заполнение полей:
	m_Fs        = Fs;
	m_Fn        = Fn;
	m_Ts        = 1 / m_Fs;
	m_Ns = m_Fs / m_Fn;
	m_order     = order - 1;
	m_filt_type = 5;
	m_scale     = false;

	// инициализация оконной функции:
	m_WIND_FCN.windInit(m_order + 1);

	// Инициализация входов/выходов:
	m_in_F = 50;
	m_out = 0;

	// Инициализация оконного буффера входной выборки:
	m_BUFF_WIND_SX.BuffInit(m_order + 2);  // буффер под выборку
	m_BUFF_WIND_CX.BuffInit(m_order + 1); // буффер под коэффициенты

	return 0;
}
//------------------------------------------------------------------------------
// функция инициализации косинусного/синусного фильтра:
int FIR_filt::SF_Init(double Fs, double Fn, int order)
{
	// Fs       - частота дискретизации АЦП
	// Fn       - номинальная частота сети
	// order    - порядок фильтра
	// flt_type - тип фильтра (true - синусный , false - косинусный)

	// порядок косинусного/синусного фильтра нужно задавать на 1 меньше расчетного.

	// Заполнение полей:
	m_Fs = Fs;
	m_Fn = Fn;
	m_Ns = m_Fs / m_Fn;
	m_Ts = 1 / m_Fs;
	m_order = order - 1;
	m_filt_type = 6;
	m_scale = false;

	// инициализация оконной функции:
	m_WIND_FCN.windInit(m_order + 1);

	// Инициализация входов/выходов:
	m_in_F = 50;
	m_out = 0;

	// Инициализация оконного буффера входной выборки:
	m_BUFF_WIND_SX.BuffInit(m_order + 2);  // буффер под выборку
	m_BUFF_WIND_CX.BuffInit(m_order + 1); // буффер под коэффициенты

	return 0;
}
//------------------------------------------------------------------------------
// функция инициализации косинусного/синусного фильтра:
int FIR_filt::HF_Init(double Fs, double Fn, double pH , int order)
{
	// Fs       - частота дискретизации АЦП
	// Fn       - номинальная частота сети
	// order    - порядок фильтра
	// flt_type - тип фильтра (true - синусный , false - косинусный)

	// порядок косинусного/синусного фильтра нужно задавать на 1 меньше расчетного.

	// Заполнение полей:
	m_Fs = Fs;
	m_Fn = Fn;
	m_Ts = 1 / m_Fs;
	m_order = order - 1;
	m_Ns = m_Fs / m_Fn;
	m_filt_type = 7;
	m_ang = pH * PI0 / 180.0;
	m_scale = false;

	// инициализация оконной функции:
	m_WIND_FCN.windInit(m_order + 1);

	// Инициализация входов/выходов:
	m_in_F = 50;
	m_out = 0;

	// Инициализация оконного буффера входной выборки:
	m_BUFF_WIND_SX.BuffInit(m_order + 2);  // буффер под выборку
	m_BUFF_WIND_CX.BuffInit(m_order + 1); // буффер под коэффициенты

	return 0;
}
//------------------------------------------------------------------------------
// функция расчета коэффициентов КИХ фильтра методом оконного сглаживания:

int FIR_filt::CoeffCalc() 
{
	// проверяем, спроектированно ли окно:
	if (m_WIND_FCN.m_wind_ready == false) // если окно не было предварительно спроектировано пользователем, то по умолчанию будет спроектировано окно Чебышева с уровнем боковых лепестков в 100 Дб
	{
		m_WIND_FCN.Chebyshev(100);
	}

	// Расчет коэффициентов КИХ фильтра:
	int    k   = 0;

	m_Fstop  = m_Fstop    / m_Fs; // нормирование частоты среза ФНЧ/ФВЧ
	m_Fstop1 = m_Fstop1  / m_Fs; // нормирование начала полосы пропускания/подавления полосового/режекторного фильтра
	m_Fstop2 = m_Fstop2 / m_Fs; // нормирование конца  полосы пропускания/подавления полосового/режекторного фильтра

	// расчет коэффициентов ФНЧ:

	switch (m_filt_type)
	{
	case 1: // расчет коэффициентов ФНЧ:

		if (m_order % 2 == 0) // считаем фильтр четного порядка
		{
			for (int n = 0; n <= m_order / 2; n++)
			{
				k = fabs(n - (m_order + 1) / 2);

				if (n == 0)
				{
					m_BUFF_WIND_CX.m_buff[k] = 2 * m_Fstop * m_WIND_FCN.m_BUFF_WIND[k];
				}
				else
				{
					m_BUFF_WIND_CX.m_buff[k] = 2 * m_Fstop * sin(n * PI2 * m_Fstop) / (n * PI2 * m_Fstop) * m_WIND_FCN.m_BUFF_WIND[k];
					m_BUFF_WIND_CX.m_buff[m_order - k] = m_BUFF_WIND_CX.m_buff[k];
				}
			}
		}
		else if (m_order % 2 != 0) // считаем фильтр нечетного порядка
		{
			double rem = ceil((double)m_order / 2) - (double)m_order / 2;
			for (int n = 0; n <= ceil(m_order / 2); n++)
			{
				k = fabs(n - m_order / 2);
				m_BUFF_WIND_CX.m_buff[k] = 2 * m_Fstop * sin((n + rem) * PI2 * m_Fstop) / ((n + rem) * PI2 * m_Fstop) * m_WIND_FCN.m_BUFF_WIND[k];
				m_BUFF_WIND_CX.m_buff[m_order - k] = m_BUFF_WIND_CX.m_buff[k];
			}
		}

		break;

	case 2: // расчет коэффициентов ФВЧ:

		if (m_order % 2 == 0) // считаем ФВЧ четного порядка
		{
			for (int n = 0; n <= m_order / 2; n++)
			{
				k = fabs(n - (m_order + 1) / 2);

				if (n == 0)
				{
					m_BUFF_WIND_CX.m_buff[k] = (1 - 2 * m_Fstop) * m_WIND_FCN.m_BUFF_WIND[k];
				}
				else
				{
					m_BUFF_WIND_CX.m_buff[k] = -2 * m_Fstop * sin(n * PI2 * m_Fstop) / (n * PI2 * m_Fstop) * m_WIND_FCN.m_BUFF_WIND[k];
					m_BUFF_WIND_CX.m_buff[m_order - k] = m_BUFF_WIND_CX.m_buff[k];
				}
			}
		}
		else if (m_order % 2 != 0) // считаем ФВЧ нечетного порядка (ФВЧ КИХ нечетного порядка считается путем преобразования ФНЧ в ФВЧ, см. книгу Айфичера раздел про эффективную КИХ - фильтрацию)
		{
		    double rem = ceil((double)m_order / 2) - (double)m_order / 2;
		    for (int n = 0; n <= ceil(m_order / 2); n++)
		    {
				k = fabs(n - m_order / 2);
				m_BUFF_WIND_CX.m_buff[k]           = -pow(-1 , n) * 2 * (0.5-m_Fstop) * sin((n + rem) * PI2 * (0.5-m_Fstop) ) / ((n + rem) * PI2 * (0.5 - m_Fstop) )* m_WIND_FCN.m_BUFF_WIND[k];
				m_BUFF_WIND_CX.m_buff[m_order - k] = -m_BUFF_WIND_CX.m_buff[k];
		    }
		}

		break;

	case 3: // расчет коэффициентов ПФ:

		if (m_order % 2 == 0) // считаем ПФ четного порядка
		{
			for (int n = 0; n <= m_order / 2; n++)
			{
				k = fabs(n - (m_order + 1) / 2);

				if (n == 0)
				{
					m_BUFF_WIND_CX.m_buff[k] = 2 * (m_Fstop2 - m_Fstop1) * m_WIND_FCN.m_BUFF_WIND[k];
				}
				else
				{
					m_BUFF_WIND_CX.m_buff[k] = 2 * (m_Fstop2 * sin(n * PI2 * m_Fstop2) / (n * PI2 * m_Fstop2) - m_Fstop1 * sin(n * PI2 * m_Fstop1) / (n * PI2 * m_Fstop1)) * m_WIND_FCN.m_BUFF_WIND[k];
					m_BUFF_WIND_CX.m_buff[m_order - k] = m_BUFF_WIND_CX.m_buff[k];
				}
			}
		}
		else if (m_order % 2 != 0) // считаем ПФ нечетного порядка
		{
			double rem = ceil((double)m_order / 2) - (double)m_order / 2;
			for (int n = 0; n <= ceil(m_order / 2); n++)
			{
				k = fabs(n - m_order / 2);
				m_BUFF_WIND_CX.m_buff[k] = 2 * (m_Fstop2 * sin((n + rem) * PI2 * m_Fstop2) / ((n + rem) * PI2 * m_Fstop2) - m_Fstop1 * sin((n + rem) * PI2 * m_Fstop1) / ((n + rem) * PI2 * m_Fstop1)) * m_WIND_FCN.m_BUFF_WIND[k];
				m_BUFF_WIND_CX.m_buff[m_order - k] = m_BUFF_WIND_CX.m_buff[k];
			}
		}

		break;

	case 4: // расчет коэффициентов РФ:


		for (int n = 0; n <= m_order / 2; n++)
		{
			k = fabs(n - (m_order + 1) / 2);

			if (n == 0)
			{
				m_BUFF_WIND_CX.m_buff[k] = 1 - 2 * (m_Fstop2 - m_Fstop1) * m_WIND_FCN.m_BUFF_WIND[k];
			}
			else
			{
					m_BUFF_WIND_CX.m_buff[k] = 2 * (m_Fstop1 * sin(n * PI2 * m_Fstop1) / (n * PI2 * m_Fstop1) - m_Fstop2 * sin(n * PI2 * m_Fstop2) / (n * PI2 * m_Fstop2)) * m_WIND_FCN.m_BUFF_WIND[k];
					m_BUFF_WIND_CX.m_buff[m_order - k] = m_BUFF_WIND_CX.m_buff[k];
			}
		}

		break;

	case 5: // рассчет коэффициентов косинусного фильтра:

		for (int n = 0; n < m_order + 1; n++) m_BUFF_WIND_CX.m_buff[n] = 2 / (double)(m_order + 1) * cos(-PI2 * (double)n / m_Ns) * m_WIND_FCN.m_BUFF_WIND[n];
	
		break;

	case 6: // рассчет коэффициентов синусного фильтра:

		for (int n = 0; n < m_order + 1; n++) m_BUFF_WIND_CX.m_buff[n] = 2 / (double)(m_order + 1) * sin(-PI2 * (double)n / m_Ns) * m_WIND_FCN.m_BUFF_WIND[n];
		
		break;

	case 7: // рассчет коэффициентов фильтра Хартли:

		for (int n = 0; n < m_order + 1; n++) m_BUFF_WIND_CX.m_buff[n] = (sin(-PI2 * (double)n / m_Ns + m_ang) + cos(-PI2 * (double)n / m_Ns + m_ang))* sqrt(2) / (m_order + 1) * m_WIND_FCN.m_BUFF_WIND[n];
		
		break;

	}

	
	// Процедура нормирования коэффициетов фильтра на частоте, равной половине полосы пропускания фильтра:
	// (сделано как в ПК MatLab, при желании, можно двигать частоту, на которой производится нормировка коэффициентов)

	if (m_scale == true) 
	{
		if (m_filt_type == 1) // считаем нормирующий коэффициент для ФНЧ 
		{
			m_in_F = 0;
			FreqCharacteristics();
		}

		if (m_filt_type == 2) // считаем нормирующий коэффициент для ФВЧ
		{
			m_in_F = m_Fs / 2;
			FreqCharacteristics();

			std::cout << " m_in_F = " << m_in_F << "\n";
			std::cout << "m_Km    = " << m_Km   << "\n";
			std::cout << "m_pH    = " << m_pH   << "\n";
		}

		if (m_filt_type == 3) // считаем нормирующий коэффициент для ПФ
		{
			m_in_F = ( m_Fstop1 + 0.5 * ( m_Fstop2 - m_Fstop1 ) ) * m_Fs;
			FreqCharacteristics();
		}

		if (m_filt_type == 4) // считаем нормирующий коэффициент для РФ
		{
			m_in_F = 0;
			FreqCharacteristics();
		}

		// нормирование коэффициентов фильтра:
		for (int n = 0; n <= m_order; n++)
		{
			m_BUFF_WIND_CX.m_buff[n] = m_BUFF_WIND_CX.m_buff[n] / m_Km;
		}

		// Возвращаем нормальные настройки коэфф. компенсации:
		m_in_F = m_Fn;
		FreqCharacteristics();
	}

	// Т.к. окно нам больше не нужно, удаляем его:
	m_WIND_FCN.deallocate();

	return 0;
}
//------------------------------------------------------------------------------
//функция расчета АЧХ и ФЧХ фильтра:
int FIR_filt::FreqCharacteristics()
{

	//Компенсация АЧХ и ФЧХ КИХ фильтра:
	double Re = 0;
	double Im = 0;

	//Надо рассчитать коэффициент ослабления спектра:
	for (int n = 0; n < m_order + 1; n++)
	{
		//Для частоты m_in_F Гц:
		Re = Re + cos(-PI2 * n * m_in_F * m_Ts) * m_BUFF_WIND_CX.m_buff[n];
		Im = Im + sin(-PI2 * n * m_in_F * m_Ts) * m_BUFF_WIND_CX.m_buff[n];
	}

	m_pH = atan2(Im, Re);
	m_Km = sqrt(Re * Re + Im * Im);

	return 0;
};
//------------------------------------------------------------------------------
// Выделение памяти:
int FIR_filt::allocate()
{
	// выделение памяти под коэффициенты КИХ фильтра:
	m_BUFF_WIND_SX.allocate(true );   // буффер под выборку с имитацией стека
	m_BUFF_WIND_CX.allocate(false);  // буффер под коэффициенты без имитации стека

	// расчет коэффициентов фильтра:
	CoeffCalc();
	
	return 0;
}
//------------------------------------------------------------------------------
// Освобождение памяти:
int FIR_filt::deallocate()
{
	m_BUFF_WIND_SX.deallocate();   // буффер под выборку
	m_BUFF_WIND_CX.deallocate();  // буффер под коэффициенты

	return 0;
}
//------------------------------------------------------------------------------
// Показать спецификацию:
int FIR_filt::ShowFiltSpec() 
{
	switch (m_filt_type)
	{
	case 1: // ФНЧ

		std::cout << "type   - " << " LowPass " << "\n";
		std::cout << "order  = " << m_order << "\n";
		std::cout << "F_stop = " << m_Fstop * m_Fs << "\n";

		std::cout << "\n" << "\n";

		break;

	case 2: // ФВЧ

		std::cout << "type   - " << " HighPass " << "\n";
		std::cout << "order  = " << m_order << "\n";
		std::cout << "F_stop = " << m_Fstop * m_Fs << "\n";

		std::cout << "\n" << "\n";

		break;

	case 3: // ПФ

		std::cout << "type    - " << " BandPass " << "\n";
		std::cout << "order   = " << m_order << "\n";
		std::cout << "F_stop1 = " << m_Fstop1 * m_Fs << "\n";
		std::cout << "F_stop2 = " << m_Fstop2 * m_Fs << "\n";

		std::cout << "\n" << "\n";

		break;

	case 4: // ПФ

		std::cout << "type    - " << " BandStop " << "\n";
		std::cout << "order   = " << m_order      << "\n";
		std::cout << "F_stop1 = " << m_Fstop1 * m_Fs     << "\n";
		std::cout << "F_stop2 = " << m_Fstop2 * m_Fs << "\n";

		std::cout << "\n" << "\n";

		break;

	case 5: // КФ

		std::cout << "type    - " << " SineFilter "   << "\n";
		std::cout << "order   = " << m_order << "\n";

		std::cout << "\n" << "\n";

		break;

	case 6: // СФ

		std::cout << "type    - " << " CosineFilter " << "\n";
		std::cout << "order   = " << m_order << "\n";

		std::cout << "\n" << "\n";

		break;
	}

	return 0;

}
//------------------------------------------------------------------------------
// Показать коэффициенты фильтра:
int FIR_filt::ShowFiltCoeffs() 
{
	for (int n = 0; n < m_BUFF_WIND_CX.getBuffSize(); n++) 
	{
		std::cout << "coeff[" << n << "]" << " = " << m_BUFF_WIND_CX.m_buff[n] << "\n";
	}

	std::cout << "\n" << "\n";

	return 0;
}
//------------------------------------------------------------------------------
// Функция каскадирования КИХ-фильтров:
int FIR_filt::Cascade( float *a , int Na )
{
  //*a  - указатель на массив с коэффициентами фильтра, с которым данный фильтр соединяется в каскад
  // Na - размерность массива *а
	
  // линейная свертка:
  int N = m_order + Na + 1; // порядок КИХ-фильтра всегда на 1 больше из-за симметрии коэффициентов !!!
  float *c = new float[N];
  for(int n = 0; n < N; n++) c[n] = 0;
  special_functions::conv(a , m_BUFF_WIND_CX.m_buff , c , Na , m_order + 1 , N , true);
  m_BUFF_WIND_CX.deallocate();
  m_BUFF_WIND_SX.deallocate();
  m_BUFF_WIND_CX.BuffInit(N-1);
  m_BUFF_WIND_SX.BuffInit(N);
  m_BUFF_WIND_CX.allocate(false);
  m_BUFF_WIND_SX.allocate(true);
  for(int n = 0; n < N-1; n++) m_BUFF_WIND_CX.m_buff[n] = c[n];
  m_order = N - 2;
  delete [] c;

  return 0;
}
