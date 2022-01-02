//--------------------------------------------------------------------------------
//A.M.Tykvinsky, 21.12.2021
//--------------------------------------------------------------------------------
// TEMPLATE RECURSIVE FOURIER CLASS
//--------------------------------------------------------------------------------

#ifndef RECURSIVE_FOURIER_H
#define RECURSIVE_FOURIER_H


#ifndef __ALG_PLATFORM // identify if the compilation is for ProsoftSystems IDE
#include "cmath"
#endif

#include "buffer.h"

// customized C data types definition:
#ifndef __ix16
#define __ix16 short
#endif

#ifndef __ix32
#define __ix32 int
#endif

#ifndef __ix64
#define __ix64 long long
#endif

#ifndef __uix16
#define __uix16 unsigned short
#endif

#ifndef __uix32
#define __uix32 unsigned int
#endif

#ifndef __uix64
#define __uix64 unsigned long long
#endif

#ifndef __fx32
#define __fx32 float
#endif

#ifndef __fx64
#define __fx64 double
#endif

// customized math constants:
#ifndef PI0 // PI
#define PI0 3.1415926535897932384626433832795
#endif

#ifndef PI2 // 2*PI
#define PI2 6.283185307179586476925286766559
#endif

// recursive Fourier
template< typename T > class recursive_fourier;

// Recursive Fourier implementations:

// float x32:
template<> class recursive_fourier<__fx32>
{
    typedef __fx32 __type;

    // system variables:
    __type m_Fn;
    __type m_Fs;
    __type m_Ts;
    __type m_Gain;
    __type m_Ns;
    __type m_hnum;
    __type m_a0 , m_Ks , m_Kc;
    __ix32 m_order;

    // buffer:
    mirror_ring_buffer<__type> m_buffer_sx;

public:
    // filter output:
    __type m_a , m_b , m_Km , m_pH;

    // memory allocation and deallocation functions:
    __ix32  allocate() { return ( m_buffer_sx.allocate( m_order + 2 ) ); }
    void deallocate() { m_buffer_sx.deallocate(); }

    // initialization function:
    void init( __type Fn , __type Fs , __ix32 hnum )
    {
        // system variables initialization:
        m_Fn      = Fn;
        m_Fs      = Fs;
        m_Ts      = 1 / m_Fs;
        m_Ns      = ceil(m_Fs / m_Fn);
        m_order   = m_Ns;
        m_Gain    = ( hnum == 0 ) ? ( 1.0 / m_Ns ) : ( 2.0 / m_Ns );
        m_hnum    = hnum;
        m_Ks      = sin( PI2 * m_hnum / m_Ns );
        m_Kc      = cos( PI2 * m_hnum / m_Ns );

        // filter output initialization:
        m_a0 = 0;
        m_a  = 0;
        m_b  = 0;
        m_Km = 0;
        m_pH = 0;
    }

    // default constructor:
    recursive_fourier()
    {
        // system variables initialization:
        m_Fn       = 50;
        m_Fs       = 4000;
        m_Ts       = 1 / m_Fs;
        m_Ns       = ceil(m_Fs / m_Fn);
        m_order    = m_Ns;
        m_Gain     = 2.0 / m_Ns;
        m_hnum     = 1;
        m_Ks       = sin( PI2 * m_hnum / m_Ns );
        m_Kc       = cos( PI2 * m_hnum / m_Ns );

        // filter output initialization:
        m_a0 = 0;
        m_a  = 0;
        m_b  = 0;
        m_Km = 0;
        m_pH = 0;
    };

    // constructor with initiallization:
    recursive_fourier( __type Fn , __type Fs , __ix32 hnum ) { init( Fn , Fs , hnum ); }

    // default destructor:
    ~recursive_fourier(){ deallocate(); }

    // filtering function:

    // float x32 input:
    inline void filt ( __type *input )
    {
        m_buffer_sx.fill_buff( input );
        m_a0 = m_a + ( *input - m_buffer_sx[ m_order ] ) * m_Gain;
        m_a  = m_a0 * m_Kc - m_b * m_Ks;
        m_b  = m_a0 * m_Ks + m_b * m_Kc;
    }

    // float x64 input ( CAUTION !!! ROUNDING ERROR OCCURS !!! )
    inline void filt ( __fx64 *input )
    {
        m_buffer_sx.fill_buff( input );
        m_a0 = m_a + ( *input - m_buffer_sx[ m_order ] ) * m_Gain;
        m_a  = m_a0 * m_Kc - m_b * m_Ks;
        m_b  = m_a0 * m_Ks + m_b * m_Kc;
    }

    // Frequency characteristics computation function:
    __ix32 FreqCharacteristics( __type F )
    {
        // complex frequency coeffs:
        __type Re1 = 0 , Im1 = 0 , Re2 = 0 , Im2 = 0 , K = 1 / m_Ns;

        // transfer function:
        Re1 = 1 - cos( -PI2 * F * m_Ns * m_Ts );
        Im1 = 0 - sin( -PI2 * F * m_Ns * m_Ts );
        Re2 = 1 - ( cos(-PI2 * F * m_Ts) * m_Kc - sin(-PI2 * F * m_Ts) * m_Ks );
        Im2 = 0 - ( cos(-PI2 * F * m_Ts) * m_Ks + sin(-PI2 * F * m_Ts) * m_Kc );

        // amplitude and phase response:
        if( F == m_Fn * m_hnum )
        {
            m_Km = 1;
            m_pH = 0;
        }
        else
        {
            m_Km = sqrt( Re1 * Re1 + Im1 * Im1 ) / sqrt( Re2 * Re2 + Im2 * Im2 ) * K;
            m_pH = atan2( Im1 , Re1 ) - atan2( Im2 , Re2 );
        }

        return 0;
    }

    // operators:

    // x32:
    inline void operator () ( __type *input ){ filt ( input ); }
};

