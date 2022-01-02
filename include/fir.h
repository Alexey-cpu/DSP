/*!
 * \file
 * \brief   FIR filters
 * \authors A.Tykvinskiy
 * \date    21.12.2021
 * \version 1.0
 *
 * The header declares FIR filters
*/

#ifndef FIR_H
#define FIR_H

#ifndef __ALG_PLATFORM
#include <iostream>
#include <fstream>
#endif

/*! \defgroup <FIR_fcn> ( FIR filters )
 *  \brief the module contains FIR filter template class and it's auxiliary functions
    @{
*/

#include "buffer.h"
#include "special_functions.h"

/*! \brief defines 32-bit floating point type */
#ifndef __fx32
#define __fx32 float
#endif

/*! \brief defines 64-bit floating point type */
#ifndef __fx64
#define __fx64 double
#endif

/*! \brief defines extended 64-bit floating point type */
#ifndef __fxx64
#define __fxx64 long double
#endif

/*! \brief defines 32-bit integer type */
#ifndef __ix32
#define __ix32 int
#endif

/*! \brief defines pi */
#ifndef PI0
#define PI0 3.1415926535897932384626433832795
#endif

/*! \brief defines 2*pi */
#ifndef PI2
#define PI2 6.283185307179586476925286766559
#endif

/*! \brief defines pi / 2 */
#ifndef PI_2
#define PI_2 1.5707963267948966192313216916398
#endif

/*! \brief defines pi / 4 */
#ifndef PI_4
#define PI_4 0.78539816339744830961566084581988
#endif

/*!
  \brief FIR filter types enumeration:
  \param[ lowpass_fir  ] lowpass  FIR
  \param[ highpass_fir ] highpass FIR
  \param[ bandpass_fir ] bandpass FIR
  \param[ bandstop_fir ] bandstop FIR
*/
enum fir_type { lowpass_fir  , highpass_fir , bandpass_fir , bandstop_fir };

/*! \brief FIR filter frequency response template data structure */
template< typename T > struct fir_fr;

/*!
  \brief FIR filter frequency response 32-bit data structure
  \param[Km] amplitude frequency response , p.u
  \param[pH] phase frequency response , rad
*/
template<> struct fir_fr< __fx32  >{ __fx32 Km , pH; };

/*!
  \brief FIR filter frequency response 64-bit data structure
  \param[Km] amplitude frequency response , p.u
  \param[pH] phase frequency response , rad
*/
template<> struct fir_fr< __fx64  >{ __fx64 Km , pH; };

/*!
  \brief FIR filter frequency response extended 64-bit data structure
  \param[Km] amplitude frequency response , p.u
  \param[pH] phase frequency response , rad
*/
template<> struct fir_fr< __fxx64 >{ __fx64 Km , pH; };

/*!
  \brief FIR filter specification data structure
  \param[Fs]    sampling frequency        , Hz
  \param[Fn]    nominal frequency         , Hz
  \param[Fc]    cutoff frequency          , Hz
  \param[BW]    stopband / passband width , Hz
  \param[Ts]    sampling period           , s
  \param[order] filter order
  \param[N]     filter coefficients number
  \param[scale] filter scaling flag ( scale = 0 - not-scaled filter , scale = 1 - scaled filter )
*/
struct fir_sp { __fx64 Fs, Fn , Fc , BW , Ts ; __ix32 order , N , scale , type; };

/*!
  * \brief FIR filter frequency response computation functon
  * \param[Fs]     sampling frequency , Hz
  * \param[F]      input frequency    , Hz
  * \param[N]      filter order
  * \param[cfbuff] coefficients buffer
  * \return The function returns FIR filter frequency response data structure:
  *         \f[
  *             W\left( j * 2 * \pi * F \right) = a_i * e^{ -j * 2 * \pi * F * i * T_s }
  *         \f]
*/
template< typename OT , typename CT > fir_fr<OT> __fir_freq_resp__(  __fx64 Fs , __fx64 F , __ix32 order , CT *cfbuff )
{
    OT Re = 0 , Im = 0 , Ts = 1 / Fs;
    for ( __ix32 i = 0; i <= order; i++)
    {
        Re = Re + cos(-PI2 * i * F * Ts) * cfbuff[ i ];
        Im = Im + sin(-PI2 * i * F * Ts) * cfbuff[ i ];
    }
    OT Km = sqrt (Re * Re + Im * Im) , pH = atan2( Im , Re );
    return { Km , pH };
}

