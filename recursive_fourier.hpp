//---------------------------------------------------------------------------------------------------------------------------------------
// A.M. Tykvinsky, 28.11.2020
//---------------------------------------------------------------------------------------------------------------------------------------
// RECURSIVE FOURIER FILTER:
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef _RECURSIVE_FOURIER_HPP
#define _RECURSIVE_FOURIER_HPP

#include <math.h>

// исходные файлы вложенных функциональных блоков:
#include "mirror_ring_buff_x32.hpp"
#include "math_const.hpp"
#include "aperiodic_filt_eq.hpp"
#include "simple_IIR.hpp"

class recursive_fourier
{
	public:
	
	// system variables:

	float m_Fn;       // nominal frequency
	float m_Fs;       // sampling frequency
	float m_Ts;       // sampling period
	float m_Gain;     // gain
	int   m_ElemNum;
	float m_Ns;       // number of samples per period
	float m_order;    // filter order
	float m_hnum;	  // harmonic number

	bool  m_oddh;

	// buffers:
	mirror_ring_buff m_BUFF_WIND_SX; // input samples buffer

	// auxiliary variables:
	float m_a , m_a0 , m_b , m_Ks, m_Kc , m_K1 , m_K2;
	double m_flt;

	// filters:
	aperiodic_filt_eq m_FILTER_EQ;

	public:
	
	// filter input:
	double m_in_F;

	// filter output:
	double m_Re , m_Im , m_Km , m_pH;

	// constructor and destructor:
	recursive_fourier();
	~recursive_fourier();
	
	// filtering function:
	inline int filt ( double *input )
	{

	    //m_FILTER_EQ.filt( input  , m_oddh );

	    // buffer filling:
	    m_BUFF_WIND_SX.fill_buff( input );

	    // filtering:		
	    m_a0 = m_a + ( *input - m_BUFF_WIND_SX.m_ptr_fill_down[ m_ElemNum ] ) * m_Gain;
	    m_a  = m_a0 * m_Kc - m_b * m_Ks;
	    m_b  = m_a0 * m_Ks + m_b * m_Kc;

	    m_Re = m_a;
	    m_Im = m_b;
				
	    return 0;
	}
	
	// Frequency characteristics calculation:
	int FreqCharacteristics();

	// filter initialization function:
	int filtInit( float Fn , float Fs , int hnum );
	
	// memory allocation and deallocation functions:
	void allocate  ();
	void deallocate();
};

#endif
