//--------------------------------------------------------------------------------
//A.M.Tykvinsky, 30.09.2020
//--------------------------------------------------------------------------------
// ЭЛЛИПТИЧЕСКИЙ ФИЛЬТР
//--------------------------------------------------------------------------------

#ifndef _ELLIPTIC_HPP
#define _ELLIPTIC_HPP

#include <math.h>
#include <stdio.h>
#include <iostream>

#include "mirror_ring_buff_x32.hpp"
#include "mirror_ring_buff_array.hpp"
#include "math_const.hpp"
#include "special_functions.hpp"
#include "cplx_operations.hpp"

// ФНЧ - фильтр нижних  частот
// ФВЧ - фильтр верхних частот
// ПФ  - фильтр полосовой
// РФ  - фильтр режекторный

class Elliptic
{

protected:

	double m_sum_num;                 // сумма числителя  (для разностного уравнения) 
	double m_sum_den;                // сумма знаменателя (для разностного уравнения)
	double m_Ts;                    // период дискретизации
	double m_Fs;	               // частота дискретизаци АЦП
	double m_Fn;                  // номинальная частота
	double m_F_stop;             // частота подавления
	double m_F_pass;            // частота среза
	double m_F_stop1;          // частота подавления 1 ( для ПФ и РФ )
	double m_F_stop2;         // частота подавления 2 ( для ПФ и РФ )
	double m_epsilon_stop;   // неравномерность в полосе подавления (в о.е. )
	double m_epsilon_pass;  // неравномерность в полосе пропускания (в о.е. )
	double m_alpha;        // вспомогательный коэффициент альфа
	double m_betta;       // вспомогательный коэффициент бетта
	double m_K;          // поправочный коэффициент (см. исходник)
	double m_Fs1;       // "искаженная" нижняя граница полосы пропускания/заграждения
	double m_Fs2;      // "искаженная" верхняя граница полосы пропускания/заграждения

	double m_Kp;
	double m_Ke;
	double m_Kw;
	double m_output_Gain;

	int    m_L;            // к - во кратных полюсов и нулей
	int    m_R;           // к - во некратных полюсов и нулей
	int    m_order;      // порядок фильтра
	int    m_filt_type; // тип фильтра: 1 - ФНЧ, 2 - ФВЧ, 3 - ПФ, 4 - РФ

	special_functions  m_f_ellip;       // библиотека для расчета эллиптических прямых и обратных функций Якоби ( и не только тих функций, см. special_functions.hpp )
	cplx_operations    m_CPLX_DIV;     // функция деления комплексных чисел
	cplx_operations    m_CPLX_MUL;    // функция умножения комплексных чисел
	cplx_operations    m_CPLX_ADD;   // функция сложения комплексных чисел
	cplx_operations    m_CPLX_SUB;  // функция вычитания комплексных чисел
	cplx_operations    m_CPLX_SQR; // функция извлечения квадратного корня из комплексного числа


	// Массивы кольцевых буфферов: 	
	mirror_ring_buff_array m_BUFF_ARR_WIND_CX;
	mirror_ring_buff_array m_BUFF_ARR_WIND_CY;

	mirror_ring_buff_array m_BUFF_ARR_WIND_SX;        // массив под входную выборку фильтра
	mirror_ring_buff_array m_BUFF_ARR_WIND_SY;       // массив под выходную выборку фильтра