/*!
  * \brief FIR digital lowpass filter coefficients computation function
  * \param[Fs]     sampling frequency , Hz
  * \param[Fc]     cut-off frequency  , Hz
  * \param[N]      filter order
  * \param[scale]  scaling factor ( scale = 0 - not-scaled coefficients , scale = 1 - scaled coefficients )
  * \param[wind]   input window function object
  * \return The function returns digital lowpass FIR filter coefficients buffer. \newline
  *         Even order FIR filter coefficients are computed as follows:
  * \f[
  *     Fc = \frac{ Fc }{ Fs }
  *     n \in \left[ 0 ; \frac{ N }{ 2 } \right]       \newline
  *     k   = \left | n - \frac{ N + 1 }{ 2 } \right | \newline
  *     a_k = \begin{cases}
  *             2 * F_c * w_i \quad , \quad n = 0
  *             \\
  *             2 * Fc * \frac{ \sin{ ( 2 * \pi * n * F_c ) } }{ 2 * \pi * n * F_c } \quad , \quad n > 0
  *             \\
  *             a_{N-k} = a_k , \quad , \quad n > 0
  *           \end{cases}
  * \f]
  *         Odd order FIR filter coefficients are computed as follows:
  * \f[
  *         Fc = \frac{ Fc }{ Fs }
  *         n \in \left[ 0 ; \frac{ N }{ 2 } \right]            \newline
  *         k   = \left | n - \frac{ N + 1 }{ 2 } \right |      \newline
  *         r   = ceil \left( \frac{N}{2} \right) - \frac{N}{2} \newline
  *         a_k = 2 * F_c * \frac{ \sin{ ( 2 * \pi * F_c * ( n + r ) ) } }{ 2 * \pi * F_c * ( n + r ) } \newline
  *         a_{N-k} = a_k
  * \f]
  *
  *         If the flag scale = 1, then coefficients are scaled as follows:
  * \f[
  *         n \in \left[ 0 ; N \right]                          \newline
  *         a_n = \frac{ a_n }{ \left| W(j*\omega) \right| }
  * \f]
  *
*/
template< typename T > T* __fir_wind_digital_lp__( __fx64 Fs , __fx64 Fc , __ix32 N , bool scale , wind_fcn &wind )
{
    typedef T __type;

    // check if the window is ready to use:
    if ( wind.is_ready() == 0 ) wind.Chebyshev(100);

    // coefficients buffer memory allocation:
    __type *cfbuff = ( __type* ) calloc( N + 1 , sizeof ( __type ) );

    // coefficients computation:
    __ix32 k = 0;
    Fc /= Fs;

    if ( N % 2 == 0) // even order filter coefficients computation
    {
        for ( __ix32 n = 0; n <= N / 2; n++)
        {
            k = abs(n - (N + 1) / 2);
            if (n == 0)
            {
                cfbuff[k] = 2 * Fc * wind[k];
            }
            else
            {
                cfbuff[k] = 2 * Fc * sin(n * PI2 * Fc ) / (n * PI2 * Fc ) * wind[k];
                cfbuff[N - k] = cfbuff[k];
            }
        }
    }
    else if ( N % 2 != 0) // odd order filter coefficients computation
    {
        __type rem = ceil( (__type)N / 2) - (__type)N / 2;
        for ( __ix32 n = 0; n <= ceil(N / 2); n++)
        {
            k = abs(n - N / 2);
            cfbuff[k] = 2 * Fc * sin((n + rem) * PI2 * Fc ) / ((n + rem) * PI2 * Fc ) * wind[k];
            cfbuff[ N - k] = cfbuff[k];
        }
    }

    // filter pulse characteristic normalization:
    if ( scale )
    {
        fir_fr< __type > fr = __fir_freq_resp__< __type , __type>( Fs , 0 , N , cfbuff );
        for ( __ix32 n = 0; n <= N; n++) cfbuff[n] /= fr.Km;
    }

    // deallocate the window function:
    wind.deallocate();

    // returning the result
    return cfbuff;
}

