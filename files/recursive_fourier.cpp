//---------------------------------------------------------------------------------------------------------------------------------------
// A.M. Tykvinsky, 28.11.2020
//---------------------------------------------------------------------------------------------------------------------------------------
// RECURSIVE FOURIER FILTER:
//---------------------------------------------------------------------------------------------------------------------------------------

#include "recursive_fourier.hpp"

//---------------------------------------------------------------------------------------------------------------------------------------
// default constructor and destructor:
//---------------------------------------------------------------------------------------------------------------------------------------
recursive_fourier::recursive_fourier()
{
    // system variables initialization:
    m_Fn       = 50;
    m_Fs       = 4000;
    m_Ts       = 1 / m_Fs;
    m_Ns       = ceil(m_Fs / m_Fn);
    m_order    = m_Ns;
    m_ElemNum  = m_Ns + 1;
    m_Gain     = 2 / m_Ns;
    m_hnum     = 1;
    m_Ks       = sin( PI2 * m_hnum / m_Ns );
    m_Kc       = cos( PI2 * m_hnum / m_Ns );
    m_in_F     = m_Fn;

    // filter output initialization:
    m_a0 = 0;
    m_a  = 0;
    m_b  = 0;
    m_Km = 0;
    m_pH = 0;

    // buffers initialization:
    m_BUFF_WIND_SX.BuffInit(m_order+2);
}

recursive_fourier::~recursive_fourier(){}
//---------------------------------------------------------------------------------------------------------------------------------------
// filter initialization function:
//---------------------------------------------------------------------------------------------------------------------------------------
int recursive_fourier::filtInit( float Fn , float Fs , int hnum )
{
    // system variables initialization:
    m_Fn      = Fn;
    m_Fs      = Fs;
    m_Ts      = 1 / m_Fs;
    m_Ns      = ceil(m_Fs / m_Fn);
    m_order   = m_Ns;
    m_ElemNum = m_Ns + 1;
    m_Gain    = 2 / m_Ns;
    m_hnum    = hnum;
    m_Ks      = sin( PI2 * m_hnum / m_Ns );
    m_Kc      = cos( PI2 * m_hnum / m_Ns );
    m_in_F    = m_Fn;
	
    // filter output initialization:
    m_a0 = 0;
    m_a  = 0;
    m_b  = 0;
    m_Km = 0;
    m_pH = 0;

    // buffers initialization:
    m_BUFF_WIND_SX.BuffInit(m_order+2);

        return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------------
// frequency characteristics calculation function:
//---------------------------------------------------------------------------------------------------------------------------------------
int recursive_fourier::FreqCharacteristics()
{
  // complex frequency coeffs:
  double Re1 = 0 , Im1 = 0 , Re2 = 0 , Im2 = 0 , K = 1 / m_Ns;

  // transfer function:
  Re1 = 1 - cos( -PI2 * m_in_F * m_Ns * m_Ts );
  Im1 = 0 - sin( -PI2 * m_in_F * m_Ns * m_Ts );
  Re2 = 1 - ( cos(-PI2 * m_in_F * m_Ts) * m_Kc - sin(-PI2 * m_in_F * m_Ts) * m_Ks );
  Im2 = 0 - ( cos(-PI2 * m_in_F * m_Ts) * m_Ks + sin(-PI2 * m_in_F * m_Ts) * m_Kc );

  // amplitude and phase response:
  m_Km = sqrt( Re1 * Re1 + Im1 * Im1 ) / sqrt( Re2 * Re2 + Im2 * Im2 ) * K;
  m_pH = atan2( Im1 , Re1 ) - atan2( Im2 , Re2 );

  if( m_in_F == m_Fn * m_hnum ) { m_Km = 1; m_pH = 0; }

  return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------------
// memoty allocation function:
//---------------------------------------------------------------------------------------------------------------------------------------
void recursive_fourier::allocate()
{
    // memory allocation:
    m_BUFF_WIND_SX.allocate(true);

}
//---------------------------------------------------------------------------------------------------------------------------------------
// memoty deallocation function:
//---------------------------------------------------------------------------------------------------------------------------------------
void recursive_fourier::deallocate()
{
    // memory deallocation:
    m_BUFF_WIND_SX.deallocate();
}
