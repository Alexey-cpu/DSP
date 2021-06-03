//-----------------------------------------------------------------------------------------------------------------------------------
// A.M. Tykvinsky, 22.02.2021
//-----------------------------------------------------------------------------------------------------------------------------------
//  класс комплексных чисел
//-----------------------------------------------------------------------------------------------------------------------------------

#ifndef _CPLX_NUM_HPP
#define _CPLX_NUM_HPP

#include <math.h>
#include <ymath.h>
#include <sstream>

template <typename T>
struct Node
{
  T Re;
  T Im;
};

typedef double cplx_type; // если нужно помен¤ть тип значени¤, то нужно мен¤ть тип псевдонима ( например вместо double вписать float )

class cplx_num
{
public:

	Node<double> N;

	// действительна¤ и мнима¤ части комплексного числа:
	cplx_type m_re;
	cplx_type m_im;

	// конструкторы:
	cplx_num();                            // конструктор по умолчаню
	cplx_num(cplx_type re);                // конструктор по умолчанию с созданием действительного числа
	cplx_num(cplx_type re , cplx_type im); // конструктор дл¤ создани¤ комплексного числа
	cplx_num(const cplx_num &cplx);        // конструктор копировани¤

	// деструктор:
	~cplx_num();

	// функции:

	// модуль комплексного числа:
	inline static cplx_type abs(cplx_num cplx)
	{
		return sqrt(cplx.m_re * cplx.m_re + cplx.m_im * cplx.m_im);
	}

	// аргумент комплексного числа:
	inline static cplx_type arg(cplx_num cplx)
	{
		return atan2(cplx.m_im , cplx.m_re);
	}

	// корень квадратный из комплексного числа:
	inline static cplx_num cplx_sqrt(cplx_num cplx)
	{
		// извлечение квадратного корня из комплексного числа:
		double abs = sqrt (cplx.m_re * cplx.m_re + cplx.m_im * cplx.m_im);
		double arg = atan2(cplx.m_im , cplx.m_re);
		cplx.m_re = sqrt(abs) * cos(arg / 2);
		cplx.m_im = sqrt(abs) * sin(arg / 2);
		return cplx;
	}

	// присваивание другого комплексного числа:
	inline cplx_num &operator = (cplx_num cplx)
	{
		m_re = cplx.m_re;
		m_im = cplx.m_im;
		return (*this);
	}

	// сложение:
	inline cplx_num operator + (cplx_num cplx)
	{
		return cplx_num( m_re + cplx.m_re , m_im + cplx.m_im);
	}

	// вычитание:
	inline cplx_num operator - (cplx_num &cplx)
	{
		return cplx_num(m_re - cplx.m_re, m_im - cplx.m_im);
	}

	// умножение:
	inline cplx_num operator * (cplx_num &cplx)
	{
		return cplx_num(m_re * cplx.m_re - m_im * cplx.m_im , m_re * cplx.m_im + m_im * cplx.m_re);
	}

	// деление:
	inline cplx_num operator / (cplx_num &cplx)
	{

		cplx_type abs2 = cplx.m_re * cplx.m_re + cplx.m_im * cplx.m_im;
		return cplx_num( (m_re * cplx.m_re + m_im * cplx.m_im) / abs2 , (m_im * cplx.m_re - m_re * cplx.m_im) / abs2 );
	}

};

#endif