/*!
  * \brief FIR digital highpass filter coefficients computation function
  * \param[Fs]     sampling frequency , Hz
  * \param[Fp]     pass frequency     , Hz
  * \param[N]      filter order
  * \param[scale]  scaling factor ( scale = 0 - not-scaled coefficients , scale = 1 - scaled coefficients )
  * \param[wind]   input window function object
  * \return The function returns digital lowpass FIR filter coefficients buffer.
  *         Even order FIR highpass filter coefficients are computed as follows:
  *
  *         \f[
  *             Fp = \frac{ Fp }{ Fs }
  *             n \in \left[ 0 ; \frac{ N }{ 2 } \right]       \newline
  *             k   = \left | n - \frac{ N + 1 }{ 2 } \right | \newline
  *             a_k = \begin{cases}
  *                   ( 1 - 2 * F_p ) * w_i \quad , \quad n = 0
  *                   \\
  *                   ( 1 - 2 * F_p ) * \frac{ \sin{ ( 2 * \pi * n * F_p ) } }{ 2 * \pi * n * F_p } * w_i \quad , \quad n > 0
  *                   \end{cases}
  *                   a_{N-k} = a_k \quad , \quad n > 0
  *         \f]
  *
  *         Odd order FIR highpass filter coefficients are computed as follows:
  *
  *         \f[
  *             Fp = \frac{ Fp }{ Fs }
  *             n \in \left[ 0 ; \frac{ N }{ 2 } \right]                \newline
  *             k   = \left | n - \frac{ N + 1 }{ 2 } \right |          \newline
  *             r   = ceil \left( n - \frac{N}{2} \right) - \frac{N}{2} \newline
  *             a_k = -1^{n} * 2 * \left[ 0.5 - F_p \right]
  *                    * \frac{ \sin{ \left[ ( n + r ) * 2 * \pi * ( 0.5 - F_p ) \right] } }{ \left[ ( n + r ) * 2 * \pi * ( 0.5 - F_p ) \right] }
  *         \f]
  *
  *         If the flag scale = 1, then coefficients are scaled as follows:
  *
  *         \f[
  *             n \in \left[ 0 ; N \right]                          \newline
  *             a_n = \frac{ a_n }{ \left| W(j*\omega) \right| }
  *         \f]
*/
template< typename T > T* __fir_wind_digital_hp__( __fx64 Fs , __fx64 Fp , __ix32 N , bool scale , wind_fcn &wind )
{
    typedef T __type;

    // check if the window is ready to use:
    if ( wind.is_ready() == 0 ) wind.Chebyshev(100);

    // coefficients buffer memory allocation:
    __type *cfbuff = ( __type* ) calloc( N + 1 , sizeof ( __type ) );

    // coefficients computation:
    __ix32 k = 0;
    Fp /= Fs;

    if ( N % 2 == 0) // even order highpass
    {
        for ( __ix32 n = 0; n <= N / 2; n++)
        {
            k = abs(n - ( N + 1) / 2);

            if (n == 0)
            {
                cfbuff[k] = (1 - 2 * Fp) * wind[k];
            }
            else
            {
                cfbuff[k] = -2 * Fp * sin(n * PI2 * Fp ) / (n * PI2 * Fp ) * wind[k];
                cfbuff[N - k] = cfbuff[k];
            }
        }
    }
    else if (N % 2 != 0) // odd order highpass
    {
        __type rem = ceil( (__type)N / 2) - (__type)N / 2;
        for ( __ix32 n = 0; n <= ceil(N / 2); n++)
        {
            k = abs(n - N / 2);
            cfbuff[k]     = -pow(-1 , n) * 2 * (0.5-Fp) * sin((n + rem) * PI2 * (0.5-Fp) ) / ((n + rem) * PI2 * (0.5 - Fp) )* wind[k];
            cfbuff[N - k] = -cfbuff[k];
        }
    }

    // filter pulse characteristic scaling:
    if ( scale )
    {
        fir_fr< __type > fr = __fir_freq_resp__< __type , __type >( Fs , Fs / 2 , N , cfbuff );
        for ( __ix32 n = 0; n <= N; n++) cfbuff[n] /= fr.Km;
    }

    // deallocate the window function:
    wind.deallocate();

    // returning the result
    return cfbuff;
}

