// A.M. Tykvinsky , 29.09.2020

#include "special_functions.hpp"

// АЛГОРИТМ ВЫЧИСЛЕНИЯ СПЕЦИАЛЬНЫХ ФУНКЦИЙ

// Список использованных источников:
// 1. M. Abramowitz, I.A.Stegun " Handbook of Mathematical functions with formulas, graphs and mathematical tables ";
// 2. E. L. Wachspress (2000)   " Evaluating elliptic functions and their inverses ";
// 3. В. П. Волчков, "Вычисление эллиптический функций Якоби для расчета характеристик фильтра Кауэра" 

// Обозначения соответствуют тем, что представлены в источнике [1].

// Расчет эллиптических функций выполнен по методике, описанной в следующей книге: 
// M. Abramowitz, I.A.Stegun "Handbook of Mathematical functions with formulas, graphs and mathematical tables"

// Расчет обратных эллиптических функций выполнен по принципу, описанном в следующей книге: 
// E. L. Wachspress (2000) Evaluating elliptic functions and their inverses

// ---------------------------------------------------
// конструктор по умолчанию:
special_functions::special_functions()
{
    m_N = 30;
}
// ---------------------------------------------------
// деструктор:
special_functions::~special_functions(){}
// ---------------------------------------------------
// факториал числа:
long double special_functions::factorial(int n)
{
	return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}
