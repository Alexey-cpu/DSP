// A.M. Tykvinsky , 29.09.2020

#ifndef _SPECIAL_FUNCTIONS_HPP
#define _SPECIAL_FUNCTIONS_HPP

//#define N 30 // к - во итераций, достаточное для обеспечения достаточной точности вычисления эллиптических функций

#include <iostream>
#include <float.h>      // required for LDBL_EPSILON, DBL_MAX
#include <math.h>
#include <fstream>

// заголовки пользовательских библиотек:
#include "math_const.hpp"

// АЛГОРИТМ ВЫЧИСЛЕНИЯ СПЕЦИАЛЬНЫХ ФУНКЦИЙ

class special_functions
{

  int m_N; // к - во итераций, достаточное для обеспечения достаточной точности вычисления эллиптических функций

public:

	// Эллиптические функции:
	double am(double u, double x);
	double sn(double u, double x);
	double cn(double u, double x);
	double dn(double u, double x);
	double cd(double u, double x);
	double sd(double u, double x);
	double nd(double u, double x);
	double dc(double u, double x);
	double nc(double u, double x);
	double sc(double u, double x);
	double ns(double u, double x);
	double ds(double u, double x);
	double cs(double u, double x);

	// Обратные эллиптические функции:
	double inv_sn(double sn , double x);
	double inv_cn(double cn , double x);
	double inv_dn(double cn , double x);
	double inv_cd(double u  , double x);
	double inv_sd(double u  , double x);
	double inv_nd(double u  , double x);
	double inv_dc(double u  , double x);
	double inv_nc(double u  , double x);
	double inv_sc(double u  , double x);
	double inv_ns(double u  , double x);
	double inv_ds(double u  , double x);
	double inv_cs(double u  , double x);

	// эллиптические интегралы:
	double ellip_K(double k); // полный эллиптический интеграл первого рода
	double ellip_E(double k); // полный эллиптический интеграл второго рода

	// гамма функция целого аргумента и факториал:
	long double gamma_int(int n);
	long double factorial(int n);

	// функции Бесселя:
	long double bessel_I0     (long double x );                // функция Бесселя первого рода нулевого порядка
	long double bessel_I1     (long double x );              // функция Бесселя первого рода первого порядка
	long double bessel_In     (long double x, int order);   // функция Бесселя первого рода произвольного порядка

	long double mod_bessel_In (long double x, int order);  // модифицированная функция Бесселя произволного порядка
	
	// функция линейной/циклической свертки двух полиномов (т.е. их перемножения):
	static int conv(double *a , double *b , double *c , int Na , int Nb , int Nc , bool mode);
	static int conv(float  *a , float  *b , float  *c , int Na , int Nb , int Nc , bool mode);

	// конструктор по умолчанию
	special_functions();

	// деструктор
	~special_functions();
};

#endif