/*!
  * \brief FIR digital bandpass filter coefficients computation function
  * \param[Fs]     sampling frequency , Hz
  * \param[Fp]     pass frequency     , Hz
  * \param[BW]     bandpass width     , Hz
  * \param[N]      filter order
  * \param[scale]  scaling factor ( scale = 0 - not-scaled coefficients , scale = 1 - scaled coefficients )
  * \param[wind]   input window function object
  * \return The function returns digital highpass FIR filter coefficients buffer.
  *         Even order FIR bandpass filter coefficients are computed as follows:
  *
  *         \f[
  *             F_{p1} = \frac{ F_{p1} }{ Fs }                 \newline
  *             F_{p2} = \frac{ F_{p1}+BW }{ Fs }              \newline
  *             n \in \left[ 0 ; \frac{ N }{ 2 } \right]       \newline
  *             k   = \left | n - \frac{ N + 1 }{ 2 } \right | \newline
  *
  *             a_k = \begin{cases}
  *                   2 * ( F_{p1} - F_{p2} ) * w_i \quad , \quad n = 0
  *                   \\
  *                   2 * \left[ F_{p2} * \frac{ \sin{ ( 2 * \pi * F_{p2} * n ) } }{ 2 * \pi * F_{p2} * n } -
  *                              F_{p1} * \frac{ \sin{ ( 2 * \pi * F_{p1} * n ) } }{ 2 * \pi * F_{p1} * n }
  *                       \right] * w_i \quad , \quad n > 0
  *                   \\
  *                   a_{N-k} = a_k \quad , \quad n > 0
  *                   \end{cases}
  *         \f]
  *
  *         Odd order FIR bandpass filter coefficients are computed as follows:
  *
  *         \f[
  *             F_{p1} = \frac{ F_{p1} }{ Fs }                          \newline
  *             F_{p2} = \frac{ F_{p1}+BW }{ Fs }                       \newline
  *             n \in \left[ 0 ; \frac{ N }{ 2 } \right]                \newline
  *             k   = \left | n - \frac{ N + 1 }{ 2 } \right |          \newline
  *             r   = ceil\left(n - \frac{N}{2} \right) - \frac{N}{2}   \newline
  *
  *             a_k = 2 * \left[ F_{p2} * \frac{ \sin{ ( 2 * \pi * F_{p2} * ( n + r ) ) } }{ 2 * \pi * F_{p2} * ( n + r ) } -
  *                              F_{p1} * \frac{ \sin{ ( 2 * \pi * F_{p1} * ( n + r ) ) } }{ 2 * \pi * F_{p1} * ( n + r ) }
  *                      \right] * w_i \newline
  *             a_{N-k} = a_k \quad , \quad n > 0
  *         \f]
  *
  *         If the flag scale = 1, then coefficients are scaled as follows:
  *
  *         \f[
  *             n \in \left[ 0 ; N \right]                          \newline
  *             a_n = \frac{ a_n }{ \left| W(j*\omega) \right| }
  *         \f]
*/
template< typename T > T* __fir_wind_digital_bp__( __fx64 Fs , __fx64 Fp , __fx64 BW , __ix32 N , bool scale , wind_fcn &wind )
{
    typedef T __type;

    // check if the window is ready to use:
    if ( wind.is_ready() == 0 ) wind.Chebyshev(100);

    // coefficients buffer memory allocation:
    __type *cfbuff = ( __type* ) calloc( N + 1 , sizeof ( __type ) );

    // coefficients computation:
    __ix32 k = 0;
    __fx64 Fp1 = Fp / Fs , Fp2 = ( Fp + BW )/ Fs;

    if ( N % 2 == 0) // even order bandpass
    {
        for ( __ix32 n = 0; n <= N / 2; n++)
        {
            k = abs(n - ( N + 1) / 2);

            if (n == 0)
            {
                cfbuff[k] = 2 * ( Fp2 - Fp1) * wind[k];
            }
            else
            {
                cfbuff[k] = 2 * (Fp2 * sin(n * PI2 * Fp2) / (n * PI2 * Fp2) - Fp1 * sin(n * PI2 * Fp1) / (n * PI2 * Fp1)) * wind[k];
                cfbuff[N - k] = cfbuff[k];
            }
        }
    }
    else if (N % 2 != 0) // odd order bandpass
    {
        __type rem = ceil((__type)N / 2) - (__type)N / 2;
        for ( __ix32 n = 0; n <= ceil(N / 2); n++)
        {
            k = abs(n - N / 2);
            cfbuff[k] = 2 * (Fp2 * sin((n + rem) * PI2 * Fp2) / ((n + rem) * PI2 * Fp2) - Fp1 * sin((n + rem) * PI2 * Fp1) / ((n + rem) * PI2 * Fp1)) * wind[k];
            cfbuff[N - k] = cfbuff[k];
        }
    }

    // filter pulse characteristic normalization:
    if ( scale )
    {
        fir_fr< __type > fr = __fir_freq_resp__< __type , __type >( Fs , ( ( Fp1 + 0.5 * ( Fp2 - Fp1 ) ) * Fs ) , N , cfbuff );
        for ( __ix32 n = 0; n <= N; n++) cfbuff[n] /= fr.Km;
    }

    // deallocate the window function:
    wind.deallocate();

    // returning the result:
    return cfbuff;
}

