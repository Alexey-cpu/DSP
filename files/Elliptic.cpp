//--------------------------------------------------------------------------------
//A.M.Tykvinsky, 30.09.2020
//--------------------------------------------------------------------------------
// ЭЛЛИПТИЧЕСКИЙ ФИЛЬТР
//--------------------------------------------------------------------------------

#include "Elliptic.hpp"

// Список использованных для реализации источников:

// 1. L.Thede    ,                   " Practical analog and digital filter design "
// 2. T.W Parks  , C.S. Burrus,      " Digital Filter Design "
// 3. C.Ifeachor , Barrie W. Jervis, " Digital signal processing - a practical approach "

// Общее описание работы класса:
// Расчет эллиптического фильтра состоит из следующих этапов:
// 1. Расчет нулей и полюсов передаточной функции фильтра;
// 2. Выполнение частотного преобразования (разнормировка нормированного ФНЧ в ФНЧ, ФВЧ, ПФ или в РФ );
// 3. Билинейное преобразование получившейся в результате передаточной функции;
// 4. Расчет коэффициентов фильтра.
// Особенностью данного проекта является то, что п.3 и п.4 совмещены в одной функции и то, что передаточноая функция
// представлена в виде каскада квадратичных секций (полиномов), см. источник [1]. Также, под расчет нулей и полюсов
// передаточной функции эллиптического фильтра написан класс специальных математических функций (см. special_functions.cpp)

