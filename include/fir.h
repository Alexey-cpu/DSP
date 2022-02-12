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

/*! \defgroup <FIR_FILTERS> ( FIR filters )
 *  \brief the module contains FIR filter template class and it's auxiliary functions
    @{
*/

#include "buffer.h"
#include "special_functions.h"

#ifndef DEBUGGING
//#define DEBUGGING
#endif

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

namespace FIR
{
    /*!
      \brief FIR filter types enumeration:
      \param[ lowpass  ] lowpass  FIR
      \param[ highpass ] highpass FIR
      \param[ bandpass ] bandpass FIR
      \param[ bandstop ] bandstop FIR
    */
    enum type { lowpass  , highpass , bandpass , bandstop };

    /*! \brief FIR filter frequency response template data structure */
    template< typename T > struct fr;

    /*!
      \brief FIR filter frequency response 32-bit data structure
      \param[Km] amplitude frequency response , p.u
      \param[pH] phase frequency response , rad
    */
    template<> struct fr< __fx32  >{ __fx32 Km , pH; };

    /*!
      \brief FIR filter frequency response 64-bit data structure
      \param[Km] amplitude frequency response , p.u
      \param[pH] phase frequency response , rad
    */
    template<> struct fr< __fx64  >{ __fx64 Km , pH; };