/*!
  * \brief FIR digital bandstop filter coefficients computation function
  * \param[Fs]     sampling frequency , Hz
  * \param[Fc]     cut-off frequency  , Hz
  * \param[BW]     bandstop width     , Hz
  * \param[N]      filter order
  * \param[scale]  scaling factor ( scale = 0 - not-scaled coefficients , scale = 1 - scaled coefficients )
  * \param[wind]   input window function object
  * \return The function returns digital bandstop FIR filter coefficients buffer.
  *         \f[
  *             F_{c1} = \frac{ F_{c1} }{ Fs }                 \newline
  *             F_{c2} = \frac{ F_{c1}+BW }{ Fs }              \newline
  *             n \in \left[ 0 ; \frac{ N }{ 2 } \right]       \newline
  *             k   = \left | n - \frac{ N + 1 }{ 2 } \right | \newline
  *             a_k = \begin{cases}
  *                   1 - 2 * ( F_{c2} - F_{c1} ) * w_k \quad , \quad n = 0
  *                   \\
  *                   2 * \left[ F_{c1} * \frac{ \sin{ ( 2 * \pi * n * F_{c1} ) } }{ 2 * \pi * n * F_{c1} } -
  *                              F_{c2} * \frac{ \sin{ ( 2 * \pi * n * F_{c2} ) } }{ 2 * \pi * n * F_{c2} }
  *                       \right] * w_k \quad , \quad n > 0
  *                   \end{cases}
  *         \f]
  *
  *         If the flag scale = 1, then coefficients are scaled as follows:
  *
  *         \f[
  *             n \in \left[ 0 ; N \right]                          \newline
  *             a_n = \frac{ a_n }{ \left| W(j*\omega) \right| }
  *         \f]
*/
template< typename T > T* __fir_wind_digital_bs__( __fx64 Fs , __fx64 Fc , __fx64 BW , __ix32 N , bool scale , wind_fcn &wind )
{
    typedef T __type;

    // check if the window is ready to use:
    if ( wind.is_ready() == 0 ) wind.Chebyshev(100);

    // coefficients buffer memory allocation:
    __type *cfbuff = ( __type* ) calloc( N + 1 , sizeof ( __type ) );

    // coefficients computation:
    __ix32 k = 0;
    __fx64 Fc1 = Fc / Fs , Fc2 = ( Fc + BW ) / Fs;

    for (int n = 0; n <= N / 2; n++)
    {
        k = abs(n - ( N + 1) / 2);

        if (n == 0)
        {
            cfbuff[k] = 1 - 2 * ( Fc2 - Fc1 ) * wind[k];
        }
        else
        {
            cfbuff[k] = 2 * ( Fc1 * sin(n * PI2 * Fc1) / (n * PI2 * Fc1 ) - Fc2 * sin(n * PI2 * Fc2) / (n * PI2 * Fc2) ) * wind[k];
            cfbuff[ N - k] = cfbuff[k];
        }
    }

    // filter pulse characteristic normalization:
    if ( scale )
    {
        fir_fr< __type > fr = __fir_freq_resp__< __type , __type >( Fs , 0 , N , cfbuff );
        for ( __ix32 n = 0; n <= N; n++) cfbuff[n] /= fr.Km;
    }

    // deallocate the window function:
    wind.deallocate();

    // return the result:
    return cfbuff;
}

/*! \brief template FIR class */
template< typename T > class fir;

