// A.M. Tykvinsky , 29.09.2020

#ifndef _WIND_FCN_HPP
#define _WIND_FCN_HPP

#include <cmath>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "special_functions.hpp"
#include "math_const.hpp"

// ������� �������

class wind_fcn
{

protected:

	double m_wind_fcn;      // ������� �������
	int    m_order;       // ������� �������

	special_functions m_SPEC_FCN;      // ����������� �������������� �������

public:

	// ������� ��������� ��������: 	
	double   *m_BUFF_WIND;

	// ����� �������:
	double    m_Km;             // ����������� ����������� ��������� ���������
	double    m_pH;            // ����������� ����������� ��������� ����
	double    m_in_F;         // ������� �������� �������

	bool      m_wind_ready; // ���������� ������, ��������� � ���, ��� ������� ������� ��� ��������������

	//�����������:
	wind_fcn();

	//����������:
	~wind_fcn();

	// ������� ������� ��� � ��� �������:
	int FreqCharacteristics();

	// ������� ������������� ����:
	int windInit(int order);

	// ������� ��������� ������ ��� ���� �������
	int allocate();

	// ������� ������������ ������
	int deallocate();

	// ������� �������:
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

	// ������� ������� ������������� �������� �������� ������� ���� ( ��� ������� ������������� ���� �������� ):
	double cheby_poly(int n, double x);

};

#endif