// float x64:
template<> class recursive_fourier<__fx64>
{
    typedef __fx64 __type;

    // system variables:
    __type m_Fn;
    __type m_Fs;
    __type m_Ts;
    __type m_Gain;
    __type m_Ns;
    __type m_hnum;
    __type m_a0 , m_Ks , m_Kc;
    __ix32 m_order;

    // buffer:
    mirror_ring_buffer<__type> m_buffer_sx;

public:
    // filter output:
    __type m_a , m_b , m_Km , m_pH;

    // memory allocation and deallocation functions:
    int  allocate() { return ( m_buffer_sx.allocate( m_order + 2 ) ); }
    void deallocate() { m_buffer_sx.deallocate(); }

    // initialization function:
    void init( __type Fn , __type Fs , __ix32 hnum )
    {
        // system variables initialization:
        m_Fn      = Fn;
        m_Fs      = Fs;
        m_Ts      = 1 / m_Fs;
        m_Ns      = ceil(m_Fs / m_Fn);
        m_order   = m_Ns;
        m_Gain    = ( hnum == 0 ) ? ( 1.0 / m_Ns ) : ( 2.0 / m_Ns );
        m_hnum    = hnum;
        m_Ks      = sin( PI2 * (__fx64)m_hnum / m_Ns );
        m_Kc      = cos( PI2 * (__fx64)m_hnum / m_Ns );

        // filter output initialization:
        m_a0 = 0;
        m_a  = 0;
        m_b  = 0;
        m_Km = 0;
        m_pH = 0;
    }

    // default constructor:
    recursive_fourier()
    {
        // system variables initialization:
        m_Fn       = 50;
        m_Fs       = 4000;
        m_Ts       = 1 / m_Fs;
        m_Ns       = ceil(m_Fs / m_Fn);
        m_order    = m_Ns;
        m_Gain     = 2.0 / m_Ns;
        m_hnum     = 1;
        m_Ks       = sin( PI2 * (__fx64)m_hnum / m_Ns );
        m_Kc       = cos( PI2 * (__fx64)m_hnum / m_Ns );

        // filter output initialization:
        m_a0 = 0;
        m_a  = 0;
        m_b  = 0;
        m_Km = 0;
        m_pH = 0;
    };

    // constructor with initiallization:
    recursive_fourier( __type Fn , __type Fs , __ix32 hnum ) { init( Fn , Fs , hnum ); }

    // default destructor:
    ~recursive_fourier(){ deallocate(); }

    // Frequency characteristics computation function:
    __ix32 FreqCharacteristics( __type F )
    {
        // complex frequency coeffs:
        __type Re1 = 0 , Im1 = 0 , Re2 = 0 , Im2 = 0 , K = 1 / m_Ns;

        // transfer function:
        Re1 = 1 - cos( -PI2 * F * m_Ns * m_Ts );
        Im1 = 0 - sin( -PI2 * F * m_Ns * m_Ts );
        Re2 = 1 - ( cos(-PI2 * F * m_Ts) * m_Kc - sin(-PI2 * F * m_Ts) * m_Ks );
        Im2 = 0 - ( cos(-PI2 * F * m_Ts) * m_Ks + sin(-PI2 * F * m_Ts) * m_Kc );

        // amplitude and phase response:
        if( F == m_Fn * m_hnum )
        {
            m_Km = 1;
            m_pH = 0;
        }
        else
        {
            m_Km = sqrt( Re1 * Re1 + Im1 * Im1 ) / sqrt( Re2 * Re2 + Im2 * Im2 ) * K;
            m_pH = atan2( Im1 , Re1 ) - atan2( Im2 , Re2 );
        }

        return 0;
    }

    // filtering function:
    inline void filt ( __type *input )
    {
        m_buffer_sx.fill_buff( input );
        m_a0 = m_a + ( *input - m_buffer_sx[ m_order ] ) * m_Gain;
        m_a  = m_a0 * m_Kc - m_b * m_Ks;
        m_b  = m_a0 * m_Ks + m_b * m_Kc;
    }

    // operators:

    // initialization operator:
    inline void operator () ( __type *input ){ filt ( input ); }
};

// customized data types exclusion to avoid aliases:
#undef __ix16
#undef __ix32
#undef __ix64
#undef __uix16
#undef __uix32
#undef __uix64
#undef __fx32
#undef __fx64

// customized math constants exclusion to avoid aliases:
#undef PI0
#undef PI2
#undef PI_2
#undef PI_4
#undef inv_PI0
#undef inv_PI2
#undef inv_PI_2
#undef inv_PI_4

#endif // RECURSIVE_FOURIER_H
