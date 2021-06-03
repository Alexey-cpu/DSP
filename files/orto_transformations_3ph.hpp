// A.M. Tykvinsky, 27.11.2020
//-----------------------------------------------------------
// “–≈’‘ј«Ќџ≈ ќ–“ќ√ќЌјЋ№Ќџ≈ ѕ–≈ќЅ–ј«ќ¬јЌ»я:
//-----------------------------------------------------------

#ifndef _ORTO_TRANSFORMATIONS_3PH_HPP
#define _ORTO_TRANSFORMATIONS_3PH_HPP

#include <math.h>
#include "math_const.hpp"

class orto_transformations_3ph
{

public:

	// ¬ыходные переменные:

	float m_A;        // ортогональна¤ составл¤юща¤ по оси alpha в преобразовании  ларка
	float m_B;       // ортогональна¤ составл¤юща¤ по оси betta в преобразовании  ларка
	float m_D;      // ортогональна¤ составл¤юща¤ по оси d в преобразовании ѕарка-√орева
	float m_Q;     // ортогональна¤ составл¤юща¤ по оси q в преобразовании ѕарка-√орева
	float m_Z;    // составл¤юща¤ нулевой последовательности
	int    m_cnt; // внутренний счетчик преобразовани¤ ѕарка-√орева доступен дл¤ обнулени¤ по PPS при синхронизации

	// функции трехфазных преобразований:
	int Clark(double *A , double *B , double *C);
	int Clark(float  *A , float  *B , float  *C );
	
	int Park (double *A, double *B, double *C , double Fn , double Fs, bool A_axis_alignment = false);
	int Park (float  *A, float  *B, float  *C , float  Fn , float  Fs, bool A_axis_alignment = false);

	// конструктор:
	orto_transformations_3ph();

	// конструктор:
	~orto_transformations_3ph();
};

#endif