//------------------------------------------------------------------------------
//Конструктор (по умолчанию):
Elliptic::Elliptic() {}
//------------------------------------------------------------------------------
//Деструктор:
Elliptic::~Elliptic() {};
//------------------------------------------------------------------------------
// функция инициализации ФНЧ без решения уравнения порядка фильтра:
int Elliptic::LP_Init(double Fs, double Fn, double g_pass, double g_stop, double F_stop, int order, bool mode_g_pu, bool mode_F_pu)
{
	// Fs     - частота дискретизации
	// Fn     - номинальная частота
	// g_stop - неравномерность в полосе подавления  ( если mode_g == true, то в о.е, если mode_g == false, то в Дб )
	// F_stop - частота подавления фильтра           ( если mode_F == true, то в долях от частоты Найквиста* , если mode_F == false, то в Гц )

	//* Частота Найквиста - это половина от частоты дискретизации

	// Заполнение полей:
	m_Fs = Fs;
	m_Fn = Fn;
	m_Ts = 1 / m_Fs;

	m_filt_type = 1;

	// Расчет неравномерности в полосе подавления и пропускания:
	if (mode_g_pu == true) // если задано в о.е.
	{
		m_epsilon_stop = sqrt(1 / g_stop - 1);
		m_epsilon_pass = sqrt(1 / g_pass - 1);
	}
	else // если задано в Дб
	{
		m_epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);
		m_epsilon_pass = sqrt(pow(10, g_pass / 10) - 1);
	}

	// Расчет частот среза и подавления:
	if (mode_F_pu == true) // если задано в долях от Частоты Найквиста
	{
		m_F_stop = F_stop * m_Fs / 2;
	}
	else // если задано в Гц
	{
		m_F_stop = F_stop;
	}

	// Задание порядка фильтра:
	m_order = order;

	// Расчет поправочного коэффициента (для перехода из области непрерывного в область дисрктеного времени):
	m_K = tan(PI2 * m_F_stop * m_Ts / 2);

	// Узнаем к - во нулей и полюсов для записи ПФ фильтра через биквадратную форму:

	m_L = trunc(m_order / 2);
	m_R = m_order - 2 * m_L;

	// Суммы взвешанных отсчетов входной и выходной выборки ( для разностного уравнения цифрового фильтра):
	m_sum_num = 0;
	m_sum_den = 0;

	// Инициализация входов/выходов:
	m_in_F = 50;
	m_out = 0;

	// Расчет коэффициентов Ke , Kw, Kp:
	supressinon_transient_factors_calc();

	// инициализация буфферов:
	m_BUFF_ZEROS_RE.BuffInit(m_L + m_R);         // буффер под действительные части нулей ПФ фильтра
	m_BUFF_ZEROS_IM.BuffInit(m_L + m_R);        // буффер под мнимые части нулей ПФ фильтра
	m_BUFF_POLES_RE.BuffInit(m_L + m_R);       // буффер под действительные части полюсов ПФ фильтра
	m_BUFF_POLES_IM.BuffInit(m_L + m_R);      // буффер под мнимые части полюсов ПФ фильтра
	m_BUFF_GAIN.BuffInit(m_L + m_R);     // буффер под коэффициенты усиления квадратичных полиномов ПФ фильтра на нулевой частоте
	m_BUFF_GAIN_POLES.BuffInit(m_L + m_R);    // буффер под коэффициенты усиления полиномов знаменателя после билинейного преобразования
	m_BUFF_GAIN_ZEROS.BuffInit(m_L + m_R);   // буффер под коэффициенты усиления полиномов числителя после билинейного преобразования

	m_BUFF_ARR_WIND_SX.RingBuffArrayInit(m_L + m_R, 3 + 1); // так задумано !!!
	m_BUFF_ARR_WIND_SY.RingBuffArrayInit(m_L + m_R, 2 + 1);

	// инициализация массивов кольцевых буфферов с коэффициентами фильтра:
	m_BUFF_ARR_WIND_CX.RingBuffArrayInit(m_L + m_R, 3);  // массив кольцевых буфферов под входную выборку
	m_BUFF_ARR_WIND_CY.RingBuffArrayInit(m_L + m_R, 3); // массив кольцевых буфферов под выходную выборку

	return 0;
}
//------------------------------------------------------------------------------
// функция инициализации ФВЧ без решения уравнения порядка фильтра:
int Elliptic::HP_Init(double Fs, double Fn, double g_pass, double g_stop, double F_stop, int order, bool mode_g_pu, bool mode_F_pu)
{
	// Fs     - частота дискретизации
	// Fn     - номинальная частота
	// g_stop - неравномерность в полосе подавления  ( если mode_g == true, то в о.е, если mode_g == false, то в Дб )
	// F_stop - частота подавления фильтра           ( если mode_F == true, то в долях от частоты Найквиста* , если mode_F == false, то в Гц )

	//* Частота Найквиста - это половина от частоты дискретизации

	// Заполнение полей:
	m_Fs = Fs;
	m_Fn = Fn;
	m_Ts = 1 / m_Fs;
	m_filt_type = 2;

	// Расчет неравномерности в полосе подавления и пропускания:
	if (mode_g_pu == true) // если задано в о.е.
	{
		m_epsilon_stop = sqrt(1 / g_stop - 1);
		m_epsilon_pass = sqrt(1 / g_pass - 1);
	}
	else // если задано в Дб
	{
		m_epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);
		m_epsilon_pass = sqrt(pow(10, g_pass / 10) - 1);
	}

	// Расчет частот среза и подавления:
	if (mode_F_pu == true) // если задано в долях от Частоты Найквиста
	{
		m_F_stop = F_stop * m_Fs / 2;
	}
	else // если задано в Гц
	{
		m_F_stop = F_stop;
	}

	// Задание порядка фильтра Чебышева II рода:
	m_order = order;

	// Расчет поправочного коэффициента (для перехода из области непрерывного в область дисрктеного времени):
	m_K = tan(PI2 * m_F_stop * m_Ts / 2);

	// Узнаем к - во нулей и полюсов для записи ПФ фильтра через биквадратную форму:
	m_L = trunc(m_order / 2);
	m_R = m_order - 2 * m_L;

	// Суммы взвешанных отсчетов входной и выходной выборки ( для разностного уравнения цифрового фильтра):
	m_sum_num = 0;
	m_sum_den = 0;

	// Инициализация входов/выходов:
	m_in_F = 50;
	m_out = 0;

	// Расчет коэффициентов Ke , Kw, Kp:
	supressinon_transient_factors_calc();

	// инициализация буфферов:
	m_BUFF_ZEROS_RE.BuffInit(m_L + m_R);         // буффер под действительные части нулей ПФ фильтра
	m_BUFF_ZEROS_IM.BuffInit(m_L + m_R);        // буффер под мнимые части нулей ПФ фильтра
	m_BUFF_POLES_RE.BuffInit(m_L + m_R);       // буффер под действительные части полюсов ПФ фильтра
	m_BUFF_POLES_IM.BuffInit(m_L + m_R);      // буффер под мнимые части полюсов ПФ фильтра
	m_BUFF_GAIN.BuffInit(m_L + m_R);     // буффер под коэффициенты усиления квадратичных полиномов ПФ фильтра на нулевой частоте
	m_BUFF_GAIN_POLES.BuffInit(m_L + m_R);    // буффер под коэффициенты усиления полиномов знаменателя после билинейного преобразования
	m_BUFF_GAIN_ZEROS.BuffInit(m_L + m_R);   // буффер под коэффициенты усиления полиномов числителя после билинейного преобразования

	m_BUFF_ARR_WIND_SX.RingBuffArrayInit(m_L + m_R, 3 + 1); // так задумано !!!
	m_BUFF_ARR_WIND_SY.RingBuffArrayInit(m_L + m_R, 2 + 1);

	// инициализация массивов кольцевых буфферов с коэффициентами фильтра:
	m_BUFF_ARR_WIND_CX.RingBuffArrayInit(m_L + m_R, 3);  // массив кольцевых буфферов под входную выборку
	m_BUFF_ARR_WIND_CY.RingBuffArrayInit(m_L + m_R, 3); // массив кольцевых буфферов под выходную выборку

	return 0;
}
//------------------------------------------------------------------------------
// функция инициализации ПФ без решения уравнения порядка фильтра:
int Elliptic::BP_Init(double Fs, double Fn, double g_pass, double g_stop, double F_stop1, double F_stop2, int order, bool mode_g_pu, bool mode_F_pu)
{
	// Fs      - частота дискретизации
	// Fn      - номинальная частота
	// g_stop  - неравномерность в полосе подавления          ( если mode_g == true, то в о.е, если mode_g == false, то в Дб )
	// g_pass  - неравномерность в полосе пропускания         ( если mode_g == true, то в о.е, если mode_g == false, то в Дб )
	// F_stop1 - начало полосы пропускания полосового фильтра ( если mode_F == true, то в долях от частоты Найквиста* , если mode_F == false, то в Гц )
	// F_stop1 - конец  полосы пропускания полосового фильтра ( если mode_F == true, то в долях от частоты Найквиста* , если mode_F == false, то в Гц )

	//* Частота Найквиста - это половина от частоты дискретизации

	// Заполнение полей:
	m_Fs = Fs;
	m_Fn = Fn;
	m_Ts = 1 / m_Fs;
	m_filt_type = 3;

	// Расчет неравномерности в полосе подавления и пропускания:
	if (mode_g_pu == true) // если задано в о.е.
	{
		m_epsilon_stop = sqrt(1 / g_stop - 1);
		m_epsilon_pass = sqrt(1 / g_pass - 1);
	}
	else // если задано в Дб
	{
		m_epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);
		m_epsilon_pass = sqrt(pow(10, g_pass / 10) - 1);
	}

	// Расчет частот подавления:
	if (mode_F_pu == true) // если задано в долях от Частоты Найквиста
	{
		m_F_stop1 = F_stop1 * m_Fs / 2;
		m_F_stop2 = F_stop2 * m_Fs / 2;
	}
	else // если задано в Гц
	{
		m_F_stop1 = F_stop1;
		m_F_stop2 = F_stop2;
	}

	// Задание порядка ПФ / РФ (порядок должен быть всегдв четным числом !!!):
	m_order = order / 2;

	// Узнаем к - во нулей и полюсов для записи ПФ фильтра через биквадратную форму:
	m_L = trunc(m_order / 2);
	m_R = m_order - 2 * m_L;

	// Суммы взвешанных отсчетов входной и выходной выборки ( для разностного уравнения цифрового фильтра):
	m_sum_num = 0;
	m_sum_den = 0;

	// Инициализация входов/выходов:
	m_in_F = 50;
	m_out = 0;

	// Расчет коэффициентов Ke , Kw, Kp:
	supressinon_transient_factors_calc();

	// инициализация буфферов:
	m_BUFF_ZEROS_RE.BuffInit(m_L + m_R);       // буффер под действительные части нулей ПФ фильтра
	m_BUFF_ZEROS_IM.BuffInit(m_L + m_R);      // буффер под мнимые части нулей ПФ фильтра
	m_BUFF_POLES_RE.BuffInit(m_L + m_R);     // буффер под действительные части полюсов ПФ фильтра
	m_BUFF_POLES_IM.BuffInit(m_L + m_R);    // буффер под мнимые части полюсов ПФ фильтра
	m_BUFF_GAIN.BuffInit(2 * m_L + m_R);   // буффер под коэффициенты усиления квадратичных полиномов ПФ фильтра на нулевой частоте
	m_BUFF_GAIN_POLES.BuffInit(2 * m_L + 2 * m_R);  // буффер под коэффициенты усиления полиномов знаменателя после билинейного преобразования
	m_BUFF_GAIN_ZEROS.BuffInit(2 * m_L + 2 * m_R); // буффер под коэффициенты усиления полиномов числителя после билинейного преобразования

	//
	m_BUFF_BP_POLES_RE.BuffInit(2 * m_L + 2 * m_R);
	m_BUFF_BP_POLES_IM.BuffInit(2 * m_L + 2 * m_R);
	m_BUFF_BP_ZEROS_RE.BuffInit(2 * m_L + 2 * m_R);
	m_BUFF_BP_ZEROS_IM.BuffInit(2 * m_L + 2 * m_R);


	m_BUFF_ARR_WIND_SX.RingBuffArrayInit(2 * m_L + m_R, 3 + 1); // так задумано !!!
	m_BUFF_ARR_WIND_SY.RingBuffArrayInit(2 * m_L + m_R, 2 + 1);

	// инициализация массивов кольцевых буфферов с коэффициентами фильтра:
	m_BUFF_ARR_WIND_CX.RingBuffArrayInit(2 * m_L + m_R, 3);  // массив кольцевых буфферов под входную выборку
	m_BUFF_ARR_WIND_CY.RingBuffArrayInit(2 * m_L + m_R, 3); // массив кольцевых буфферов под выходную выборку

	return 0;
}
//------------------------------------------------------------------------------
// функция инициализации РФ без решения уравнения порядка фильтра:
int Elliptic::BS_Init(double Fs, double Fn, double g_pass, double g_stop, double F_stop1, double F_stop2, int order, bool mode_g_pu, bool mode_F_pu)
{
	// Fs      - частота дискретизации
	// Fn      - номинальная частота
	// g_stop  - неравномерность в полосе подавления           ( если mode_g == true, то в о.е, если mode_g == false, то в Дб )
	// g_pass  - неравномерность в полосе пропускания          ( если mode_g == true, то в о.е, если mode_g == false, то в Дб )
	// F_stop1 - начало полосы подавления режекторного фильтра ( если mode_F == true, то в долях от частоты Найквиста* , если mode_F == false, то в Гц )
	// F_stop1 - конец  полосы подавления режекторного фильтра ( если mode_F == true, то в долях от частоты Найквиста* , если mode_F == false, то в Гц )

	//* Частота Найквиста - это половина от частоты дискретизации

	// Заполнение полей:
	m_Fs = Fs;
	m_Fn = Fn;
	m_Ts = 1 / m_Fs;
	m_filt_type = 4;

	// Расчет неравномерности в полосе подавления и пропускания:
	if (mode_g_pu == true) // если задано в о.е.
	{
		m_epsilon_stop = sqrt(1 / g_stop - 1);
		m_epsilon_pass = sqrt(1 / g_pass - 1);
	}
	else // если задано в Дб
	{
		m_epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);
		m_epsilon_pass = sqrt(pow(10, g_pass / 10) - 1);
	}

	// Расчет частот подавления:
	if (mode_F_pu == true) // если задано в долях от Частоты Найквиста
	{
		m_F_stop1 = F_stop1 * m_Fs / 2;
		m_F_stop2 = F_stop2 * m_Fs / 2;
	}
	else // если задано в Гц
	{
		m_F_stop1 = F_stop1;
		m_F_stop2 = F_stop2;
	}

	// Задание порядка ПФ / РФ (порядок должен быть всегдв четным числом !!!):
	m_order = order / 2;

	// Узнаем к - во нулей и полюсов для записи ПФ фильтра через биквадратную форму:
	m_L = trunc(m_order / 2);
	m_R = m_order - 2 * m_L;

	// Суммы взвешанных отсчетов входной и выходной выборки ( для разностного уравнения цифрового фильтра):
	m_sum_num = 0;
	m_sum_den = 0;

	// Инициализация входов/выходов:
	m_in_F = 50;
	m_out = 0;

	// Расчет коэффициентов Ke , Kw, Kp:
	supressinon_transient_factors_calc();

	// инициализация буфферов:
	m_BUFF_ZEROS_RE.BuffInit(m_L + m_R);       // буффер под действительные части нулей ПФ фильтра
	m_BUFF_ZEROS_IM.BuffInit(m_L + m_R);      // буффер под мнимые части нулей ПФ фильтра
	m_BUFF_POLES_RE.BuffInit(m_L + m_R);     // буффер под действительные части полюсов ПФ фильтра
	m_BUFF_POLES_IM.BuffInit(m_L + m_R);    // буффер под мнимые части полюсов ПФ фильтра
	m_BUFF_GAIN.BuffInit(2 * m_L + m_R);   // буффер под коэффициенты усиления квадратичных полиномов ПФ фильтра на нулевой частоте
	m_BUFF_GAIN_POLES.BuffInit(2 * m_L + 2 * m_R);  // буффер под коэффициенты усиления полиномов знаменателя после билинейного преобразования
	m_BUFF_GAIN_ZEROS.BuffInit(2 * m_L + 2 * m_R); // буффер под коэффициенты усиления полиномов числителя после билинейного преобразования

	//
	m_BUFF_BP_POLES_RE.BuffInit(2 * m_L + 2 * m_R);
	m_BUFF_BP_POLES_IM.BuffInit(2 * m_L + 2 * m_R);
	m_BUFF_BP_ZEROS_RE.BuffInit(2 * m_L + 2 * m_R);
	m_BUFF_BP_ZEROS_IM.BuffInit(2 * m_L + 2 * m_R);


	m_BUFF_ARR_WIND_SX.RingBuffArrayInit(2 * m_L + m_R, 3 + 1); // так задумано !!!
	m_BUFF_ARR_WIND_SY.RingBuffArrayInit(2 * m_L + m_R, 2 + 1);

	// инициализация массивов кольцевых буфферов с коэффициентами фильтра:
	m_BUFF_ARR_WIND_CX.RingBuffArrayInit(2 * m_L + m_R, 3);  // массив кольцевых буфферов под входную выборку
	m_BUFF_ARR_WIND_CY.RingBuffArrayInit(2 * m_L + m_R, 3); // массив кольцевых буфферов под выходную выборку

	return 0;
}
//------------------------------------------------------------------------------
//функция расчета АЧХ и ФЧХ фильтра:
int Elliptic::FreqCharacteristics()
{
	// расчет коэффициентов комплексного коэффициента передачи:

	double Re_nom  = 0;
	double Im_nom  = 0;
	double ABS_nom = 1;
	double ARG_nom = 0;

	double Re_den  = 0;
	double Im_den  = 0;
	double ABS_den = 1;
	double ARG_den = 0;

	double Gain    = 1;

	// Расчет амплитуды и угла комплексного числа числителя:

	for (int n = 0; n < m_BUFF_ARR_WIND_CX.getRingBuffArraySize() ; n++)
	{
		Re_nom = m_BUFF_ARR_WIND_CX.m_MBUFF[n].m_buff[0] + cos(-PI2 * m_in_F * m_Ts) * m_BUFF_ARR_WIND_CX.m_MBUFF[n].m_buff[1] + cos(-PI2 * m_in_F * 2 * m_Ts) * m_BUFF_ARR_WIND_CX.m_MBUFF[n].m_buff[2];
		Im_nom =                                          sin(-PI2 * m_in_F * m_Ts) * m_BUFF_ARR_WIND_CX.m_MBUFF[n].m_buff[1] + sin(-PI2 * m_in_F * 2 * m_Ts) * m_BUFF_ARR_WIND_CX.m_MBUFF[n].m_buff[2];
		
		ABS_nom = ABS_nom * sqrt (Re_nom * Re_nom + Im_nom * Im_nom);
		ARG_nom = ARG_nom + atan2(Im_nom , Re_nom);
	}

	// Расчет амплитуды и угла комплексного числа знаменателя:

	for (int n = 0; n < m_BUFF_ARR_WIND_CY.getRingBuffArraySize(); n++)
	{
		Re_den = m_BUFF_ARR_WIND_CY.m_MBUFF[n].m_buff[0] + cos(-PI2 * m_in_F * m_Ts) * m_BUFF_ARR_WIND_CY.m_MBUFF[n].m_buff[1] + cos(-PI2 * m_in_F * 2 * m_Ts) * m_BUFF_ARR_WIND_CY.m_MBUFF[n].m_buff[2];
		Im_den =                                          sin(-PI2 * m_in_F * m_Ts) * m_BUFF_ARR_WIND_CY.m_MBUFF[n].m_buff[1] + sin(-PI2 * m_in_F * 2 * m_Ts) * m_BUFF_ARR_WIND_CY.m_MBUFF[n].m_buff[2];

		ABS_den = ABS_den * sqrt (Re_den * Re_den + Im_den * Im_den);
		ARG_den = ARG_den + atan2(Im_den, Re_den);
	}

	// Расчет коэффициента компенсации искажения амплитуды:
	for (int n = 0 ; n < m_BUFF_GAIN.getBuffSize() ; n++)
	{
		Gain = Gain * m_BUFF_GAIN.m_buff[n];
	}

	m_Km = ABS_nom / ABS_den * Gain;

	// Расчет угла компенсации искажения фазы:

	m_pH = ARG_nom - ARG_den;

	return 0;
};
//------------------------------------------------------------------------------
// функция расчета коэффициентов степени подавления и ширины переходной полосы эллиптического фильтра ( названия коэффициентов придумал сам, в литературе названий не нашел ):
int Elliptic::supressinon_transient_factors_calc()
{

	double KE;
	double SN;

	m_Ke = m_epsilon_pass / m_epsilon_stop;
	m_Kp = 1;

	double m = sqrt(1 - m_Ke * m_Ke);

	for (int n = 0; n < m_L; n++)
	{
		m_alpha = (2 * (n + 1) - 1) / ((double)m_order);

		KE = m_f_ellip.ellip_K(m);
		SN = m_f_ellip.sn(m_alpha * KE, m);

		m_Kp = m_Kp * SN*SN*SN*SN;

	}

	m_Kp = pow(m, m_order) * m_Kp;
	m_Kw = sqrt(1 - m_Kp * m_Kp);

	return 0;
};
//------------------------------------------------------------------------------
// функция расчета нулей нормированного эллиптического фильтра:
int Elliptic::zeros_calc() 
{

	double SN;    // переменная для хранения значения эллиптического синуса
	double KE;  // переменная для хранения значения эллиптического интеграла первого рода

	// Расчет нулей аналогового прототипа фильтра:
	KE = m_f_ellip.ellip_K(m_Kw);

	for (int n = 0; n < m_L; n++)
	{

		if(m_order % 2 == 0) m_alpha = (2 * n + 1) / ((double)m_order) * KE; // для четного порядка
		if(m_order % 2 != 0) m_alpha = (2 * n + 2) / ((double)m_order) * KE; // для нечетного порядка

		SN		= m_f_ellip.sn     (m_alpha, m_Kw);

		m_BUFF_ZEROS_IM.m_buff[n] = 1 /( m_Kw * SN );
		m_BUFF_ZEROS_RE.m_buff[n] = 0;
	}

	return 0;
}
//------------------------------------------------------------------------------
// функция расчета полюсов нормированного эллиптического фильтра:
int Elliptic::poles_calc()
{
	double KE;  // переменная для хранения значения эллиптического интеграла первого рода
	double V0; // специальная переменная для расчета полюсов эллиптического фильтра (см. [1] в списке использованных источников )

	// промежуточные переменные:
	double A;
	double B;
	double C;
	double D;
	double E;
	double F;

	// Считаем полюса фильтра:
//	V0 = -m_f_ellip.ellip_K(m_Kw) / (m_f_ellip.inv_sc(1 / m_epsilon_pass, m_Ke) * ((double)m_order));

	V0 = -m_f_ellip.ellip_K(m_Kw) * m_f_ellip.inv_sc(1 / m_epsilon_pass, sqrt(1 - m_Ke * m_Ke)) / (m_f_ellip.ellip_K(m_Ke) * ((double)m_order));

	KE =  m_f_ellip.ellip_K(m_Kw);

	if (m_R == 1)
	{
		for (int n = 0; n < m_L; n++)
		{
			m_alpha = (2 * n + 2) / ((double)m_order) * KE; // для нечетного порядка

			// Считаем промежуточные переменные:
			A = m_f_ellip.cn(m_alpha, m_Kw);
			B = m_f_ellip.dn(m_alpha, m_Kw);
			C = m_f_ellip.sn(V0, sqrt(1 - m_Kw * m_Kw));
			D = sqrt(1 - C * C);
			E = B * B;
			F = C * C;

			// считаем действительную часть полюса эллиптического фильтра:
			m_BUFF_POLES_RE.m_buff[n] = A * B * C * D / (1 - E * F);

			// Повторно считаем промежуточные переменные:
			A = m_f_ellip.sn(m_alpha, m_Kw);
			B = m_f_ellip.dn(V0, sqrt(1 - m_Kw * m_Kw));
			C = m_f_ellip.dn(m_alpha, m_Kw);
			D = m_f_ellip.sn(V0, sqrt(1 - m_Kw * m_Kw));;
			E = C * C;
			F = D * D;

			// считаем мнимую часть полюса эллиптического фильтра:
			m_BUFF_POLES_IM.m_buff[n] = A * B / (1 - E * F);
		}

		// Считаем промежуточные переменные:
		A = m_f_ellip.sn(V0, sqrt(1 - m_Kw * m_Kw));
		B = m_f_ellip.cn(V0, sqrt(1 - m_Kw * m_Kw));
		C = A * A;

		// расчет некратного вещественного полюса с помещением в конец буффера:
		m_BUFF_POLES_RE.m_buff[m_L + m_R - 1] = A * B / ( 1 - C );
		m_BUFF_POLES_IM.m_buff[m_L + m_R - 1] = 0;

	}
	else 
	{
		for (int n = 0; n < m_L; n++)
		{
			m_alpha = (2 * n + 1) / ((double)m_order) * KE; // для четного порядка

			// Считаем промежуточные переменные:
			A = m_f_ellip.cn(m_alpha, m_Kw);
			B = m_f_ellip.dn(m_alpha, m_Kw);
			C = m_f_ellip.sn(V0, sqrt(1 - m_Kw * m_Kw));
			D = sqrt(1 - C * C);
			E = B * B;
			F = C * C;

			// считаем действительную часть полюса эллиптического фильтра:
			m_BUFF_POLES_RE.m_buff[n] = A * B * C * D / (1 - E * F);

			// Повторно считаем промежуточные переменные:
			A = m_f_ellip.sn(m_alpha, m_Kw);
			B = m_f_ellip.dn(V0, sqrt(1 - m_Kw * m_Kw));
			C = m_f_ellip.dn(m_alpha, m_Kw);
			D = m_f_ellip.sn(V0, sqrt(1 - m_Kw * m_Kw));;
			E = C * C;
			F = D * D;

			// считаем мнимую часть полюса эллиптического фильтра:
			m_BUFF_POLES_IM.m_buff[n] = A * B / (1 - E * F);
		}
	}
	
	return 0;
}
//------------------------------------------------------------------------------
// функция расчета коэффициента усиления нормированного фильтра на нулевой частоте:
int Elliptic::gain_calc()
{
	double num = 1;
	double den = 1;

	m_output_Gain = sqrt( 1 / (1 + m_epsilon_stop * m_epsilon_stop) );
	m_output_Gain = pow ( m_output_Gain , 1 / ( (double) m_L )      );

	if (m_R == 1)
	{
		// коэфф.усиления квадратичных полиномов:
		for (int n = 0; n < m_L; n++)
		{
			num = m_BUFF_ZEROS_RE.m_buff[n] * m_BUFF_ZEROS_RE.m_buff[n] + m_BUFF_ZEROS_IM.m_buff[n] * m_BUFF_ZEROS_IM.m_buff[n];
			den = m_BUFF_POLES_RE.m_buff[n] * m_BUFF_POLES_RE.m_buff[n] + m_BUFF_POLES_IM.m_buff[n] * m_BUFF_POLES_IM.m_buff[n];

			m_BUFF_GAIN.m_buff[n] = den/num;
		}

		m_BUFF_GAIN.m_buff[m_L + m_R - 1] = -1 / (1 / m_BUFF_POLES_RE.m_buff[m_L + m_R - 1]);

	}
	else
	{
		// коэфф.усиления квадратичных полиномов:
		for (int n = 0; n < m_L; n++) // если порядок фильтра нечетный, то коэффициент усиления "размазывается" по всем квадратичным секциям фильтра
		{
			m_BUFF_GAIN.m_buff[n] = m_output_Gain;
		}

	}

	return 0;
}
//------------------------------------------------------------------------------
// функция билинейного преобразования ФНЧ - ФНЧ:
int Elliptic::LP_to_LP_bilinear_transform()
{

	// пересчет нулей и полюсов (для квадратичных полиномов, см ПЗ):

	for (int n = 0; n < m_L ; n++) 
	{

		// ПЕРЕСЧЕТ НУЛЕЙ:

		// домножение нулей ПФ фильтра на поправочный коэффициент:
		m_BUFF_ZEROS_IM.m_buff[n] = m_BUFF_ZEROS_IM.m_buff[n] * m_K;
		m_BUFF_ZEROS_RE.m_buff[n] = m_BUFF_ZEROS_RE.m_buff[n] * m_K;

		// расчет коэфф. усиления числителя:

		m_BUFF_GAIN_ZEROS.m_buff[n] = (1 + m_BUFF_ZEROS_IM.m_buff[n] * m_BUFF_ZEROS_IM.m_buff[n]);

		// пересчет нуля:
		m_CPLX_ADD.add  (1 , 0 , m_BUFF_ZEROS_RE.m_buff[n] , m_BUFF_ZEROS_IM.m_buff[n]);
		m_CPLX_SUB.sub(1 , 0 , m_BUFF_ZEROS_RE.m_buff[n] , m_BUFF_ZEROS_IM.m_buff[n]);
		m_CPLX_DIV.div(m_CPLX_ADD.m_Re, m_CPLX_ADD.m_Im, m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		// перезапись нуля в массиве с нулями:
		m_BUFF_ZEROS_RE.m_buff[n] = m_CPLX_DIV.m_Re;
		m_BUFF_ZEROS_IM.m_buff[n] = m_CPLX_DIV.m_Im;

		// ПЕРЕСЧЕТ ПОЛЮСОВ:

		//домножение полюсов ПФ фильтра на поправочный коэффициент:
		m_BUFF_POLES_IM.m_buff[n] = m_BUFF_POLES_IM.m_buff[n] * m_K;
		m_BUFF_POLES_RE.m_buff[n] = m_BUFF_POLES_RE.m_buff[n] * m_K;

		// расчет коэфф. усиления знаменателя:
		m_BUFF_GAIN_POLES.m_buff[n] = (1 - 2 * m_BUFF_POLES_RE.m_buff[n] + m_BUFF_POLES_IM.m_buff[n] * m_BUFF_POLES_IM.m_buff[n] + m_BUFF_POLES_RE.m_buff[n] * m_BUFF_POLES_RE.m_buff[n]);

		// пересчет полюса:
		m_CPLX_ADD.add(1, 0, m_BUFF_POLES_RE.m_buff[n], m_BUFF_POLES_IM.m_buff[n]);
		m_CPLX_SUB.sub(1, 0, m_BUFF_POLES_RE.m_buff[n], m_BUFF_POLES_IM.m_buff[n]);
		m_CPLX_DIV.div(m_CPLX_ADD.m_Re, m_CPLX_ADD.m_Im, m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		// перезапись полюса в массиве с полюсами:
		m_BUFF_POLES_RE.m_buff[n] = m_CPLX_DIV.m_Re;
		m_BUFF_POLES_IM.m_buff[n] = m_CPLX_DIV.m_Im;
	}

	if (m_R == 1) 
	{
		// если порядок фильтра нечетный, то в конец буффера добавляется 1 лишний нуль (см. ПЗ ):
		m_BUFF_GAIN_ZEROS.m_buff[ m_L + m_R - 1 ] =  1;
		m_BUFF_ZEROS_RE  .m_buff[ m_L + m_R - 1 ] = -1;
		m_BUFF_ZEROS_IM  .m_buff[ m_L + m_R - 1 ] =  0;

		// если порядок фильтра нечетный, то делаем билинейное преобразование нечетного полюса:
		m_BUFF_POLES_IM.m_buff[m_L + m_R - 1] = m_BUFF_POLES_IM.m_buff[m_L + m_R - 1] * m_K;
		m_BUFF_POLES_RE.m_buff[m_L + m_R - 1] = m_BUFF_POLES_RE.m_buff[m_L + m_R - 1] * m_K;

		m_BUFF_GAIN_POLES.m_buff[m_L + m_R - 1] = (1 - m_BUFF_POLES_RE.m_buff[m_L + m_R - 1]) / m_K;

		// пересчет полюса:
		m_CPLX_ADD.add(1, 0, m_BUFF_POLES_RE.m_buff[m_L + m_R - 1], m_BUFF_POLES_IM.m_buff[m_L + m_R - 1]);
		m_CPLX_SUB.sub(1, 0, m_BUFF_POLES_RE.m_buff[m_L + m_R - 1], m_BUFF_POLES_IM.m_buff[m_L + m_R - 1]);
		m_CPLX_DIV.div(m_CPLX_ADD.m_Re, m_CPLX_ADD.m_Im, m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		// перезапись полюса в массиве с полюсами:
		m_BUFF_POLES_RE.m_buff[m_L + m_R - 1] = m_CPLX_DIV.m_Re;
		m_BUFF_POLES_IM.m_buff[m_L + m_R - 1] = m_CPLX_DIV.m_Im;
	}

	return 0;
}
//------------------------------------------------------------------------------
// функция билинейного преобразования ФНЧ - ФВЧ:
int Elliptic::LP_to_HP_bilinear_transform()
{
	// пересчет нулей и полюсов (для квадратичных полиномов, см. ПЗ):

	for (int n = 0; n < m_L; n++)
	{
		// ПЕРЕСЧЕТ НУЛЕЙ:

		// домножение нулей ПФ фильтра на поправочный коэффициент:
		m_BUFF_ZEROS_IM.m_buff[n] = m_BUFF_ZEROS_IM.m_buff[n] / m_K;
		m_BUFF_ZEROS_RE.m_buff[n] = m_BUFF_ZEROS_RE.m_buff[n] / m_K;

		// расчет коэфф. усиления числителя:
		m_BUFF_GAIN_ZEROS.m_buff[n] = (1 + m_BUFF_ZEROS_IM.m_buff[n] * m_BUFF_ZEROS_IM.m_buff[n]);

		// пересчет нуля:
		m_CPLX_ADD.add  (1, 0, m_BUFF_ZEROS_RE.m_buff[n], m_BUFF_ZEROS_IM.m_buff[n]);
		m_CPLX_SUB.sub(1, 0, m_BUFF_ZEROS_RE.m_buff[n], m_BUFF_ZEROS_IM.m_buff[n]);
		m_CPLX_DIV.div(m_CPLX_ADD.m_Re, m_CPLX_ADD.m_Im, m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		// перезапись нуля в массиве с нулями:
		m_BUFF_ZEROS_RE.m_buff[n] = -m_CPLX_DIV.m_Re;
		m_BUFF_ZEROS_IM.m_buff[n] = -m_CPLX_DIV.m_Im;

		// ПЕРЕСЧЕТ ПОЛЮСОВ:

		//домножение полюсов ПФ фильтра на поправочный коэффициент:
		m_BUFF_POLES_IM.m_buff[n] = m_BUFF_POLES_IM.m_buff[n] / m_K;
		m_BUFF_POLES_RE.m_buff[n] = m_BUFF_POLES_RE.m_buff[n] / m_K;

		// расчет коэфф. усиления:
		m_BUFF_GAIN_POLES.m_buff[n] = (1 - 2 * m_BUFF_POLES_RE.m_buff[n] + m_BUFF_POLES_IM.m_buff[n] * m_BUFF_POLES_IM.m_buff[n] + m_BUFF_POLES_RE.m_buff[n] * m_BUFF_POLES_RE.m_buff[n]);

		// пересчет полюса:
		m_CPLX_ADD.add(1, 0, m_BUFF_POLES_RE.m_buff[n], m_BUFF_POLES_IM.m_buff[n]);
		m_CPLX_SUB.sub(1, 0, m_BUFF_POLES_RE.m_buff[n], m_BUFF_POLES_IM.m_buff[n]);
		m_CPLX_DIV.div(m_CPLX_ADD.m_Re, m_CPLX_ADD.m_Im, m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		// перезапись полюса в массиве с полюсами:
		m_BUFF_POLES_RE.m_buff[n] = -m_CPLX_DIV.m_Re;
		m_BUFF_POLES_IM.m_buff[n] = -m_CPLX_DIV.m_Im;
	}

	if (m_R == 1)
	{
		// если порядок фильтра нечетный, то в конец буффера добавляется 1 лишний нуль (см. ПЗ ):
		m_BUFF_GAIN_ZEROS.m_buff[m_L + m_R - 1] = 1;
		m_BUFF_ZEROS_RE  .m_buff[m_L + m_R - 1] = 1;
		m_BUFF_ZEROS_IM  .m_buff[m_L + m_R - 1] = 0;

		// если порядок фильтра нечетный, то делаем билинейное преобразование нечетного полюса:
		m_BUFF_POLES_IM.m_buff[m_L + m_R - 1] = m_BUFF_POLES_IM.m_buff[m_L + m_R - 1] / m_K;
		m_BUFF_POLES_RE.m_buff[m_L + m_R - 1] = m_BUFF_POLES_RE.m_buff[m_L + m_R - 1] / m_K;
		m_BUFF_GAIN_POLES.m_buff[m_L + m_R - 1] = (1 - m_BUFF_POLES_RE.m_buff[m_L + m_R - 1]) * m_K;

		// пересчет полюса:
		m_CPLX_ADD.add(1, 0, m_BUFF_POLES_RE.m_buff[m_L + m_R - 1], m_BUFF_POLES_IM.m_buff[m_L + m_R - 1]);
		m_CPLX_SUB.sub(1, 0, m_BUFF_POLES_RE.m_buff[m_L + m_R - 1], m_BUFF_POLES_IM.m_buff[m_L + m_R - 1]);
		m_CPLX_DIV.div(m_CPLX_ADD.m_Re, m_CPLX_ADD.m_Im, m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		// перезапись полюса в массиве с полюсами:
		m_BUFF_POLES_RE.m_buff[m_L + m_R - 1] = -m_CPLX_DIV.m_Re;
		m_BUFF_POLES_IM.m_buff[m_L + m_R - 1] = -m_CPLX_DIV.m_Im;
	}

	return 0;
}

// функция билинейного преобразования ФНЧ - ПФ:
int Elliptic::LP_to_BP_bilinear_transform()
{

	m_Fs1 = tan(PI2 * m_F_stop1 * m_Ts / 2);
	m_Fs2 = tan(PI2 * m_F_stop2 * m_Ts / 2);

	// пересчет нулей полюсов ФНЧ в нули и полюса полосового фильтра:
	int    m     = 0;

	double Gain1 = 0;
	double Gain2 = 0;
	double Gain3 = 0;

	for ( int n = 0; n < m_L; n++ )
	{

		Gain1 = 1 / ( (m_BUFF_ZEROS_RE.m_buff[n] * m_BUFF_ZEROS_RE.m_buff[n] + m_BUFF_ZEROS_IM.m_buff[n] * m_BUFF_ZEROS_IM.m_buff[n]) / (m_BUFF_POLES_RE.m_buff[n] * m_BUFF_POLES_RE.m_buff[n] + m_BUFF_POLES_IM.m_buff[n] * m_BUFF_POLES_IM.m_buff[n]) );

		// ПЕРЕСЧЕТ НУЛЕЙ:

		// ( a + 1i * b ) * ( m_F_stop2 - m_F_stop1 ) :
		m_BUFF_ZEROS_RE.m_buff[n] = m_BUFF_ZEROS_RE.m_buff[n] * (m_Fs2 - m_Fs1);
		m_BUFF_ZEROS_IM.m_buff[n] = m_BUFF_ZEROS_IM.m_buff[n] * (m_Fs2 - m_Fs1);

		// ( a + 1i * b ) * ( a + 1i * b ):
		m_CPLX_MUL.mul  (m_BUFF_ZEROS_RE.m_buff[n] , m_BUFF_ZEROS_IM.m_buff[n] , m_BUFF_ZEROS_RE.m_buff[n], m_BUFF_ZEROS_IM.m_buff[n]);

		// ( a + 1i * b ) * ( a + 1i * b ) - 4 * m_F_stop1 * m_F_stop2 :
		m_CPLX_SUB.sub(m_CPLX_MUL.m_Re , m_CPLX_MUL.m_Im , 4 * m_Fs1 * m_Fs2, 0);

		// sqrt( ( a + 1i * b ) * ( a + 1i * b ) - 4 * m_F_stop1 * m_F_stop2 ):
		m_CPLX_SQR.sqr( m_CPLX_SUB.m_Re , m_CPLX_SUB.m_Im );

		//
		m_CPLX_SUB.sub( m_BUFF_ZEROS_RE.m_buff[n] , m_BUFF_ZEROS_IM.m_buff[n] , m_CPLX_SQR.m_Re , m_CPLX_SQR.m_Im );
		m_CPLX_ADD.add  ( m_BUFF_ZEROS_RE.m_buff[n] , m_BUFF_ZEROS_IM.m_buff[n] , m_CPLX_SQR.m_Re , m_CPLX_SQR.m_Im );
		
		// Запись расчетных полюсов и нулей в соответствующие массивы:
		m_BUFF_BP_ZEROS_RE.m_buff[m]     = m_CPLX_SUB.m_Re / 2;
		m_BUFF_BP_ZEROS_IM.m_buff[m]     = m_CPLX_SUB.m_Im / 2;
		m_BUFF_BP_ZEROS_RE.m_buff[m + 1] = m_CPLX_ADD.m_Re / 2;
		m_BUFF_BP_ZEROS_IM.m_buff[m + 1] = m_CPLX_ADD.m_Im / 2;

		// ПЕРЕСЧЕТ ПОЛЮСОВ:

		// ( a + 1i * b ) * ( m_F_stop2 - m_F_stop1 ) :
		m_BUFF_POLES_RE.m_buff[n] = m_BUFF_POLES_RE.m_buff[n] * (m_Fs2 - m_Fs1);
		m_BUFF_POLES_IM.m_buff[n] = m_BUFF_POLES_IM.m_buff[n] * (m_Fs2 - m_Fs1);

		// ( a + 1i * b ) * ( a + 1i * b ):
		m_CPLX_MUL.mul(m_BUFF_POLES_RE.m_buff[n], m_BUFF_POLES_IM.m_buff[n], m_BUFF_POLES_RE.m_buff[n], m_BUFF_POLES_IM.m_buff[n]);

		// ( a + 1i * b ) * ( a + 1i * b ) - 4 * m_F_stop1 * m_F_stop2 :
		m_CPLX_SUB.sub(m_CPLX_MUL.m_Re, m_CPLX_MUL.m_Im, 4 * m_Fs1 * m_Fs2, 0);

		// sqrt( ( a + 1i * b ) * ( a + 1i * b ) - 4 * m_F_stop1 * m_F_stop2 ):
		m_CPLX_SQR.sqr(m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		//
		m_CPLX_SUB.sub(m_BUFF_POLES_RE.m_buff[n], m_BUFF_POLES_IM.m_buff[n], m_CPLX_SQR.m_Re, m_CPLX_SQR.m_Im);
		m_CPLX_ADD.add  (m_BUFF_POLES_RE.m_buff[n], m_BUFF_POLES_IM.m_buff[n], m_CPLX_SQR.m_Re, m_CPLX_SQR.m_Im);

		// Запись расчетных полюсов и нулей в соответствующие массивы:
		m_BUFF_BP_POLES_RE.m_buff[m]     = m_CPLX_SUB.m_Re / 2;
		m_BUFF_BP_POLES_IM.m_buff[m]     = m_CPLX_SUB.m_Im / 2;
		m_BUFF_BP_POLES_RE.m_buff[m + 1] = m_CPLX_ADD.m_Re / 2;
		m_BUFF_BP_POLES_IM.m_buff[m + 1] = m_CPLX_ADD.m_Im / 2;

		Gain2 = 1/((m_BUFF_BP_ZEROS_RE.m_buff[m  ] * m_BUFF_BP_ZEROS_RE.m_buff[m  ] + m_BUFF_BP_ZEROS_IM.m_buff[m  ] * m_BUFF_BP_ZEROS_IM.m_buff[m  ]) / (m_BUFF_BP_POLES_RE.m_buff[m  ] * m_BUFF_BP_POLES_RE.m_buff[m  ] + m_BUFF_BP_POLES_IM.m_buff[m  ] * m_BUFF_BP_POLES_IM.m_buff[m  ]));
		Gain3 = 1/((m_BUFF_BP_ZEROS_RE.m_buff[m+1] * m_BUFF_BP_ZEROS_RE.m_buff[m+1] + m_BUFF_BP_ZEROS_IM.m_buff[m+1] * m_BUFF_BP_ZEROS_IM.m_buff[m+1]) / (m_BUFF_BP_POLES_RE.m_buff[m+1] * m_BUFF_BP_POLES_RE.m_buff[m+1] + m_BUFF_BP_POLES_IM.m_buff[m+1] * m_BUFF_BP_POLES_IM.m_buff[m+1]));

		m_BUFF_GAIN.m_buff[m]     = Gain1 * Gain2 * Gain3;
		m_BUFF_GAIN.m_buff[m + 1] = m_BUFF_GAIN.m_buff[m];

		m = m + 2;
	}

	if (m_R == 1) // если порядок ФНЧ нечетный, то пересчитываем некратный полюс
	{

		Gain1 = -m_BUFF_POLES_RE.m_buff[m_L + m_R - 1];

		// ( a + 1i * b ) * ( m_F_stop2 - m_F_stop1 ) :
		m_BUFF_POLES_RE.m_buff[m_L + m_R - 1] = m_BUFF_POLES_RE.m_buff[m_L + m_R - 1] * (m_Fs2 - m_Fs1);
		m_BUFF_POLES_IM.m_buff[m_L + m_R - 1] = m_BUFF_POLES_IM.m_buff[m_L + m_R - 1] * (m_Fs2 - m_Fs1);

		// ( a + 1i * b ) * ( a + 1i * b ):
		m_CPLX_MUL.mul(m_BUFF_POLES_RE.m_buff[m_L + m_R - 1], m_BUFF_POLES_IM.m_buff[m_L + m_R - 1], m_BUFF_POLES_RE.m_buff[m_L + m_R - 1], m_BUFF_POLES_IM.m_buff[m_L + m_R - 1]);

		// ( a + 1i * b ) * ( a + 1i * b ) - 4 * m_F_stop1 * m_F_stop2 :
		m_CPLX_SUB.sub(m_CPLX_MUL.m_Re, m_CPLX_MUL.m_Im, 4 * m_Fs1 * m_Fs2, 0);

		// sqrt( ( a + 1i * b ) * ( a + 1i * b ) - 4 * m_F_stop1 * m_F_stop2 ):
		m_CPLX_SQR.sqr(m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		//
		m_CPLX_SUB.sub(m_BUFF_POLES_RE.m_buff[m_L + m_R - 1], m_BUFF_POLES_IM.m_buff[m_L + m_R - 1], m_CPLX_SQR.m_Re, m_CPLX_SQR.m_Im);
		m_CPLX_ADD.add  (m_BUFF_POLES_RE.m_buff[m_L + m_R - 1], m_BUFF_POLES_IM.m_buff[m_L + m_R - 1], m_CPLX_SQR.m_Re, m_CPLX_SQR.m_Im);

		// Запись расчетных полюсов в соответствующие массивы:
		m_BUFF_BP_POLES_RE.m_buff[m] = m_CPLX_SUB.m_Re / 2;
		m_BUFF_BP_POLES_IM.m_buff[m] = m_CPLX_SUB.m_Im / 2;
		m_BUFF_BP_POLES_RE.m_buff[m+1] = m_CPLX_ADD.m_Re / 2;
		m_BUFF_BP_POLES_IM.m_buff[m+1] = m_CPLX_ADD.m_Im / 2;

		//Gain2 = 1 / (1 - 2 * m_BUFF_BP_POLES_RE.m_buff[m] + m_BUFF_BP_POLES_IM.m_buff[m] * m_BUFF_BP_POLES_IM.m_buff[m] + m_BUFF_BP_POLES_RE.m_buff[m] * m_BUFF_BP_POLES_RE.m_buff[m]);

		Gain2 = 1 / ((1 - (m_BUFF_BP_POLES_RE.m_buff[m] + m_BUFF_BP_POLES_RE.m_buff[m + 1]) + m_BUFF_BP_POLES_RE.m_buff[m] * m_BUFF_BP_POLES_RE.m_buff[m + 1] + m_BUFF_BP_POLES_IM.m_buff[m] * m_BUFF_BP_POLES_IM.m_buff[m]));

		// билинейное преобразование некратного полюса:
		m_CPLX_ADD.add  (1, 0, m_BUFF_BP_POLES_RE.m_buff[m], m_BUFF_BP_POLES_IM.m_buff[m]);
		m_CPLX_SUB.sub(1, 0, m_BUFF_BP_POLES_RE.m_buff[m], m_BUFF_BP_POLES_IM.m_buff[m]);
		m_CPLX_DIV.div(m_CPLX_ADD.m_Re, m_CPLX_ADD.m_Im, m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		// перезапись нуля в массиве с нулями:
		m_BUFF_BP_POLES_RE.m_buff[m] = m_CPLX_DIV.m_Re;
		m_BUFF_BP_POLES_IM.m_buff[m] = m_CPLX_DIV.m_Im;

		m_BUFF_GAIN.m_buff[m] = (m_Fs2 - m_Fs1) * Gain1 * Gain2;
	}

	// билинейное преобразование нулей и полюсов ПФ полосового фильтра:
	for (int n = 0; n < 2 * m_L; n++)
	{
		// расчет коэффициентов усиления:
		m_BUFF_GAIN_ZEROS.m_buff[n] = (1 - 2 * m_BUFF_BP_ZEROS_RE.m_buff[n] + m_BUFF_BP_ZEROS_IM.m_buff[n] * m_BUFF_BP_ZEROS_IM.m_buff[n] + m_BUFF_BP_ZEROS_RE.m_buff[n] * m_BUFF_BP_ZEROS_RE.m_buff[n]);
		m_BUFF_GAIN_POLES.m_buff[n] = (1 - 2 * m_BUFF_BP_POLES_RE.m_buff[n] + m_BUFF_BP_POLES_IM.m_buff[n] * m_BUFF_BP_POLES_IM.m_buff[n] + m_BUFF_BP_POLES_RE.m_buff[n] * m_BUFF_BP_POLES_RE.m_buff[n]);

		// пересчет нуля:
		m_CPLX_ADD.add  (1, 0, m_BUFF_BP_ZEROS_RE.m_buff[n], m_BUFF_BP_ZEROS_IM.m_buff[n]);
		m_CPLX_SUB.sub(1, 0, m_BUFF_BP_ZEROS_RE.m_buff[n], m_BUFF_BP_ZEROS_IM.m_buff[n]);
		m_CPLX_DIV.div  (m_CPLX_ADD.m_Re, m_CPLX_ADD.m_Im, m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		// перезапись нуля в массиве с нулями:
		m_BUFF_BP_ZEROS_RE.m_buff[n] = m_CPLX_DIV.m_Re;
		m_BUFF_BP_ZEROS_IM.m_buff[n] = m_CPLX_DIV.m_Im;

		// пересчет полюса:
		m_CPLX_ADD.add(1, 0, m_BUFF_BP_POLES_RE.m_buff[n], m_BUFF_BP_POLES_IM.m_buff[n]);
		m_CPLX_SUB.sub(1, 0, m_BUFF_BP_POLES_RE.m_buff[n], m_BUFF_BP_POLES_IM.m_buff[n]);
		m_CPLX_DIV.div(m_CPLX_ADD.m_Re, m_CPLX_ADD.m_Im, m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		// перезапись полюса в массиве с полюсами:
		m_BUFF_BP_POLES_RE.m_buff[n] = m_CPLX_DIV.m_Re;
		m_BUFF_BP_POLES_IM.m_buff[n] = m_CPLX_DIV.m_Im;

	}

	m     = 0;
	Gain1 = 0;
	Gain2 = 0;
	Gain3 = 0;

	for (int n = 0 ; n < m_L ; n++)
	{
		Gain1 = m_BUFF_GAIN_ZEROS.m_buff[m  ] / m_BUFF_GAIN_POLES.m_buff[m];
		Gain2 = m_BUFF_GAIN_ZEROS.m_buff[m+1] / m_BUFF_GAIN_POLES.m_buff[m+1];

		m_BUFF_GAIN.m_buff[m]   = sqrt( Gain1 * Gain2 * m_BUFF_GAIN.m_buff[m] );
		m_BUFF_GAIN.m_buff[m+1] = m_BUFF_GAIN.m_buff[m];

		m = m + 2;

	}

	return 0;
}
//------------------------------------------------------------------------------
// Функция билинейного преобразования ФНЧ - РФ:
int Elliptic::LP_to_BS_bilinear_transform() 
{

	m_Fs1 = tan(PI2 * m_F_stop1 * m_Ts / 2);
	m_Fs2 = tan(PI2 * m_F_stop2 * m_Ts / 2);

	// пересчет нулей полюсов ФНЧ в нули и полюса заграждающего фильтра:
	int    m = 0;

	double Gain1 = 0;
	double Gain2 = 0;
	double Gain3 = 0;

	for (int n = 0; n < m_L; n++)
	{
		// коэфф. усиления n-ой секции на нулевой частоте до преобразования ФНЧ - РФ:
		Gain1 = 1; // (он сокращается в процессе дробно-рациональной подстановки )

		// ПЕРЕСЧЕТ НУЛЕЙ:

		m_CPLX_DIV.div( (m_Fs2 - m_Fs1) , 0 , m_BUFF_ZEROS_RE.m_buff[n] , m_BUFF_ZEROS_IM.m_buff[n]);

		m_BUFF_ZEROS_RE.m_buff[n] = m_CPLX_DIV.m_Re;
		m_BUFF_ZEROS_IM.m_buff[n] = m_CPLX_DIV.m_Im;

		// ( a + 1i * b ) * ( a + 1i * b ):
		m_CPLX_MUL.mul(m_BUFF_ZEROS_RE.m_buff[n], m_BUFF_ZEROS_IM.m_buff[n], m_BUFF_ZEROS_RE.m_buff[n], m_BUFF_ZEROS_IM.m_buff[n]);

		// ( a + 1i * b ) * ( a + 1i * b ) - 4 * m_F_stop1 * m_F_stop2 :
		m_CPLX_SUB.sub(m_CPLX_MUL.m_Re , m_CPLX_MUL.m_Im , 4 * m_Fs1 * m_Fs2, 0);

		// sqrt( ( a + 1i * b ) * ( a + 1i * b ) - 4 * m_F_stop1 * m_F_stop2 ):
		m_CPLX_SQR.sqr(m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		//
		m_CPLX_SUB.sub(m_BUFF_ZEROS_RE.m_buff[n], m_BUFF_ZEROS_IM.m_buff[n], m_CPLX_SQR.m_Re, m_CPLX_SQR.m_Im);
		m_CPLX_ADD.add  (m_BUFF_ZEROS_RE.m_buff[n], m_BUFF_ZEROS_IM.m_buff[n], m_CPLX_SQR.m_Re, m_CPLX_SQR.m_Im);

		// Запись расчетных полюсов и нулей в соответствующие массивы:
		m_BUFF_BP_ZEROS_RE.m_buff[m]     = m_CPLX_SUB.m_Re / 2;
		m_BUFF_BP_ZEROS_IM.m_buff[m]     = m_CPLX_SUB.m_Im / 2;
		m_BUFF_BP_ZEROS_RE.m_buff[m + 1] = m_CPLX_ADD.m_Re / 2;
		m_BUFF_BP_ZEROS_IM.m_buff[m + 1] = m_CPLX_ADD.m_Im / 2;

		// ПЕРЕСЧЕТ ПОЛЮСОВ:

		m_CPLX_DIV.div((m_Fs2 - m_Fs1) , 0, m_BUFF_POLES_RE.m_buff[n], m_BUFF_POLES_IM.m_buff[n]);

		m_BUFF_POLES_RE.m_buff[n] = m_CPLX_DIV.m_Re;
		m_BUFF_POLES_IM.m_buff[n] = m_CPLX_DIV.m_Im;


		// ( a + 1i * b ) * ( a + 1i * b ):
		m_CPLX_MUL.mul(m_BUFF_POLES_RE.m_buff[n], m_BUFF_POLES_IM.m_buff[n], m_BUFF_POLES_RE.m_buff[n], m_BUFF_POLES_IM.m_buff[n]);

		// ( a + 1i * b ) * ( a + 1i * b ) - 4 * m_F_stop1 * m_F_stop2 :
		m_CPLX_SUB.sub(m_CPLX_MUL.m_Re, m_CPLX_MUL.m_Im, 4 * m_Fs1 * m_Fs2, 0);

		// sqrt( ( a + 1i * b ) * ( a + 1i * b ) - 4 * m_F_stop1 * m_F_stop2 ):
		m_CPLX_SQR.sqr(m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		//
		m_CPLX_SUB.sub(m_BUFF_POLES_RE.m_buff[n], m_BUFF_POLES_IM.m_buff[n], m_CPLX_SQR.m_Re, m_CPLX_SQR.m_Im);
		m_CPLX_ADD.add  (m_BUFF_POLES_RE.m_buff[n], m_BUFF_POLES_IM.m_buff[n], m_CPLX_SQR.m_Re, m_CPLX_SQR.m_Im);

		// Запись расчетных полюсов и нулей в соответствующие массивы:
		m_BUFF_BP_POLES_RE.m_buff[m]     = m_CPLX_SUB.m_Re / 2;
		m_BUFF_BP_POLES_IM.m_buff[m]     = m_CPLX_SUB.m_Im / 2;
		m_BUFF_BP_POLES_RE.m_buff[m + 1] = m_CPLX_ADD.m_Re / 2;
		m_BUFF_BP_POLES_IM.m_buff[m + 1] = m_CPLX_ADD.m_Im / 2;

		Gain2 = 1 / ((m_BUFF_BP_ZEROS_RE.m_buff[m    ] * m_BUFF_BP_ZEROS_RE.m_buff[m    ] + m_BUFF_BP_ZEROS_IM.m_buff[m    ] * m_BUFF_BP_ZEROS_IM.m_buff[m    ]) / (m_BUFF_BP_POLES_RE.m_buff[m    ] * m_BUFF_BP_POLES_RE.m_buff[m    ] + m_BUFF_BP_POLES_IM.m_buff[m    ] * m_BUFF_BP_POLES_IM.m_buff[m    ]));
		Gain3 = 1 / ((m_BUFF_BP_ZEROS_RE.m_buff[m + 1] * m_BUFF_BP_ZEROS_RE.m_buff[m + 1] + m_BUFF_BP_ZEROS_IM.m_buff[m + 1] * m_BUFF_BP_ZEROS_IM.m_buff[m + 1]) / (m_BUFF_BP_POLES_RE.m_buff[m + 1] * m_BUFF_BP_POLES_RE.m_buff[m + 1] + m_BUFF_BP_POLES_IM.m_buff[m + 1] * m_BUFF_BP_POLES_IM.m_buff[m + 1]));

		m_BUFF_GAIN.m_buff[m] = Gain1 * Gain2 * Gain3;
		m_BUFF_GAIN.m_buff[m + 1] = m_BUFF_GAIN.m_buff[m];

		m = m + 2;
	}

	if (m_R == 1) // если порядок ФНЧ нечетный, то пересчитываем некратный вещественный полюс
	{

		Gain1 = -m_BUFF_POLES_RE.m_buff[m_L + m_R - 1];

		m_CPLX_DIV.div((m_Fs2 - m_Fs1), 0, m_BUFF_POLES_RE.m_buff[m_L + m_R - 1], m_BUFF_POLES_IM.m_buff[m_L + m_R - 1]);

		m_BUFF_POLES_RE.m_buff[m_L + m_R - 1] = m_CPLX_DIV.m_Re;
		m_BUFF_POLES_IM.m_buff[m_L + m_R - 1] = m_CPLX_DIV.m_Im;

		// ( a + 1i * b ) * ( a + 1i * b ):
		m_CPLX_MUL.mul(m_BUFF_POLES_RE.m_buff[m_L + m_R - 1], m_BUFF_POLES_IM.m_buff[m_L + m_R - 1], m_BUFF_POLES_RE.m_buff[m_L + m_R - 1], m_BUFF_POLES_IM.m_buff[m_L + m_R - 1]);

		// ( a + 1i * b ) * ( a + 1i * b ) - 4 * m_F_stop1 * m_F_stop2 :
		m_CPLX_SUB.sub(m_CPLX_MUL.m_Re, m_CPLX_MUL.m_Im, 4 * m_Fs1 * m_Fs2, 0);

		// sqrt( ( a + 1i * b ) * ( a + 1i * b ) - 4 * m_F_stop1 * m_F_stop2 ):
		m_CPLX_SQR.sqr(m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		//
		m_CPLX_SUB.sub(m_BUFF_POLES_RE.m_buff[m_L + m_R - 1], m_BUFF_POLES_IM.m_buff[m_L + m_R - 1], m_CPLX_SQR.m_Re, m_CPLX_SQR.m_Im);
		m_CPLX_ADD.add(m_BUFF_POLES_RE.m_buff[m_L + m_R - 1], m_BUFF_POLES_IM.m_buff[m_L + m_R - 1], m_CPLX_SQR.m_Re, m_CPLX_SQR.m_Im);

		// Запись расчетных полюсов в соответствующие массивы:
		m_BUFF_BP_POLES_RE.m_buff[m] = m_CPLX_SUB.m_Re / 2;
		m_BUFF_BP_POLES_IM.m_buff[m] = m_CPLX_SUB.m_Im / 2;
		m_BUFF_BP_POLES_RE.m_buff[m + 1] = m_CPLX_ADD.m_Re / 2;
		m_BUFF_BP_POLES_IM.m_buff[m + 1] = m_CPLX_ADD.m_Im / 2;

		Gain2 = 1 / ((1 - (m_BUFF_BP_POLES_RE.m_buff[m] + m_BUFF_BP_POLES_RE.m_buff[m + 1]) + m_BUFF_BP_POLES_RE.m_buff[m] * m_BUFF_BP_POLES_RE.m_buff[m + 1] + m_BUFF_BP_POLES_IM.m_buff[m] * m_BUFF_BP_POLES_IM.m_buff[m]));

		// билинейное преобразование некратного полюса 1:
		m_CPLX_ADD.add(1, 0, m_BUFF_BP_POLES_RE.m_buff[m], m_BUFF_BP_POLES_IM.m_buff[m]);
		m_CPLX_SUB.sub(1, 0, m_BUFF_BP_POLES_RE.m_buff[m], m_BUFF_BP_POLES_IM.m_buff[m]);
		m_CPLX_DIV.div(m_CPLX_ADD.m_Re, m_CPLX_ADD.m_Im, m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		// перезапись полюса в массиве с полюсами:
		m_BUFF_BP_POLES_RE.m_buff[m] = m_CPLX_DIV.m_Re;
		m_BUFF_BP_POLES_IM.m_buff[m] = m_CPLX_DIV.m_Im;

		// билинейное преобразование некратного полюса 2:
		m_CPLX_ADD.add(1, 0, m_BUFF_BP_POLES_RE.m_buff[m + 1], m_BUFF_BP_POLES_IM.m_buff[m + 1]);
		m_CPLX_SUB.sub(1, 0, m_BUFF_BP_POLES_RE.m_buff[m + 1], m_BUFF_BP_POLES_IM.m_buff[m + 1]);
		m_CPLX_DIV.div(m_CPLX_ADD.m_Re, m_CPLX_ADD.m_Im, m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		// перезапись полюса в массиве с полюсами:
		m_BUFF_BP_POLES_RE.m_buff[m + 1] = m_CPLX_DIV.m_Re;
		m_BUFF_BP_POLES_IM.m_buff[m + 1] = m_CPLX_DIV.m_Im;

		m_BUFF_GAIN.m_buff[m] = Gain2 * (1 + m_Fs1 * m_Fs2); // исправлен расчетный баг
	}

	// билинейное преобразование нулей и полюсов ПФ заграждающего фильтра:
	for (int n = 0; n < 2 * m_L; n++)
	{
		// расчет коэффициентов усиления:
		m_BUFF_GAIN_ZEROS.m_buff[n] = (1 - 2 * m_BUFF_BP_ZEROS_RE.m_buff[n] + m_BUFF_BP_ZEROS_IM.m_buff[n] * m_BUFF_BP_ZEROS_IM.m_buff[n] + m_BUFF_BP_ZEROS_RE.m_buff[n] * m_BUFF_BP_ZEROS_RE.m_buff[n]);
		m_BUFF_GAIN_POLES.m_buff[n] = (1 - 2 * m_BUFF_BP_POLES_RE.m_buff[n] + m_BUFF_BP_POLES_IM.m_buff[n] * m_BUFF_BP_POLES_IM.m_buff[n] + m_BUFF_BP_POLES_RE.m_buff[n] * m_BUFF_BP_POLES_RE.m_buff[n]);

		// пересчет нуля:
		m_CPLX_ADD.add  (1, 0, m_BUFF_BP_ZEROS_RE.m_buff[n], m_BUFF_BP_ZEROS_IM.m_buff[n]);
		m_CPLX_SUB.sub(1, 0, m_BUFF_BP_ZEROS_RE.m_buff[n], m_BUFF_BP_ZEROS_IM.m_buff[n]);
		m_CPLX_DIV.div(m_CPLX_ADD.m_Re, m_CPLX_ADD.m_Im, m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		// перезапись нуля в массиве с нулями:
		m_BUFF_BP_ZEROS_RE.m_buff[n] = m_CPLX_DIV.m_Re;
		m_BUFF_BP_ZEROS_IM.m_buff[n] = m_CPLX_DIV.m_Im;

		// пересчет полюса:
		m_CPLX_ADD.add(1, 0, m_BUFF_BP_POLES_RE.m_buff[n], m_BUFF_BP_POLES_IM.m_buff[n]);
		m_CPLX_SUB.sub(1, 0, m_BUFF_BP_POLES_RE.m_buff[n], m_BUFF_BP_POLES_IM.m_buff[n]);
		m_CPLX_DIV.div(m_CPLX_ADD.m_Re, m_CPLX_ADD.m_Im, m_CPLX_SUB.m_Re, m_CPLX_SUB.m_Im);

		// перезапись полюса в массиве с полюсами:
		m_BUFF_BP_POLES_RE.m_buff[n] = m_CPLX_DIV.m_Re;
		m_BUFF_BP_POLES_IM.m_buff[n] = m_CPLX_DIV.m_Im;

	}

	m = 0;
	Gain1 = 0;
	Gain2 = 0;
	Gain3 = 0;

	for (int n = 0; n < m_L; n++)
	{
		Gain1 = m_BUFF_GAIN_ZEROS.m_buff[m]      / m_BUFF_GAIN_POLES.m_buff[m];
		Gain2 = m_BUFF_GAIN_ZEROS.m_buff[m + 1] / m_BUFF_GAIN_POLES.m_buff[m + 1];

		m_BUFF_GAIN.m_buff[m]     = sqrt(Gain1 * Gain2 * m_BUFF_GAIN.m_buff[m]);
		m_BUFF_GAIN.m_buff[m + 1] = m_BUFF_GAIN.m_buff[m];

		m = m + 2;

	}

	return 0;
}
//------------------------------------------------------------------------------
// функция расчета коэффициентов ФНЧ - ПФ:
int Elliptic::LPF_HPF_CoeffCalc()
{

	for (int n = 0; n < m_L; n++)
	{
		// Расчет коэффициентов усиления квадратичных полиномов:
		m_BUFF_GAIN.m_buff[n] = m_BUFF_GAIN.m_buff[n] * m_BUFF_GAIN_ZEROS.m_buff[n] / m_BUFF_GAIN_POLES.m_buff[n];

		// Расчет коэффициентов квадратичных полиномов числителя ПФ:
		m_BUFF_ARR_WIND_CX.m_MBUFF[n].m_buff[0] = 1;
		m_BUFF_ARR_WIND_CX.m_MBUFF[n].m_buff[1] = - 2 * m_BUFF_ZEROS_RE.m_buff[n];
		m_BUFF_ARR_WIND_CX.m_MBUFF[n].m_buff[2] =       m_BUFF_ZEROS_IM.m_buff[n] * m_BUFF_ZEROS_IM.m_buff[n] + m_BUFF_ZEROS_RE.m_buff[n] * m_BUFF_ZEROS_RE.m_buff[n];

		// Расчет коэффициентов квадратичных полиномов знаменателя ПФ:
		m_BUFF_ARR_WIND_CY.m_MBUFF[n].m_buff[0] = 1;
		m_BUFF_ARR_WIND_CY.m_MBUFF[n].m_buff[1] = -2 * m_BUFF_POLES_RE.m_buff[n];
		m_BUFF_ARR_WIND_CY.m_MBUFF[n].m_buff[2] = m_BUFF_POLES_IM.m_buff[n] * m_BUFF_POLES_IM.m_buff[n] + m_BUFF_POLES_RE.m_buff[n] * m_BUFF_POLES_RE.m_buff[n];
	}
	
	if (m_R == 1) 
	{
		// дорасчет коэффициента усиления полинома некратного полюса:
		m_BUFF_GAIN.m_buff[m_L + m_R - 1] = m_BUFF_GAIN.m_buff[m_L + m_R - 1] * m_BUFF_GAIN_ZEROS.m_buff[m_L + m_R - 1] / m_BUFF_GAIN_POLES.m_buff[m_L + m_R - 1];

		//  дорасчет коэффициентов полинома числителя:
		m_BUFF_ARR_WIND_CX.m_MBUFF[m_L + m_R - 1].m_buff[0] = 1;
		m_BUFF_ARR_WIND_CX.m_MBUFF[m_L + m_R - 1].m_buff[1] = - m_BUFF_ZEROS_RE.m_buff[m_L + m_R - 1];
		m_BUFF_ARR_WIND_CX.m_MBUFF[m_L + m_R - 1].m_buff[2] = 0;

		//  дорасчет коэффициентов полинома знаменателя:
		m_BUFF_ARR_WIND_CY.m_MBUFF[m_L + m_R - 1].m_buff[0] = 1;
		m_BUFF_ARR_WIND_CY.m_MBUFF[m_L + m_R - 1].m_buff[1] = -m_BUFF_POLES_RE.m_buff[m_L + m_R - 1];
		m_BUFF_ARR_WIND_CY.m_MBUFF[m_L + m_R - 1].m_buff[2] = 0;
	}

	return 0;
}
//------------------------------------------------------------------------------
// функция расчета коэффициентов ПФ / РФ:
int Elliptic::BPF_BSF_CoeffCalc()
{
	for (int n = 0; n < 2 * m_L; n++)
	{
		// Расчет коэффициентов квадратичных полиномов числителя ПФ:
		m_BUFF_ARR_WIND_CX.m_MBUFF[n].m_buff[0] = 1;
		m_BUFF_ARR_WIND_CX.m_MBUFF[n].m_buff[1] = -2 * m_BUFF_BP_ZEROS_RE.m_buff[n];
		m_BUFF_ARR_WIND_CX.m_MBUFF[n].m_buff[2] =      m_BUFF_BP_ZEROS_IM.m_buff[n] * m_BUFF_BP_ZEROS_IM.m_buff[n] + m_BUFF_BP_ZEROS_RE.m_buff[n] * m_BUFF_BP_ZEROS_RE.m_buff[n];

		// Расчет коэффициентов квадратичных полиномов знаменателя ПФ:
		m_BUFF_ARR_WIND_CY.m_MBUFF[n].m_buff[0] = 1;
		m_BUFF_ARR_WIND_CY.m_MBUFF[n].m_buff[1] = -2 * m_BUFF_BP_POLES_RE.m_buff[n];
		m_BUFF_ARR_WIND_CY.m_MBUFF[n].m_buff[2] =      m_BUFF_BP_POLES_IM.m_buff[n] * m_BUFF_BP_POLES_IM.m_buff[n] + m_BUFF_BP_POLES_RE.m_buff[n] * m_BUFF_BP_POLES_RE.m_buff[n];
	}

	if (m_R == 1 && m_filt_type == 3) // если порядок фильтра нечетный и рассчитываемый фильтр является полосовым, то:
	{
		// Расчет коэффициентов квадратичных полиномов числителя ПФ:
		m_BUFF_ARR_WIND_CX.m_MBUFF[2 * m_L + m_R - 1].m_buff[0] =  1;
		m_BUFF_ARR_WIND_CX.m_MBUFF[2 * m_L + m_R - 1].m_buff[1] =  0;
		m_BUFF_ARR_WIND_CX.m_MBUFF[2 * m_L + m_R - 1].m_buff[2] = -1;

		// Расчет коэффициентов квадратичных полиномов знаменателя ПФ:
		m_BUFF_ARR_WIND_CY.m_MBUFF[2 * m_L + m_R - 1].m_buff[0] =  1;
		m_BUFF_ARR_WIND_CY.m_MBUFF[2 * m_L + m_R - 1].m_buff[1] = -2 * m_BUFF_BP_POLES_RE.m_buff[2 * m_L + m_R - 1];;
		m_BUFF_ARR_WIND_CY.m_MBUFF[2 * m_L + m_R - 1].m_buff[2] =      m_BUFF_BP_POLES_IM.m_buff[2 * m_L + m_R - 1] * m_BUFF_BP_POLES_IM.m_buff[2 * m_L + m_R - 1] + m_BUFF_BP_POLES_RE.m_buff[2 * m_L + m_R - 1] * m_BUFF_BP_POLES_RE.m_buff[2 * m_L + m_R - 1];
	}

	if (m_R == 1 && m_filt_type == 4) // если порядок фильтра нечетный и рассчитываемый фильтр является заграждающим, то:
	{
		// Расчет коэффициентов квадратичных полиномов числителя ПФ:
		m_BUFF_ARR_WIND_CX.m_MBUFF[2 * m_L + m_R - 1].m_buff[0] = 1;
		m_BUFF_ARR_WIND_CX.m_MBUFF[2 * m_L + m_R - 1].m_buff[1] = 2*(m_Fs1*m_Fs2 - 1) / (m_Fs1*m_Fs2 + 1);
		m_BUFF_ARR_WIND_CX.m_MBUFF[2 * m_L + m_R - 1].m_buff[2] = 1;

		// Расчет коэффициентов квадратичных полиномов знаменателя ПФ:
		m_BUFF_ARR_WIND_CY.m_MBUFF[2 * m_L + m_R - 1].m_buff[0] = 1;
		m_BUFF_ARR_WIND_CY.m_MBUFF[2 * m_L + m_R - 1].m_buff[1] = -(m_BUFF_BP_POLES_RE.m_buff[2 * m_L + 2 * m_R - 1] + m_BUFF_BP_POLES_RE.m_buff[2 * m_L + 2 * m_R - 2]);
		m_BUFF_ARR_WIND_CY.m_MBUFF[2 * m_L + m_R - 1].m_buff[2] =  (m_BUFF_BP_POLES_RE.m_buff[2 * m_L + 2 * m_R - 1] * m_BUFF_BP_POLES_RE.m_buff[2 * m_L + 2 * m_R - 2]) + (m_BUFF_BP_POLES_IM.m_buff[2 * m_L + 2 * m_R - 1] * m_BUFF_BP_POLES_IM.m_buff[2 * m_L + 2 * m_R - 1]);
	}

	return 0;
}
//------------------------------------------------------------------------------
// Выделение памяти:
int Elliptic::allocate()
{
	// выделение памяти под кольцевые буфферы:
	m_BUFF_ZEROS_RE  .allocate(false);
	m_BUFF_ZEROS_IM  .allocate(false);
	m_BUFF_POLES_RE  .allocate(false);
	m_BUFF_POLES_IM  .allocate(false);
	m_BUFF_GAIN      .allocate(false);
	m_BUFF_GAIN_POLES.allocate(false);
	m_BUFF_GAIN_ZEROS.allocate(false);

	// выделение памяти под массивы кольцевых буфферов:
	m_BUFF_ARR_WIND_SX.allocate(true);
	m_BUFF_ARR_WIND_SY.allocate(true);
	m_BUFF_ARR_WIND_CX.allocate(false);
	m_BUFF_ARR_WIND_CY.allocate(false);

	// выделение памяти под массивы нулей и полюсов полосового / режекторного фильтра:
	if ( m_filt_type == 3 || m_filt_type == 4) 
	{
		m_BUFF_BP_POLES_RE.allocate(false);
		m_BUFF_BP_POLES_IM.allocate(false);
		m_BUFF_BP_ZEROS_RE.allocate(false);
		m_BUFF_BP_ZEROS_IM.allocate(false);
	}

 	zeros_calc();  // расчет нулей  нормированного фильтра Чебышева втрого рода
	poles_calc(); // расчет полюсов нормированного фильтра Чебышева втрого рода


	if ( m_filt_type == 1 ) // считаем ФНЧ
	{
		gain_calc();                    // расчет коэффициентов усиления фильтра на нулевой частоте
		LP_to_LP_bilinear_transform(); // билинейное преобразование с частотным преобразованием ФНЧ - ФНЧ
		LPF_HPF_CoeffCalc();          // расчет коэффицицентов
		FreqCharacteristics();
	}

	if (m_filt_type == 2) // считаем ФВЧ
	{
		gain_calc();                    // расчет коэффициентов усиления фильтра на нулевой частоте
		LP_to_HP_bilinear_transform(); // билинейное преобразование с частотным преобразованием ФНЧ - ФВЧ
		LPF_HPF_CoeffCalc();		  // расчет коэффицицентов
		FreqCharacteristics();
	}

	if (m_filt_type == 3) // считаем ПФ
	{
		LP_to_BP_bilinear_transform();  // билинейное преобразование с частотным преобразованием ФНЧ - ПФ
		BPF_BSF_CoeffCalc();           // расчет коэффицицентов
		FreqCharacteristics();
	}


	if (m_filt_type == 4) // считаем РФ
	{
		LP_to_BS_bilinear_transform(); // билинейное преобразование с частотным преобразованием ФНЧ - РФ
		BPF_BSF_CoeffCalc();          // расчет коэффицицентов
		FreqCharacteristics();
	}

	// уничтожение буфферов с промежуточными пепеменными:
	m_BUFF_ZEROS_RE  .deallocate();
	m_BUFF_ZEROS_IM  .deallocate();
	m_BUFF_POLES_RE  .deallocate();
	m_BUFF_POLES_IM  .deallocate();
	m_BUFF_GAIN_POLES.deallocate();
	m_BUFF_GAIN_ZEROS.deallocate();

	if (m_filt_type == 3 || m_filt_type == 4)
	{
		m_BUFF_BP_POLES_RE.deallocate();
		m_BUFF_BP_POLES_IM.deallocate();
		m_BUFF_BP_ZEROS_RE.deallocate();
		m_BUFF_BP_ZEROS_IM.deallocate();
	}


	return 0;
}
//------------------------------------------------------------------------------
// Освобождение памяти:
int Elliptic::deallocate()
{
	m_BUFF_GAIN.deallocate();

	m_BUFF_ARR_WIND_SX.deallocate();
	m_BUFF_ARR_WIND_SY.deallocate();
	m_BUFF_ARR_WIND_CX.deallocate();
	m_BUFF_ARR_WIND_CY.deallocate();

	return 0;
}
//------------------------------------------------------------------------------
// Функция фильтрации:
int Elliptic::filt(double *in)
{
	// *in      - указатель на массив со входной выборкой


	m_sum_num = 0;  // сумма числителя передаточной функции
	m_sum_den = 0; // сумма знаменателя передаточной функции

	// копируем входное значение в нулевой кольцевой буффер массива с кольцевыми буфферами:

	m_BUFF_ARR_WIND_SX.m_MBUFF[0].fill_buff(in);

	for (int m = 0; m < m_BUFF_ARR_WIND_SX.getRingBuffArraySize(); m++)
	{
		// считаем сумму числителя квадратичной секции передаточной функции фильтра:
		m_sum_num = m_BUFF_GAIN.m_buff[m] * (m_BUFF_ARR_WIND_SX.m_MBUFF[m].m_ptr_fill_down[1] * m_BUFF_ARR_WIND_CX.m_MBUFF[m].m_buff[0] + m_BUFF_ARR_WIND_SX.m_MBUFF[m].m_ptr_fill_down[2] * m_BUFF_ARR_WIND_CX.m_MBUFF[m].m_buff[1] + m_BUFF_ARR_WIND_SX.m_MBUFF[m].m_ptr_fill_down[3] * m_BUFF_ARR_WIND_CX.m_MBUFF[m].m_buff[2]);

		// считаем сумму знаменателя квадратичной секции передаточной функции фильтра:
		m_sum_den = m_BUFF_ARR_WIND_SY.m_MBUFF[m].m_ptr_fill_down[1] * m_BUFF_ARR_WIND_CY.m_MBUFF[m].m_buff[1] + m_BUFF_ARR_WIND_SY.m_MBUFF[m].m_ptr_fill_down[2] * m_BUFF_ARR_WIND_CY.m_MBUFF[m].m_buff[2];

		// решаем разностное уравнение:
		m_out = m_sum_num - m_sum_den;

		// записываем решение в текущий массив выходных значений:
		m_BUFF_ARR_WIND_SY.m_MBUFF[m].fill_buff(&m_out);

		// записываем решение на вход следующей квадратичной секции:
		if (m < m_BUFF_ARR_WIND_SX.getRingBuffArraySize() - 1)
		{
			m_BUFF_ARR_WIND_SX.m_MBUFF[m + 1].fill_buff(&m_out);
		}

	}

	return 0;
}
// функция отображения спецификации фильтра:
void Elliptic::showFiltSpec()
{
	if (m_filt_type == 1 || m_filt_type == 2) 
	{
		std::cout << "Elliptic filter specification:" << "\n" << "\n";

		std::cout << "filt_type    - " << m_filt_type    <<"( 1 - lowpass; 2 - highpass; 3 - bandpass; 4 - bandstop)" <<"\n";
		std::cout << "Fs           = " << m_Fs           << "\n";
		std::cout << "Ts           = " << m_Ts           << "\n";
		std::cout << "Fn           = " << m_Fn           << "\n";
		std::cout << "F_stop       = " << m_F_stop       << "\n";
		std::cout << "order        = " << m_order        << "\n";
		std::cout << "m_L          = " << m_L            << "( number of complex conjugate pairs of poles )" << "\n";
		std::cout << "m_R          = " << m_R            << "( number of simple  poles                    )" << "\n";
		std::cout << "epsilon_pass = " << m_epsilon_pass << "\n";
		std::cout << "epsilon_stop = " << m_epsilon_pass << "\n";

		std::cout << "\n" << "\n";
	}
	else if(m_filt_type == 3 || m_filt_type == 4)
	{
		std::cout << "Elliptic filter specification:" << "\n" << "\n";

		std::cout << "filt_type     - " << m_filt_type        << "( 1 - lowpass; 2 - highpass; 3 - bandpass; 4 - bandstop)" << "\n";
		std::cout << "Fs            = " << m_Fs               << "\n";
		std::cout << "Ts            = " << m_Ts               << "\n";
		std::cout << "Fn            = " << m_Fn               << "\n";
		std::cout << "F_stop1       = " << m_F_stop1          << "\n";
		std::cout << "F_stop2       = " << m_F_stop2          << "\n";
		std::cout << "order         = " << 2 * m_order        << "\n";
		std::cout << "m_L           = " << m_L                << "( number of complex conjugate pairs of poles )" << "\n";
		std::cout << "m_R           = " << m_R                << "( number of simple  poles                    )" << "\n";
		std::cout << "epsilon_pass  = " << m_epsilon_pass     << "\n";
		std::cout << "epsilon_stop  = " << m_epsilon_pass     << "\n";

		std::cout << "\n" << "\n";
	}

	
}
//------------------------------------------------------------------------------
// функция отображения нулей и полюсов ФНЧ и ФВЧ:
void Elliptic::showFiltZerosPoles()
{

	if ( m_filt_type == 1 || m_filt_type == 2 ) 
	{
		std::cout << "Elliptic filter zeros:" << "\n" << "\n";

		std::cout << "Real part: " << "\n" << "\n";
		m_BUFF_ZEROS_RE.ShowBuff();
		std::cout << "\n" << "\n";

		std::cout << "Imaginary part: " << "\n" << "\n";
		m_BUFF_ZEROS_IM.ShowBuff();
		std::cout << "\n" << "\n";

		std::cout << "Chebyshev II filter poles:" << "\n" << "\n";

		std::cout << "Real part: " << "\n" << "\n";
		m_BUFF_POLES_RE.ShowBuff();
		std::cout << "\n" << "\n";

		std::cout << "Imaginary part: " << "\n" << "\n";
		m_BUFF_POLES_IM.ShowBuff();
		std::cout << "\n" << "\n";
	}

	if (m_filt_type == 3 || m_filt_type == 4)
	{
		std::cout << "Elliptic filter zeros:" << "\n" << "\n";

		std::cout << "Real part: " << "\n" << "\n";
		m_BUFF_BP_ZEROS_RE.ShowBuff();
		std::cout << "\n" << "\n";

		std::cout << "Imaginary part: " << "\n" << "\n";
		m_BUFF_BP_ZEROS_IM.ShowBuff();
		std::cout << "\n" << "\n";

		std::cout << "Chebyshev II filter poles:" << "\n" << "\n";

		std::cout << "Real part: " << "\n" << "\n";
		m_BUFF_BP_POLES_RE.ShowBuff();
		std::cout << "\n" << "\n";

		std::cout << "Imaginary part: " << "\n" << "\n";
		m_BUFF_BP_POLES_IM.ShowBuff();
		std::cout << "\n" << "\n";
	}

}
//------------------------------------------------------------------------------
// функция отображения коэффициентов фильтра:
void Elliptic::showFiltCoeffs()
{
	std::cout << "Elliptic filter coeffs:" << "\n" << "\n";

	std::cout << "Second order sections gains:" << "\n" << "\n";
	m_BUFF_GAIN.ShowBuff();
	std::cout << "\n" << "\n";

	std::cout << "Numerator second order sections:" << "\n" << "\n";
	m_BUFF_ARR_WIND_CX.ShowBuffArray();
	std::cout << "\n" << "\n";

	std::cout << "Denominator second order sections:" << "\n" << "\n";
	m_BUFF_ARR_WIND_CY.ShowBuffArray();
	std::cout << "\n" << "\n";

	std::cout << "Km and pH for Fn:" << "\n" << "\n";
	std::cout << "Km = " << m_Km << "\n";
	std::cout << "pH = " << m_pH << "\n";
	std::cout << "\n" << "\n";
}