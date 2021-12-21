//--------------------------------------------------------------------------------
//A.M.Tykvinsky, 21.12.2021
//--------------------------------------------------------------------------------
// TEMPLATE FINITE IMPULSE RESPONSE FILTER CLASS
//--------------------------------------------------------------------------------

#ifndef FIR_H
#define FIR_H

#ifndef __ALG_PLATFORM // identify if the compilation is for ProsoftSystems IDE
#include <iostream>
#include <fstream>
#endif

#include "buffer.h"
#include "special_functions.h"

#ifndef __fx32
#define __fx32 float
#endif

#ifndef __fx64
#define __fx64 double
#endif

#ifndef __ix32
#define __ix32 int
#endif

// fir templates:
template< typename T > class fir_lp; // fir lowpass
template< typename T > class fir_hp; // fir highpass
template< typename T > class fir_bp; // fir bandpass
template< typename T > class fir_bs; // fir bandstop

// lowpass fir implemetations:

// float x32:
template<> class fir_lp<__fx32>
{
    typedef __fx32 __type ;
    typedef bool   __bool ;
    typedef void   __void ;

    // system variables:
    __type m_Ts;
    __type m_Fs;
    __type m_Fn;
    __type m_Fstop;
    __type m_Ns;
    __ix32 m_order;
    __bool m_scale;

    // coefficients:
    __type *m_buff_cx;

public:

    __type m_pH;
    __type m_Km;
    __type m_out;

    // buffers:
    mirror_ring_buffer< __type > m_buff_sx;

    // window function object:
    wind_fcn m_wind_fcn;

    // coefficients computation function:
    __void coeff_calc()
    {
        // check if the window is ready to use:
        if ( m_wind_fcn.is_ready() == 0 ) m_wind_fcn.Chebyshev(100);

        // coefficients computation:
        __ix32 k = 0;
        m_Fstop /= m_Fs;

        if (m_order % 2 == 0) // even order filter coefficients computation
        {
            for ( __ix32 n = 0; n <= m_order / 2; n++)
            {
                k = abs(n - (m_order + 1) / 2);

                if (n == 0)
                {
                    m_buff_cx[k] = 2 * m_Fstop * m_wind_fcn[k];
                }
                else
                {
                    m_buff_cx[k] = 2 * m_Fstop * sin(n * PI2 * m_Fstop) / (n * PI2 * m_Fstop) * m_wind_fcn[k];
                    m_buff_cx[m_order - k] = m_buff_cx[k];
                }
            }
        }
        else if (m_order % 2 != 0) // odd order filter coefficients computation
        {
            __type rem = ceil((__type)m_order / 2) - (__type)m_order / 2;
            for ( __ix32 n = 0; n <= ceil(m_order / 2); n++)
            {
                k = abs(n - m_order / 2);
                m_buff_cx[k] = 2 * m_Fstop * sin((n + rem) * PI2 * m_Fstop) / ((n + rem) * PI2 * m_Fstop) * m_wind_fcn[k];
                m_buff_cx[m_order - k] = m_buff_cx[k];
            }
        }

        // filter pulse characteristic normalization:
        if ( m_scale )
        {
            freq_ch(0);
            for ( __ix32 n = 0; n <= m_order; n++) m_buff_cx[n] = m_buff_cx[n] / m_Km;
            freq_ch(m_Fn);
        }

        // deallocate the window function:
        m_wind_fcn.deallocate();
    }

    // Frequency characteristics:
    __void freq_ch( __type F )
    {
        __type m_W_re = 0;
        __type m_W_im = 0;
        for ( __ix32 n = 0; n <= m_order; n++)
        {
            m_W_re = m_W_re + cos(-PI2 * n * F * m_Ts) * m_buff_cx[n];
            m_W_im = m_W_im + sin(-PI2 * n * F * m_Ts) * m_buff_cx[n];
        }
        m_pH   = atan2( m_W_im , m_W_im );
        m_Km   = sqrt (m_W_re * m_W_re + m_W_im * m_W_im);
    }

    #ifndef __ALG_PLATFORM

    // filter response store to a text files:
    __void freq_rp( std::string directory )
    {
        // open files:
        std::ofstream ampr;
        std::ofstream phsr;
        std::ofstream freq;
        std::ofstream plsr;
        ampr.open( directory + "\\ampr.txt" );
        phsr.open( directory + "\\phsr.txt" );
        freq.open( directory + "\\freq.txt" );
        plsr.open( directory + "\\plsr.txt" );

        // frequency response
        for( int i = 0 ; i < m_Fs * 0.5 ; i++ )
        {
            freq_ch( i );
            ampr << m_Km << "\n";
            phsr << m_pH << "\n";
            freq << i    << "\n";
        }

        // pulse response:
        for( int i = 0 ; i <= m_order ; i++ ) { plsr << m_buff_cx[i] << "\n"; }

        // close files:
        ampr.close();
        phsr.close();
        freq.close();
        plsr.close();
    }

    #endif

    // memory allocation:
    __bool allocate()
    {
        // memory allocation:
        m_buff_cx = ( __type* ) calloc( m_order + 1 , sizeof ( __type ) );
        m_buff_sx.allocate( m_order + 2 );
        // compute FIR filter coefficients:
        coeff_calc();
        return 1;
    }

    // memory deallocation:
    __void deallocate()
    {
        if( m_buff_cx != 0 ) free( m_buff_cx );
        m_buff_sx.deallocate();
        m_wind_fcn.deallocate();
    }

    // initialization function:
    __void init( __type Fs, __type Fn, __type F_stop, __ix32 order , __bool scale )
    {
        m_Fs      = Fs;
        m_Fn      = Fn;
        m_Fstop   = F_stop;
        m_Ts      = 1 / m_Fs;
        m_order   = order;
        m_Ns      = m_order;
        m_scale   = scale;
        m_buff_cx = 0;
        m_out     = 0;

        // window function initialization:
        m_wind_fcn.init( m_order + 1 );
    }

    // default constructor:
    fir_lp()
    {
        m_Fs      = 4000;
        m_Fn      = 50;
        m_Fstop   = 100;
        m_Ts      = 1 / m_Fs;
        m_order   = 80;
        m_Ns      = m_order;
        m_scale   = false;
        m_buff_cx = 0;
        m_out     = 0;

        // window function initialization:
        m_wind_fcn.init( m_order + 1 );
    }

    // initializing constructor:
    fir_lp( __type Fs, __type Fn, __type F_stop, __ix32 order , __bool scale ) { init( Fs , Fn , F_stop , order , scale ); }

    // destructor:
    ~fir_lp() { deallocate(); }

    // filtering function:

    // x32:
    inline __type filt( __type *input )
    {
        m_buff_sx( input );
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    // x64:
    inline __type filt( __fx64 *input )
    {
        m_buff_sx( input );
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    inline __type filt()
    {
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    // operators:
    inline __type operator() ( __type *input ) { return filt( input ); }
    inline __type operator() ( __fx64 *input ) { return filt( input ); }
    inline __type operator() () { return filt(); }
};

// float x64:
template<> class fir_lp<__fx64>
{
    typedef __fx64 __type ;
    typedef bool   __bool ;
    typedef void   __void ;

    // system variables:
    __type m_Ts;
    __type m_Fs;
    __type m_Fn;
    __type m_Fstop;
    __type m_Ns;
    __ix32 m_order;
    __bool m_scale;

    // coefficients:
    __type *m_buff_cx;

public:

    __type m_pH;
    __type m_Km;
    __type m_out;

    // buffers:
    mirror_ring_buffer< __type > m_buff_sx;

    // window function object:
    wind_fcn m_wind_fcn;

    // coefficients computation function:
    __void coeff_calc()
    {
        // check if the window is ready to use:
        if ( m_wind_fcn.is_ready() == 0 ) m_wind_fcn.Chebyshev(100);

        // coefficients computation:
        __ix32 k = 0;
        m_Fstop /= m_Fs;

        if (m_order % 2 == 0) // even order filter coefficients computation
        {
            for ( __ix32 n = 0; n <= m_order / 2; n++)
            {
                k = abs(n - (m_order + 1) / 2);

                if (n == 0)
                {
                    m_buff_cx[k] = 2 * m_Fstop * m_wind_fcn[k];
                }
                else
                {
                    m_buff_cx[k] = 2 * m_Fstop * sin(n * PI2 * m_Fstop) / (n * PI2 * m_Fstop) * m_wind_fcn[k];
                    m_buff_cx[m_order - k] = m_buff_cx[k];
                }
            }
        }
        else if (m_order % 2 != 0) // odd order filter coefficients computation
        {
            __type rem = ceil((__type)m_order / 2) - (__type)m_order / 2;
            for ( __ix32 n = 0; n <= ceil(m_order / 2); n++)
            {
                k = abs(n - m_order / 2);
                m_buff_cx[k] = 2 * m_Fstop * sin((n + rem) * PI2 * m_Fstop) / ((n + rem) * PI2 * m_Fstop) * m_wind_fcn[k];
                m_buff_cx[m_order - k] = m_buff_cx[k];
            }
        }

        // filter pulse characteristic normalization:
        if ( m_scale )
        {
            freq_ch(0);
            for ( __ix32 n = 0; n <= m_order; n++) m_buff_cx[n] = m_buff_cx[n] / m_Km;
            freq_ch(m_Fn);
        }

        // deallocate the window function:
        m_wind_fcn.deallocate();
    }

    // Frequency characteristics:
    __void freq_ch( __type F )
    {
        __type m_W_re = 0;
        __type m_W_im = 0;
        for ( __ix32 n = 0; n <= m_order; n++)
        {
            m_W_re = m_W_re + cos(-PI2 * n * F * m_Ts) * m_buff_cx[n];
            m_W_im = m_W_im + sin(-PI2 * n * F * m_Ts) * m_buff_cx[n];
        }
        m_pH   = atan2( m_W_im , m_W_im );
        m_Km   = sqrt (m_W_re * m_W_re + m_W_im * m_W_im);
    }

    #ifndef __ALG_PLATFORM

    // filter response store to a text files:
    __void freq_rp( std::string directory )
    {
        // open files:
        std::ofstream ampr;
        std::ofstream phsr;
        std::ofstream freq;
        std::ofstream plsr;
        ampr.open( directory + "\\ampr.txt" );
        phsr.open( directory + "\\phsr.txt" );
        freq.open( directory + "\\freq.txt" );
        plsr.open( directory + "\\plsr.txt" );

        // frequency response
        for( int i = 0 ; i < m_Fs * 0.5 ; i++ )
        {
            freq_ch( i );
            ampr << m_Km << "\n";
            phsr << m_pH << "\n";
            freq << i    << "\n";
        }

        // pulse response:
        for( int i = 0 ; i <= m_order ; i++ ) { plsr << m_buff_cx[i] << "\n"; }

        // close files:
        ampr.close();
        phsr.close();
        freq.close();
        plsr.close();
    }

    #endif

    // memory allocation:
    __bool allocate()
    {
        // memory allocation:
        m_buff_cx = ( __type* ) calloc( m_order + 1 , sizeof ( __type ) );
        m_buff_sx.allocate( m_order + 2 );
        // compute FIR filter coefficients:
        coeff_calc();
        return 1;
    }

    // memory deallocation:
    __void deallocate()
    {
        if( m_buff_cx != 0 ) free( m_buff_cx );
        m_buff_sx.deallocate();
        m_wind_fcn.deallocate();
    }

    // initialization function:
    __void init( __type Fs, __type Fn, __type F_stop, __ix32 order , __bool scale )
    {
        m_Fs      = Fs;
        m_Fn      = Fn;
        m_Fstop   = F_stop;
        m_Ts      = 1 / m_Fs;
        m_order   = order;
        m_Ns      = m_order;
        m_scale   = scale;
        m_buff_cx = 0;
        m_out     = 0;

        // window function initialization:
        m_wind_fcn.init( m_order + 1 );
    }

    // default constructor:
    fir_lp()
    {
        m_Fs      = 4000;
        m_Fn      = 50;
        m_Fstop   = 100;
        m_Ts      = 1 / m_Fs;
        m_order   = 80;
        m_Ns      = m_order;
        m_scale   = false;
        m_buff_cx = 0;
        m_out     = 0;

        // window function initialization:
        m_wind_fcn.init( m_order + 1 );
    }

    // initializing constructor:
    fir_lp( __type Fs, __type Fn, __type F_stop, __ix32 order , __bool scale ) { init( Fs , Fn , F_stop , order , scale ); }

    // destructor:
    ~fir_lp() { deallocate(); }

    // filtering function:

    // x32:
    inline __type filt( __type *input )
    {
        m_buff_sx( input );
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    inline __type filt()
    {
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    // operators:
    inline __type operator() ( __type *input ) { return filt( input ); }
    inline __type operator() () { return filt(); }
};

// highpass fir implemetations:

// float x32:
template<> class fir_hp<__fx32>
{
    typedef __fx32 __type ;
    typedef bool   __bool ;
    typedef void   __void ;

    // system variables:
    __type m_Ts;
    __type m_Fs;
    __type m_Fn;
    __type m_Fstop;
    __type m_Ns;
    __ix32 m_order;
    __bool m_scale;

    // coefficients:
    __type *m_buff_cx;

public:

    __type m_pH;
    __type m_Km;
    __type m_out;

    // buffers:
    mirror_ring_buffer< __type > m_buff_sx;

    // window function object:
    wind_fcn m_wind_fcn;

    // coefficients computation function:
    __void coeff_calc()
    {
        // check if the window is ready to use:
        if ( m_wind_fcn.is_ready() == 0 ) m_wind_fcn.Chebyshev(100);

        // coefficients computation:
        __ix32 k = 0;
        m_Fstop /= m_Fs;

        if (m_order % 2 == 0) // even order highpass
        {
            for ( __ix32 n = 0; n <= m_order / 2; n++)
            {
                k = abs(n - (m_order + 1) / 2);

                if (n == 0)
                {
                    m_buff_cx[k] = (1 - 2 * m_Fstop) * m_wind_fcn[k];
                }
                else
                {
                    m_buff_cx[k] = -2 * m_Fstop * sin(n * PI2 * m_Fstop) / (n * PI2 * m_Fstop) * m_wind_fcn[k];
                    m_buff_cx[m_order - k] = m_buff_cx[k];
                }
            }
        }
        else if (m_order % 2 != 0) // odd order highpass
        {
            __type rem = ceil((__type)m_order / 2) - (__type)m_order / 2;
            for ( __ix32 n = 0; n <= ceil(m_order / 2); n++)
            {
                k = abs(n - m_order / 2);
                m_buff_cx[k]           = -pow(-1 , n) * 2 * (0.5-m_Fstop) * sin((n + rem) * PI2 * (0.5-m_Fstop) ) / ((n + rem) * PI2 * (0.5 - m_Fstop) )* m_wind_fcn[k];
                m_buff_cx[m_order - k] = -m_buff_cx[k];
            }
        }

        // filter pulse characteristic normalization:
        if ( m_scale )
        {
            freq_ch( m_Fs / 2 );
            for ( __ix32 n = 0; n <= m_order; n++) m_buff_cx[n] = m_buff_cx[n] / m_Km;
            freq_ch(m_Fn);
        }

        // deallocate the window function:
        m_wind_fcn.deallocate();
    }

    // Frequency characteristics:
    __void freq_ch( __type F )
    {
        __type m_W_re = 0;
        __type m_W_im = 0;
        for ( __ix32 n = 0; n <= m_order ; n++)
        {
            m_W_re = m_W_re + cos(-PI2 * n * F * m_Ts) * m_buff_cx[n];
            m_W_im = m_W_im + sin(-PI2 * n * F * m_Ts) * m_buff_cx[n];
        }
        m_pH   = atan2( m_W_im , m_W_im );
        m_Km   = sqrt (m_W_re * m_W_re + m_W_im * m_W_im);
    }

    #ifndef __ALG_PLATFORM

    // filter response store to a text files:
    __void freq_rp( std::string directory )
    {
        // open files:
        std::ofstream ampr;
        std::ofstream phsr;
        std::ofstream freq;
        std::ofstream plsr;
        ampr.open( directory + "\\ampr.txt" );
        phsr.open( directory + "\\phsr.txt" );
        freq.open( directory + "\\freq.txt" );
        plsr.open( directory + "\\plsr.txt" );

        // frequency response
        for( int i = 0 ; i < m_Fs * 0.5 ; i++ )
        {
            freq_ch( i );
            ampr << m_Km << "\n";
            phsr << m_pH << "\n";
            freq << i    << "\n";
        }

        // pulse response:
        for( int i = 0 ; i <= m_order ; i++ ) { plsr << m_buff_cx[i] << "\n"; }

        // close files:
        ampr.close();
        phsr.close();
        freq.close();
        plsr.close();
    }

    #endif

    // memory allocation:
    __bool allocate()
    {
        // memory allocation:
        m_buff_cx = ( __type* ) calloc( m_order + 1 , sizeof ( __type ) );
        m_buff_sx.allocate( m_order + 2 );
        // compute FIR filter coefficients:
        coeff_calc();
        return 1;
    }

    // memory deallocation:
    __void deallocate()
    {
        if( m_buff_cx != 0 ) free( m_buff_cx );
        m_buff_sx.deallocate();
        m_wind_fcn.deallocate();
    }

    // initialization function:
    __void init( __type Fs, __type Fn, __type F_stop, __ix32 order , __bool scale )
    {
        m_Fs      = Fs;
        m_Fn      = Fn;
        m_Fstop   = F_stop;
        m_Ts      = 1 / m_Fs;
        m_order   = order;
        m_Ns      = m_order;
        m_scale   = scale;
        m_buff_cx = 0;
        m_out     = 0;

        // window function initialization:
        m_wind_fcn.init( m_order + 1 );
    }

    // default constructor:
    fir_hp()
    {
        m_Fs      = 4000;
        m_Fn      = 50;
        m_Fstop   = 100;
        m_Ts      = 1 / m_Fs;
        m_order   = 80;
        m_Ns      = m_order;
        m_scale   = false;
        m_buff_cx = 0;
        m_out     = 0;

        // window function initialization:
        m_wind_fcn.init( m_order + 1 );
    }

    // initializing constructor:
    fir_hp( __type Fs, __type Fn, __type F_stop, __ix32 order , __bool scale ) { init( Fs , Fn , F_stop , order , scale ); }

    // destructor:
    ~fir_hp() { deallocate(); }

    // filtering function:

    // x32:
    inline __type filt( __type *input )
    {
        m_buff_sx( input );
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    // x64 input ( CAUTION !!! ROUNDING ERROR OCCURS !!! )
    inline __type filt( __fx64 *input )
    {
        m_buff_sx( input );
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    inline __type filt()
    {
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    // operators:
    inline __type operator() ( __type *input ) { return filt( input ); }
    inline __type operator() ( __fx64 *input ) { return filt( input ); }
    inline __type operator() () { return filt(); }
};

// float x64:
template<> class fir_hp<__fx64>
{
    typedef __fx64 __type ;
    typedef bool   __bool ;
    typedef void   __void ;

    // system variables:
    __type m_Ts;
    __type m_Fs;
    __type m_Fn;
    __type m_Fstop;
    __type m_Ns;
    __type m_pH;
    __type m_Km;
    __type m_out;
    __ix32 m_order;
    __bool m_scale;

    // coefficients:
    __type *m_buff_cx;

public:

    // buffers:
    mirror_ring_buffer< __type > m_buff_sx;

    // window function object:
    wind_fcn m_wind_fcn;

    // coefficients computation function:
    __void coeff_calc()
    {
        // check if the window is ready to use:
        if ( m_wind_fcn.is_ready() == 0 ) m_wind_fcn.Chebyshev(100);

        // coefficients computation:
        __ix32 k = 0;
        m_Fstop /= m_Fs;

        if (m_order % 2 == 0) // even order highpass
        {
            for ( __ix32 n = 0; n <= m_order / 2; n++)
            {
                k = abs(n - (m_order + 1) / 2);

                if (n == 0)
                {
                    m_buff_cx[k] = (1 - 2 * m_Fstop) * m_wind_fcn[k];
                }
                else
                {
                    m_buff_cx[k] = -2 * m_Fstop * sin(n * PI2 * m_Fstop) / (n * PI2 * m_Fstop) * m_wind_fcn[k];
                    m_buff_cx[m_order - k] = m_buff_cx[k];
                }
            }
        }
        else if (m_order % 2 != 0) // odd order highpass
        {
            __type rem = ceil((__type)m_order / 2) - (__type)m_order / 2;
            for ( __ix32 n = 0; n <= ceil(m_order / 2); n++)
            {
                k = abs(n - m_order / 2);
                m_buff_cx[k]           = -pow(-1 , n) * 2 * (0.5-m_Fstop) * sin((n + rem) * PI2 * (0.5-m_Fstop) ) / ((n + rem) * PI2 * (0.5 - m_Fstop) )* m_wind_fcn[k];
                m_buff_cx[m_order - k] = -m_buff_cx[k];
            }
        }

        // filter pulse characteristic normalization:
        if ( m_scale )
        {
            freq_ch( m_Fs / 2 );
            for ( __ix32 n = 0; n <= m_order; n++) m_buff_cx[n] = m_buff_cx[n] / m_Km;
            freq_ch(m_Fn);
        }

        // deallocate the window function:
        m_wind_fcn.deallocate();
    }

    // Frequency characteristics:
    __void freq_ch( __type F )
    {
        __type m_W_re = 0;
        __type m_W_im = 0;
        for ( __ix32 n = 0; n <= m_order ; n++)
        {
            m_W_re = m_W_re + cos(-PI2 * n * F * m_Ts) * m_buff_cx[n];
            m_W_im = m_W_im + sin(-PI2 * n * F * m_Ts) * m_buff_cx[n];
        }
        m_pH   = atan2( m_W_im , m_W_im );
        m_Km   = sqrt (m_W_re * m_W_re + m_W_im * m_W_im);
    }

    #ifndef __ALG_PLATFORM

    // filter response store to a text files:
    __void freq_rp( std::string directory )
    {
        // open files:
        std::ofstream ampr;
        std::ofstream phsr;
        std::ofstream freq;
        std::ofstream plsr;
        ampr.open( directory + "\\ampr.txt" );
        phsr.open( directory + "\\phsr.txt" );
        freq.open( directory + "\\freq.txt" );
        plsr.open( directory + "\\plsr.txt" );

        // frequency response
        for( int i = 0 ; i < m_Fs * 0.5 ; i++ )
        {
            freq_ch( i );
            ampr << m_Km << "\n";
            phsr << m_pH << "\n";
            freq << i    << "\n";
        }

        // pulse response:
        for( int i = 0 ; i <= m_order ; i++ ) { plsr << m_buff_cx[i] << "\n"; }

        // close files:
        ampr.close();
        phsr.close();
        freq.close();
        plsr.close();
    }

    #endif

    // memory allocation:
    __bool allocate()
    {
        // memory allocation:
        m_buff_cx = ( __type* ) calloc( m_order + 1 , sizeof ( __type ) );
        m_buff_sx.allocate( m_order + 2 );
        // compute FIR filter coefficients:
        coeff_calc();
        return 1;
    }

    // memory deallocation:
    __void deallocate()
    {
        if( m_buff_cx != 0 ) free( m_buff_cx );
        m_buff_sx.deallocate();
        m_wind_fcn.deallocate();
    }

    // initialization function:
    __void init( __type Fs, __type Fn, __type F_stop, __ix32 order , __bool scale )
    {
        m_Fs      = Fs;
        m_Fn      = Fn;
        m_Fstop   = F_stop;
        m_Ts      = 1 / m_Fs;
        m_order   = order;
        m_Ns      = m_order;
        m_scale   = scale;
        m_buff_cx = 0;
        m_out     = 0;

        // window function initialization:
        m_wind_fcn.init( m_order + 1 );
    }

    // default constructor:
    fir_hp()
    {
        m_Fs      = 4000;
        m_Fn      = 50;
        m_Fstop   = 100;
        m_Ts      = 1 / m_Fs;
        m_order   = 80;
        m_Ns      = m_order;
        m_scale   = false;
        m_buff_cx = 0;
        m_out     = 0;

        // window function initialization:
        m_wind_fcn.init( m_order + 1 );
    }

    // initializing constructor:
    fir_hp( __type Fs, __type Fn, __type F_stop, __ix32 order , __bool scale ) { init( Fs , Fn , F_stop , order , scale ); }

    // destructor:
    ~fir_hp() { deallocate(); }

    // filtering function:

    // x32:
    inline __type filt( __type *input )
    {
        m_buff_sx( input );
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    inline __type filt()
    {
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    // operators:
    inline __type operator() ( __type *input ) { return filt( input ); }
    inline __type operator() () { return filt(); }
};

// bandpass implementations:

// float x32:
template<> class fir_bp<__fx32>
{
    typedef __fx32 __type ;
    typedef bool   __bool ;
    typedef void   __void ;

    // system variables:
    __type m_Ts;
    __type m_Fs;
    __type m_Fn;
    __type m_Fstop1;
    __type m_Fstop2;
    __type m_Ns;
    __ix32 m_order;
    __bool m_scale;

    // coefficients:
    __type *m_buff_cx;

public:

    __type m_pH;
    __type m_Km;
    __type m_out;

    // buffers:
    mirror_ring_buffer< __type > m_buff_sx;

    // window function object:
    wind_fcn m_wind_fcn;

    // coefficients computation function:
    __void coeff_calc()
    {
        // check if the window is ready to use:
        if ( m_wind_fcn.is_ready() == 0 ) m_wind_fcn.Chebyshev(100);

        // coefficients computation:
        __ix32 k = 0;
        m_Fstop1 /= m_Fs;
        m_Fstop2 /= m_Fs;

        if (m_order % 2 == 0) // считаем ПФ четного порядка
        {
            for (int n = 0; n <= m_order / 2; n++)
            {
                k = abs(n - (m_order + 1) / 2);

                if (n == 0)
                {
                    m_buff_cx[k] = 2 * (m_Fstop2 - m_Fstop1) * m_wind_fcn[k];
                }
                else
                {
                    m_buff_cx[k] = 2 * (m_Fstop2 * sin(n * PI2 * m_Fstop2) / (n * PI2 * m_Fstop2) - m_Fstop1 * sin(n * PI2 * m_Fstop1) / (n * PI2 * m_Fstop1)) * m_wind_fcn[k];
                    m_buff_cx[m_order - k] = m_buff_cx[k];
                }
            }
        }
        else if (m_order % 2 != 0) // считаем ПФ нечетного порядка
        {
            double rem = ceil((double)m_order / 2) - (double)m_order / 2;
            for (int n = 0; n <= ceil(m_order / 2); n++)
            {
                k = abs(n - m_order / 2);
                m_buff_cx[k] = 2 * (m_Fstop2 * sin((n + rem) * PI2 * m_Fstop2) / ((n + rem) * PI2 * m_Fstop2) - m_Fstop1 * sin((n + rem) * PI2 * m_Fstop1) / ((n + rem) * PI2 * m_Fstop1)) * m_wind_fcn[k];
                m_buff_cx[m_order - k] = m_buff_cx[k];
            }
        }

        // filter pulse characteristic normalization:
        if ( m_scale )
        {
            freq_ch( ( m_Fstop1 + 0.5 * ( m_Fstop2 - m_Fstop1 ) ) * m_Fs );
            for ( __ix32 n = 0; n <= m_order; n++) m_buff_cx[n] = m_buff_cx[n] / m_Km;
            freq_ch(m_Fn);
        }

        // deallocate the window function:
        m_wind_fcn.deallocate();
    }

    // Frequency characteristics:
    __void freq_ch( __type F )
    {
        __type m_W_re = 0;
        __type m_W_im = 0;
        for ( __ix32 n = 0; n <= m_order ; n++)
        {
            m_W_re = m_W_re + cos(-PI2 * n * F * m_Ts) * m_buff_cx[n];
            m_W_im = m_W_im + sin(-PI2 * n * F * m_Ts) * m_buff_cx[n];
        }
        m_pH   = atan2( m_W_im , m_W_im );
        m_Km   = sqrt (m_W_re * m_W_re + m_W_im * m_W_im);
    }

    #ifndef __ALG_PLATFORM

    // filter response store to a text files:
    __void freq_rp( std::string directory )
    {
        // open files:
        std::ofstream ampr;
        std::ofstream phsr;
        std::ofstream freq;
        std::ofstream plsr;
        ampr.open( directory + "\\ampr.txt" );
        phsr.open( directory + "\\phsr.txt" );
        freq.open( directory + "\\freq.txt" );
        plsr.open( directory + "\\plsr.txt" );

        // frequency response
        for( int i = 0 ; i < m_Fs * 0.5 ; i++ )
        {
            freq_ch( i );
            ampr << m_Km << "\n";
            phsr << m_pH << "\n";
            freq << i    << "\n";
        }

        // pulse response:
        for( int i = 0 ; i <= m_order ; i++ ) { plsr << m_buff_cx[i] << "\n"; }

        // close files:
        ampr.close();
        phsr.close();
        freq.close();
        plsr.close();
    }

    #endif

    // memory allocation:
    __bool allocate()
    {
        // memory allocation:
        m_buff_cx = ( __type* ) calloc( m_order + 1 , sizeof ( __type ) );
        m_buff_sx.allocate( m_order + 2 );
        // compute FIR filter coefficients:
        coeff_calc();
        return 1;
    }

    // memory deallocation:
    __void deallocate()
    {
        if( m_buff_cx != 0 ) free( m_buff_cx );
        m_buff_sx.deallocate();
        m_wind_fcn.deallocate();
    }

    // initialization function:
    __void init( __type Fs, __type Fn, __type F_stop , __type BandWidth , __ix32 order , __bool scale )
    {
        m_Fs      = Fs;
        m_Fn      = Fn;
        m_Fstop1  = F_stop;
        m_Fstop2  = F_stop + BandWidth;
        m_Ts      = 1 / m_Fs;
        m_order   = order;
        m_Ns      = m_order;
        m_scale   = scale;
        m_buff_cx = 0;
        m_out     = 0;

        // window function initialization:
        m_wind_fcn.init( m_order + 1 );
    }

    // default constructor:
    fir_bp()
    {
        m_Fs      = 4000;
        m_Fn      = 50;
        m_Fstop1  = 100;
        m_Fstop1  = 200;
        m_Ts      = 1 / m_Fs;
        m_order   = 80;
        m_Ns      = m_order;
        m_scale   = false;
        m_buff_cx = 0;
        m_out     = 0;

        // window function initialization:
        m_wind_fcn.init( m_order + 1 );
    }

    // initializing constructor:
    fir_bp( __type Fs, __type Fn, __type F_stop , __type BandWidth , __ix32 order , __bool scale ) { init( Fs , Fn , F_stop , BandWidth , order , scale ); }

    // destructor:
    ~fir_bp() { deallocate(); }

    // filtering function:

    // x32:
    inline __type filt( __type *input )
    {
        m_buff_sx( input );
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    // x64:
    inline __type filt( __fx64 *input )
    {
        m_buff_sx( input );
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    inline __type filt()
    {
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    // operators:
    inline __type operator() ( __type *input ) { return filt( input ); }
    inline __type operator() ( __fx64 *input ) { return filt( input ); }
    inline __type operator() () { return filt(); }
};

// float x64:
template<> class fir_bp<__fx64>
{
    typedef __fx64 __type ;
    typedef bool   __bool ;
    typedef void   __void ;

    // system variables:
    __type m_Ts;
    __type m_Fs;
    __type m_Fn;
    __type m_Fstop1;
    __type m_Fstop2;
    __type m_Ns;
    __ix32 m_order;
    __bool m_scale;

    // coefficients:
    __type *m_buff_cx;

public:

    __type m_pH;
    __type m_Km;
    __type m_out;

    // buffers:
    mirror_ring_buffer< __type > m_buff_sx;

    // window function object:
    wind_fcn m_wind_fcn;

    // coefficients computation function:
    __void coeff_calc()
    {
        // check if the window is ready to use:
        if ( m_wind_fcn.is_ready() == 0 ) m_wind_fcn.Chebyshev(100);

        // coefficients computation:
        __ix32 k = 0;
        m_Fstop1 /= m_Fs;
        m_Fstop2 /= m_Fs;

        if (m_order % 2 == 0) // считаем ПФ четного порядка
        {
            for (int n = 0; n <= m_order / 2; n++)
            {
                k = abs(n - (m_order + 1) / 2);

                if (n == 0)
                {
                    m_buff_cx[k] = 2 * (m_Fstop2 - m_Fstop1) * m_wind_fcn[k];
                }
                else
                {
                    m_buff_cx[k] = 2 * (m_Fstop2 * sin(n * PI2 * m_Fstop2) / (n * PI2 * m_Fstop2) - m_Fstop1 * sin(n * PI2 * m_Fstop1) / (n * PI2 * m_Fstop1)) * m_wind_fcn[k];
                    m_buff_cx[m_order - k] = m_buff_cx[k];
                }
            }
        }
        else if (m_order % 2 != 0) // считаем ПФ нечетного порядка
        {
            double rem = ceil((double)m_order / 2) - (double)m_order / 2;
            for (int n = 0; n <= ceil(m_order / 2); n++)
            {
                k = abs(n - m_order / 2);
                m_buff_cx[k] = 2 * (m_Fstop2 * sin((n + rem) * PI2 * m_Fstop2) / ((n + rem) * PI2 * m_Fstop2) - m_Fstop1 * sin((n + rem) * PI2 * m_Fstop1) / ((n + rem) * PI2 * m_Fstop1)) * m_wind_fcn[k];
                m_buff_cx[m_order - k] = m_buff_cx[k];
            }
        }

        // filter pulse characteristic normalization:
        if ( m_scale )
        {
            freq_ch( ( m_Fstop1 + 0.5 * ( m_Fstop2 - m_Fstop1 ) ) * m_Fs );
            for ( __ix32 n = 0; n <= m_order; n++) m_buff_cx[n] = m_buff_cx[n] / m_Km;
            freq_ch(m_Fn);
        }

        // deallocate the window function:
        m_wind_fcn.deallocate();
    }

    // Frequency characteristics:
    __void freq_ch( __type F )
    {
        __type m_W_re = 0;
        __type m_W_im = 0;
        for ( __ix32 n = 0; n <= m_order ; n++)
        {
            m_W_re = m_W_re + cos(-PI2 * n * F * m_Ts) * m_buff_cx[n];
            m_W_im = m_W_im + sin(-PI2 * n * F * m_Ts) * m_buff_cx[n];
        }
        m_pH   = atan2( m_W_im , m_W_im );
        m_Km   = sqrt (m_W_re * m_W_re + m_W_im * m_W_im);
    }

    #ifndef __ALG_PLATFORM

    // filter response store to a text files:
    __void freq_rp( std::string directory )
    {
        // open files:
        std::ofstream ampr;
        std::ofstream phsr;
        std::ofstream freq;
        std::ofstream plsr;
        ampr.open( directory + "\\ampr.txt" );
        phsr.open( directory + "\\phsr.txt" );
        freq.open( directory + "\\freq.txt" );
        plsr.open( directory + "\\plsr.txt" );

        // frequency response
        for( int i = 0 ; i < m_Fs * 0.5 ; i++ )
        {
            freq_ch( i );
            ampr << m_Km << "\n";
            phsr << m_pH << "\n";
            freq << i    << "\n";
        }

        // pulse response:
        for( int i = 0 ; i <= m_order ; i++ ) { plsr << m_buff_cx[i] << "\n"; }

        // close files:
        ampr.close();
        phsr.close();
        freq.close();
        plsr.close();
    }

    #endif

    // memory allocation:
    __bool allocate()
    {
        // memory allocation:
        m_buff_cx = ( __type* ) calloc( m_order + 1 , sizeof ( __type ) );
        m_buff_sx.allocate( m_order + 2 );
        // compute FIR filter coefficients:
        coeff_calc();
        return 1;
    }

    // memory deallocation:
    __void deallocate()
    {
        if( m_buff_cx != 0 ) free( m_buff_cx );
        m_buff_sx.deallocate();
        m_wind_fcn.deallocate();
    }

    // initialization function:
    __void init( __type Fs, __type Fn, __type F_stop , __type BandWidth , __ix32 order , __bool scale )
    {
        m_Fs      = Fs;
        m_Fn      = Fn;
        m_Fstop1  = F_stop;
        m_Fstop2  = F_stop + BandWidth;
        m_Ts      = 1 / m_Fs;
        m_order   = order;
        m_Ns      = m_order;
        m_scale   = scale;
        m_buff_cx = 0;
        m_out     = 0;

        // window function initialization:
        m_wind_fcn.init( m_order + 1 );
    }

    // default constructor:
    fir_bp()
    {
        m_Fs      = 4000;
        m_Fn      = 50;
        m_Fstop1  = 100;
        m_Fstop1  = 200;
        m_Ts      = 1 / m_Fs;
        m_order   = 80;
        m_Ns      = m_order;
        m_scale   = false;
        m_buff_cx = 0;
        m_out     = 0;

        // window function initialization:
        m_wind_fcn.init( m_order + 1 );
    }

    // initializing constructor:
    fir_bp( __type Fs, __type Fn, __type F_stop , __type BandWidth , __ix32 order , __bool scale ) { init( Fs , Fn , F_stop , BandWidth , order , scale ); }

    // destructor:
    ~fir_bp() { deallocate(); }

    // filtering function:

    // x32:
    inline __type filt( __type *input )
    {
        m_buff_sx( input );
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    inline __type filt()
    {
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    // operators:
    inline __type operator() ( __type *input ) { return filt( input ); }
    inline __type operator() () { return filt(); }
};

// bandstop implementations:

// float x64:
template<> class fir_bs<__fx32>
{
    typedef __fx32 __type ;
    typedef bool   __bool ;
    typedef void   __void ;

    // system variables:
    __type m_Ts;
    __type m_Fs;
    __type m_Fn;
    __type m_Fstop1;
    __type m_Fstop2;
    __type m_Ns;
    __ix32 m_order;
    __bool m_scale;

    // coefficients:
    __type *m_buff_cx;

public:

    __type m_pH;
    __type m_Km;
    __type m_out;

    // buffers:
    mirror_ring_buffer< __type > m_buff_sx;

    // window function object:
    wind_fcn m_wind_fcn;

    // coefficients computation function:
    __void coeff_calc()
    {
        // check if the window is ready to use:
        if ( m_wind_fcn.is_ready() == 0 ) m_wind_fcn.Chebyshev(100);

        // coefficients computation:
        __ix32 k = 0;
        m_Fstop1 /= m_Fs;
        m_Fstop2 /= m_Fs;

        for (int n = 0; n <= m_order / 2; n++)
        {
            k = abs(n - (m_order + 1) / 2);

            if (n == 0)
            {
                m_buff_cx[k] = 1 - 2 * (m_Fstop2 - m_Fstop1) * m_wind_fcn[k];
            }
            else
            {
                m_buff_cx[k] = 2 * (m_Fstop1 * sin(n * PI2 * m_Fstop1) / (n * PI2 * m_Fstop1) - m_Fstop2 * sin(n * PI2 * m_Fstop2) / (n * PI2 * m_Fstop2)) * m_wind_fcn[k];
                m_buff_cx[m_order - k] = m_buff_cx[k];
            }
        }

        // filter pulse characteristic normalization:
        if ( m_scale )
        {
            freq_ch(0);
            for ( __ix32 n = 0; n <= m_order; n++) m_buff_cx[n] = m_buff_cx[n] / m_Km;
            freq_ch(m_Fn);
        }

        // deallocate the window function:
        m_wind_fcn.deallocate();
    }

    // Frequency characteristics:
    __void freq_ch( __type F )
    {
        __type m_W_re = 0;
        __type m_W_im = 0;
        for ( __ix32 n = 0; n <= m_order ; n++)
        {
            m_W_re = m_W_re + cos(-PI2 * n * F * m_Ts) * m_buff_cx[n];
            m_W_im = m_W_im + sin(-PI2 * n * F * m_Ts) * m_buff_cx[n];
        }
        m_pH   = atan2( m_W_im , m_W_im );
        m_Km   = sqrt (m_W_re * m_W_re + m_W_im * m_W_im);
    }

    #ifndef __ALG_PLATFORM // identify if the compilation is for ProsoftSystems IDE

    // filter response store to a text files:
    __void freq_rp( std::string directory )
    {
        // open files:
        std::ofstream ampr;
        std::ofstream phsr;
        std::ofstream freq;
        std::ofstream plsr;
        ampr.open( directory + "\\ampr.txt" );
        phsr.open( directory + "\\phsr.txt" );
        freq.open( directory + "\\freq.txt" );
        plsr.open( directory + "\\plsr.txt" );

        // frequency response
        for( int i = 0 ; i < m_Fs * 0.5 ; i++ )
        {
            freq_ch( i );
            ampr << m_Km << "\n";
            phsr << m_pH << "\n";
            freq << i    << "\n";
        }

        // pulse response:
        for( int i = 0 ; i <= m_order ; i++ ) { plsr << m_buff_cx[i] << "\n"; }

        // close files:
        ampr.close();
        phsr.close();
        freq.close();
        plsr.close();
    }

    #endif

    // memory allocation:
    __bool allocate()
    {
        // memory allocation:
        m_buff_cx = ( __type* ) calloc( m_order + 1 , sizeof ( __type ) );
        m_buff_sx.allocate( m_order + 2 );
        // compute FIR filter coefficients:
        coeff_calc();
        return 1;
    }

    // memory deallocation:
    __void deallocate()
    {
        if( m_buff_cx != 0 ) free( m_buff_cx );
        m_buff_sx.deallocate();
        m_wind_fcn.deallocate();
    }

    // initialization function:
    __void init( __type Fs, __type Fn, __type F_stop , __type BandWidth , __ix32 order , __bool scale )
    {
        m_Fs      = Fs;
        m_Fn      = Fn;
        m_Fstop1  = F_stop;
        m_Fstop2  = F_stop + BandWidth;
        m_Ts      = 1 / m_Fs;
        m_order   = order;
        m_Ns      = m_order;
        m_scale   = scale;
        m_buff_cx = 0;
        m_out     = 0;

        // window function initialization:
        m_wind_fcn.init( m_order + 1 );
    }

    // default constructor:
    fir_bs()
    {
        m_Fs      = 4000;
        m_Fn      = 50;
        m_Fstop1  = 100;
        m_Fstop1  = 200;
        m_Ts      = 1 / m_Fs;
        m_order   = 80;
        m_Ns      = m_order;
        m_scale   = false;
        m_buff_cx = 0;
        m_out     = 0;

        // window function initialization:
        m_wind_fcn.init( m_order + 1 );
    }

    // initializing constructor:
    fir_bs( __type Fs, __type Fn, __type F_stop , __type BandWidth , __ix32 order , __bool scale ) { init( Fs , Fn , F_stop , BandWidth , order , scale ); }

    // destructor:
    ~fir_bs() { deallocate(); }

    // filtering function:

    // x32:
    inline __type filt( __type *input )
    {
        m_buff_sx( input );
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    // x64:
    inline __type filt( __fx64 *input )
    {
        m_buff_sx( input );
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    inline __type filt()
    {
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[ n ] * m_buff_cx[n];
        return m_out;
    }

    // operators:
    inline __type operator() ( __type *input ) { return filt( input ); }
    inline __type operator() ( __fx64 *input ) { return filt( input ); }
    inline __type operator() () { return filt(); }
};

// float x32:
template<> class fir_bs<__fx64>
{
    typedef __fx64 __type ;
    typedef bool   __bool ;
    typedef void   __void ;

    // system variables:
    __type m_Ts;
    __type m_Fs;
    __type m_Fn;
    __type m_Fstop1;
    __type m_Fstop2;
    __type m_Ns;
    __ix32 m_order;
    __bool m_scale;

    // coefficients:
    __type *m_buff_cx;

public:

    __type m_pH;
    __type m_Km;
    __type m_out;

    // buffers:
    mirror_ring_buffer< __type > m_buff_sx;

    // window function object:
    wind_fcn m_wind_fcn;

    // coefficients computation function:
    __void coeff_calc()
    {
        // check if the window is ready to use:
        if ( m_wind_fcn.is_ready() == 0 ) m_wind_fcn.Chebyshev(100);

        // coefficients computation:
        __ix32 k = 0;
        m_Fstop1 /= m_Fs;
        m_Fstop2 /= m_Fs;

        for (int n = 0; n <= m_order / 2; n++)
        {
            k = abs(n - (m_order + 1) / 2);

            if (n == 0)
            {
                m_buff_cx[k] = 1 - 2 * (m_Fstop2 - m_Fstop1) * m_wind_fcn[k];
            }
            else
            {
                m_buff_cx[k] = 2 * (m_Fstop1 * sin(n * PI2 * m_Fstop1) / (n * PI2 * m_Fstop1) - m_Fstop2 * sin(n * PI2 * m_Fstop2) / (n * PI2 * m_Fstop2)) * m_wind_fcn[k];
                m_buff_cx[m_order - k] = m_buff_cx[k];
            }
        }

        // filter pulse characteristic normalization:
        if ( m_scale )
        {
            freq_ch(0);
            for ( __ix32 n = 0; n <= m_order; n++) m_buff_cx[n] = m_buff_cx[n] / m_Km;
            freq_ch(m_Fn);
        }

        // deallocate the window function:
        m_wind_fcn.deallocate();
    }

    // Frequency characteristics:
    __void freq_ch( __type F )
    {
        __type m_W_re = 0;
        __type m_W_im = 0;
        for ( __ix32 n = 0; n <= m_order ; n++)
        {
            m_W_re = m_W_re + cos(-PI2 * n * F * m_Ts) * m_buff_cx[n];
            m_W_im = m_W_im + sin(-PI2 * n * F * m_Ts) * m_buff_cx[n];
        }
        m_pH   = atan2( m_W_im , m_W_im );
        m_Km   = sqrt (m_W_re * m_W_re + m_W_im * m_W_im);
    }

    #ifndef __ALG_PLATFORM

    // filter response store to a text files:
    __void freq_rp( std::string directory )
    {
        // open files:
        std::ofstream ampr;
        std::ofstream phsr;
        std::ofstream freq;
        std::ofstream plsr;
        ampr.open( directory + "\\ampr.txt" );
        phsr.open( directory + "\\phsr.txt" );
        freq.open( directory + "\\freq.txt" );
        plsr.open( directory + "\\plsr.txt" );

        // frequency response
        for( int i = 0 ; i < m_Fs * 0.5 ; i++ )
        {
            freq_ch( i );
            ampr << m_Km << "\n";
            phsr << m_pH << "\n";
            freq << i    << "\n";
        }

        // pulse response:
        for( int i = 0 ; i <= m_order ; i++ ) { plsr << m_buff_cx[i] << "\n"; }

        // close files:
        ampr.close();
        phsr.close();
        freq.close();
        plsr.close();
    }

    #endif

    // memory allocation:
    __bool allocate()
    {
        // memory allocation:
        m_buff_cx = ( __type* ) calloc( m_order + 1 , sizeof ( __type ) );
        m_buff_sx.allocate( m_order + 2 );
        // compute FIR filter coefficients:
        coeff_calc();
        return 1;
    }

    // memory deallocation:
    __void deallocate()
    {
        if( m_buff_cx != 0 ) free( m_buff_cx );
        m_buff_sx.deallocate();
        m_wind_fcn.deallocate();
    }

    // initialization function:
    __void init( __type Fs, __type Fn, __type F_stop , __type BandWidth , __ix32 order , __bool scale )
    {
        m_Fs      = Fs;
        m_Fn      = Fn;
        m_Fstop1  = F_stop;
        m_Fstop2  = F_stop + BandWidth;
        m_Ts      = 1 / m_Fs;
        m_order   = ( order % 2 != 0) ? ( order + 1 ) : order;
        m_Ns      = m_order;
        m_scale   = scale;
        m_buff_cx = 0;
        m_out     = 0;

        // window function initialization:
        m_wind_fcn.init( m_order + 1 );
    }

    // default constructor:
    fir_bs()
    {
        m_Fs      = 4000;
        m_Fn      = 50;
        m_Fstop1  = 100;
        m_Fstop1  = 200;
        m_Ts      = 1 / m_Fs;
        m_order   = 80;
        m_Ns      = m_order;
        m_scale   = false;
        m_buff_cx = 0;
        m_out     = 0;

        // window function initialization:
        m_wind_fcn.init( m_order + 1 );
    }

    // initializing constructor:
    fir_bs( __type Fs, __type Fn, __type F_stop , __type BandWidth , __ix32 order , __bool scale ) { init( Fs , Fn , F_stop , BandWidth , order , scale ); }

    // destructor:
    ~fir_bs() { deallocate(); }

    // filtering function:

    // x32:
    inline __type filt( __type *input )
    {
        m_buff_sx( input );
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[n] * m_buff_cx[n];
        return m_out;
    }

    inline __type filt()
    {
        m_out = 0;
        for ( __ix32 n = m_order; n >= 0; n--) m_out += m_buff_sx[n] * m_buff_cx[n];
        return m_out;
    }

    // operators:
    inline __type operator() ( __type *input ) { return filt( input ); }
    inline __type operator() () { return filt(); }
};

#undef __fx32
#undef __fx64
#undef __ix32

#endif