/*! \brief 32-bit floating point FIR */
template<> class fir<__fx32>
{
    typedef __fx32 __type ;
    typedef bool   __bool ;
    typedef void   __void ;

    /*! \brief lowpass specification data structure */
    fir_sp  m_sp;

    /*! \brief lowpass coefficients buffer */
    __type *m_cf;

    /*! \brief lowpass input buffer */
    mirror_ring_buffer< __type > m_bx;

public:

    /*! \brief lowpass output */
    __type m_out;

   /*! \brief lowpass window function object */
    wind_fcn m_wind;

    /*! \brief lowpass memory allocation function */
    __ix32 allocate()
    {
        switch ( m_sp.type )
        {
            case fir_type::lowpass_fir:
            m_cf = __fir_wind_digital_lp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , m_sp.scale , m_wind );
            break;

            case fir_type::highpass_fir:
            m_cf = __fir_wind_digital_hp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , m_sp.scale , m_wind );
            break;

            case fir_type::bandpass_fir:
            m_cf = __fir_wind_digital_bp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , m_sp.scale , m_wind );
            break;

            case fir_type::bandstop_fir:
            m_cf = __fir_wind_digital_bs__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , m_sp.scale , m_wind );
            break;
        }

        m_bx.allocate( m_sp.N + 1 );
        return ( m_cf != 0 );
    }

    /*! \brief lowpass memory deallocation function */
    __void deallocate()
    {
        if( m_cf != 0 ) free( m_cf );
        m_bx.deallocate();
        m_wind.deallocate();
    }

    /*! \brief Lowpass initialization function
     *  \details the function initializes lowpass FIR filter
     *  \param[Fs]      - sampling frequency , Hz
     *  \param[Fn]      - nominal frequency  , Hz
     *  \param[Fc]      - cut-off frequency  , Hz
     *  \param[order]   - filter order
     *  \param[scale]   - filter scaling flag ( 0 - not-scaled filter , 1 - scaled filter )
    */
    __void lp_init( __type Fs, __type Fn, __type Fc, __ix32 order , __bool scale )
    {
        m_sp  = fir_sp{ Fs , Fn , Fc , -1 , 1 / Fs , order , order + 1 , scale , fir_type::lowpass_fir };
        m_cf  = 0;
        m_out = 0;
        m_wind.init( m_sp.N );
    }

    /*! \brief Highpass initialization function
     *  \details the function initializes highpass FIR filter
     *  \param[Fs]      - sampling frequency , Hz
     *  \param[Fn]      - nominal frequency  , Hz
     *  \param[Fp]      - pass frequency     , Hz
     *  \param[order]   - filter order
     *  \param[scale]   - filter scaling flag ( 0 - not-scaled filter , 1 - scaled filter )
    */
    __void hp_init( __type Fs, __type Fn, __type Fp , __ix32 order , __bool scale )
    {
        m_sp  = fir_sp{ Fs , Fn , Fp , -1 , 1 / Fs , order , order + 1 , scale , fir_type::highpass_fir };
        m_cf  = 0;
        m_out = 0;
        m_wind.init( m_sp.N );
    }

    /*! \brief Bandpass initialization function
     *  \details the function initializes bandpass FIR filter
     *  \param[Fs]      - sampling frequency , Hz
     *  \param[Fn]      - nominal frequency  , Hz
     *  \param[Fp]      - cut-off frequency  , Hz
     *  \param[BW]      - pass bandwidth     , Hz
     *  \param[order]   - filter order
     *  \param[scale]   - filter scaling flag ( 0 - not-scaled filter , 1 - scaled filter )
    */
    __void bp_init( __type Fs, __type Fn, __type Fp , __fx64 BW , __ix32 order , __bool scale )
    {
        m_sp  = fir_sp{ Fs , Fn , Fp , BW , 1 / Fs , order , order + 1 , scale , fir_type::bandpass_fir };
        m_cf  = 0;
        m_out = 0;
        m_wind.init( m_sp.N );
    }

    /*! \brief Bandstop initialization function
     *  \details The function initializes bandstop FIR filter
     *  \param[Fs]      - sampling frequency , Hz
     *  \param[Fn]      - nominal frequency  , Hz
     *  \param[Fc]      - cut-off frequency  , Hz
     *  \param[BW]      - stop bandwidth     , Hz
     *  \param[order]   - filter order
     *  \param[scale]   - filter scaling flag ( 0 - not-scaled filter , 1 - scaled filter )
    */
    __void bs_init( __type Fs, __type Fn, __type Fc , __fx64 BW , __ix32 order , __bool scale )
    {
        m_sp  = fir_sp{ Fs , Fn , Fc , BW , 1 / Fs , order , order + 1 , scale , fir_type::bandstop_fir };
        m_cf  = 0;
        m_out = 0;
        m_wind.init( m_sp.N );
    }

    /*! \brief default constructor */
    fir()
    {
        m_sp  = fir_sp{ 4000 , 50 , 100 , -1 , 1 / 4000 , 80 , 80 + 1 , 1 , fir_type::lowpass_fir };
        m_cf  = 0;
        m_out = 0;
        m_wind.init( m_sp.N  );
    }

    /*! \brief  destructor */
    ~fir() { deallocate(); }

    /*!
     *  \brief  frequency response computation function
     *  \param[F] input frequency
     *  \return the function returns fir_fr data structure
    */
    inline fir_fr< __fx64 > freq_resp( __fx64 F ) { return __fir_freq_resp__< __fx64 , __type >( m_sp.Fs , F , m_sp.order , m_cf ); }

    /*!
     *  \brief  FIR pulse response getting function
     *  \param[F] pulse response sample number
     *  \return the function returns FIR impulse response sample
    */
    inline __type get_coeff( __ix32 n ) { return ( n <= m_sp.order ) ? m_cf[ n ] : 1e6; }

    /*!
     *  \brief  FIR filtering function
     *  \param[input] pointer to the input data array
     *  \return the function returns FIR filtering result
    */
    inline __type filt( __type *input )
    {
        m_bx( input ); m_out = 0;
        for ( __ix32 n = m_sp.order ; n >= 0; n--) m_out += m_bx[ n ] * m_cf[n];
        return m_out;
    }

    /*!
     *  \brief  FIR filtering function
     *  \return the function returns FIR filtering result
    */
    inline __type filt()
    {
        m_out = 0;
        for ( __ix32 n = m_sp.order ; n >= 0; n--) m_out += m_bx[ n ] * m_cf[n];
        return m_out;
    }

    /*!
     *  \brief  FIR filtering () operator
     *  \param[input] pointer to the input data
     *  \return the () operator calls filt( __type *input ) function that returns FIR filtering result
    */
    inline __type operator() ( __type *input ) { return filt( input ); }
};

