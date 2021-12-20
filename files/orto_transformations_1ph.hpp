// A.M. Tykvinsky, 28.11.2020
//-----------------------------------------------------------
// ќƒЌќ‘ј«Ќџ≈ ќ–“ќ√ќЌјЋ№Ќџ≈ ѕ–≈ќЅ–ј«ќ¬јЌ»я:
//-----------------------------------------------------------

#ifndef _ORTO_TRANSFORMATIONS_1PH_HPP
#define _ORTO_TRANSFORMATIONS_1PH_HPP

#include <math.h>

// исходные файлы вложенных функциональных блоков:
#include "mirror_ring_buff_x32.hpp"
#include "wind_fcn.hpp"
#include "math_const.hpp"
#include "cplx_operations.hpp"

class orto_transformations_1ph
{
private:

	float m_Ts;					// период дискретизации
	float m_Fs;				   // частота дискретизаци ј÷ѕ
	float m_Fn;				  // номинальна¤ частота
	float m_Gain;				 // нормирующий множитель
	float m_a;					// промежуточна¤ переменна¤, обозначающа¤ нефильтрованную квадратурную компоненту по оси X в квадратурном умножителе
	float m_b;				   // промежуточна¤ переменна¤, обозначающа¤ нефильтрованную квадратурную компоненту по оси Y в квадратурном умножителе
	float m_d;				  // промежуточна¤ переменна¤, обозначающа¤ фильтрованную квадратурную компоненту по оси X в квадратурном умножителе
	float m_q;				 // промежуточна¤ переменна¤, обозначающа¤ фильтрованную квадратурную компоненту по оси Y в квадратурном умножителе
	int    m_cnt;           // счетчик опорного синуса и косинуса
	float  m_Ns;           // число точек на период сигнала частотой m_Fn
	float  m_order;       // пор¤док фильтра (ширина окна наблюдени¤)
	int    m_filt_type;  // тип фильтра: 1 - ‘урье, 2 - ’артли
	int    m_ElemNum1;
	int    m_ElemNum2;
	bool   m_mode;

public:

	// Ѕуфферы под выборку и коэффициенты фильтра: 	
	mirror_ring_buff m_BUFF_WIND_CX;    // массив под коэффициенты косинусного фильтра в блоке ‘урье
	mirror_ring_buff m_BUFF_WIND_CY;   // массив под коэффициенты синусного фильтра в блоке ‘урье
	mirror_ring_buff m_BUFF_WIND_SX;  // массив под входную выборку синусного   фильтра
	mirror_ring_buff m_BUFF_WIND_SY;  // массив под входную выборку косинусного фильтра

	// входные переменные:
	float m_in_F;

	//выходные переменные:
	float m_Re;    // ортогональна¤ составл¤юща¤ по оси X
	float m_Im;   // ортогональна¤ составл¤юща¤ по оси Y
	float m_Km;  // коэффициент ј„’ фильтра
	float m_pH; // коэффициент ‘„’ фильтра

	// оконна¤ функци¤:
	wind_fcn         m_WIND_FCN;

	// функци¤ инициализации:
	int DFT_Init(float Fs, float Fn);              // инициализаци¤ дискретного преобразовани¤ ‘урье
	int DHT_Init(float Fs, float Fn);              // инициализаци¤ дискретного преобразовани¤ ’артли
	int DQT_Init(float Fs, float Fn , bool mode);  // инициализаци¤ дискретного квадратурного преобразовани¤
	int DFT_Init(float Fs, float Fn , int order);  // инициализаци¤ дискретного преобразовани¤ ‘урье  с заданием ширины окна наблюдени¤
	int DHT_Init(float Fs, float Fn , int order);  // инициализаци¤ дискретного преобразовани¤ ’артли с заданием ширины окна наблюдени¤

	// функци¤ расчета ј„’ и ‘„’ фильтра:
	int FreqCharacteristics();

	// функци¤ расчета коэффициентов фильтра:
	int CoeffCalc();

	// функци¤ выделени¤ пам¤ти:
	int allocate();

	// функци¤ освобождени¤ пам¤ти:
	int deallocate();

	// конструктор:
	orto_transformations_1ph();

	// конструктор:
	~orto_transformations_1ph();

	// встроенные функции:
	//------------------------------------------------------------------------------
	// ‘ункци¤ фильтрации с заполнением буффера:
	inline int filt(double *in)
	{
		if ( m_filt_type != 3 ) // если в качестве фильтра не используетс¤ фильтр квадратурных компонент, то: 
		{
			// заполнение буффера:
			m_BUFF_WIND_SX.fill_buff(in);

			// обнуление выходных значений:
			m_Re = 0;
			m_Im = 0;

			// дискретное ортогональное преобразование:
			for (int n = m_order - 1; n >= 0; n--)
			{
				m_Re += m_BUFF_WIND_SX.m_ptr_fill_down[n + 1] * m_BUFF_WIND_CX.m_buff[n];
				m_Im += m_BUFF_WIND_SX.m_ptr_fill_down[n + 1] * m_BUFF_WIND_CY.m_buff[n];
			}
		}
		else // если же используетс¤ фильтр квадратурных компонент, то:
		{
			// выполнение квадратурного умножени¤:
			m_a = m_BUFF_WIND_CY.m_buff[m_cnt] * (*in);
			m_b = m_BUFF_WIND_CX.m_buff[m_cnt] * (*in);

			// заполнение буффера под квадратурные компоненты:
			m_BUFF_WIND_SX.fill_buff(&m_a);
			m_BUFF_WIND_SY.fill_buff(&m_b);

			// фильтраци¤ квадратурных компонент:
			m_d = (m_a - m_BUFF_WIND_SX.m_ptr_fill_down[m_ElemNum1]) * m_Gain + m_d;
			m_q = (m_b - m_BUFF_WIND_SY.m_ptr_fill_down[m_ElemNum1]) * m_Gain + m_q;

			// развертка в мгновенные значени¤ ортогоналей:
			if (m_mode == true) 
			{
				m_Im = m_d * m_BUFF_WIND_CY.m_buff[m_cnt] + m_q * m_BUFF_WIND_CX.m_buff[m_cnt];
				m_Re = m_d * m_BUFF_WIND_CX.m_buff[m_cnt] - m_q * m_BUFF_WIND_CY.m_buff[m_cnt];
			}
			else
			{
				m_Re = m_d;
				m_Im = m_q;
			}

			// условие обнулени¤ счетчика опорной синусоиды:
			if (m_cnt < m_ElemNum2)
			{
				m_cnt++;
			}
			else
			{
				m_cnt = 0;
			}
		}

		return 0;
	}
	//------------------------------------------------------------------------------
	// ‘ункци¤ фильтрации без заполнени¤ буффера:
	inline int filt()
	{
		// функци¤ предполагает, что буффер заполн¤етс¤ внешне (см. hpp - файл !!!)
		// обнуление выходных значений:
		m_Re = 0;
		m_Im = 0;
		
		// дискретное ортогональное преобразование:
		for (int n = m_order - 1; n >= 0; n--) 
		{
			m_Re += m_BUFF_WIND_SX.m_ptr_fill_down[n+1] * m_BUFF_WIND_CX.m_buff[n];
			m_Im += m_BUFF_WIND_SX.m_ptr_fill_down[n+1] * m_BUFF_WIND_CY.m_buff[n];
		}

		return 0;
	}
	//------------------------------------------------------------------------------

};

#endif
