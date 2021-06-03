// A.M. Tykvinsky, 19.10.2020

#include "quad_mltpx.hpp"

// ВЫЧИСЛИТЕЛЬ РЗиА НА БАЗЕ КВАДРАТУРНОГО УМНОЖИТЕЛЯ
// вычислитель производит вычисление ортогональных составляющих и частоты вращения вектора
//-----------------------------------------------------------------------------------------------------------------------------------------------
// конструктор по умолчанию:
quad_mltpx::quad_mltpx() 
{
	// Fs              - частота дискретизации АЦП
	// Fn              - номинальная частота сети ( 50 Гц )
	// SideLobeAtten   - уровень боковых лепестков спектра, Дб
	// FiltWindowWidth - ширина окна фильтра                  (данную настройку нужно вынести в DiCON вместо QBuff   )
	// CycleWidth      - длительность такта работы устройства (данную настройку нужно вынести в DiCON вместо IrqTime )
	
	// инициализация полей:
	m_Fs               = 4000;
	m_Ts               = 1 / m_Fs;
	m_Fn               = 50;
	m_F                = m_Fn;
	m_HBuffSize        = m_Fs / 1000 * 5;
	m_SamplesPerPeriod = m_Fs / m_Fn;
	m_K                = m_Fn * (double)m_SamplesPerPeriod / (double)m_HBuffSize / PI2;
	
	// Переменные опорных сигналов:
	m_cnt 			   = 0;
	m_ElemNum          = m_SamplesPerPeriod - 1;
	
	// инициализации фильтра апериодической слагающей:
	m_APFILT.filtInit(m_Fs , m_Fn);
	
	//------------------------------------------------------------------------------------------
	// инициализация линейной свертки полиномов входного фильтра и фильтра квадратуных компонент
	m_order_eq = 95 + (m_Fs / m_Fn / 2 + 1);
	/*if( m_order_eq % 2 != 0 )
	{
		m_order_eq = FiltWindowWidth + (m_Fs / m_Fn / 2 + 1) + 1;
	}*/
	//------------------------------------------------------------------------------------------
	
	// инициализация фильтров квадратутрных компонент (настройки по умолчанию):
	m_FIR_Re.LP_Init (m_Fs , m_Fn , 0.1 , 95 , true);
	m_FIR_Im.LP_Init (m_Fs , m_Fn , 0.1 , 95 , true);
	m_FIR_Re.m_WIND_FCN.Chebyshev(60);
	m_FIR_Im.m_WIND_FCN.Chebyshev(60);
	
	// инициализация фильтра скольжения (настройки по умолчанию):
	m_FIR_dF.LP_Init (m_Fs , m_Fn , 5 , 10 * (m_Fs / m_HBuffSize / 250) , true); // порядок фильтра скольжения пересчитывается в зависимости от текущей длительности такта относительно такта длинной в 4 мс
	m_FIR_dF.m_WIND_FCN.Chebyshev(60);
	
	// инициализация буфферов под орт.составляющие для расчета скольжения:
	m_BUFF_Re.BuffInit(2);
	m_BUFF_Im.BuffInit(2);

	// инициализация выходных переменных (начальные условия):
	m_Re  = 0;
	m_Im  = 0;
	m_F   = m_Fn;
	m_dF  = 0;
	
	// Начальные значения коэффициентов компенсации фазового и амплитудного искажения:
	m_A1 = 0;
	m_B1 = 0;
	m_C1 = 1;
	m_A2 = 0;
	m_B2 = 0;
	m_C2 = 1;
	m_Km = 1;
	
	// инициализация указателей на массивы с опорным косинусом и синусом:
	m_BUFF_COS = NULL;
	m_BUFF_SIN = NULL;

	m_BUFF_out1.BuffInit(m_HBuffSize);
	m_BUFF_out2.BuffInit(m_HBuffSize);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------
// деструктор:
quad_mltpx::~quad_mltpx() {}
//-----------------------------------------------------------------------------------------------------------------------------------------------
// функция инициализации вычислителя:
int quad_mltpx::procInit(double Fs, double Fn, double SideLobeAtten , int FiltWindowWidth , int CycleWidth)
{
	
	// Fs              - частота дискретизации АЦП
	// Fn              - номинальная частота сети ( 50 Гц )
	// SideLobeAtten   - уровень боковых лепестков спектра, Дб
	// FiltWindowWidth - ширина окна фильтра                  (данную настройку нужно вынести в DiCON вместо QBuff   )
	// CycleWidth      - длительность такта работы устройства (данную настройку нужно вынести в DiCON вместо IrqTime )
	
	// инициализация полей:
	m_Fs               = Fs;
	m_Ts               = 1 / m_Fs;
	m_Fn               = Fn;
	m_F                = m_Fn;
	m_HBuffSize        = m_Fs / 1000 * CycleWidth;
	m_SamplesPerPeriod = m_Fs / m_Fn;
	m_K                = m_Fn * (double)m_SamplesPerPeriod / (double)m_HBuffSize / PI2;
	
	// Переменные опорных сигналов:
	m_cnt 			   = 0;
	m_ElemNum          = m_SamplesPerPeriod - 1;
	
	// инициализации фильтра апериодической слагающей:
	m_APFILT.filtInit(m_Fs , m_Fn);
	
	//------------------------------------------------------------------------------------------
	// инициализация линейной свертки полиномов входного фильтра и фильтра квадратуных компонент
	m_order_eq = FiltWindowWidth + (m_Fs / m_Fn / 2 + 1);
	if( m_order_eq % 2 != 0 )
	{
	    FiltWindowWidth++;
	    m_order_eq = FiltWindowWidth + (m_Fs / m_Fn / 2 + 1);
	}
	//------------------------------------------------------------------------------------------
	
	// инициализация фильтров квадратутрных компонент (настройки по умолчанию):
	m_FIR_Re.LP_Init (m_Fs , m_Fn , 0.1 , FiltWindowWidth , true);
	m_FIR_Im.LP_Init (m_Fs , m_Fn , 0.1 , FiltWindowWidth , true);
	m_FIR_Re.m_WIND_FCN.Chebyshev(SideLobeAtten);
	m_FIR_Im.m_WIND_FCN.Chebyshev(SideLobeAtten);
	
	// инициализация фильтра скольжения (настройки по умолчанию):
	m_FIR_dF.LP_Init (m_Fs , m_Fn , 5 , 10 * (m_Fs / m_HBuffSize / 250) , true); // порядок фильтра скольжения пересчитывается в зависимости от текущей длительности такта относительно такта длинной в 4 мс
	m_FIR_dF.m_WIND_FCN.Chebyshev(60);
	
	// инициализация буфферов под орт.составляющие для расчета скольжения:
	m_BUFF_Re.BuffInit(2);
	m_BUFF_Im.BuffInit(2);

	// инициализация выходных переменных (начальные условия):
	m_Re  = 0;
	m_Im  = 0;
	m_F   = m_Fn;
	m_dF  = 0;
	
	// Начальные значения коэффициентов компенсации фазового и амплитудного искажения:
	m_A1 = 0;
	m_B1 = 0;
	m_C1 = 1;
	m_A2 = 0;
	m_B2 = 0;
	m_C2 = 1;
	m_Km = 1;
	
	// инициализация указателей на массивы с опорным косинусом и синусом:
	m_BUFF_COS = NULL;
	m_BUFF_SIN = NULL;

	m_BUFF_out1.BuffInit(m_HBuffSize);
	m_BUFF_out2.BuffInit(m_HBuffSize);

	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------
// функция расчета орт.составляющих и частоты:
int quad_mltpx::process(double *input  , double intChanDelay )
{	
	// *input        - указатель на входной массив данных
	//  intChanDelay - величина межканальной задержки в секундах
	
	// расчет ортогональных составляющих:
	//for ( int n = m_HBuffSize-1 ; n > 0; n--)
	for ( int n = 0 ; n < m_HBuffSize; n++)
	{
		// удаление апериодической слагающей и второй гармоники:
		//m_APFILT.filt( &input[n] );
		
		// квадратурное умножение:
		
		// Умножение на 2 для коррекции орт.соствавляющих (см. теорию квадратурного умножителя) не выполняется, т.к.
		// коэффициент амплитудного искажения фильтра апериодической слагающей и второй гармоники уже равен 2.
		
		/*m_a = m_BUFF_SIN[m_cnt] * m_APFILT.m_out;
		m_b = m_BUFF_COS[m_cnt] * m_APFILT.m_out;*/
		
		//double in = input[n];
		m_a = m_BUFF_SIN[m_cnt] * input[n];
		m_b = m_BUFF_COS[m_cnt] * input[n];

		// условие обнуления счетчика опорной синусоиды:
		if (m_cnt < m_ElemNum) 
		{
			m_cnt++;
		}
		else 
		{
			m_cnt = 0;
		}
		
		// заполнение внутренних буфферов фильтров квадратурных компонент:
		m_FIR_Re.m_BUFF_WIND_SX.fill_buff( &m_a );
		m_FIR_Im.m_BUFF_WIND_SX.fill_buff( &m_b );
		
		// вызов функции фильтрации фильтров квадратурных компонент:
		//m_FIR_Re.filt();
		//m_FIR_Im.filt();

		//m_BUFF_out1.fill_buff( &m_FIR_Re.m_out );
		//m_BUFF_out2.fill_buff( &m_FIR_Im.m_out );

	}
	
	// вызов функции фильтрации фильтров квадратурных компонент:
	//m_FIR_Re.filt();
	//m_FIR_Im.filt();

	//m_FIR_Re.filt_sym();
	//m_FIR_Im.filt_sym();

	//m_Re = m_FIR_Re.m_out;
	//m_Im = m_FIR_Im.m_out;


	// расчет вектора:
	m_CPLX_OPERATIONS.mul(m_FIR_Re.m_out , m_FIR_Im.m_out , 1 , -PI2 * m_F * intChanDelay  ); // межканальная задержка очень маленькая, поэтому cos = 1, sin = PI2 * m_F * intChanDelay
	m_Re  =  m_CPLX_OPERATIONS.m_Re; // деление на sqrt(2) учтено в опорных синусе и косинусе ( см. реализацию функции allocate ) !!!
	m_Im  =  m_CPLX_OPERATIONS.m_Im;
	
	// расчет скольжения:
	m_BUFF_Re.fill_buff( &m_Re  );
	m_BUFF_Im.fill_buff( &m_Im  );	
	m_CPLX_OPERATIONS.mul( m_BUFF_Re.m_ptr_fill_down[1], m_BUFF_Im.m_ptr_fill_down[1], m_BUFF_Re.m_ptr_fill_down[0], -m_BUFF_Im.m_ptr_fill_down[0] );
	m_dF = atan2( m_CPLX_OPERATIONS.m_Im , m_CPLX_OPERATIONS.m_Re ) * m_K;	
	m_FIR_dF.filt( &m_dF );
	m_dF = m_FIR_dF.m_out;
	
	// расчет частоты:
	m_F = m_Fn + m_dF;
	
	// частотная коррекция замеров:
	
	if( fabs(m_dF) < 5.5 )
	{
		// частотная коррекция производится только в рабочем диапазоне частот [ 45 ; 55 ] Гц. 
		// При этом, допустимое скольжение берется с запасом для того, чтобы избежать погрешности, обусловленной флуктуациями частоты на краях рабочего диапазона.
		
		//m_Km = (2 - ( m_A1 * m_dF * m_dF + m_B1 * fabs(m_dF) + m_C1 ) ) * (2 - ( m_A2 * m_dF * m_dF + m_B2 * fabs(m_dF) + m_C2 ) );
		
		m_Km = (2 - ( m_A2 * m_dF * m_dF + m_B2 * fabs(m_dF) + m_C2 ) );
		
	}
	else
	{
		//m_Km = (2 - ( m_A1 * 25 + m_B1 * 5 + m_C1 ) ) * (2 - ( m_A2 * 25 + m_B2 * 5 + m_C2 ) );
		m_Km = (2 - ( m_A2 * 25 + m_B2 * 5 + m_C2 ) );
	}
	
	m_Re = m_Re * m_Km; // частотная коррекция ортогональной составляющей по оси X
	m_Im = m_Im * m_Km; // частотная коррекция ортогональной составляющей по оси Y
	
	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------
// функция выделения памяти:
int quad_mltpx::allocate()
{
	// выделение памяти под фильтры:
	m_FIR_Re .allocate();
	m_FIR_Im .allocate();
	m_FIR_dF .allocate();
	m_APFILT .allocate();

	m_BUFF_out1.allocate(false);
	m_BUFF_out2.allocate(false);
	
	// выделение памяти:
	m_BUFF_Re.allocate(true); // выделение памяти реализуется под кольцевой буффер с имитацией стека (см. исходники класса mirror_ring_buff)
	m_BUFF_Im.allocate(true);
	
	// буфферы под опорный синус и косинус:
	m_BUFF_COS = new float [ m_SamplesPerPeriod ];
	m_BUFF_SIN = new float [ m_SamplesPerPeriod ];
	
	// заполнение буфферов с опорными синусом и косинусом:
	for( int n = 0; n < m_SamplesPerPeriod ; n++)
	{
		m_BUFF_SIN[n] = sin( PI2 * m_Fn * (float)n / m_Fs) / sqrt(2);
		m_BUFF_COS[n] = cos( PI2 * m_Fn * (float)n / m_Fs) / sqrt(2);
	}
	
	//----------------------------------------------------------------------------------------------------------------------
	// линейная свертка полинмов входного фильтра и фильтра квадратуных компонент:
	//----------------------------------------------------------------------------------------------------------------------
	// инициализация импульсной характеристики входного фильтра:
	int order_apfilt = (m_Fs / m_Fn / 2 + 1 );
	float *apfilt_pr = new float[order_apfilt];
	
	for( int n = 0 ; n < order_apfilt; n++ )
	{
		if( (n == 0) || ( n == order_apfilt - 1 ) )
		{
			apfilt_pr[n] = 1;
		}
		else
		{
			apfilt_pr[n] = 0;
		}
	}
	
	// инициализация массива с коэффициентами импульсной характеристики эквивалента:
	m_FIR_Re.Cascade(apfilt_pr , order_apfilt);
	m_FIR_Im.Cascade(apfilt_pr , order_apfilt);
	
	delete[] apfilt_pr;

	//----------------------------------------------------------------------------------------------------------------------
	
	
	//----------------------------------------------------------------------------------------------------------------------
	// Расчет коэффициентов амплитудной коррекции замера:
	//----------------------------------------------------------------------------------------------------------------------
	// Начальные условия:
	//----------------------------------------------------------------------------------------------------------------------
	double dF1 = 5 ; // скольжение 1
	double dF2 = 10; // скольжение 2
	double Km1 = 0 ; // коэффициент амплитудного искажения фильтра при скольжении 1
	double Km2 = 0 ; // коэффициент амплитудного искажения фильтра при скольжении 2
	
	//----------------------------------------------------------------------------------------------------------------------
	// коэффициент амплитудной коррекции фильтра квадратурных компонент:
	//----------------------------------------------------------------------------------------------------------------------
	// расчет коэффициента АЧХ при скольжении dF1:
	m_FIR_Re.m_in_F = dF1;
	m_FIR_Re.FreqCharacteristics();
	Km1 = m_FIR_Re.m_Km * 0.5;
	
	// расчет коэффициента АЧХ при скольжении dF2:
	m_FIR_Re.m_in_F = dF2;
	m_FIR_Re.FreqCharacteristics();
	Km2 = m_FIR_Re.m_Km * 0.5;
	
	//----------------------------------------------------------------------------------------------------------------------
	// коэффициент амплитудной коррекции каскада "входной фильтр - фильтр квадратуных комопонент":
	//----------------------------------------------------------------------------------------------------------------------
	// расчет коэффициентов компенсации амплитудного искажения:
	m_A2 = ( Km1 * ( dF1 - dF2 ) - (Km1 - Km2) * dF1 - (dF1 - dF2) ) / ( dF1*dF1*(dF1 - dF2) - dF1*(dF1*dF1 - dF2*dF2) );
	m_B2 = ( (Km1 - Km2) - m_A2 * ( dF1 * dF1 - dF2 * dF2 ) ) / ( dF1 - dF2 );
	m_C2 = 1;
	
	//----------------------------------------------------------------------------------------------------------------------
	// коэффициент амплитудной коррекции входного фильтра:
	//----------------------------------------------------------------------------------------------------------------------
	// расчет коэффициента АЧХ при скольжении dF1:
	/*m_APFILT.m_in_F = m_Fn + dF1;
	m_APFILT.FreqCharacteristics();
	Km1 = m_APFILT.m_Km;
	
	// расчет коэффициента АЧХ при скольжении dF2:
	m_APFILT.m_in_F = m_Fn + dF2;
	m_APFILT.FreqCharacteristics();
	Km2 = m_APFILT.m_Km;
	
	// расчет коэффициентов компенсации амплитудного искажения:
	m_A1 = ( Km1 * ( dF1 - dF2 ) - (Km1 - Km2) * dF1 - (dF1 - dF2) ) / ( dF1*dF1*(dF1 - dF2) - dF1*(dF1*dF1 - dF2*dF2) );
	m_B1 = ( (Km1 - Km2) - m_A1 * ( dF1 * dF1 - dF2 * dF2 ) ) / ( dF1 - dF2 );
	m_C1 = 1; 
	//----------------------------------------------------------------------------------------------------------------------
	// коэффициент амплитудной коррекции фильтра квадратурных компонент:
	//----------------------------------------------------------------------------------------------------------------------
	// расчет коэффициента АЧХ при скольжении dF1:
	m_FIR_Re.m_in_F = dF1;
	m_FIR_Re.FreqCharacteristics();
	Km1 = m_FIR_Re.m_Km;
	
	// расчет коэффициента АЧХ при скольжении dF2:
	m_FIR_Re.m_in_F = dF2;
	m_FIR_Re.FreqCharacteristics();
	Km2 = m_FIR_Re.m_Km;
	
	//----------------------------------------------------------------------------------------------------------------------
	// коэффициент амплитудной коррекции каскада "входной фильтр - фильтр квадратуных комопонент":
	//----------------------------------------------------------------------------------------------------------------------
	// расчет коэффициентов компенсации амплитудного искажения:
	m_A2 = ( Km1 * ( dF1 - dF2 ) - (Km1 - Km2) * dF1 - (dF1 - dF2) ) / ( dF1*dF1*(dF1 - dF2) - dF1*(dF1*dF1 - dF2*dF2) );
	m_B2 = ( (Km1 - Km2) - m_A2 * ( dF1 * dF1 - dF2 * dF2 ) ) / ( dF1 - dF2 );
	m_C2 = 1; */
	//----------------------------------------------------------------------------------------------------------------------
	
	
	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------
// функци¤ освобождения памяти:
int quad_mltpx::deallocate()
{
	// освобождение пам¤ти из под фильтров:
	m_FIR_Re  .deallocate();
	m_FIR_Im  .deallocate();
	m_FIR_dF  .deallocate();
	m_APFILT  .deallocate();

	m_BUFF_out1.deallocate();
	m_BUFF_out2.deallocate();
	
	// освобождение памяти из под опорных синуса и косинуса:
	if( m_BUFF_SIN != NULL ) delete[] m_BUFF_SIN;
	if( m_BUFF_COS != NULL ) delete[] m_BUFF_COS;

	// освобождение памяти из под буфферов:
	m_BUFF_Re  .deallocate();
	m_BUFF_Im  .deallocate();

	return 0;
}