// ---------------------------------------------------
// Эллиптическая функция am:
double special_functions::am(double u, double k )
{
	long double a[m_N + 1];
	long double g[m_N + 1];
	long double c[m_N + 1];
	long double two_n;
	long double phi;
	int n;

	k = fabsl((long double)k);

	// Проверяем :
	if ( k < -1 ) // если  значение k вышло за диапазон [-1 ; +1] значит устанавливаем граничное значение !
	{
		k = -1;
	}
	else if( k > 1 )
	{
		k = +1;
	}

	// Проверяем особые случаи k = 1 and x = 0 :
	if (k == 0.0) return u;
	if (k == 1.0) return 2.0 * atan(exp(u)) - PI2;

	// Выполняем инициализацию переменных метода арифметико - геометрического среднего (АГС):
	a[0] = 1.0L;
	g[0] = sqrtl(1.0L - k * k);
	c[0] = k;


	// Выполняем последовательность Гауссовых преобразований арифметических 
	// и геометрических значений до тех пор, пока последовательность не сойдется 
	// к определенному значению ( в пределах машиной точности):

	two_n = 1.0L;

	for (n = 0; n < m_N; n++) 
	{
		if (fabsl(a[n] - g[n]) < (a[n] * LDBL_EPSILON)) break;
		two_n += two_n;
		a[n + 1] = 0.5L * (a[n] + g[n]);
		g[n + 1] = sqrtl(a[n] * g[n]);
		c[n + 1] = 0.5L * (a[n] - g[n]);
	}

	// Готовимся к обратной подстановке:

	phi = two_n * a[n] * u;

	// Выполняем обратную подстановку:

	for (; n > 0; n--) phi = 0.5L * (phi + asinl(c[n] * sinl(phi) / a[n]));

	return (double)phi;
}
// ---------------------------------------------------
// Эллиптическая функция sn :
double special_functions::sn(double u, double x)
{
	return sin( am( u , x ) );
}
// ---------------------------------------------------
// Эллиптическая функция cn :
double special_functions::cn(double u, double x)
{
	return cos(am(u , x));
}
// ---------------------------------------------------
// Эллиптическая функция dn :
double special_functions::dn(double u, double x)
{

	// считаем эллиптический синус:
	double SN = sn( u , x );

	// считаем эллиптическую функцию dn:
	return sqrt(1.0 - x * x * SN * SN);
}
// ---------------------------------------------------
// Эллиптическая функция cd :
double special_functions::cd(double u, double x)
{
	return cn(u, x) / dn(u, x);
}
// ---------------------------------------------------
// Эллиптическая функция sd :
double special_functions::sd(double u, double x)
{
	return sn(u, x) / dn(u, x);
}
// ---------------------------------------------------
// Эллиптическая функция nd :
double special_functions::nd(double u, double x)
{
	return 1 / dn(u, x);
}
// ---------------------------------------------------
// Эллиптическая функция dc :
double special_functions::dc(double u, double x)
{
	return dn(u, x) / cn(u, x);
}
// ---------------------------------------------------
// Эллиптическая функция nc :
double special_functions::nc(double u, double x)
{
	return 1 / cn(u, x);
}
// ---------------------------------------------------
// Эллиптическая функция sc :
double special_functions::sc(double u, double x)
{
	return sn(u, x) / cn(u, x);
}
// ---------------------------------------------------
// Эллиптическая функция ns :
double special_functions::ns(double u, double x)
{
	return 1 / sn(u, x);
}
// ---------------------------------------------------
// Эллиптическая функция ds :
double special_functions::ds(double u, double x)
{
	return dn(u, x) / sn(u, x);
}
// ---------------------------------------------------
// Эллиптическая функция cs :
double special_functions::cs(double u, double x)
{
	return cn(u, x) / sn(u, x);
}
// ---------------------------------------------------
// Обратная эллиптическая функция sn:
double special_functions::inv_sn(double sn , double x)
{
	double a[m_N + 1];
	double g[m_N + 1];
	//double c[m_N + 1];
	double s[m_N + 1];

	double two_n;
	double phi;
	double phi_old;
	double k;
	
	double out;
	
	int n   = 0;

	// инициализация:

	k = fabs(x);

	a[0] = 1.0L;
	g[0] = sqrt(1.0 - k * k);
	//c[0] = k;

	// прямой проход:

	two_n = 1.0L;

	for (n = 0; n < m_N - 1; n++)
	{
		s[n]     = 0;

		two_n   += two_n;
		a[n + 1] = 0.5 * (a[n] + g[n]);
		g[n + 1] = sqrt(a[n] * g[n]);
		//c[n + 1] = 0.5 * (a[n] - g[n]);
	}

	// расчет нулевого значения угла:

	phi = asin(sn);

	// расчет конечного значения угла:

	for (n = 1 ; n < m_N ; n++)
	{

		phi_old = phi;
		phi     = atan( g[n-1]/a[n-1] * tan(phi)) + phi;

		if ( fabs(2*phi_old - phi) < 0.001 ) 
		{
			s[n] = 2 * s[n - 1];
		}
		else if (fabs(2 * phi_old - phi) > PI_2) 
		{
			s[n] = 2 * s[n - 1] + 1;
		}

	}

	// Определение переменной u:

	if (sn < 0) 
	{
		out = -(phi + s[n - 1] * PI0) / two_n / a[n - 1];
	}
	else 
	{
		out = +(phi + s[n - 1] * PI0) / two_n / a[n - 1];
	}

	return  out;
}
// ---------------------------------------------------
// Обратная эллиптическая функция cn:
double special_functions::inv_cn(double sn, double x)
{
	double a[m_N + 1];
	double g[m_N + 1];
	//double c[m_N + 1];
	double s[m_N + 1];

	double two_n;
	double phi;
	double phi_old;
	double k;

	double out = 0;
	int    n   = 0;

	// инициализация:

	k = fabs(x);

	a[0] = 1.0L;
	g[0] = sqrt(1.0 - k * k);
	//c[0] = k;

	// прямой проход:

	two_n = 1.0L;

	for (n = 0; n < m_N - 1; n++)
	{
		s[n]     = 0;
		two_n   += two_n;
		a[n + 1] = 0.5 * (a[n] + g[n]);
		g[n + 1] = sqrt(a[n] * g[n]);
		//c[n + 1] = 0.5 * (a[n] - g[n]);
	}

	// расчет нулевого значения угла:

	phi = acos(sn);

	// расчет конечного значения угла:

	for (n = 1; n < m_N ; n++)
	{

		phi_old = phi;
		phi = atan(g[n - 1] / a[n - 1] * tan(phi)) + phi;

		if (fabs(2 * phi_old - phi) < 5 * PI0 / 180 )
		{
			s[n] = 2 * s[n - 1];
		}
		else if (fabs(2 * phi_old - phi) > PI_2)
		{
			s[n] = 2 * s[n - 1] + 1;
		}
	}

	// Определение переменной u:

	out = +(phi + s[n-1] * PI0) / two_n / a[n-1];

	return  out;
}
// ---------------------------------------------------
// Обратная эллиптическая функция dn:
double special_functions::inv_dn(double dn, double x)
{
	double a[m_N + 1];
	double g[m_N + 1];
	//double c[m_N + 1];
	double s[m_N + 1];

	double two_n;
	double phi;
	double phi_old;
	double k;

	double out;
	
	int n = 0;

	// инициализация:

	k = fabs(x);

	a[0] = 1.0L;
	g[0] = sqrt(1.0 - k * k);
	//c[0] = k;

	// прямой проход:

	two_n = 1.0L;

	for (n = 0; n < m_N - 1; n++)
	{
		s[n] = 0;
		two_n += two_n;
		a[n + 1] = 0.5 * (a[n] + g[n]);
		g[n + 1] = sqrt(a[n] * g[n]);
		//c[n + 1] = 0.5 * (a[n] - g[n]);
	}

	// расчет нулевого значения угла:

	phi = asin( sqrt( (1  - dn * dn) / k / k ) );

	// расчет конечного значения угла:

	for (n = 1; n < m_N; n++)
	{
		phi_old = phi;
		phi = atan(g[n - 1] / a[n - 1] * tan(phi)) + phi;

		if (fabs(2 * phi_old - phi) < 5 * PI0 / 180)
		{
			s[n] = 2 * s[n - 1];
		}
		else if (fabs(2 * phi_old - phi) > PI_2)
		{
			s[n] = 2 * s[n - 1] + 1;
		}
	}

	// Определение переменной u:

	out = +(phi + s[n - 1] * PI0) / two_n / a[n - 1];

	return  out;
}
// ---------------------------------------------------
// Обратная эллиптическая функция сd:
double special_functions::inv_cd(double cd, double x)
{
	double a[m_N + 1];
	double g[m_N + 1];
	//double c[m_N + 1];
	double s[m_N + 1];

	double two_n;
	double phi;
	double phi_old;
	double k;

	double out;

	int n = 0;

	// инициализация:

	k = fabs(x);

	a[0] = 1.0L;
	g[0] = sqrt(1.0 - k * k);
	//c[0] = k;

	// прямой проход:

	two_n = 1.0L;

	for (n = 0; n < m_N - 1; n++)
	{
		s[n] = 0;
		two_n += two_n;
		a[n + 1] = 0.5 * (a[n] + g[n]);
		g[n + 1] = sqrt(a[n] * g[n]);
		//c[n + 1] = 0.5 * (a[n] - g[n]);
	}

	// расчет нулевого значения угла:

	phi = acos(sqrt(cd*cd*(x*x-1)/(cd*cd*x*x-1)));

	// расчет конечного значения угла:

	for (n = 1; n < m_N; n++)
	{
		phi_old = phi;
		phi = atan(g[n - 1] / a[n - 1] * tan(phi)) + phi;

		if (fabs(2 * phi_old - phi) < 5 * PI0 / 180)
		{
			s[n] = 2 * s[n - 1];
		}
		else if (fabs(2 * phi_old - phi) > PI_2)
		{
			s[n] = 2 * s[n - 1] + 1;
		}
	}

	// Определение переменной u:

	out = +(phi + s[n - 1] * PI0) / two_n / a[n - 1];

	return  out;
}
// ---------------------------------------------------
// Обратная эллиптическая функция sd:
double special_functions::inv_sd(double sd , double x)
{
	double a[m_N + 1];
	double g[m_N + 1];
	//double c[m_N + 1];
	double s[m_N + 1];

	double two_n;
	double phi;
	double phi_old;
	double k;

	double out;

	int n = 0;

	// инициализация:

	k = fabs(x);

	a[0] = 1.0L;
	g[0] = sqrt(1.0 - k * k);
	//c[0] = k;

	// прямой проход:

	two_n = 1.0L;

	for (n = 0; n < m_N - 1; n++)
	{
		s[n] = 0;
		two_n += two_n;
		a[n + 1] = 0.5 * (a[n] + g[n]);
		g[n + 1] = sqrt(a[n] * g[n]);
		//c[n + 1] = 0.5 * (a[n] - g[n]);
	}

	// расчет нулевого значения угла:

	phi = asin(sqrt(sd*sd / (1 + sd * sd*k*k)));

	// расчет конечного значения угла:

	for (n = 1; n < m_N; n++)
	{
		phi_old = phi;
		phi = atan(g[n - 1] / a[n - 1] * tan(phi)) + phi;

		if (fabs(2 * phi_old - phi) < 5 * PI0 / 180)
		{
			s[n] = 2 * s[n - 1];
		}
		else if (fabs(2 * phi_old - phi) > PI_2)
		{
			s[n] = 2 * s[n - 1] + 1;
		}
	}

	// Определение переменной u:

	if (sd < 0)
	{
		out = -(phi + s[n - 1] * PI0) / two_n / a[n - 1];
	}
	else
	{
		out = +(phi + s[n - 1] * PI0) / two_n / a[n - 1];
	}

	return  out;
}
// ---------------------------------------------------
// Обратная эллиптическая функция nd:
double special_functions::inv_nd(double nd , double x)
{
	double dn;
	
	// считаем эллиптическую функцию dn черех nd:
	dn = 1 / nd;
	
	// считаем обратную эллиптическую функцию dn:
	return inv_dn(dn, x);
}
// ---------------------------------------------------
// Обратная эллиптическая функция dc:
double special_functions::inv_dc(double dc , double x)
{
	double a[m_N + 1];
	double g[m_N + 1];
	//double c[m_N + 1];
	double s[m_N + 1];

	double two_n;
	double phi;
	double phi_old;
	double k;

	double out;

	int n = 0;

	// инициализация:

	k = fabs(x);

	a[0] = 1.0L;
	g[0] = sqrt(1.0 - k * k);
	//c[0] = k;

	// прямой проход:

	two_n = 1.0L;

	for (n = 0; n < m_N - 1; n++)
	{
		s[n] = 0;
		two_n += two_n;
		a[n + 1] = 0.5 * (a[n] + g[n]);
		g[n + 1] = sqrt(a[n] * g[n]);
		//c[n + 1] = 0.5 * (a[n] - g[n]);
	}

	// расчет нулевого значения угла:

	phi = acos(sqrt((1-k*k)/(dc*dc-k*k)));

	// расчет конечного значения угла:

	for (n = 1; n < m_N; n++)
	{
		phi_old = phi;
		phi = atan(g[n - 1] / a[n - 1] * tan(phi)) + phi;

		if (fabs(2 * phi_old - phi) < 5 * PI0 / 180)
		{
			s[n] = 2 * s[n - 1];
		}
		else if (fabs(2 * phi_old - phi) > PI_2)
		{
			s[n] = 2 * s[n - 1] + 1;
		}
	}

	// Определение переменной u:

	out = +(phi + s[n - 1] * PI0) / two_n / a[n - 1];

	return  out;
}
// ---------------------------------------------------
// Обратная эллиптическая функция nc:
double special_functions::inv_nc(double nc , double x)
{
	double cn;

	// Считаем эллиптическую функцию cn черех nc:
	cn = 1 / nc;

	// Считаем обратную эллиптическую функцию nc:

	return  inv_cn(cn , x);
}
// ---------------------------------------------------
// Обратная эллиптическая функция sc:
double special_functions::inv_sc(double sc , double x)
{
	double a[m_N + 1];
	double g[m_N + 1];
	//double c[m_N + 1];
	double s[m_N + 1];

	double two_n;
	double phi;
	double phi_old;
	double k;

	double out;

	int n = 0;

	// инициализация:

	k = fabs(x);

	a[0] = 1.0L;
	g[0] = sqrt(1.0 - k * k);
	//c[0] = k;

	// прямой проход:

	two_n = 1.0L;

	for (n = 0; n < m_N - 1; n++)
	{
		s[n] = 0;
		two_n += two_n;
		a[n + 1] = 0.5 * (a[n] + g[n]);
		g[n + 1] = sqrt(a[n] * g[n]);
		//c[n + 1] = 0.5 * (a[n] - g[n]);
	}

	// расчет нулевого значения угла:

	phi = atan(sc);

	// расчет конечного значения угла:

	for (n = 1; n < m_N; n++)
	{
		phi_old = phi;
		phi = atan(g[n - 1] / a[n - 1] * tan(phi)) + phi;

		if (fabs(2 * phi_old - phi) < 5 * PI0 / 180)
		{
			s[n] = 2 * s[n - 1];
		}
		else if (fabs(2 * phi_old - phi) > PI_2)
		{
			s[n] = 2 * s[n - 1] + 1;
		}
	}

	// Определение переменной u:

	if (sc < 0)
	{
		out = -(phi + s[n - 1] * PI0) / two_n / a[n - 1];
	}
	else
	{
		out = +(phi + s[n - 1] * PI0) / two_n / a[n - 1];
	}

	return  out;
}
// ---------------------------------------------------
// Обратная эллиптическая функция ns:
double special_functions::inv_ns(double ns , double x)
{
	double sn;

	// Считаем эллиптическую функцию sn черех ns:

	sn = 1 / ns;

	// Считаем обратную эллиптическую функцию ns:

	return  inv_sn(sn, x);
}
// ---------------------------------------------------
// Обратная эллиптическая функция ds:
double special_functions::inv_ds(double ds, double x)
{
	double sd;

	// Считаем функцию sd через ds:
	sd = 1 / ds;

	// Считаем обратную функцию sd:
	return inv_sd(sd , x);
}
// ---------------------------------------------------
// Обратная эллиптическая функция cs:
double special_functions::inv_cs(double cs, double x)
{
	double sc;

	// Считаем эллиптическую функцию sc через cs:
	sc = 1 / cs;

	// Считаем обратную функцию (cs)^-1:

	return  inv_sc(sc , x);
}
// ---------------------------------------------------
// Полный эллиптический интеграл первого рода:
double special_functions::ellip_K(double k) 
{
	long double a[m_N + 1];
	long double g[m_N + 1];
	//long double c[m_N + 1];
	long double two_n;
	//long double phi;
	int n;

	k = fabsl((long double)k);

	// Проверяем :
	if (k < -1) // если  значение k вышло за диапазон [-1 ; +1] значит устанавливаем граничное значение !
	{
		k = -1;
	}
	else if (k > 1)
	{
		k = +1;
	}

	// Выполняем инициализацию переменных метода арифметико - геометрического среднего (АГС):
	a[0] = 1.0L;
	g[0] = sqrtl(1.0L - k * k);
	//c[0] = k;

	// Выполняем последовательность Гауссовых преобразований арифметических 
	// и геометрических значений до тех пор, пока последовательность не сойдется 
	// к определенному значению ( в пределах машиной точности):

	two_n = 1.0L;

	for (n = 0; n < m_N; n++)
	{
		if (fabsl(a[n] - g[n]) < (a[n] * LDBL_EPSILON)) break;
		two_n += two_n;
		a[n + 1] = 0.5L * (a[n] + g[n]);
		g[n + 1] = sqrtl(a[n] * g[n]);
		//c[n + 1] = 0.5L * (a[n] - g[n]);
	}

	return PI_2 / a[n];
}
// ---------------------------------------------------
// Полный эллиптический интеграл второго рода:
double special_functions::ellip_E(double k)
{
	long double a[m_N + 1];
	long double g[m_N + 1];
	long double c[m_N + 1];
	long double two_n = 1.0L;
	long double sum   = 0;
	int n;

	k = fabsl((long double)k);

	// Проверяем :
	if (k < -1) // если  значение k вышло за диапазон [-1 ; +1] значит устанавливаем граничное значение !
	{
		k = -1;
	}
	else if (k > 1)
	{
		k = +1;
	}

	// Выполняем инициализацию переменных метода арифметико - геометрического среднего (АГС):
	a[0] = 1.0L;
	g[0] = sqrtl(1.0L - k * k);
	c[0] = k;

	// Выполняем последовательность Гауссовых преобразований арифметических 
	// и геометрических значений до тех пор, пока последовательность не сойдется 
	// к определенному значению ( в пределах машиной точности):

	for (n = 0; n < m_N; n++)
	{
		if (fabsl(a[n] - g[n]) < (a[n] * LDBL_EPSILON)) break;
		two_n += two_n;
		a[n + 1] = 0.5L * (a[n] + g[n]);
		g[n + 1] = sqrtl(a[n] * g[n]);
		c[n + 1] = 0.5L * (a[n] - g[n]);

		sum = 0.5 * c[n + 1] * c[n + 1] * two_n + sum;

	}

	return PI_2*(1 - (sum + 0.5 * c[0] * c[0]))/ a[n];
}
// ---------------------------------------------------
// гамма функция целочисленного аргумента:
long double special_functions::gamma_int(int n)
{
	return factorial(n-1);
}
// ---------------------------------------------------
// функция Бесселя первого рода нулевого порядка:
long double special_functions::bessel_I0(long double x)
{

	long double  sum    = 0;
	long double  fact_n = 0;

	for (int n = 0 ; n < m_N; n++) 
	{
		fact_n = powl(factorial(n), 2);

		if (fact_n == 0) break; // если вышли за пределы разрядной сетки, то заканчиваем считать

		sum    = sum + pow(-1, n) * pow(x / 2, 2 * n) / fact_n;
	}

	return sum;
}
// ---------------------------------------------------
// функция Бесселя первого рода первого порядка:
long double special_functions::bessel_I1(long double x)
{
	long double    sum      = 0;
	long double    fact_n   = 0;
	long double    fact_n_1 = 0;

	for (int n = 0; n < m_N; n++)
	{
		fact_n   = factorial(n);
		fact_n_1 = factorial(n + 1);

		if (fact_n == 0 || fact_n_1 == 0) break; // если вышли за пределы разрядной сетки, то заканчиваем считать

		sum = sum + pow(-1, n) * powl(x / 2, 2 * n) / fact_n / fact_n_1;
	}

	return sum;
}
// ---------------------------------------------------
// функция Бесселя первого рода произвольного порядка:
long double special_functions::bessel_In(long double x , int order)
{
	long double sum          = 0;
	long double fact_n       = 0;
	long double fact_n_order = 0;

	for (int n = 0; n < m_N; n++)
	{

		fact_n       = factorial(n);
		fact_n_order = factorial(n + order);

		if (fact_n == 0 || fact_n_order == 0) break; // если вышли за пределы разрядной сетки, то заканчиваем считать

		sum = sum + powl(-1, n) * powl(x / 2, 2 * n + order) / fact_n / fact_n_order;
	}

	return sum;
}
// ---------------------------------------------------
// модифицированная функция Бесселя первого рода произвольного порядка:
long double special_functions::mod_bessel_In(long double x, int order)
{
	long double  sum = 0;
	long double  gamma1    = 0;
	long double  gamma2    = 0;

	for (int n = 0; n < 16; n++)
	{

		if ( (double)gamma2 / (double)ULLONG_MAX > 0.9 ) break; // если вышли за пределы разрядной сетки, то заканчиваем считать

		gamma1    = gamma_int(n + 1);
		gamma2    = gamma_int(n + order + 1);
		sum       = sum + powl(x / 2, 2 * n + order) / gamma1 / gamma2;

	}

	return sum;
}
// -----------------------------------------------------------------------------------------------------------------------------------
// функция линейной свертки полиномов:
int special_functions::conv(double *a, double *b, double *c, int Na, int Nb, int Nc, bool mode)
{
	//int idx_a = 0;
	int idx_b = 0;
	int idx_e = 0;

	double A = 0;
	double B = 0;

	if (mode == true) // свертка линейная
	{
		if ( (Nc >= Na + Nb - 1) && ( (a != NULL) && (b != NULL) && (c != NULL) ) )
		{
			// цикл расчета линейной свертки двух массивов:
			for (int n = 0; n <= Nc - 1; n++)
			{
				idx_b = n;
				idx_e = n;

				if (idx_e >= Na)
				{
					idx_e = Na - 1;
				}

				for (int m = 0; m <= idx_e; m++)
				{

					A = a[m];

					if (idx_b >= Nb)
					{
						B = 0;
					}
					else
					{
						B = b[idx_b];
					}

					c[n] = c[n] + A * B;
					idx_b = idx_b - 1;
				}
			}
		}
		else
		{
			for (int n = 0; n <= Nc; n++) c[n] = 0;
		}
	}
	else 
	{
		if ( (Nb == Na) && (Nc == Nb) && (a != NULL) && (b != NULL) && (c != NULL) )
		{
			// цикл расчета линейной свертки двух массивов:
			for (int n = 0; n <= Nc - 1; n++)
			{
				idx_b = n;
				idx_e = n;

				if (idx_e >= Na)
				{
					idx_e = Na - 1;
				}

				for (int m = 0; m <= idx_e; m++)
				{

					A = a[m];

					if (idx_b >= Nb)
					{
						B = 0;
					}
					else
					{
						B = b[idx_b];
					}

					c[n] = c[n] + A * B;
					idx_b = idx_b - 1;
				}
			}
		}
		else
		{
			for (int n = 0; n <= Nc; n++) c[n] = 0;
		}
	}

	return 0;
}
// -----------------------------------------------------------------------------------------------------------------------------------
// функция линейной свертки полиномов:
int special_functions::conv(float *a, float *b, float *c, int Na, int Nb, int Nc, bool mode)
{
	//int idx_a = 0;
	int idx_b = 0;
	int idx_e = 0;

	double A = 0;
	double B = 0;

	if (mode == true) // свертка линейная
	{
		if ( (Nc >= Na + Nb - 1) && (a != NULL) && (b != NULL) && (c != NULL) )
		{
			// цикл расчета линейной свертки двух массивов:
			for (int n = 0; n <= Nc - 1; n++)
			{
				idx_b = n;
				idx_e = n;

				if (idx_e >= Na)
				{
					idx_e = Na - 1;
				}

				for (int m = 0; m <= idx_e; m++)
				{

					A = a[m];

					if (idx_b >= Nb)
					{
						B = 0;
					}
					else
					{
						B = b[idx_b];
					}

					c[n] = c[n] + A * B;
					idx_b = idx_b - 1;
				}
			}
		}
		else
		{
			for (int n = 0; n <= Nc; n++) c[n] = 0;
		}
	}
	else
	{
		if ( (Nb == Na) && (Nc == Nb) && (a != NULL) && (b != NULL) && (c != NULL) )
		{
			// цикл расчета линейной свертки двух массивов:
			for (int n = 0; n <= Nc - 1; n++)
			{
				idx_b = n;
				idx_e = n;

				if (idx_e >= Na)
				{
					idx_e = Na - 1;
				}

				for (int m = 0; m <= idx_e; m++)
				{

					A = a[m];

					if (idx_b >= Nb)
					{
						B = 0;
					}
					else
					{
						B = b[idx_b];
					}

					c[n] = c[n] + A * B;
					idx_b = idx_b - 1;
				}
			}
		}
		else
		{
			for (int n = 0; n <= Nc; n++) c[n] = 0;
		}
	}

	return 0;
}
