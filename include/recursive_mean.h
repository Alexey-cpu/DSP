#ifndef RECURSIVE_MEAN_H
#define RECURSIVE_MEAN_H

// identify if the compilation is for ProsoftSystems IDE
#ifndef __ALG_PLATFORM
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

template< typename T > class recursive_mean;    // recursive mean

// Recursive mean implementation:

// float x32
template<> class recursive_mean<__fx32>
{
    typedef __fx32 __type;
    // system variables:
    __type m_Fn;
    __type m_Fs;
    __type m_Ts;
    __type m_Gain;
    __type m_Ns;
    __ix32 m_order;

    // buffer:
    mirror_ring_buffer<__type> m_buffer_sx;

public:
    // filter output:
    __type m_out , m_Km , m_pH;

    // memory allocation and deallocation functions:
    __ix32 allocate() { return ( m_buffer_sx.allocate( m_order + 2 ) ); }
    void deallocate() { m_buffer_sx.deallocate(); }

    // initialization function:
    void filtInit( __type Fn , __type Fs , __ix32 order )
    {
        m_Fs      = Fs;
        m_Fn      = Fn;
        m_order   = order;
        m_Ns      = m_order;
        m_Ts      = 1 / m_Fs;
        m_Gain    = 1 / m_Ns;
    }

    // default constructor:
    recursive_mean()
    {
        m_Fs      = 4000;
        m_Fn      = 50;
        m_order   = m_Fs / m_Fn;
        m_Ns      = m_order;
        m_Ts      = 1 / m_Fs;
        m_Gain    = 1 / m_Ns;
    };

    // constructor with initiallization:
    recursive_mean( __type Fn , __type Fs , __ix32 order ) { filtInit( Fn , Fs , order ); }

    // default destructor:
    ~recursive_mean(){ deallocate(); }

    // Frequency characteristics computation function:
    __ix32 FreqCharacteristics( __type F )
    {
        __type Re_nom = 0 , Im_nom = 0 , Re_den = 0 , Im_den = 0 , Re = 0 , Im = 0;
        Re_nom = 1 - cos(-PI2 * F * m_Ns * m_Ts);
        Im_nom = 0 - sin(-PI2 * F * m_Ns * m_Ts);
        Re_den = 1 - cos(-PI2 * F * m_Ts);
        Im_den = 0 - sin(-PI2 * F * m_Ts);
        Re   = (Re_nom * Re_den + Im_nom * Im_den) / (Re_den * Re_den + Im_den * Im_den);
        Im   = (Re_den * Im_nom - Re_nom * Im_den) / (Re_den * Re_den + Im_den * Im_den);
        m_Km = m_Gain * sqrt(Re * Re + Im * Im);
        m_pH = atan2(Im , Re);
        return 0;
    }

    // filtering function:

    // float x32 input:
    inline void filt ( __type *input )
    {
        m_buffer_sx.fill_buff( input );
        m_out = m_Gain * (*input - m_buffer_sx[ m_order ] ) + m_out;
    }

    // float x64 input ( CAUTION !!! ROUNDING ERROR OCCURS !!! ):
    inline void filt ( __fx64 *input )
    {
        m_buffer_sx.fill_buff( input );
        m_out = m_Gain * (*input - m_buffer_sx[ m_order ] ) + m_out;
    }

    // operators:

    // x32:
    inline void operator () ( __type *input ){ filt ( input ); }
    // x64:
    inline void operator () ( __fx64 *input ){ filt ( input ); }
};

// float x64:
template<> class recursive_mean<__fx64>
{
    typedef __fx64 __type;
    // system variables:
    __type m_Fn;
    __type m_Fs;
    __type m_Ts;
    __type m_Gain;
    __type m_Ns;
    __ix32 m_order;

    // buffer:
    mirror_ring_buffer<__type> m_buffer_sx;

public:
    // filter output:
    __type m_out , m_Km , m_pH;

    // memory allocation and deallocation functions:
    __ix32 allocate() { return ( m_buffer_sx.allocate( m_order + 2 ) ); }
    void deallocate() { m_buffer_sx.deallocate(); }

    // initialization function:
    void filtInit( __type Fn , __type Fs , __ix32 order )
    {
        m_Fs      = Fs;
        m_Fn      = Fn;
        m_order   = order;
        m_Ns      = m_order;
        m_Ts      = 1 / m_Fs;
        m_Gain    = 1 / m_Ns;
    }

    // default constructor:
    recursive_mean()
    {
        m_Fs      = 4000;
        m_Fn      = 50;
        m_order   = m_Fs / m_Fn;
        m_Ns      = m_order;
        m_Ts      = 1 / m_Fs;
        m_Gain    = 1 / m_Ns;
    };

    // constructor with initiallization:
    recursive_mean( __type Fn , __type Fs , __ix32 order ) { filtInit( Fn , Fs , order ); }

    // default destructor:
    ~recursive_mean(){ deallocate(); }

    // Frequency characteristics computation function:
    __ix32 FreqCharacteristics( __type F )
    {
        __type Re_nom = 0 , Im_nom = 0 , Re_den = 0 , Im_den = 0 , Re = 0 , Im = 0;
        Re_nom = 1 - cos(-PI2 * F * m_Ns * m_Ts);
        Im_nom = 0 - sin(-PI2 * F * m_Ns * m_Ts);
        Re_den = 1 - cos(-PI2 * F * m_Ts);
        Im_den = 0 - sin(-PI2 * F * m_Ts);
        Re   = (Re_nom * Re_den + Im_nom * Im_den) / (Re_den * Re_den + Im_den * Im_den);
        Im   = (Re_den * Im_nom - Re_nom * Im_den) / (Re_den * Re_den + Im_den * Im_den);
        m_Km = m_Gain * sqrt(Re * Re + Im * Im);
        m_pH = atan2(Im , Re);
        return 0;
    }

    // filtering function:
    inline void filt ( __type *input )
    {
        m_buffer_sx.fill_buff( input );
        m_out = m_Gain * (*input - m_buffer_sx[ m_order ] ) + m_out;
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

#endif // RECURSIVE_MEAN_H
