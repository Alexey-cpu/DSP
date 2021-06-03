//#pragma once

#ifndef _SIMPLE_IIR_HPP
#define _SIMPLE_IIR_HPP

// заголовки стандартной библиотеки C++:
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Заголовки пользовательских библиотек:
#include "mirror_ring_buff_x32.hpp"
#include "math_const.hpp"

// ПРОСТЕЙШИЕ БИХ - ФИЛЬТРЫ

class simple_IIR
{

private:

	// Системные переменные класса:
	double    m_Ts;                     // период дискретизации АЦП
	double    m_Fs;                    // частота дискретизации АЦП
	double    m_Fn;                   // номинальная частота
	double    m_T1;                  // постоянная времени 1
	double    m_T2;                 // постоянная времени 2
	double	  m_Fc;                // частота среза фильтра
	double    m_Fb;               // ширина полосы заграждения (для РФ)
	double	  m_Kd;              // коэффицент демпфирования
	double    m_Gain;           // коэффициент усиления фильтра на нулевой частоте
	double	  m_sum_num;       // накапливаемая сумма входа разностного уравнения фильтра
	double	  m_sum_den;      // накапливаемая сумма выхода разностного уравнения фильтра
	int       m_filt_type;   // тип фильтра
	int       m_N;          // к - во элементов полинома числителя
	int       m_Nx;        // к - во элементов полинома числителя
	int       m_Ny;       // к - во элементов полинома знаменателя
	int       m_ElemNum;

	// Значения, которые может принимать переменная m_filt_type:
	// m_filt_type = 1  - реальное дифференцирующее звено
	// m_filt_type = 2  - апериодическое звено
	// m_filt_type = 3  - фазосдвигающее звено
	// m_filt_type = 4  - интегрирующее звено
	// m_filt_type = 5  - ФНЧ второго порядка
	// m_filt_type = 6  - ФВЧ второго порядка
	// m_filt_type = 7  - ПФ  второго порядка
	// m_filt_type = 8  - РФ второго порядка 1 ( билинейное преобразование аналогового прототипа )
	// m_filt_type = 9  - РФ второго порядка 2 ( проектирование режекторного фильтра на базе всепропускающего )
	// m_filt_type = 10 - сглаживающий рекурсинвый фильтр с линейной ФЧХ
	// m_filt_type = 11 - дифференцирующий фильтр второго порядка

public:

	mirror_ring_buff m_BUFF_SX;    // буффер под входные значения
	mirror_ring_buff m_BUFF_SY;   // буффер под выходные значения фильтра
	mirror_ring_buff m_BUFF_CX;  // буффер под коэффициенты числителя передаточной функции фильтра
	mirror_ring_buff m_BUFF_CY; // буффер под коэффициенты знаменателя передаточной функции фильтра

	// входные переменные фильтра:
	double m_in_F;   // частота входного сигнала

	// выходные переменные фильтра:
	double m_out;  // выход фильтра
	double m_Km;  // коэффициент АЧХ фильтра на частоте m_in_F
	double m_pH; // коэффициент ФЧХ фильтра на частоте m_in_F

	// инициализация типовых динамических звеньев:
	int DIF_Init(double Fs , double Fn , double T1);               // реальное дифференцирующее звено
	int EXP_Init(double Fs , double Fn , double T1);              // апериодическое звено первого порядка
	int LEG_Init(double Fs , double Fn , double T1 , double T2); // фазосдвигающее звено
	int INT_Init(double Fs, double Fn);                         // интегрирующее звено

	// ининциализация фильтров второго порядка:
	int LP_Init (double Fs, double Fn, double Kd, double Fc);     // ФНЧ второго порядка
	int HP_Init (double Fs, double Fn, double Kd, double Fc);    // ФВЧ второго порядка
	int BP_Init (double Fs, double Fn, double Kd, double Fc);   // ПФ второго порядка
	int BS1_Init(double Fs, double Fn, double Kd, double Fc);  // РФ второго порядка 1 ( билинейное преобразование аналогового прототипа )
	int BS2_Init(double Fs, double Fn, double Fc, double Fb); // РФ второго порядка 2 ( проектирование режекторного фильтра на базе всепропускающего )

	// инициализация сглаживающего рекурсивного фильтра с линейной ФЧХ:
	int SM_Init(double Fs, double Fn , int order);
	
	// ининциализация дифференцирущего фильтра второго порядка:
	int DIFF2_init( double Fs , double Fn );

	// функция расчета коэффициентов фильтра:
	int CoeffCalc();

	// функция расчета АЧХ и ФЧХ фильтра:
	int FreqCharacteristics();

	// конструктор:
	simple_IIR();

	// деструктор:
	~simple_IIR();

	// функция выделения памяти:
	int allocate();

	// функция освобождения памяти:
	int deallocate();

	// ВСТРОЕННЫЕ ФУНКЦИИ:
	//------------------------------------------------------------------------------
	// Функция фильтрации:
	//------------------------------------------------------------------------------
	inline int filt(double *in)
	{
		// *in      - указатель на массив со входной выборкой
		
		switch(m_filt_type)
		{
			case 10: // сглаживающий рекурсивный фильтр
			
				// заполнение внутреннего буффера фильтра:
				m_BUFF_SX.fill_buff(in);
				// фильтрация:
				m_out = m_Gain * (*in - m_BUFF_SX.m_ptr_fill_down[m_ElemNum]) + m_out;
				
			break;
			
			case 11: // дифференцирующий фильтр второго порядка
			
				// заполнение внутреннего буффера фильтра:
				m_BUFF_SX.fill_buff(in);
				// фильтрация:
				m_out = ( *in - m_BUFF_SX.m_ptr_fill_down[m_ElemNum] ) * m_Kd;
				
			break;
			
			default: // все остальные фильтры
			
				m_sum_num = 0;  // сумма числителя передаточной функции
				m_sum_den = 0; // сумма знаменателя передаточной функции

				// копируем значение во внутренний буффер фильтра:
				m_BUFF_SX.fill_buff(in);

				for (int m = 0; m < m_N; m++)
				{
					// считаем сумму числителя передаточной функции фильтра:
					m_sum_num = m_sum_num + m_Gain * (m_BUFF_SX.m_ptr_fill_down[m + 1] * m_BUFF_CX.m_buff[m]);

					// считаем сумму знаменателя передаточной функции фильтра:
					if (m < m_Ny) m_sum_den = m_sum_den + m_BUFF_SY.m_ptr_fill_down[m + 1] * m_BUFF_CY.m_buff[m + 1];
				}

				// решаем разностное уравнение:
				m_out = m_sum_num - m_sum_den;

				// записываем решение в текущий массив выходных значений:
				m_BUFF_SY.fill_buff(&m_out);
				
			break;
		}

		return 0;
	}
	//------------------------------------------------------------------------------

};

#endif