    /*!
      \brief FIR filter frequency response extended 64-bit data structure
      \param[Km] amplitude frequency response , p.u
      \param[pH] phase frequency response , rad
    */
    template<> struct fr< __fxx64 >{ __fx64 Km , pH; };

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
    struct sp { __fx64 Fs, Fn , Fc , BW , Ts ; __ix32 order , N , scale , type; };

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
    template< typename OT , typename CT > fr<OT> __freq_resp__(  __fx64 Fs , __fx64 F , __ix32 order , CT *cfbuff )
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
            fr< __type > fr = __freq_resp__< __type , __type>( Fs , 0 , N , cfbuff );
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
            fr< __type > fr = __freq_resp__< __type , __type >( Fs , Fs / 2 , N , cfbuff );
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
            fr< __type > fr = __freq_resp__< __type , __type >( Fs , ( ( Fp1 + 0.5 * ( Fp2 - Fp1 ) ) * Fs ) , N , cfbuff );
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
            fr< __type > fr = __freq_resp__< __type , __type >( Fs , 0 , N , cfbuff );
            for ( __ix32 n = 0; n <= N; n++) cfbuff[n] /= fr.Km;
        }

        // deallocate the window function:
        wind.deallocate();

        // return the result:
        return cfbuff;
    }

    #ifndef __ALG_PLATFORM

    template< typename T > void __show_fir__( sp sp , T *cf , wind_fcn &wind )
    {
        printf( wind.m_name );
        printf( " " );

        switch ( sp.type )
        {
            case type::lowpass:
            printf( "lowpass specifications: \n" );
            break;

            case type::highpass:
            printf( "highpass specifications: \n" );
            break;

            case type::bandpass:
            printf( "bandpass specifications: \n" );
            break;

            case type::bandstop:
            printf( "bandstop specifications: \n" );
            break;
        }

        printf( "\n" );
        if( sp.Fs > 0    ) { printf( "Fs    = %.6f Hz \n"   , sp.Fs    ); }
        if( sp.Ts > 0    ) { printf( "Ts    = %.6f s  \n"   , sp.Ts    ); }
        if( sp.Fn > 0    ) { printf( "Fn    = %.6f Hz \n"   , sp.Fn    ); }
        if( sp.Fc > 0    ) { printf( "Fc    = %.6f Hz \n"   , sp.Fc    ); }
        if( sp.BW > 0    ) { printf( "BW    = %.6f Hz \n"   , sp.BW    ); }
        if( sp.order > 0 ) { printf( "order = %d      \n"   , sp.order ); }

        if( cf )
        {
            if( sp.scale ) printf( "scaled coefficients: \n\n" );
            else printf( "not-scaled coefficients: \n\n" );
            for( __ix32 i = 0 ; i < sp.order + 1 ; i++ ) printf( " coeff[ %d ] = %.12f \n" , i , cf[i] );
        }
        else
        {
            printf( "coefficients : coefficients have not been computed yet... \n" );
        }


    }

    #endif

    /*! \brief template abstract FIR filter class */
    template < typename T > class fir_abstract
    {
        typedef __fx32 __type ;
        typedef bool   __bool ;
        typedef void   __void ;

    protected:
        /*! \brief lowpass specification data structure */
        sp  m_sp;

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
                case type::lowpass:
                m_cf = __fir_wind_digital_lp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , m_sp.scale , m_wind );
                break;

                case type::highpass:
                m_cf = __fir_wind_digital_hp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , m_sp.scale , m_wind );
                break;

                case type::bandpass:
                m_cf = __fir_wind_digital_bp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , m_sp.scale , m_wind );
                break;

                case type::bandstop:
                m_cf = __fir_wind_digital_bs__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , m_sp.scale , m_wind );
                break;
            }

            m_bx.allocate( m_sp.N + 1 );
            return ( m_cf != 0 );
        }

        /*! \brief lowpass memory deallocation function */
        __void deallocate()
        {
            if( m_cf != 0 ) { free( m_cf ); m_cf = 0; }
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
            m_sp  = sp{ Fs , Fn , Fc , -1 , 1 / Fs , order , order + 1 , scale , type::lowpass };
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
            m_sp  = sp{ Fs , Fn , Fp , -1 , 1 / Fs , order , order + 1 , scale , type::highpass };
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
            m_sp  = sp{ Fs , Fn , Fp , BW , 1 / Fs , order , order + 1 , scale , type::bandpass };
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
            m_sp  = sp{ Fs , Fn , Fc , BW , 1 / Fs , order , order + 1 , scale , type::bandstop };
            m_cf  = 0;
            m_out = 0;
            m_wind.init( m_sp.N );
        }

        /*! \brief default constructor */
        fir_abstract()
        {
            m_sp  = sp{ 4000 , 50 , 100 , -1 , 1 / 4000 , 80 , 80 + 1 , 1 , type::lowpass };
            m_cf  = 0;
            m_out = 0;
            m_wind.init( m_sp.N  );
        }

        /*! \brief  virtual destructor */
        virtual ~fir_abstract()
        {
            deallocate();
            #ifdef DEBUGGING
            printf( "fir_abstract destructor call \n" );
            #endif
        }

        /*!
         *  \brief  frequency response computation function
         *  \param[F] input frequency
         *  \return the function returns fr data structure
        */
        inline fr< __fx64 > freq_resp( __fx64 F ) { return __freq_resp__< __fx64 , __type >( m_sp.Fs , F , m_sp.order , m_cf ); }

        /*!
         *  \brief  FIR pulse response getting function
         *  \param[F] pulse response sample number
         *  \return the function returns FIR impulse response sample
        */
        inline __type get_coeff( __ix32 n ) { return ( n <= m_sp.order ) ? m_cf[ n ] : 1e6; }

        /*!
         *  \brief  32-bit FIR filter buffer filling function
         *  \param[input] pointer to the input data array
         *  \return the function returns FIR filtering result
        */
        inline void fill_fir_buff( __fx32 *input ) { m_bx( input ); }

        /*!
         *  \brief  32-bit FIR filter buffer filling function
         *  \param[input] pointer to the input data array
         *  \return the function returns FIR filtering result
        */
        inline void fill_fir_buff( __fx64 *input ) { m_bx( input ); }

        /*!
         *  \brief  32-bit FIR filtering function
         *  \param[input] pointer to the input data array
         *  \return the function returns FIR filtering result
        */
        inline __type filt( __fx32 *input )
        {
            m_bx( input );
            m_out = 0;
            for ( __ix32 n = m_sp.order ; n >= 0; n--) m_out += m_bx[ n ] * m_cf[n];
            return m_out;
        }

        /*!
         *  \brief  64-bit FIR filtering function
         *  \param[input] pointer to the input data array
         *  \return the function returns FIR filtering result
        */
        inline __type filt( __fx64 *input )
        {
            m_bx( input );
            m_out = 0;
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
         *  \brief 32-bit FIR filtering () operator
         *  \param[input] pointer to the input data
         *  \return the () operator calls filt( __type *input ) function that returns FIR filtering result
        */
        inline __type operator() ( __fx32 *input ) { return filt( input ); }

        /*!
         *  \brief 32-bit FIR filtering () operator
         *  \param[input] pointer to the input data
         *  \return the () operator calls filt( __type *input ) function that returns FIR filtering result
        */
        inline __type operator() ( __fx64 *input ) { return filt( input ); }

        #ifndef __ALG_PLATFORM
        void show_properties() { __show_fir__< __type >( m_sp , m_cf , m_wind ); }
        #endif
    };

    /*! \brief template comb filter abstract class ( base ) */
    template < typename T > class fcomb_abstract
    {
        typedef T __type ;
    protected:

        /*! \brief input signal nominal frequency  , Hz */
        __fx64 m_Fn;
        /*! \brief input signal sampling frequency , Hz */
        __fx64 m_Fs;
        /*! \brief input signal nominal period     , s */
        __fx64 m_Ts;
        /*! \brief comb filter order */
        __ix32 m_order;

        /*! \brief comb filter buffer */
         mirror_ring_buffer< __type > m_bx;

    public:

        /*! \brief filter output */
        __fx64 m_out;
        /*! \brief filter frequency phase response */
        __fx64 m_pH;
        /*! \brief filter frequency amplitude response */
        __fx64 m_Km;

        /*!
         *  \brief comb filter initialization function
         *  \param[Fs] - input signal sampling frequency
         *  \param[Fn] - input signal nominal frequency
        */
        __ix32 init( __fx64 Fs , __fx64 Fn )
        {
            m_Fs      = Fs;
            m_Fn      = Fn;
            m_Ts      = 1 / m_Fs;
            m_order   = m_Fs / m_Fn / 2;
            m_out     = 0;
            m_Km      = 0;
            m_pH      = 0;
            return 0;
        }

        /*! \brief memory allocation function */
        __ix32 allocate() { return m_bx.allocate( m_order + 1 ); }

        /*! \brief memory deallocation function */
        void deallocate() { m_bx.deallocate(); }

        /*! \brief default constructor */
        fcomb_abstract()
        {
            m_Fs      = 4000;
            m_Fn      = 50;
            m_Ts      = 1 / m_Fs;
            m_order   = m_Fs / m_Fn / 2;
            m_out     = 0;
            m_Km      = 0;
            m_pH      = 0;
        }

        /*! \brief default destructor */
        virtual ~fcomb_abstract()
        {
            deallocate();
            #ifdef DEBUGGING
            printf( "fcomb_abstract destructor call \n" );
            #endif
        }

        /*! \brief frequency response computation function */
        fr< __fx64 > freq_resp( __fx64 F )
        {
            __fx64 Re = 1 - cos(-6.283185307179586476925286766559  * m_order * F * m_Ts);
            __fx64 Im = 0 - sin(-6.283185307179586476925286766559  * m_order * F * m_Ts);
            m_pH = atan2(Im, Re);
            m_Km = sqrt(Re * Re + Im * Im) * 0.5;
            return { m_Km , m_pH };
        }

        /*!
         *  \brief 32-bit floating point filtering function
         *  \param[input] - pointer to the input signal samples buffer
         *  \return The function returns filtering result
        */
        inline __fx64 filt( __fx32 *input)
        {
            m_bx( input );
            return ( m_out = (__fx64)*input - (__fx64)m_bx[ m_order ] );
        }

        /*!
         *  \brief 64-bit floating point filtering function
         *  \param[input] - pointer to the input signal samples buffer
         *  \return The function returns filtering result
        */
        inline __fx64 filt( __fx64 *input)
        {
            m_bx( input );
            return ( m_out = *input - m_bx[ m_order ] );
        }

        /*!
         *  \brief 32-bit floating point filtering operator
         *  \param[input] - pointer to the input signal samples buffer
         *  \return The operatoe calls the function that returns filtering result
        */
        inline __fx64 operator ()( __fx32 *input ) { return filt(input); }

        /*!
         *  \brief 64-bit floating point filtering operator
         *  \param[input] - pointer to the input signal samples buffer
         *  \return The operatoe calls the function that returns filtering result
        */
        inline __fx64 operator ()( __fx64 *input ) { return filt(input); }
    };

    /*! \brief template equalized comb filter abstract class ( base ) */
    template < typename T > class fcombeq_abstract
    {
        typedef T __type ;
    private:

        /*! \brief input signal frequency deviation from nominal for which the amplitude frequency response slope compensation is implemented , Hz */
        __fx64 m_dF;
        /*! \brief residual amplitude frequency response slope for the input signal frequency deviation dF from nominal , p.u. */
        __fx64 m_d_Amp;
        /*! \brief input signal nominal frequency  , Hz */
        __fx64 m_Fn;
        /*! \brief input signal sampling frequency , Hz */
        __fx64 m_Fs;
        /*! \brief input signal sampling period , Hz */
        __fx64 m_Ts;
        /*! \brief auxiliary coefficient K1 */
        __fx64 m_K1;
        /*! \brief auxiliary coefficient K2 */
        __fx64 m_K2;
        /*! \brief filter order */
        __ix32 m_order;
        /*! \brief auxiliary coefficient ElemNum1 */
        __ix32 m_ElemNum1;
        /*! \brief auxiliary coefficient ElemNum2 */
        __ix32 m_ElemNum2;

        /*! \brief comb filter buffer */
         mirror_ring_buffer< __type > m_bx;

    public:

        /*! \brief filter output */
        __fx64 m_out;
        /*! \brief filter frequency phase response */
        __fx64 m_pH;
        /*! \brief filter frequency amplitude response */
        __fx64 m_Km;

        /*! \brief default constructor */
        fcombeq_abstract()
        {
            m_Fs       = 4000;
            m_Fn       = 50;
            m_dF       = 5;
            m_d_Amp    = 0;
            m_order    = m_Fs / m_Fn / 2;
            m_ElemNum1 = 1 * m_order;
            m_ElemNum2 = 2 * m_order;
            m_Ts       = 1 / m_Fs;
            m_out      = 0;
            m_Km       = 0;
            m_pH       = 0;
        }

        /*! \brief default destructor */
        virtual ~fcombeq_abstract()
        {
            deallocate();
            #ifdef DEBUGGING
            printf( "fcombeq_abstract destructor call \n" );
            #endif
        }

        /*!
         *  \brief comb filter initialization function
         *  \param[ Fs    ] - input signal sampling frequency
         *  \param[ Fn    ] - input signal nominal frequency
         *  \param[ dF    ] - input signal frequency deviation from nominal for which the amplitude frequency response slope compensation is implemented , Hz
         *  \param[ d_Amp ] - residual amplitude frequency response slope for the input signal frequency deviation dF from nominal , p.u.
        */
        void init( __fx64 Fs, __fx64 Fn, __fx64 dF, __fx64 d_Amp )
        {
            m_Fs       = Fs;
            m_Fn       = Fn;
            m_dF       = dF;
            m_d_Amp	   = d_Amp;
            m_order    = m_Fs / m_Fn / 2;
            m_ElemNum1 = 1 * m_order;
            m_ElemNum2 = 2 * m_order;
            m_Ts       = 1 / m_Fs;
            m_K1       = ( 1 + 0.19/0.5 );
            m_K2       =  0.19 / 0.5;
            m_out      = 0;
            m_Km       = 0;
            m_pH       = 0;
        }

        /*! \brief memory allocation function */
        __ix32 allocate()
        {
            // auxiliary coefficients:
            __fx64 A =  0.5 - 0.5 * cos(-6.283185307179586476925286766559 * (m_Fn + m_dF) * (__fx64)m_order * m_Ts );
            __fx64 B =  1.0 - cos(-6.283185307179586476925286766559 * (m_Fn + m_dF) * (__fx64)m_order * 2.0 * m_Ts);
            __fx64 C = -0.5 * sin(-6.283185307179586476925286766559 * (m_Fn + m_dF) * (__fx64)m_order * m_Ts);
            __fx64 D = -sin(-6.283185307179586476925286766559 * (m_Fn + m_dF) * (__fx64)m_order * 2.0 * m_Ts);

            // square equation coefficients:
            __fx64 a  = 1;
            __fx64 b  = 2 * ( A * B + C * D ) / (B * B + D * D);
            __fx64 c  = (A * A + C * C - (1 + m_d_Amp / 100) * (1 + m_d_Amp / 100) ) / (B * B + D * D);

            // square equation solve:
            __fx64 discr  = b * b - 4 * a * c;
            __fx64 K1     = 0;
            __fx64 K2     = 0;

            if ( discr > 0 ) // discriminant check
            {
                // roots computation:
                K1 = ( -b - sqrt( discr ) ) * 0.5;
                K2 = ( -b + sqrt( discr ) ) * 0.5;

                // take the greates of the roots ( although, it does not matter which root you take... ):
                m_K1 = (1 + fmax(K1 , K2) / 0.5);
                m_K2 = fmax(K1, K2) / 0.5;
            }
            else // if discriminant is negative, then the amplitude frequency slope is not compensated
            {
                m_K1 = 1;
                m_K2 = 0;
            }

            return m_bx.allocate( m_ElemNum2 + 1 );;
        }

        /*! \brief memory deallocation function */
        void deallocate() { m_bx.deallocate(); }

        /*! \brief frequency response computation function */
        fr< __fx64 > freq_resp( __fx64 F , bool mode = 0 )
        {
            __fx64 Re  = 0 , Im  = 0;

            if( !mode ) // even filter
            {
                Re = m_K1 - cos(-PI2 * (__fx64)m_order * F * m_Ts) - m_K2 * cos(-PI2 * 2 * (__fx64)m_order * F * m_Ts);
                Im = 0    - sin(-PI2 * (__fx64)m_order * F * m_Ts) - m_K2 * sin(-PI2 * 2 * (__fx64)m_order * F * m_Ts);
            }
            else // odd filter
            {
                Re = m_K1 + cos(-PI2 * (__fx64)m_order * F * m_Ts) - m_K2 * cos(-PI2 * 2 * (__fx64)m_order * F * m_Ts);
                Im = 0    + sin(-PI2 * (__fx64)m_order * F * m_Ts) - m_K2 * sin(-PI2 * 2 * (__fx64)m_order * F * m_Ts);
            }

            m_pH = atan2(Im, Re);
            m_Km = sqrt(Re * Re + Im * Im);
            return { m_Km , m_pH };
        }

        /*!
         *  \brief 32-bit floating point filtering function
         *  \param[input] - pointer to the input signal samples buffer
         *  \return The function returns filtering result
        */
        inline __fx64 filt( __fx32 *input , bool odd = true)
        {
            m_bx(input);
            return ( m_out = ( odd ) ? ( (__fx64)*input * m_K1 - (__fx64)m_bx[m_ElemNum1] - (__fx64)m_bx[m_ElemNum2] * m_K2 ) : ( (__fx64)*input * m_K1 + (__fx64)m_bx[m_ElemNum1] - (__fx64)m_bx[m_ElemNum2] * m_K2 ) );
        }

        /*!
         *  \brief 64-bit floating point filtering function
         *  \param[input] - pointer to the input signal samples buffer
         *  \return The function returns filtering result
        */
        inline __fx64 filt( __fx64 *input , bool odd = true)
        {
            m_bx(input);
            return ( m_out = ( odd ) ? ( *input * m_K1 - m_bx[m_ElemNum1] - m_bx[m_ElemNum2] * m_K2 ) : ( *input * m_K1 + (__fx64)m_bx[m_ElemNum1] - m_bx[m_ElemNum2] * m_K2 ) );
        }

        /*!
         *  \brief 32-bit floating point filtering operator
         *  \param[input] - pointer to the input signal samples buffer
         *  \return The operatoe calls the function that returns filtering result
        */
        inline __fx64 operator ()( __fx32 *input , bool odd = true ) { return filt( input , odd ); }

        /*!
         *  \brief 64-bit floating point filtering operator
         *  \param[input] - pointer to the input signal samples buffer
         *  \return The operatoe calls the function that returns filtering result
        */
        inline __fx64 operator ()( __fx64 *input , bool odd = true ) { return filt( input , odd ); }
    };

    /*! \brief template recursive Fourier filter abstract class ( base ) */
    template < typename T > class recursive_fourier_abstract
    {
        typedef T __type;
    private:
        /*! \brief nominal input signal frequency , Hz */
        __fx64 m_Fn;
        /*! \brief input signal sampling frequency , Hz */
        __fx64 m_Fs;
        /*! \brief input signal sampling period , s */
        __fx64 m_Ts;
        /*! \brief recursive Fourier filter gain */
        __fx64 m_Gain;
        /*! \brief recursive Fourier filter buffer size */
        __fx64 m_Ns;
        /*! \brief computed harmonic number */
        __fx64 m_hnum;
        /*! \brief auxiliary variable */
        __fx64 m_a0;
        /*! \brief auxiliary variable */
        __fx64 m_Ks;
        /*! \brief auxiliary variable */
        __fx64 m_Kc;
        /*! \brief recursive Fourier filter order */
        __ix32 m_order;
        /*! \brief recursive Fourier filter buffer */
        mirror_ring_buffer<__type> m_buffer_sx;
    public:

        struct result { __fx64 re , im; };

         /*! \brief harmonic real output component */
        __fx64 m_a;
        /*! \brief harmonic imaginary output component */
        __fx64 m_b;
        /*! \brief recursive Fourier filter frequency amplitude response */
        __fx64 m_Km;
        /*! \brief recursive Fourier filter frequency phase response */
        __fx64 m_pH;

         /*! \brief  recursive Fourier filter memory allocation function
          *  \return the function allocates memory for the recursive Fourier filter buffer
        */
        __ix32  allocate() { return ( m_buffer_sx.allocate( m_order + 2 ) ); }

        /*! \brief  recursive Fourier filter memory deallocation function
         *  \return the function deallocates memory of the recursive Fourier buffer
       */
        void deallocate() { m_buffer_sx.deallocate(); }

        /*! \brief  recursive Fourier filter initialization function
         *  \param[Fn  ] - input signal nominal frequency  , Hz
         *  \param[Fs  ] - input signal sampling frequency , hz
         *  \param[hnum] - number of computed harmonic
         *  \return the function initializes recursive Fourier filter
       */
        void init( __type Fs , __type Fn , __ix32 hnum )
        {
            // system variables initialization:
            m_Fn      = Fn;
            m_Fs      = Fs;
            m_Ts      = 1 / m_Fs;
            m_Ns      = ceil(m_Fs / m_Fn);
            m_order   = m_Ns;
            m_Gain    = ( hnum == 0 ) ? ( 1.0 / m_Ns ) : ( 2.0 / m_Ns / sqrt(2) );
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

        /*! \brief  recursive Fourier filter default constructor */
        recursive_fourier_abstract()
        {
            // system variables initialization:
            m_Fn       = 50;
            m_Fs       = 4000;
            m_Ts       = 1 / m_Fs;
            m_Ns       = ceil(m_Fs / m_Fn);
            m_order    = m_Ns;
            m_Gain     = 2.0 / m_Ns / sqrt(2);
            m_hnum     = 1;
            m_Ks      = sin( PI2 * (__fx64)m_hnum / m_Ns );
            m_Kc      = cos( PI2 * (__fx64)m_hnum / m_Ns );

            // filter output initialization:
            m_a0 = 0;
            m_a  = 0;
            m_b  = 0;
            m_Km = 0;
            m_pH = 0;
        };

        /*! \brief  recursive Fourier filter initializing constructor
         *  \param[Fn  ] - input signal nominal frequency  , Hz
         *  \param[Fs  ] - input signal sampling frequency , hz
         *  \param[hnum] - number of computed harmonic
         *  \return This constructor calls initialization function
        */
        recursive_fourier_abstract( __type Fs , __type Fn , __ix32 hnum ) { init( Fs , Fn , hnum ); }

        /*! \brief  recursive Fourier filter default destructor */
        virtual ~recursive_fourier_abstract(){ deallocate(); }

        /*! \brief  32-bit recursive Fourier filter filtering function
         *  \param[input] - pointer to the nput signal frames
         *  \return The function computes real and imaginary harmonic component
        */
        inline result filt ( __fx32 *input )
        {
            m_buffer_sx.fill_buff( input );
            m_a0 = m_a + ( ( __fx64 )*input - ( __fx64 )m_buffer_sx[ m_order ] ) * m_Gain;
            m_a  = m_a0 * m_Kc - m_b * m_Ks;
            m_b  = m_a0 * m_Ks + m_b * m_Kc;
            return { m_a , m_b };
        }

        /*! \brief  64-bit recursive Fourier filter filtering function
         *  \param[input] - pointer to the nput signal frames
         *  \return The function computes real and imaginary harmonic component
        */
        inline result filt ( __fx64 *input )
        {
            m_buffer_sx.fill_buff( input );
            m_a0 = m_a + ( *input - m_buffer_sx[ m_order ] ) * m_Gain;
            m_a  = m_a0 * m_Kc - m_b * m_Ks;
            m_b  = m_a0 * m_Ks + m_b * m_Kc;
            return { m_a , m_b };
        }

        /*! \brief recursive Fourier filter frequency response computation function
         *  \param[F] - input signal frequency , Hz
         *  \return The function computes phase and amplitude frequency response for the signal having frequency F
        */
        fr< __fx64 > freq_resp( __type F )
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

            return { m_Km , m_pH };
        }

        /*! \brief  32-bit recursive Fourier filter filtering operator
         *  \param[input] - pointer to the nput signal frames
         *  \return The operator calls the function that computes real and imaginary harmonic component
        */
        inline result operator () ( __fx32 *input ){ return filt ( input ); }

        /*! \brief  64-bit recursive Fourier filter filtering operator
         *  \param[input] - pointer to the nput signal frames
         *  \return The operator calls the function that computes real and imaginary harmonic component
        */
        inline result operator () ( __fx64 *input ){ return filt ( input ); }
    };

    /*! \brief template recursive mean filter abstract class ( base ) */
    template < typename T > class recursive_mean_abstract
    {
        typedef __fx32 __type;
    private:
        /*! \brief nominal input signal frequency , Hz */
        __fx64 m_Fn;
        /*! \brief input signal sampling frequency , Hz */
        __fx64 m_Fs;
        /*! \brief input signal sampling period , s */
        __fx64 m_Ts;
        /*! \brief recursive mean filter gain */
        __fx64 m_Gain;
        /*! \brief recursive mean filter buffer size */
        __fx64 m_Ns;
        /*! \brief recursive mean filter order */
        __ix32 m_order;
        /*! \brief recursive mean filter buffer */
        mirror_ring_buffer<__type> m_buffer_sx;

    public:
        /*! \brief recursive mean filter output */
        __fx64 m_out;
        /*! \brief recursive mean filter frequency amplitude response */
        __fx64 m_Km ;
        /*! \brief recursive mean filter frequency amplitude response */
        __fx64 m_pH ;

        /*! \brief  recursive mean filter memory allocation function
         *  \return the function allocates memory for the recursive mean filter buffer
       */
        __ix32 allocate() { return ( m_buffer_sx.allocate( m_order + 2 ) ); }

        /*! \brief  recursive mean filter memory deallocation function
         *  \return the function deallocates memory of the recursive mean buffer
       */
        void deallocate() { m_buffer_sx.deallocate(); }

        /*! \brief  recursive mean filter initialization function
         *  \param[Fn  ]  - input signal nominal frequency  , Hz
         *  \param[Fs  ]  - input signal sampling frequency , hz
         *  \param[order] - recursive mean filter order
         *  \return the function initializes recursive mean filter
       */
        void init( __fx64 Fs , __fx64 Fn , __ix32 order )
        {
            m_Fs      = Fs;
            m_Fn      = Fn;
            m_order   = order;
            m_Ns      = m_order;
            m_Ts      = 1 / m_Fs;
            m_Gain    = 1 / m_Ns;
        }

        /*! \brief  recursive mean filter default constructor */
        recursive_mean_abstract()
        {
            m_Fs      = 4000;
            m_Fn      = 50;
            m_order   = m_Fs / m_Fn;
            m_Ns      = m_order;
            m_Ts      = 1 / m_Fs;
            m_Gain    = 1 / m_Ns;
        };

        /*! \brief  recursive Fourier filter initializing constructor
         *  \param[Fn  ]  - input signal nominal frequency  , Hz
         *  \param[Fs  ]  - input signal sampling frequency , hz
         *  \param[order] - recursive mean filter order
         *  \return This constructor calls initialization function
        */
        recursive_mean_abstract( __fx64 Fs , __fx64 Fn , __ix32 order ) { init( Fs , Fn , order ); }

        /*! \brief  recursive mean filter default destructor */
        virtual ~recursive_mean_abstract(){ deallocate(); }

        /*! \brief recursive mean filter frequency response computation function
         *  \param[F] - input signal frequency , Hz
         *  \return The function computes phase and amplitude frequency response for the signal having frequency F
        */
        fr< __fx64 > freq_resp( __fx64 F )
        {
            __fx64 Re_nom = 0 , Im_nom = 0 , Re_den = 0 , Im_den = 0 , Re = 0 , Im = 0;
            Re_nom = 1 - cos(-PI2 * F * m_Ns * m_Ts);
            Im_nom = 0 - sin(-PI2 * F * m_Ns * m_Ts);
            Re_den = 1 - cos(-PI2 * F * m_Ts);
            Im_den = 0 - sin(-PI2 * F * m_Ts);
            Re   = (Re_nom * Re_den + Im_nom * Im_den) / (Re_den * Re_den + Im_den * Im_den);
            Im   = (Re_den * Im_nom - Re_nom * Im_den) / (Re_den * Re_den + Im_den * Im_den);
            m_Km = m_Gain * sqrt(Re * Re + Im * Im);
            m_pH = atan2(Im , Re);
            return { m_Km , m_pH };
        }

        /*! \brief  32-bit recursive mean filter filtering function
         *  \param[input] - pointer to the nput signal frames
         *  \return The function computes recursive mean fiter output
        */
        inline __fx64 filt ( __fx32 *input )
        {
            m_buffer_sx.fill_buff( input );
            return (m_out = m_Gain * ( ( __fx64 )*input - ( __fx64 )m_buffer_sx[ m_order ] ) + m_out);
        }

        /*! \brief  64-bit recursive mean filter filtering function
         *  \param[input] - pointer to the nput signal frames
         *  \return The function computes recursive mean fiter output
        */
        inline __fx64 filt ( __fx64 *input )
        {
            m_buffer_sx.fill_buff( input );
            return (m_out = m_Gain * ( ( __fx64 )*input - ( __fx64 )m_buffer_sx[ m_order ] ) + m_out);
        }

        /*! \brief  32-bit recursive mean filter filtering operator
         *  \param[input] - pointer to the nput signal frames
         *  \return The operator calls the function that computes real and imaginary harmonic component
        */
        inline __fx64 operator () ( __fx32 *input ){ return filt ( input ); }

        /*! \brief  64-bit recursive mean filter filtering operator
         *  \param[input] - pointer to the nput signal frames
         *  \return The operator calls the function that computes real and imaginary harmonic component
        */
        inline __fx64 operator () ( __fx64 *input ){ return filt ( input ); }
    };

    /*! \brief template recursive rms filter abstract class ( base ) */
    template < typename T > class recursive_rms_abstract
    {
        typedef T __type;
    private:
        /*! \brief nominal input signal frequency , Hz */
        __fx64 m_Fn;
        /*! \brief input signal sampling frequency , Hz */
        __fx64 m_Fs;
        /*! \brief input signal sampling period , s */
        __fx64 m_Ts;
        /*! \brief recursive root mean square filter gain */
        __fx64 m_Gain;
         /*! \brief recursive root mean square filter buffer size */
        __fx64 m_Ns;
         /*! \brief auxiliary variable */
        __fx64 m_auxv;
        /*! \brief auxiliary variable */
        __fx64 m_y;
        /*! \brief recursive root mean square filter order */
        __ix32 m_order;
        /*! \brief recursive root mean square filter buffer */
        mirror_ring_buffer<__type> m_buffer_sx;

    public:
        /*! \brief recursive root mean square filter output */
        __fx64 m_out;
        /*! \brief recursive root mean square filter frequency amplitude response */
        __fx64 m_Km ;
        /*! \brief recursive root mean square filter frequency amplitude response */
        __fx64 m_pH ;

        /*! \brief  recursive root mean square filter memory allocation function
         *  \return the function allocates memory for the recursive rott mean square filter buffer
       */
        __ix32 allocate() { return ( m_buffer_sx.allocate( m_order + 2 ) ); }

        /*! \brief  recursive root mean square filter memory deallocation function
         *  \return the function deallocates memory for the recursive root mean square filter buffer
       */
        void deallocate() { m_buffer_sx.deallocate(); }

        /*! \brief  recursive root mean square filter initialization function
         *  \param[Fn  ]  - input signal nominal frequency  , Hz
         *  \param[Fs  ]  - input signal sampling frequency , hz
         *  \param[order] - recursive mean filter order
         *  \return the function initializes recursive root mean square filter
       */
        void init( __fx64 Fs , __fx64 Fn , __ix32 order )
        {
            m_Fs      = Fs;
            m_Fn      = Fn;
            m_order   = order;
            m_Ns      = m_order;
            m_Ts      = 1 / m_Fs;
            m_Gain    = 1 / m_Ns;
            m_y		  = 0;
            m_out     = 0;
        }

        /*! \brief  recursive root mean square filter default constructor */
        recursive_rms_abstract()
        {
            m_Fs      = 4000;
            m_Fn      = 50;
            m_order   = m_Fs / m_Fn;
            m_Ns      = m_order;
            m_Ts      = 1 / m_Fs;
            m_Gain    = 1 / m_Ns;
            m_y		  = 0;
            m_out     = 0;
        };

        /*! \brief  recursive root mean square filter initializing constructor
         *  \param[Fn  ]  - input signal nominal frequency  , Hz
         *  \param[Fs  ]  - input signal sampling frequency , hz
         *  \param[order] - recursive mean filter order
         *  \return This constructor calls initialization function
        */
        recursive_rms_abstract( __fx64 Fs , __fx64 Fn , __ix32 order ) { init( Fs , Fn , order ); }

        /*! \brief  recursive root mean square filter default destructor */
        virtual ~recursive_rms_abstract(){ deallocate(); }

        /*! \brief recursive root mean square filter frequency response computation function
         *  \param[F] - input signal frequency , Hz
         *  \return The function computes phase and amplitude frequency response for the signal having frequency F
        */
        fr< __fx64 > freq_resp( __fx64 F )
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
            return { m_Km , m_pH };
        }


        /*! \brief  32-bit recursive root mean square filter filtering function
         *  \param[input] - pointer to the nput signal frames
         *  \return The function computes recursive mean fiter output
        */
        inline __fx64 filt ( __fx32 *input )
        {
            m_auxv = (__fx64)( *input ) * (__fx64)( *input );
            m_buffer_sx.fill_buff( &m_auxv );
            m_y = m_Gain * ( m_auxv - (__fx64)m_buffer_sx[ m_order ] ) + m_y;
            return ( m_out =  ( m_y <= 0 ) ? 0 : sqrtf( m_y ) );
        }

        /*! \brief  64-bit recursive root mean square filter filtering function
         *  \param[input] - pointer to the nput signal frames
         *  \return The function computes recursive mean fiter output
        */
        inline __fx64 filt ( __fx64 *input )
        {
            m_auxv = (__fx64)( *input ) * (__fx64)( *input );
            m_buffer_sx.fill_buff( &m_auxv );
            m_y = m_Gain * ( m_auxv - (__fx64)m_buffer_sx[ m_order ] ) + m_y;
            return ( m_out =  ( m_y <= 0 ) ? 0 : sqrtf( m_y ) );
        }

        /*! \brief  32-bit recursive root mean square filter filtering operator
         *  \param[input] - pointer to the nput signal frames
         *  \return The operator calls the function that computes recursive mean fiter output
        */
        inline __fx32 operator () ( __fx32 *input ) { return filt ( input ); }

        /*! \brief  64-bit recursive root mean square filter filtering operator
         *  \param[input] - pointer to the nput signal frames
         *  \return The operator calls the function that computes recursive mean fiter output
        */
        inline __fx64 operator () ( __fx64 *input ) { return filt ( input ); }
    };

    /*! \brief template FIR filter class */
    template < typename T > class fir;

    /*! \brief template comb FIR filter class */
    template < typename T > class fcomb;

    /*! \brief template comb FIR filter class */
    template < typename T > class fcombeq;

    /*! \brief template recursive Fourier filter class */
    template < typename T > class recursive_fourier;

    /*! \brief template recursive mean filter class */
    template < typename T > class recursive_mean;

    /*! \brief template recursive rms filter class */
    template < typename T > class recursive_rms;

    /*! \brief FIR filter class 32-bit realization */
    template <> class fir < __fx32 > : public fir_abstract< __fx32 >
    {
        typedef __fx32 __type ;
    public:
         fir() : fir_abstract(){}
        ~fir()
         {
            #ifdef DEBUGGING
            printf( "fir child destructor call \n" );
            #endif
         }
    };

    /*! \brief FIR filter class 32-bit realization */
    template <> class fir < __fx64 > : public fir_abstract< __fx64 >
    {
        typedef __fx64 __type ;
    public:
         fir() : fir_abstract(){}
        ~fir()
         {
            #ifdef DEBUGGING
            printf( "fir child destructor call \n" );
            #endif
         }
    };

    /*! \brief comb FIR filter class 32-bit realization */
    template <> class fcomb < __fx32 > : public fcomb_abstract< __fx32 >
    {
        typedef __fx32 __type ;
    public:
         fcomb() : fcomb_abstract(){}
        ~fcomb()
         {
            #ifdef DEBUGGING
            printf( "fcomb child destructor call \n" );
            #endif
         }
    };

    /*! \brief comb FIR filter class 32-bit realization */
    template <> class fcomb < __fx64 > : public fcomb_abstract< __fx64 >
    {
        typedef __fx64 __type ;
    public:
         fcomb() : fcomb_abstract(){}
        ~fcomb()
         {
            #ifdef DEBUGGING
            printf( "fcomb child destructor call \n" );
            #endif
         }
    };

    /*! \brief equalized comb FIR filter class 32-bit realization */
    template <> class fcombeq < __fx32 > : public fcombeq_abstract< __fx32 >
    {
        typedef __fx32 __type ;
    public:
         fcombeq() : fcombeq_abstract(){}
        ~fcombeq()
         {
            #ifdef DEBUGGING
            printf( "fcombeq child destructor call \n" );
            #endif
         }
    };

    /*! \brief equalized comb FIR filter class 32-bit realization */
    template <> class fcombeq< __fx64 > : public fcombeq_abstract< __fx64 >
    {
        typedef __fx64 __type ;
    public:
         fcombeq() : fcombeq_abstract(){}
        ~fcombeq()
         {
            #ifdef DEBUGGING
            printf( "fcombeq child destructor call \n" );
            #endif
         }
    };

    /*! \brief recursive Fourier class 32-bit realization */
    template <> class recursive_fourier < __fx32 > : public recursive_fourier_abstract < __fx32 >
    {
        typedef __fx32 __type ;
    public:
         recursive_fourier() : recursive_fourier_abstract() {}
        ~recursive_fourier() {}
    };

    /*! \brief recursive Fourier class 64-bit realization */
    template <> class recursive_fourier < __fx64 > : public recursive_fourier_abstract < __fx64 >
    {
        typedef __fx64 __type ;
    public:
         recursive_fourier() : recursive_fourier_abstract() {}
        ~recursive_fourier() {}
    };

    /*! \brief recursive mean class 32-bit realization */
    template <> class recursive_mean < __fx32 > : public recursive_mean_abstract < __fx32 >
    {
        typedef __fx32 __type ;
    public:
         recursive_mean() : recursive_mean_abstract() {}
        ~recursive_mean() {}
    };

    /*! \brief recursive mean class 64-bit realization */
    template <> class recursive_mean < __fx64 > : public recursive_mean_abstract < __fx64 >
    {
        typedef __fx64 __type ;
    public:
         recursive_mean() : recursive_mean_abstract() {}
        ~recursive_mean() {}
    };

    /*! \brief recursive rms class 32-bit realization */
    template <> class recursive_rms < __fx32 > : public recursive_rms_abstract < __fx32 >
    {
        typedef __fx32 __type ;
    public:
         recursive_rms() : recursive_rms_abstract() {}
        ~recursive_rms() {}
    };

    /*! \brief recursive rms class 32-bit realization */
    template <> class recursive_rms < __fx64 > : public recursive_rms_abstract < __fx64 >
    {
        typedef __fx64 __type ;
    public:
         recursive_rms() : recursive_rms_abstract() {}
        ~recursive_rms() {}
    };

};
/*! @} */

#undef __fx32
#undef __fx64
#undef __ix32
#undef __fxx64

// customized pi undef:
#undef PI0
#undef PI2
#undef PI_2
#undef PI_4
#undef DEBUGGING

#endif