	mirror_ring_buff m_BUFF_GAIN;                  // буффер под коэффициенты усиления квадратичных полиномов ПФ фильтра
	mirror_ring_buff m_BUFF_GAIN_POLES;           // буффер под полюса ПФ фильтра
	mirror_ring_buff m_BUFF_GAIN_ZEROS;          // буффер под нули ПФ фильтра
	mirror_ring_buff m_BUFF_ZEROS_RE;           // буффер под действительные части нулей ПФ фильтра
	mirror_ring_buff m_BUFF_ZEROS_IM;          // буффер под мнимые части нулей ПФ фильтра
	mirror_ring_buff m_BUFF_POLES_RE;         // буффер под действительные части полюсов ПФ фильтра
	mirror_ring_buff m_BUFF_POLES_IM;        // буффер под мнимые части полюсов ПФ фильтра
	mirror_ring_buff m_BUFF_BP_ZEROS_RE;    // массив под действительные части нулей полосового / заграждающего фильтра
	mirror_ring_buff m_BUFF_BP_ZEROS_IM;   // массив под мнимые части нулей полосового / заграждающего фильтра
	mirror_ring_buff m_BUFF_BP_POLES_RE;  // массив под действительные части полюсов полосового / заграждающего фильтра
	mirror_ring_buff m_BUFF_BP_POLES_IM; // массив под мнимые части полюсов полосового / заграждающего фильтра

	// функция расчета коэффициентов фильтра:
	int LPF_HPF_CoeffCalc();
	int BPF_BSF_CoeffCalc();

	// функция расчета коэффициентов степени подавления и ширины переходной полосы эллиптического фильтра ( названия коэффициентов придумал сам, в литературе ничего не нашел ):
	int supressinon_transient_factors_calc();

	// функция расчета нулей ПФ фильтра:
	int zeros_calc();

	// функция расчета полюсов ПФ фильтра:
	int poles_calc();

	// функция расчета коэффициентов усиения на нулевой частоте квадратичных полиномов фильтра:
	int gain_calc();

	// функция билинейного преобразования ПФ фильтра по ее нулям и полюсам :

	int LP_to_LP_bilinear_transform();    // ФНЧ - ФНЧ с последующим билинейным преобразованием 
	int LP_to_HP_bilinear_transform();   // ФНЧ - ФВЧ с последующим билинейным преобразованием
	int LP_to_BP_bilinear_transform();  // ФНЧ - ПФ  с последующим билинейным преобразованием
	int LP_to_BS_bilinear_transform(); // ФНЧ - РФ с последующим билинейным преобразованием

public:

	// входы фильтра:
	double    m_Km;         // Коэффициент компенсации искажения амплитуды
	double    m_pH;        // Коэффициент компенсации искажения фазы
	double    m_in_F;     // текущее значение частоты

	// выход фильтра:
	double    m_out;     // выход функции фильтрации значений в окне наблюдения фильтра

	//Конструктор:
	Elliptic();

	//Деструктор:
	~Elliptic();

	// функция расчета АЧХ и ФЧХ фильтра:
	int FreqCharacteristics();

	// функция фильтрации:
	int filt(double *in);
	
	// функция инициализации фильтра:
	int LP_Init(double Fs , double Fn , double g_pass , double g_stop , double F_stop  , int order , bool mode_g_pu , bool mode_F_pu);                     // функция инициализации инверсного ФНЧ Чебышева
	int HP_Init(double Fs , double Fn , double g_pass , double g_stop , double F_stop  , int order , bool mode_g_pu , bool mode_F_pu);                    // функция инициализации инверсного ФВЧ Чебышева
	int BP_Init(double Fs , double Fn , double g_pass , double g_stop , double F_stop1 , double F_stop2 , int order , bool mode_g_pu , bool mode_F_pu);  // функция инициализации инверсного ПФ Чебышева
	int BS_Init(double Fs , double Fn , double g_pass , double g_stop , double F_stop1 , double F_stop2 , int order , bool mode_g_pu , bool mode_F_pu); // функция инициализации инверсного РФ Чебышева

	// функция выделения памяти под окно фильтра
	int allocate();

	// функция освобождения памяти
	int deallocate();

	// функция отображения спецификации фильтра   (отладочная):
	void showFiltSpec();

	// функция отображения нулей и поюсов фильтра (отладочная, для пользования этой функией надо закоментить кусок с удалением массивов с нулями и полюсами в функции allocate, см. строку 1216 файла cpp):
	void showFiltZerosPoles();

	// функция отображения коэффициентов фильтра  (отладочная):
	void showFiltCoeffs();
};

#endif