/*! \brief 64-bit floating point FIR */
template<> class fir<__fx64>
{
    typedef __fx64 __type ;
    typedef bool   __bool ;
    typedef void   __void ;

    /*! \brief lowpass specification data structure */
    fir_sp  m_sp;

    /*! \brief lowpass coefficients buffer */
    __type *m_cf;

    /*! \brief lowpass input buffer */
    mirror_ring_buffer< __type > m_bx;

public:

    /*! \brief lowpass output */
    __type m_out;

   /*! \brief lowpass window function object */
    wind_fcn m_wind;

    /*! \brief lowpass memory allocation function */
    __ix32 allocate()
    {
        switch ( m_sp.type )
        {
            case fir_type::lowpass_fir:
            m_cf = __fir_wind_digital_lp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , m_sp.scale , m_wind );
            break;

            case fir_type::highpass_fir:
            m_cf = __fir_wind_digital_hp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , m_sp.scale , m_wind );
            break;

            case fir_type::bandpass_fir:
            m_cf = __fir_wind_digital_bp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , m_sp.scale , m_wind );
            break;

            case fir_type::bandstop_fir:
            m_cf = __fir_wind_digital_bs__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , m_sp.scale , m_wind );
            break;
        }

        m_bx.allocate( m_sp.N + 1 );
        return ( m_cf != 0 );
    }

    /*! \brief lowpass memory deallocation function */
    __void deallocate()
    {
        if( m_cf != 0 ) free( m_cf );
        m_bx.deallocate();
        m_wind.deallocate();
    }

    /*! \brief Lowpass initialization function
     *  \details the function initializes lowpass FIR filter
     *  \param[Fs]      - sampling frequency , Hz
     *  \param[Fn]      - nominal frequency  , Hz
     *  \param[Fc]      - cut-off frequency  , Hz
     *  \param[order]   - filter order
     *  \param[scale]   - filter scaling flag ( 0 - not-scaled filter , 1 - scaled filter )
    */
    __void lp_init( __type Fs, __type Fn, __type Fc, __ix32 order , __bool scale )
    {
        m_sp  = fir_sp{ Fs , Fn , Fc , -1 , 1 / Fs , order , order + 1 , scale , fir_type::lowpass_fir };
        m_cf  = 0;
        m_out = 0;
        m_wind.init( m_sp.N );
    }

    /*! \brief Highpass initialization function
     *  \details the function initializes highpass FIR filter
     *  \param[Fs]      - sampling frequency , Hz
     *  \param[Fn]      - nominal frequency  , Hz
     *  \param[Fp]      - pass frequency     , Hz
     *  \param[order]   - filter order
     *  \param[scale]   - filter scaling flag ( 0 - not-scaled filter , 1 - scaled filter )
    */
    __void hp_init( __type Fs, __type Fn, __type Fp , __ix32 order , __bool scale )
    {
        m_sp  = fir_sp{ Fs , Fn , Fp , -1 , 1 / Fs , order , order + 1 , scale , fir_type::highpass_fir };
        m_cf  = 0;
        m_out = 0;
        m_wind.init( m_sp.N );
    }

    /*! \brief Bandpass initialization function
     *  \details the function initializes bandpass FIR filter
     *  \param[Fs]      - sampling frequency , Hz
     *  \param[Fn]      - nominal frequency  , Hz
     *  \param[Fp]      - cut-off frequency  , Hz
     *  \param[BW]      - pass bandwidth     , Hz
     *  \param[order]   - filter order
     *  \param[scale]   - filter scaling flag ( 0 - not-scaled filter , 1 - scaled filter )
    */
    __void bp_init( __type Fs, __type Fn, __type Fp , __fx64 BW , __ix32 order , __bool scale )
    {
        m_sp  = fir_sp{ Fs , Fn , Fp , BW , 1 / Fs , order , order + 1 , scale , fir_type::bandpass_fir };
        m_cf  = 0;
        m_out = 0;
        m_wind.init( m_sp.N );
    }

    /*! \brief Bandstop initialization function
     *  \details The function initializes bandstop FIR filter
     *  \param[Fs]      - sampling frequency , Hz
     *  \param[Fn]      - nominal frequency  , Hz
     *  \param[Fc]      - cut-off frequency  , Hz
     *  \param[BW]      - stop bandwidth     , Hz
     *  \param[order]   - filter order
     *  \param[scale]   - filter scaling flag ( 0 - not-scaled filter , 1 - scaled filter )
    */
    __void bs_init( __type Fs, __type Fn, __type Fc , __fx64 BW , __ix32 order , __bool scale )
    {
        m_sp  = fir_sp{ Fs , Fn , Fc , BW , 1 / Fs , order , order + 1 , scale , fir_type::bandstop_fir };
        m_cf  = 0;
        m_out = 0;
        m_wind.init( m_sp.N );
    }

    /*! \brief default constructor */
    fir()
    {
        m_sp  = fir_sp{ 4000 , 50 , 100 , -1 , 1 / 4000 , 80 , 80 + 1 , 1 , fir_type::lowpass_fir };
        m_cf  = 0;
        m_out = 0;
        m_wind.init( m_sp.N  );
    }

    /*! \brief  destructor */
    ~fir() { deallocate(); }

    /*!
     *  \brief  frequency response computation function
     *  \param[F] input frequency
     *  \return the function returns fir_fr data structure
    */
    inline fir_fr< __fx64 > freq_resp( __type F ) { return __fir_freq_resp__< __fx64 , __type >( m_sp.Fs , F , m_sp.order , m_cf ); }

    /*!
     *  \brief  FIR pulse response getting function
     *  \param[F] pulse response sample number
     *  \return the function returns FIR impulse response sample
    */
    inline __type get_coeff( __ix32 n ) { return ( n <= m_sp.order ) ? m_cf[ n ] : 1e6; }

    /*!
     *  \brief  FIR filtering function
     *  \param[input] pointer to the input data array
     *  \return the function returns FIR filtering result
    */
    inline __type filt( __type *input )
    {
        m_bx( input ); m_out = 0;
        for ( __ix32 n = m_sp.order ; n >= 0; n--) m_out += m_bx[ n ] * m_cf[n];
        return m_out;
    }

    /*!
     *  \brief  FIR filtering function
     *  \return the function returns FIR filtering result
    */
    inline __type filt()
    {
        m_out = 0;
        for ( __ix32 n = m_sp.order ; n >= 0; n--) m_out += m_bx[ n ] * m_cf[n];
        return m_out;
    }

    /*!
     *  \brief  FIR filtering () operator
     *  \param[input] pointer to the input data
     *  \return the () operator calls filt( __type *input ) function that returns FIR filtering result
    */
    inline __type operator() ( __type *input ) { return filt( input ); }
};

/*! @} */

#undef __fx32
#undef __fx64
#undef __ix32

// customized pi undef:
#undef PI0
#undef PI2
#undef PI_2
#undef PI_4

#endif
