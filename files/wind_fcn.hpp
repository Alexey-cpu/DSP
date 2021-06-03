// A.M. Tykvinsky , 29.09.2020

#ifndef _WIND_FCN_HPP
#define _WIND_FCN_HPP

#include <cmath>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "special_functions.hpp"
#include "math_const.hpp"

// ОКОННАЯ ФУНКЦИЯ

class wind_fcn
{

protected:

	double m_wind_fcn;      // оконная функция
	int    m_order;       // порядок фильтра

	special_functions m_SPEC_FCN;      // специальные математические функции

public:

	// Массивы кольцевых буфферов: 	
	double   *m_BUFF_WIND;

	// входы фильтра:
	double    m_Km;             // Коэффициент компенсации искажения амплитуды
	double    m_pH;            // Коэффициент компенсации искажения фазы
	double    m_in_F;         // текущее значение частоты

	bool      m_wind_ready; // логический маркер, говорящий о том, что оконная функция уже спроектирована

	//Конструктор:
	wind_fcn();

	//Деструктор:
	~wind_fcn();

	// функция расчета АЧХ и ФЧХ фильтра:
	int FreqCharacteristics();

	// функция инициализации окна:
	int windInit(int order);

	// функция выделения памяти под окно фильтра
	int allocate();

	// функция освобождения памяти
	int deallocate();

	// оконные функции:
	int Bartlett	   ();
	int BartlettHanning();
	int Blackman	   ();
	int BlackmanHarris ();
	int Bohman         ();
	int Chebyshev      (double atten);
	int FlatTop        ();
	int Gaussian       (double alpha);
	int Hamming        ();
	int Hann           ();
	int Kaiser         (double betta);
	int Nutall         ();
	int Parzen         ();
	int Rectangular    ();
	int Triangular     ();
	int Tukey          (double R);

	// функция расчета коэффициентов полинома Чебышева первого рода ( для расчета коэффициентов окна Чебышева ):
	double cheby_poly(int n, double x);

};

#endif
