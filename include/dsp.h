#ifndef DSP_H
#define DSP_H

#ifndef __ALG_PLATFORM
#include "cmath"
#include "math.h"
#include <iostream>
#include <cstring>
#endif

#include "buffer.h"
#include "fcomplex.h"
#include "special_functions.h"

/*! \defgroup <DSPB> ( Digital Signal Processing basics )
 *  \brief The module contains basic functions and classes for Digital Signal Processing
    @{
*/

#define DEBUG

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

namespace DSP
{

    /*! \defgroup <DSPB_Elementary_Types> ( Digital Signal Processing basics elemetary types and enumerations )
     *  \ingroup DSPB
     *  \brief The module contains basic Digital Signal Processing elemetary types and enumerations
        @{
    */

    /*!
     *  \brief filter passband/stopband attenuation data structure
     *  \param[Fc] filter passband/stopband frequency, Hz
     *  \param[BW] filter passband/stopband width, Hz
    */
    struct bandwidth
    {
        __fx64 Fc;
        __fx64 BW;
    };

    /*!
     *  \brief filter passband/stopband attenuation data structure
     *  \param[G1] filter passband attenuation, Db
     *  \param[G2] filter stopband attenuation, Db
    */
    struct attenuation
    {
        __fx64 G1;
        __fx64 G2;
    };

    /*!
     *  \brief harmonic data structure
     *  \param[re] harmonic real component
     *  \param[im] harmonic imaginary component
     *  \param[df] harmonic frequency deviation from nominal, Hz
     *  \param[f ] harmonic nominal/computed frequency      , Hz
    */
    struct harmonic
    {
        __fx64 re;
        __fx64 im;
        __fx64 df;
        __fx64 f;
    };

    /*! \brief filter type enumeration */
    enum filter_type
    {
        lowpass,
        highpass,
        bandpass,
        bandstop
    };

    /*!
     * \brief IIR filter lowpass normalized analogue prototype zeros/poles plain
     * \param[ plp ] lowpass normalized analogue prototype complex conjugate poles pairs
     * \param[ zlp ] lowpass normalized analogue prototype complex conjugate zeros pairs
     * \param[ glp ] lowpass normalized analogue prototype gains vector
     * \param[ L     ] IIR filter complex conjugate poles/zeros pairs number
     * \param[ R     ] IIR filter real odd pole existance flag
     * \param[ N     ] IIR filter order
    */
    template< typename __type > struct zp
    {
        fcomplex< __type > *plp;
        fcomplex< __type > *zlp;
        fcomplex< __type > *glp;
        __ix32 L;
        __ix32 R;
        __ix32 N;
    };

    /*!
     * \brief IIR filter coefficients matrix data structure
     * \param[ cfnum ] IIR filter numerator quadratic sections coefficients matrix
     * \param[ cfden ] IIR filter denominator quadratic sections coefficients matrix
     * \param[ gains ] IIR filter quadratic sections gains vector
     * \param[ L     ] IIR filter complex conjugate poles/zeros pairs number
     * \param[ R     ] IIR filter real odd pole existance flag
     * \param[ N     ] IIR filter order
    */
    template< typename __type > struct cf
    {
        __type *cfnum;
        __type *cfden;
        __type *gains;
        __ix32 L;
        __ix32 R;
        __ix32 N;
    };

    /*!
     * \brief Filter frequency response data structure
     * \param[Km] amplitude frequency response, p.u (Db)
     * \param[pH] phase frequency response, degrees
    */
    struct fr
    {
        __fx64 Km;
        __fx64 pH;
    };

    /*! @} */


    /*! \defgroup <DSPB_UtilityFunctions> ( Digital Signal Processing basics utility functions )
     *  \ingroup DSPB
     *  \brief The module contains basic Digital Signal Processing utility functions
        @{
    */

    /*!
     * \brief IIR filter quadratic sections coefficiets matrix memory clean
     * \param[ cfmatrix ] IIR filter quadratic sections coefficiets matrix data structure
     * \return The function cleans the memory of IIR filter quadratic sections coefficiets matrix
    */
    template<typename __type> cf<__type> __cf_free__( cf<__type> cfmatrix )
    {
        if( cfmatrix.cfden != nullptr ) free( cfmatrix.cfden );
        if( cfmatrix.cfnum != nullptr ) free( cfmatrix.cfnum );
        if( cfmatrix.gains != nullptr ) free( cfmatrix.gains );
        return { nullptr , nullptr , nullptr , -1 , -1 , -1 };
    }

    /*!
     * \brief Frequency response computation function
     * \param[ cfnum ] IIR filter numerator quadratic sections coefficients matrix
     * \param[ cfden ] IIR filter denominator quadratic sections coefficients matrix
     * \param[ gains ] IIR filter quadratic sections gains vector
     * \param[ N     ] IIR filter filter order
     * \param[ Fs    ] sampling frequency, Hz
     * \param[ F     ] input signal frequency, Hz
     * \return The function computes IIR filter transfer function frequency response
    */
    template<typename __type> fr __freq_resp__
    (
            __type *cfnum ,
            __type *cfden ,
            __type *gains ,
            __ix32 N ,
            __fx64 Fs ,
            __fx64 F
    )
    {
        // check the input:
        if( !cfnum || !cfden || !gains ) return { -1 , -1 };

        // sampling period:
        __fx64 Ts = 1 / Fs;

        // transfer function initialization:
        fcomplex< __fx64 > tsf = fcomplex< __fx64 >( 1, 0 );

        for( __ix32 i = 0 ; i < N ; i++ )
        {
            // fcomplex exponents:
            fcomplex< __fx64 > z0 = fcomplex<__fx64>( cos( -PI2 * F * Ts * 0 ) , sin( -PI2 * F * Ts * 0 ) );
            fcomplex< __fx64 > z1 = fcomplex<__fx64>( cos( -PI2 * F * Ts * 1 ) , sin( -PI2 * F * Ts * 1 ) );
            fcomplex< __fx64 > z2 = fcomplex<__fx64>( cos( -PI2 * F * Ts * 2 ) , sin( -PI2 * F * Ts * 2 ) );

            // transfer function:
            fcomplex< __fx64 > num =  z0 * (__fx64)cfnum[ 3 * i + 0 ] + z1 * (__fx64)cfnum[ 3 * i + 1 ] + z2 * (__fx64)cfnum[ 3 * i + 2 ];
            fcomplex< __fx64 > den =  z0 * (__fx64)cfden[ 3 * i + 0 ] + z1 * (__fx64)cfden[ 3 * i + 1 ] + z2 * (__fx64)cfden[ 3 * i + 2 ];
            tsf *= num / den * (__fx64)gains[i];
        }

        // multiply by an output gain:
        tsf *= (__fx64)gains[N];

        // output:
        return fr{ __cabsf__<__fx64>( tsf ) , __cargf__<__fx64>( tsf ) };
    }

    template< typename __type > fr __freq_resp__
    (
            __type *_cfnum,
            __type *_cfden,
            __type  _gain,
            __ix32 _N,
            __fx64 _Fs,
            __fx64 _F
    )
    {
        fcomplex<__fx64> num(0,0), den(0,0), Wz (0,0);
        for ( __ix32 i = 0; i <_N; i++)
        {
            num += fcomplex<__fx64>( cos(-PI2 * _F * (__fx64)i / _Fs) * _cfnum[i], sin(-PI2 * _F * (__fx64)i / _Fs) * _cfnum[i] );
            den += fcomplex<__fx64>( cos(-PI2 * _F * (__fx64)i / _Fs) * _cfden[i], sin(-PI2 * _F * (__fx64)i / _Fs) * _cfden[i] );
        }
        Wz = num / den * (__fx64)_gain;
        return { __cabsf__<__fx64>( Wz ) , __cargf__<__fx64>( Wz ) };
    }

    /*!
     * \brief Frequency response computation function
     * \param[ cfbuff ] FIR filter coefficients buffer
     * \param[ N      ] FIR filter order
     * \param[ Fs     ] sampling frequency
     * \param[ F      ] input signal frequency
     * \return   The function computes FIR filter transfer function frequency response
    */
    template< typename __type > fr __freq_resp__
    (
            __type *cfbuff ,
            __ix32 N ,
            __fx64 Fs ,
            __fx64 F
    )
    {
        __type Re = 0 , Im = 0 , Ts = 1 / Fs;
        for ( __ix32 i = 0; i < N; i++)
        {
            Re = Re + cos(-PI2 * i * F * Ts) * cfbuff[ i ];
            Im = Im + sin(-PI2 * i * F * Ts) * cfbuff[ i ];
        }
        return { sqrt (Re * Re + Im * Im) , atan2( Im , Re ) };
    }

    /*!
     * \brief IIR filter filtering function
     * \param[ *input   ] pointer to the input signal samples buffer
     * \param[ *cfnum   ] IIR filter numerator quadratic sections coefficients matrix
     * \param[ *cfden   ] IIR filter denominator quadratic sections coefficients matrix
     * \param[ *gains   ] IIR filter quadratic sections gains vector
     * \param[ N        ] IIR filter filter order
     * \param[ *buff_sx ] IIR filter quadratic sections input  buffers vector
     * \param[ *buff_sy ] IIR filter quadratic sections output buffers vector
     * \return   The function implements input signal filtering using
     *           IIR filter coefficients matrix, gains and I/O buffers vectors
    */
    template< typename __type > inline __attribute__( (always_inline) ) __type __filt__
    (
            __type *input,
            __type *cfnum,
            __type *cfden,
            __type *gains,
            __ix32 N,
            delay< __type > *buff_sx,
            delay< __type > *buff_sy
    )
    {
        if( !cfnum || !cfden || !gains || !input || !buff_sx || !buff_sy || ( N < 0 ) ) return 0;

        // initialization:
        __type sum_num = 0;
        __type sum_den = 0;
        __type out     = 0;

        // filtering:
        buff_sx[0]( input );
        for ( __ix32 i = 0 ; i < N ; i++)
        {
            sum_num = gains[i]*( buff_sx[ i ][ 0 ] * cfnum[ 3 * i + 0 ] + buff_sx[ i ][ 1 ] * cfnum[ 3 * i + 1 ] + buff_sx[ i ][ 2 ] * cfnum[ 3 * i + 2 ] );
            sum_den = ( buff_sy[ i ][ 0 ] * cfden[ 3 * i + 1 ] + buff_sy[ i ][ 1 ] * cfden[ 3 * i + 2 ] );
            buff_sy[ i ]( &( out = sum_num - sum_den ) );
            if( i < N - 1 ) buff_sx[ i + 1 ]( &out );
        }
        return out;
    }

    /*!
     * \brief Unrwapped IIR filter filtering function
     * \param[ *input   ] pointer to the input signal samples buffer
     * \param[ *cfnum   ] IIR filter numerator quadratic sections coefficients vector
     * \param[ *cfden   ] IIR filter denominator quadratic sections coefficients vector
     * \param[ *gains   ] IIR filter quadratic sections gains vector
     * \param[ N        ] IIR filter filter order
     * \param[ *buff_sx ] IIR filter quadratic sections input  buffers vector
     * \param[ *buff_sy ] IIR filter quadratic sections output buffers vector
     * \return   The function implements input signal filtering using
     *           IIR filter coefficients, gains and I/O buffers vectors
    */
    template< typename __type > inline __attribute__( (always_inline) ) __type __filt__
    (
            __type *input ,
            __type *cfnum ,
            __type *cfden ,
            __type gain ,
            __ix32 Nx ,
            __ix32 Ny ,
            delay<__type> &bx ,
            delay<__type> &by
    )
    {
        __type sum_num = 0 , sum_den = 0 , out = 0;
        bx( input );
        for ( __ix32 m = 0 ; m < Nx ; m++)
        {
            sum_num += gain * bx[m] * cfnum[m];
            if ( m < Ny ) sum_den += by[m] * cfden[m + 1];
        }
        by( &( out = sum_num - sum_den ) );
        return out;
    }

    /*!
     * \brief FIR filter filtering function
     * \param[ *input   ] pointer to the input signal samples buffer
     * \param[ *coeffs  ] FIR filter coefficitns vector
     * \param[ *buff_sx ] FIR filter buffer
     * \param[ N        ] FIR filter filter order
     * \return   The function implements input signal filtering using
     *           FIR filter coefficients vector and buffer
    */
    template< typename CoefficientsType , typename BufferType > inline  BufferType __filt__
    (
            CoefficientsType    *input,
            BufferType          *coeffs,
            delay< BufferType > &buff_sx,
            __ix32               N
    )
    {
        buff_sx( input );
        BufferType out = 0;
        for ( __ix32 n = N-1 ; n >= 0; n--) out += buff_sx[ n ] * coeffs[n];
        return out;
    }

    /*!
     * \brief FIR filter filtering function
     * \param[ *coeffs  ] FIR filter coefficitns vector
     * \param[ *buff_sx ] FIR filter buffer
     * \param[ N        ] FIR filter filter order
     * \return   The function implements input signal filtering using
     *           FIR filter coefficients vector and ALREADY FILLED buffer
    */
    template< typename __type > inline  __type __filt__
    (
            __type          *coeffs ,
            delay< __type > &buff_sx ,
            __ix32 N
    )
    {
        __type out = 0;
        for ( __ix32 n = N-1 ; n >= 0; n--) out += buff_sx[ n ] * coeffs[n];
        return out;
    }

    #ifndef __ALG_PLATFORM
    /*!
     *  \brief FIR filter display function
     *  \param[ _coeffs ] FIR filter coefficients vector
     *  \param[ _N      ] FIR filter order
     *  \param[ _type   ] FIR filter type (lowpass, highpass, bandpass )
     */
    template<typename __type> void __show__(__type _coeffs, __ix32 _N, filter_type _type )
    {
        if( _coeffs )
        {
            switch ( _type )
            {
                case filter_type::lowpass:
                printf("Lowapass coefficients vector:\n");
                break;

                case filter_type::highpass:
                printf("Highpass coefficients vector:\n");
                break;

                case filter_type::bandpass:
                printf("BandPass coefficients vector:\n");
                break;

                case filter_type::bandstop:
                printf("BandStop coefficients vector:\n");
                break;
            }
            for( __ix32 i = 0 ; i <= _N ; i++ ) printf("a[%d] = %.12f \n" , i , _coeffs[i]);
        }
        else
        {
            printf("The filter has been destryed or it's coeffcients have not been computed yet...\n");
        }
    }

    /*!
     * \brief             IIR filter coefficients show function
     * \param[coeffs  ] - IIR coefficients data structure
    */
    template < typename __type > void __show__( cf< __type > _coeffs, filter_type _type )
    {
        if( _coeffs.cfnum && _coeffs.cfden && _coeffs.gains )
        {
            switch ( _type )
            {
                case filter_type::lowpass:
                printf("Lowapass coefficients vector:\n");
                break;

                case filter_type::highpass:
                printf("Highpass coefficients vector:\n");
                break;

                case filter_type::bandpass:
                printf("BandPass coefficients vector:\n");
                break;

                case filter_type::bandstop:
                printf("BandStop coefficients vector:\n");
                break;
            }

            __ix32  L     = _coeffs.L;
            __ix32  R     = _coeffs.R;
            __ix32  N     = L + R;
            __fx64 *cfnum = _coeffs.cfnum;
            __fx64 *cfden = _coeffs.cfden;
            __fx64 *gains = _coeffs.gains;
            __fx64  fgain = 1;

            for( __ix32 i = 0 ; i < N ; i++ )
            {
                printf( "section: %d \n " , i + 1 );
                printf( "num: %.12f %.12f %.12f \n " , cfnum[ 3 * i + 0 ] , cfnum[ 3 * i + 1 ] , cfnum[ 3 * i + 2 ] );
                printf( "den: %.12f %.12f %.12f \n " , cfden[ 3 * i + 0 ] , cfden[ 3 * i + 1 ] , cfden[ 3 * i + 2 ] );
                printf( "gf0: %.12f \n\n " , gains[i] );
                fgain *= gains[i];
            }
            fgain *= gains[N];
            printf( "output gain = : %.12f \n " , gains[N] );
            printf( "filter gain = : %.12f \n " , fgain    );
        }
        else
        {

        }
    }

    #endif

    /*!
     * \brief Butterworth lowpass analogue prototype zeros/poles plain computation function
     * \param[g_stop] stopband attenuation , Db
     * \param[order]  filter order
     * \return   The function computes Butterworth zeros/poles pairs of the Butterworth lowpass
     *           analogue prototype. It also compute zero frequency gains. All the data is stored
     *           within zp data structure and returned.
    */
    template< typename T > zp< T > __butt_zeros_poles_plain__
    (
            __ix32 order,
            __fx64 g_stop
    )
    {
        // number of zeros, poles, coeffs:
        __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

        // stopband attenuation:
        T epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

        // memory allocation for the lowpass analogue prototype poles, zeros and gains:
        fcomplex<T> *plp = ( fcomplex<T>* ) calloc( N+0 , sizeof ( fcomplex<T> ) );
        fcomplex<T> *zlp = ( fcomplex<T>* ) calloc( N+0 , sizeof ( fcomplex<T> ) );
        fcomplex<T> *glp = ( fcomplex<T>* ) calloc( N+1 , sizeof ( fcomplex<T> ) );

        if( plp == 0 || zlp == 0 || glp == 0 )
        {
            free( plp );
            free( zlp );
            free( glp );
            return zp<T>{ 0 , 0 , 0 , -1 , -1 , -1 };
        }

        // fcomplex-conjugate pairs:
        T alpha = 0 , betta = 1 / sqrt( pow( epsilon_stop , 1 / order ) );

        for( __ix32 i = 0 ; i < L ; i++ )
        {
            alpha = (2 * (i + 1) - 1) * PI0 / (2 * order);
            plp[i] = fcomplex<T>( ( -1 / betta * sin(alpha) ) , ( +1 / betta * cos(alpha) ) );
            glp[i] = plp[i] * __conjf__( plp[i] ) ;
        }

        // real odd pole:
        if( R == 1 )
        {
            plp[ N - 1 ] = fcomplex< T >( ( -1 / betta ) , 0 );
            glp[ N - 1 ] = -__realf__<__fx64>( plp[ N - 1 ] );
        }

        // setting the output gain:
        glp[ N ] = 1;

        return zp< T > { plp , zlp , glp , L , R , N };
    }

    /*!
     * \brief Chebyshev I lowpass analogue prototype zeros/poles plain computation function
     * \param[g_stop] stopband attenuation , Db
     * \param[order]  filter order
     * \return   The function computes Chebyshev I zeros/poles pairs of the Butterworth lowpass
     *           analogue prototype. It also compute zero frequency gains. All the data is stored
     *           within zp data structure and returned.
    */
    template< typename T > zp< T > __cheb1_zeros_poles_plain__
    (
            __ix32 order,
            __fx64 g_stop
    )
    {
        // number of zeros, poles, coeffs:
        __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

        // stopband attenuation:
        T epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

        // memory allocation for the lowpass analogue prototype poles, zeros and gains:
        fcomplex< T >  *plp = ( fcomplex<T>* ) calloc( N   , sizeof ( fcomplex< T > ) );
        fcomplex< T >  *zlp = ( fcomplex<T>* ) calloc( N   , sizeof ( fcomplex< T > ) );
        fcomplex< T >  *glp = ( fcomplex<T>* ) calloc( N+1 , sizeof ( fcomplex< T > ) );

        if( plp == 0 || zlp == 0 || glp == 0 )
        {
            free( plp );
            free( zlp );
            free( glp );
            return zp< T >{ 0 , 0 , 0 , -1 , -1 , -1 };
        }

        // fcomplex conjugate pairs:
        T alpha = 0 , betta = asinh( 1 / epsilon_stop ) / order;
        for ( __ix32 i = 0; i < L; i++)
        {
            alpha = (2 * (i + 1) - 1) * PI0 / (2 * order);
            plp[i] = fcomplex< T >( -sin( alpha ) * sinh( betta ) , +cos( alpha ) * cosh( betta ) );
            glp[i] = plp[i] * __conjf__( plp[i] );
        }

        // real odd pole:
        if ( R == 1 )
        {
            plp[ N - 1 ] = fcomplex< T >( -sinh( betta ) , 0 );
            glp[ N - 1 ] = -__realf__<__fx64>( plp[ N - 1 ] );
        }

        // setting the output gain:
        glp[N] = ( R < 1 ) ? ( sqrt(1 / (1 + epsilon_stop * epsilon_stop) ) ) : 1;

        // end the computation and return the filter info:
        return zp< T >{ plp , zlp , glp , L , R , N };
    }

    /*!
     * \brief Chebyshev II lowpass analogue prototype zeros/poles plain computation function
     * \param[g_stop] stopband attenuation , Db
     * \param[order]  filter order
     * \return   The function computes Chebyshev II zeros/poles pairs of the Butterworth lowpass
     *           analogue prototype. It also compute zero frequency gains. All the data is stored
     *           within zp data structure and returned.
    */
    template< typename T > zp< T > __cheb2_zeros_poles_plain__
    (
            __ix32 order,
            __fx64 g_stop
    )
    {
        // stopband attenuation:
        __fx64 epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

        // identify the number of zeros and poles:
        __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

        // allocate zeros and poles arrays:
        fcomplex< T > *plp = ( fcomplex< T >* ) calloc( N   , sizeof ( fcomplex< T > ) );
        fcomplex< T > *zlp = ( fcomplex< T >* ) calloc( N   , sizeof ( fcomplex< T > ) );
        fcomplex< T > *glp = ( fcomplex< T >* ) calloc( N+1 , sizeof ( fcomplex< T > ) );

        if( plp == 0 || zlp == 0 || glp == 0 )
        {
            free( plp );
            free( zlp );
            free( glp );
            return zp< T >{ 0 , 0 , 0 , -1 , -1 , -1 };
        }

        // ZEROS AND POLES COMPUTATION:

        // fcomplex conjugate pairs:

        // auxiliary variables:
        T alpha , betta = asinh( epsilon_stop ) / order , re , im;

        // zeros , poles, gains computation:
        for ( __ix32 i = 0 ; i < L ; i++ )
        {
            alpha = (2 * ( i + 1 ) - 1) * PI0 / ( 2 * order );

            // zeros:
            zlp[i] = fcomplex< T >( 0 , 1 / cos( alpha ) );

            // poles:
            re = -( sin( alpha ) * sinh( betta ) ) / ( cos( alpha ) * cos( alpha ) * cosh( betta ) * cosh( betta ) + sin( alpha ) * sin( alpha ) * sinh( betta ) * sinh( betta ) );
            im = +( cos( alpha ) * cosh( betta ) ) / ( cos( alpha ) * cos( alpha ) * cosh( betta ) * cosh( betta ) + sin( alpha ) * sin( alpha ) * sinh( betta ) * sinh( betta ) );
            plp[i] = fcomplex< T >( re , im );

            // gains:
            glp[i] = ( zlp[i] * __conjf__( zlp[i] ) ) / ( plp[i] * __conjf__( plp[i] ) );
            glp[i] = fcomplex< T >( 1 , 0 ) / glp[i];
        }

        // real odd pole:
        if( R >= 1 )
        {
            plp[ N - 1 ] = fcomplex< T >( -1 / sinh( betta ) , 0 );
            glp[ N - 1 ] = -__realf__<__fx64>( plp[ N - 1 ] );
        }

        // set the output gain:
        glp[N] = 1;

        return zp< T >{ plp , zlp , glp , L , R , N };
    }


    /*!
     * \brief Elliptic lowpass analogue prototype zeros/poles plain computation function
     * \param[g_stop] stopband attenuation , Db
     * \param[order]  filter order
     * \return   The function computes Elliptic zeros/poles pairs of the Butterworth lowpass
     *           analogue prototype. It also compute zero frequency gains. All the data is stored
     *           within zp data structure and returned.
    */
    template< typename T > zp< T > __ellip_zeros_poles_plain__
    (
            __ix32 order,
            __fx64 g_pass,
            __fx64 g_stop
    )
    {
        // INITIALIZATION:

        // stopband and passband attenuation:
        __fx64 epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);
        __fx64 epsilon_pass = sqrt(pow(10, g_pass / 10) - 1);

        // identify the number of zeros and poles:
        __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

        // allocate zeros and poles arrays:
        fcomplex< T > *plp = ( fcomplex< T >* ) calloc( N   , sizeof ( fcomplex< T > ) );
        fcomplex< T > *zlp = ( fcomplex< T >* ) calloc( N   , sizeof ( fcomplex< T > ) );
        fcomplex< T > *glp = ( fcomplex< T >* ) calloc( N+1 , sizeof ( fcomplex< T > ) );

        if( plp == 0 || zlp == 0 || glp == 0 )
        {
            free( plp );
            free( zlp );
            free( glp );
            return zp< T >{ 0 , 0 , 0 , -1 , -1 , -1 };
        }

        // TRANSIENT SUPPRESSION FACTORS COMPUTATION:

        // auxiliary variables:
        T SN , KE;
        T Ke = epsilon_pass / epsilon_stop , Kp = 1;
        T m = sqrt( 1 - Ke * Ke ) , alpha , Kw;

        // transient suppression factors computation:
        for ( __ix32 i = 0; i < L; i++ )
        {
            alpha = (2 * ( i + 1 ) - 1) / ( ( T )order);
            KE = __ellip_k__( m );
            SN = __sn__( alpha * KE , m );
            Kp = Kp * SN*SN*SN*SN;
        }

        Kp = pow( m , order) * Kp;
        Kw = sqrt(1 - Kp * Kp);

        // ZEROS COMPUTATION:

        KE = __ellip_k__( Kw );
        if( order % 2 == 0 ) // even order filter
        {
            for ( __ix32 i = 0; i < L; i++)
            {
                alpha = (2 * i + 1) / ( ( T )order) * KE;
                SN = __sn__( alpha , Kw );
                zlp[i] = fcomplex< T >( 0 , 1 /( Kw * SN ) );
            }
        }
        else if( order % 2 != 0 ) // odd order filter
        {
            for ( __ix32 i = 0; i < L; i++)
            {
                alpha = (2 * i + 2) / ( ( T )order) * KE;
                SN = __sn__( alpha , Kw );
                zlp[i] = fcomplex< T >( 0 , 1 /( Kw * SN ) );
            }
        }

        // POLES COMPUTATION:
        T V0 = 0 , A = 0 , B = 0 , C = 0 , D = 0 , E = 0 , F = 0;
        V0 = -__ellip_k__( Kw ) * __isc__(1 / epsilon_pass, sqrt(1 - Ke * Ke)) / ( __ellip_k__( Ke ) * ( ( T ) order ) );
        KE =  __ellip_k__( Kw );

        // fcomplex conjugate pairs:
        for ( __ix32 i = 0; i < L; i++)
        {
            if( order % 2 == 0 ) // even order filter
            {
                alpha = (2 * i + 1) / ( ( T ) order ) * KE;
            }
            else // odd order filter
            {
                alpha = (2 * i + 2) / ( ( T ) order ) * KE;
            }

            // compute auxiliary variables:
            A = __cn__( alpha , Kw );
            B = __dn__( alpha , Kw );
            C = __sn__( V0, sqrt( 1 - Kw * Kw));
            D = sqrt(1 - C * C);
            E = B * B;
            F = C * C;

            // pole real part:
            __fx64 pre = A * B * C * D / (1 - E * F);

            // compute auxiliary variables:
            A = __sn__( alpha, Kw );
            B = __dn__( V0 , sqrt(1 - Kw * Kw ) );
            C = __dn__( alpha, Kw );
            D = __sn__( V0, sqrt( 1 - Kw * Kw ) );
            E = C * C;
            F = D * D;

            // pole imaginary part:
            __fx64 pim = A * B / (1 - E * F);

            // pole initialization:
            plp[i]( pre , pim );
        }

        // real odd pole:
        if( R >= 1 )
        {
            // compute auxiliary variables:
            A = __sn__( V0 , sqrt( 1 - Kw * Kw ) );
            B = __cn__( V0 , sqrt( 1 - Kw * Kw ) );
            C = A * A;

            // compute the real odd pole:
            plp[ N - 1 ] = fcomplex< T >( A * B / ( 1 - C )  , 0 );
        }

        // GAINS COMPUTATION:

        // fcomplex conjugate pairs:
        if( R >= 1 )
        {
            // fcomplex conjugate pairs:
            for ( __ix32 i = 0; i < L; i++)
            {
                glp[i] = ( zlp[i] * __conjf__( zlp[i] ) ) / ( plp[i] * __conjf__( plp[i] ) );
                glp[i] = fcomplex< T >( 1 , 0 ) / glp[i];
            }

            // real odd pole:
            glp[ N - 1 ] = -__realf__<__fx64>( plp[ N - 1 ] );

            // output gain:
            glp[N] = 1;
        }
        else
        {
            T a = sqrt( 1 / (1 + epsilon_stop * epsilon_stop) );
            glp[N] = pow ( a , 1 / ( ( T ) L ) );
            for ( __ix32 i = 0; i < L; i++) glp[i] = glp[N];
        }

        // output gain:
        glp[N] = 1;

        return zp< T >{ plp , zlp , glp , L , R , N };
    }

    // IIR coefficients computation functions:
    /*!
     * \brief Butterworth or Chebyshev I digital lowpass filter coefficients computation function
     * \param[Fs]     sampling frequency , Hz
     * \param[Fc]     cut-off frequency , Hz
     * \param[order]  filter order
     * \param[type]   filter type ( 0 - Butterworth , 1 - Chebyshev_I )
     * \param[g_stop] stopband attenuation , Db
     * \return   The function computes Butterworth or Chebyshev I digital lowpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */

    template < typename T > cf< T > __butt_cheb1_digital_lp__
    (
            __fx64 Fs,
            __fx64 Fc,
            __ix32 order,
            __ix32 type   = 0,
            __fx64 g_stop = 1
    )
    {
        // COMPUTE LOWPASS ANALOGUE PROTOTYPE ZEROS, POLES AND GAINS:
        zp< __fx64 > zp = ( !type ) ? __butt_zeros_poles_plain__< __fx64 >( order , g_stop ) : __cheb1_zeros_poles_plain__< __fx64 >( order , g_stop );

        // allocate zeros and poles arrays:
        fcomplex< __fx64 > *plp = zp.plp;
        fcomplex< __fx64 > *zlp = zp.zlp;
        fcomplex< __fx64 > *glp = zp.glp;
        __ix32 L = zp.L , R = zp.R , N = L + R;

        // frequency deformation coefficient:
        __fx64 K = tan( PI2 * Fc / 2 / Fs );

        // coefficients matrix computation:
        T *cfnum = ( T* )calloc( 3 * N , sizeof ( T ) );
        T *cfden = ( T* )calloc( 3 * N , sizeof ( T ) );
        T *gains = ( T* )calloc( N + 1 , sizeof ( T ) );

        if( cfnum == 0 || cfden == 0 || gains == 0 || plp == 0 || zlp == 0 || glp == 0 )
        {
            free( cfnum ); free( cfden ); free( gains );
            free( plp )  ; free( zlp )  ; free( glp );
            return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 };
        }

        // BILLINEAR LP-LP TRANSFORM:

        // fcomplex-conjugate pairs:
        for( __ix32 i = 0 ; i < L ; i++ )
        {
            // quadratic section gain:
            fcomplex< __fx64 > gain0 = glp[i];
            fcomplex< __fx64 > gain1 = fcomplex< __fx64 >( K * K , 0 );
            fcomplex< __fx64 > gain2 = ( fcomplex< __fx64 >( 1 , 0 ) - plp[i] * K ) * ( fcomplex< __fx64 >( 1 , 0 ) - __conjf__( plp[i] ) * K );
            gains[i] = __realf__( gain0 * gain1 / gain2 );

            // zeros and poles transformation:
            zlp[i] = fcomplex< __fx64 >( -1 , 0 );
            plp[i] = ( fcomplex< __fx64 >( 1 , 0 ) + plp[i] * K ) / ( fcomplex< __fx64 >( 1 , 0 ) - plp[i] * K );

            // quadratic section numerator coefficients:
            cfnum[ 3 * i + 0 ] = +1;
            cfnum[ 3 * i + 1 ] = +2;
            cfnum[ 3 * i + 2 ] = +1;

            // quadratic section denumerator coefficients:
            cfden[ 3 * i + 0 ] = 1;
            cfden[ 3 * i + 1 ] = -__realf__( plp[i] + __conjf__( plp[i] ) );
            cfden[ 3 * i + 2 ] = +__realf__( plp[i] * __conjf__( plp[i] ) );
        }

        // real odd pole:
        if ( R == 1 )
        {
            // quadratic section gain:
            fcomplex< __fx64 > gain0 = glp[N-1];
            fcomplex< __fx64 > gain1 = fcomplex< __fx64 >( K , 0 );
            fcomplex< __fx64 > gain2 = ( fcomplex< __fx64 >( 1 , 0 ) - plp[N-1] * K );
            gains[N-1] = __realf__( gain0 * gain1 / gain2 );

            // zeros and poles transformation:
            zlp[ N - 1 ] = fcomplex< __fx64 >( -1 , 0 );
            plp[ N - 1 ] = ( fcomplex< __fx64 >( 1 , 0 ) + plp[N-1] * K ) / ( fcomplex< __fx64 >( 1 , 0 ) - plp[N-1] * K );

            // real odd pole section coefficients computation:

            // numerator:
            cfnum[ 3 * ( N - 1 ) + 0 ] = 1;
            cfnum[ 3 * ( N - 1 ) + 1 ] = -__realf__( zlp[ N - 1 ] );
            cfnum[ 3 * ( N - 1 ) + 2 ] = 0;

            // denominator:
            cfden[ 3 * ( N - 1 ) + 0 ] = 1;
            cfden[ 3 * ( N - 1 ) + 1 ] = -__realf__( plp[ N - 1 ] );
            cfden[ 3 * ( N - 1 ) + 2 ] = 0;
        }

        // setting filter output gain:
        gains[N] = __realf__( glp[N] );

        // memory free:
        free( plp  );
        free( zlp  );
        free( glp  );

        return cf< T >{ cfnum , cfden , gains , L , R , N };
    }

    /*!
     * \brief Butterworth or Chebyshev I digital highpass filter coefficients computation function
     * \param[Fs]     sampling frequency , Hz
     * \param[Fp]     pass frequency , Hz
     * \param[order]  filter order
     * \param[type]   filter type ( 0 - Butterworth , 1 - Chebyshev_I )
     * \param[g_stop] stopband attenuation , Db
     * \return   The function computes Butterworth or Chebyshev I digital highpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */

    template < typename T > cf< T > __butt_cheb1_digital_hp__
    (
            __fx64 Fs,
            __fx64 Fp,
            __ix32 order,
            __ix32 type   = 0,
            __fx64 g_stop = 1
    )
    {
        // INITIALIZATION:
        zp < __fx64 > zp = ( !type ) ? __butt_zeros_poles_plain__< __fx64 >( order , g_stop ) : __cheb1_zeros_poles_plain__< __fx64 >( order , g_stop );
        fcomplex< __fx64 > *plp = zp.plp;
        fcomplex< __fx64 > *zlp = zp.zlp;
        fcomplex< __fx64 > *glp = zp.glp;
        __ix32 L = zp.L , R = zp.R , N = L + R;

        // frequency deformation coefficient:
        T w = tan( PI2 * Fp / 2 / Fs );

        // coefficients matrix computation:
        T *cfnum = ( T* )calloc( 3 * N , sizeof ( T ) );
        T *cfden = ( T* )calloc( 3 * N , sizeof ( T ) );
        T *gains = ( T* )calloc( N + 1 , sizeof ( T ) );

        if( cfnum == 0 || cfden == 0 || gains == 0 || plp == 0 || zlp == 0 || glp == 0 )
        {
            free( cfnum ); free( cfden ); free( gains );
            free( plp )  ; free( zlp )  ; free( glp );
            return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 };
        }

        // BILLINEAR LP-HP TRANSFORM:

        // fcomplex conjugate pairs:
        for ( __ix32 i = 0; i < L; i++)
        {
            // gains compputation:
            fcomplex< __fx64 > gain0 = glp[i];
            fcomplex< __fx64 > gain1 = ( fcomplex< __fx64 >( 1 , 0 ) - plp[i] / (__fx64)w ) * ( fcomplex< __fx64 >( 1 , 0 ) - __conjf__( plp[i] ) / (__fx64)w );
            gains[i] = __realf__( gain0 / gain1 / (__fx64)w / (__fx64)w );

            // zeros and poles transformation:
            zlp[i] = fcomplex< __fx64 >( 1 , 0 );
            plp[i] = ( fcomplex< __fx64 >( 1 , 0 ) + plp[i] / (__fx64)w ) / ( fcomplex< __fx64 >( 1 , 0 ) - plp[i] / (__fx64)w ) * (__fx64)(-1);

            // digital highpass coefficients computation:

            // numerator:
            cfnum[3*i+0] = +1;
            cfnum[3*i+1] = -2;
            cfnum[3*i+2] = +1;

            // denominator:
            cfden[3*i+0] = 1;
            cfden[3*i+1] = -__realf__( plp[i] + __conjf__(plp[i]) );
            cfden[3*i+2] = +__realf__( plp[i] * __conjf__(plp[i]) );
        }

        // real odd pole:
        if ( R == 1 )
        {
            // gains compputation:
            fcomplex< __fx64 > gain0 = glp[N-1];
            fcomplex< __fx64 > gain1 = ( fcomplex< __fx64 >( 1 , 0 ) - plp[N-1] / (__fx64)w );
            gains[N-1] = __realf__( gain0 / gain1 / (__fx64)w );

            // zeros and poles transformation:
            zlp[ N - 1 ] = fcomplex< __fx64 >( 1 , 0 );
            plp[ N - 1 ] = ( fcomplex< __fx64 >( 1 , 0 ) + plp[ N - 1 ] / (__fx64)w ) / ( fcomplex< __fx64 >( 1 , 0 ) - plp[ N - 1 ] / (__fx64)w ) * (__fx64)(-1);

            // digital highpass coefficients computation:

            // numerator:
            cfnum[3*(N-1)+0] = +1;
            cfnum[3*(N-1)+1] = -2;
            cfnum[3*(N-1)+2] = +1;

            // denominator:
            cfden[3*(N-1)+0] = 1;
            cfden[3*(N-1)+1] = -__realf__(plp[ N - 1 ]);
            cfden[3*(N-1)+2] = 0;
        }

        // setting filter output gain:
        gains[N] = __realf__(glp[N]);

        // memory free:
        free( plp  );
        free( zlp  );
        free( glp  );

        return cf< T >{ cfnum , cfden , gains , L , R , N };
    }

    /*!
     * \brief Butterworth or Chebyshev I digital bandpass filter coefficients computation function
     * \param[Fs]        sampling frequency , Hz
     * \param[Fp]        pass frequency , Hz
     * \param[BandWidth] passband width , Hz
     * \param[order]     filter order
     * \param[type]      filter type ( 0 - Butterworth , 1 - Chebyshev_I )
     * \param[g_stop]    stopband attenuation , Db
     * \return   The function computes Butterworth or Chebyshev I digital bandpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */

    template < typename T > cf<T> __butt_cheb1_digital_bp__
    (
            __fx64 Fs,
            __fx64 Fp,
            __fx64 BandWidth,
            __ix32 order,
            __ix32 type   = 0,
            __fx64 g_stop = 1
    )
    {
        order /= 2;

        // frequency deformation coefficient:
        __fx64 w1 = tan( PI2 * Fp / 2 / Fs ) , w2 = tan( PI2 * ( Fp + BandWidth ) / 2 / Fs );

        // allocate zeros and poles arrays:

        // lowpass analogue prototype poles, zeros and gains:
        zp < __fx64 > zp = ( !type ) ? __butt_zeros_poles_plain__< __fx64 >( order , g_stop ) : __cheb1_zeros_poles_plain__< __fx64 >( order , g_stop );
        fcomplex< __fx64 > *plp = zp.plp;
        fcomplex< __fx64 > *glp = zp.glp;
        fcomplex< __fx64 > *zlp = zp.zlp;
        __ix32 L = zp.L , R = zp.R , N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        fcomplex< __fx64 > *pbp = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *zbp = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *gbp = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );

        // coefficients matrix computation:
        T *cfnum = ( T* )calloc( 3 * (2*L+R) , sizeof ( T ) );
        T *cfden = ( T* )calloc( 3 * (2*L+R) , sizeof ( T ) );
        T *gains = ( T* )calloc( (2*L+R + 1) , sizeof ( T ) );

        if( cfnum == 0 || cfden == 0 || gains == 0 ||
            plp == 0 || zlp == 0 || glp == 0 ||
            pbp == 0 || zbp == 0 || gbp == 0)
        {
            free( cfnum ); free( cfden ); free( gains );
            free( plp )  ; free( zlp )  ; free( glp );
            free( pbp )  ; free( zbp )  ; free( gbp );
            return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 };
        }

        // LP-BP BILLINEAR TRANSFORM:

        // poles transformation:
        __fx64 w0 = w1 * w2;
        __fx64 dW = w2 - w1;

        // fcomplex conjugate pairs:
        __ix32 j = 0;
        for( __ix32 i = 0 ; i < L ; i++ , j+=2 )
        {
            // poles transformation by means of square equation solve:
            fcomplex< __fx64 > a( 1 / w0 , 0 );
            fcomplex< __fx64 > b( -__realf__(plp[i]) * dW / w0 , -__imagf__(plp[i]) * dW / w0 );
            fcomplex< __fx64 > c( 1 , 0 );
            fcomplex< __fx64 > D = b * b - a * c * (__fx64)4;
            fcomplex< __fx64 > p1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            fcomplex< __fx64 > p2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // zeros and poles bilinear transform:
            zbp[ j + 0 ] = fcomplex< __fx64 >( +1 , 0 );
            zbp[ j + 1 ] = fcomplex< __fx64 >( -1 , 0 );
            pbp[ j + 0 ] = ( fcomplex< __fx64 >( 1 , 0 ) + p1 ) / ( fcomplex< __fx64 >( 1 , 0 ) - p1 );
            pbp[ j + 1 ] = ( fcomplex< __fx64 >( 1 , 0 ) + p2 ) / ( fcomplex< __fx64 >( 1 , 0 ) - p2 );

            // digital filter coefficients computation:

            // numerator:
            cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
            cfnum[ 3 * ( j + 0 ) + 1 ] = 0;
            cfnum[ 3 * ( j + 0 ) + 2 ] = -1;
            cfnum[ 3 * ( j + 1 ) + 0 ] = 1;
            cfnum[ 3 * ( j + 1 ) + 1 ] = 0;
            cfnum[ 3 * ( j + 1 ) + 2 ] = -1;

            // denominator:
            cfden[ 3 * ( j + 0 ) + 0 ] = 1;
            cfden[ 3 * ( j + 0 ) + 1 ] = -__realf__( pbp[j] + __conjf__( pbp[j] ) );
            cfden[ 3 * ( j + 0 ) + 2 ] = +__realf__( pbp[j] * __conjf__( pbp[j] ) );
            cfden[ 3 * ( j + 1 ) + 0 ] = 1;
            cfden[ 3 * ( j + 1 ) + 1 ] = -__realf__( pbp[j+1] + __conjf__( pbp[j+1] ) );
            cfden[ 3 * ( j + 1 ) + 2 ] = +__realf__( pbp[j+1] * __conjf__( pbp[j+1] ) );

            // fcomplex conjugate quadratic sections gains computation:
            fcomplex< __fx64 > gain0 = glp[i];
            fcomplex< __fx64 > gain1 = p1 * __conjf__( p1 );
            fcomplex< __fx64 > gain2 = p2 * __conjf__( p2 );
            fcomplex< __fx64 > gain3 = (fcomplex< __fx64 >(1,0) - p1)*(fcomplex< __fx64 >(1,0) - __conjf__(p1));
            fcomplex< __fx64 > gain4 = (fcomplex< __fx64 >(1,0) - p2)*(fcomplex< __fx64 >(1,0) - __conjf__(p2));
            fcomplex< __fx64 > gain5 = gain0 * gain1 * gain2 / gain3 / gain4 * dW * dW / w0 / w0;
            gain5 = __csqrtf__( gain5 );
            gains[j+0] = __realf__(gain5);
            gains[j+1] = __realf__(gain5);
        }

        // real odd pole:
        if( R == 1 )
        {
            // pole transformation by means of square equation solve:
            fcomplex< __fx64 > a( 1 / w0 , 0 );
            fcomplex< __fx64 > b( -__realf__(plp[ N - 1 ]) * dW / w0 , -__realf__(plp[ N - 1 ]) * dW / w0 );
            fcomplex< __fx64 > c( 1 , 0 );
            fcomplex< __fx64 > D = b * b - a * c * (__fx64)4;
            fcomplex< __fx64 > p1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            fcomplex< __fx64 > p2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // zeros and poles bilinear transform:
            zbp[ j + 0 ] = fcomplex< __fx64 >( +1 , 0 );
            zbp[ j + 1 ] = fcomplex< __fx64 >( -1 , 0 );
            pbp[ j + 0 ] = ( fcomplex< __fx64 >( 1 , 0 ) + p1 ) / ( fcomplex< __fx64 >( 1 , 0 ) - p1 );
            pbp[ j + 1 ] = ( fcomplex< __fx64 >( 1 , 0 ) + p2 ) / ( fcomplex< __fx64 >( 1 , 0 ) - p2 );

            // digital filter coefficients computation:

            // numerator:
            cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
            cfnum[ 3 * ( j + 0 ) + 1 ] = 0;
            cfnum[ 3 * ( j + 0 ) + 2 ] = -1;

            // denominator:
            cfden[ 3 * ( j + 0 ) + 0 ] = 1;
            cfden[ 3 * ( j + 0 ) + 1 ] = -__realf__( pbp[j] + pbp[j+1] );
            cfden[ 3 * ( j + 0 ) + 2 ] = +__realf__( pbp[j] * pbp[j+1] );

            // fcomplex conjugate quadratic sections gains computation:
            fcomplex< __fx64 > gain0 = glp[N-1];
            fcomplex< __fx64 > gain1 = p1 * p2;
            fcomplex< __fx64 > gain2 = (fcomplex< __fx64 >(1,0) - p1)*(fcomplex< __fx64 >(1,0) - p2);
            fcomplex< __fx64 > gain3 = gain0 * gain1 / gain2 * dW / w0;
            gains[j+0] = __realf__(gain3);
        }

        // setting filter output gain:
        gains[ 2*L+R ] = __realf__(glp[N]);

        // memory free:
        free( plp );
        free( zlp );
        free( glp );
        free( pbp );
        free( zbp );
        free( gbp );

        return cf<  T  >{ cfnum , cfden , gains , 2*L , R , 2*L+R  };
    }

    /*!
     * \brief Butterworth or Chebyshev I digital bandstop filter coefficients computation function
     * \param[Fs]        sampling frequency , Hz
     * \param[Fc]        cut-off frequency  , Hz
     * \param[BandWidth] cut-off bandwidth  , Hz
     * \param[order]     filter order
     * \param[type]      filter type ( 0 - Butterworth , 1 - Chebyshev_I )
     * \param[g_stop]    stopband attenuation , Db
     * \return   The function computes Butterworth or Chebyshev I digital bandstop filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */

    template < typename T > cf<T> __butt_cheb1_digital_bs__
    (
            __fx64 Fs,
            __fx64 Fc,
            __fx64 BandWidth,
            __ix32 order,
            __fx32 type   = 0,
            __fx64 g_stop = 1
    )
    {

       // INITIALIZATION:
       order /= 2;

       // frequency deformation coefficient:
       __fx64 w1 = tan( PI2 * Fc / 2 / Fs ) , w2 = tan( PI2 * ( Fc + BandWidth ) / 2 / Fs );

       // allocate zeros and poles arrays:

       // lowpass analogue prototype poles, zeros and gains:
       zp < __fx64 > zp = ( !type ) ? __butt_zeros_poles_plain__< __fx64 >( order , g_stop ) : __cheb1_zeros_poles_plain__< __fx64 >( order , g_stop );
       fcomplex< __fx64 > *plp = zp.plp;
       fcomplex< __fx64 > *glp = zp.glp;
       fcomplex< __fx64 > *zlp = zp.zlp;
       __ix32 L = zp.L , R = zp.R , N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        fcomplex< __fx64 > *pbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *zbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *gbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );

        // coefficients matrix computation:
        T *cfnum = (T* )calloc( 3 * (2*L+R) , sizeof ( T ) );
        T *cfden = (T* )calloc( 3 * (2*L+R) , sizeof ( T ) );
        T *gains = (T* )calloc( (2*L+R+1)   , sizeof ( T ) );

        if( cfnum == 0 || cfden == 0 || gains == 0 ||
            plp == 0 || zlp == 0 || glp == 0 ||
            pbs == 0 || zbs == 0 || gbs == 0)
        {
            free( cfnum ); free( cfden ); free( gains );
            free( plp )  ; free( zlp )  ; free( glp );
            free( pbs )  ; free( zbs )  ; free( gbs );
            return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 };
        }

        // LP-BS BILLINEAR TRANSFORM:

        // poles transformation:
        __fx64 w0 = w1 * w2;
        __fx64 dW = w2 - w1;

        // fcomplex conjugate pairs:
        __ix32 j = 0;
        for( __ix32 i = 0 ; i < L ; i++ , j+=2 )
        {
            // poles transformation by means of square equation solve:
            fcomplex< __fx64 > a = fcomplex< __fx64 >( -1 / w0 , 0 );
            fcomplex< __fx64 > b = fcomplex< __fx64 >( dW , 0 ) / plp[i] / w0;
            fcomplex< __fx64 > c = fcomplex< __fx64 >( -1 , 0 );
            fcomplex< __fx64 > D = b * b - a * c * (__fx64)4;
            fcomplex< __fx64 > p1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            fcomplex< __fx64 > p2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // zeros and poles bilinear transform:
            zbs[ j + 0 ] = fcomplex< __fx64 >( +1 , 0 );
            zbs[ j + 1 ] = fcomplex< __fx64 >( -1 , 0 );
            pbs[ j + 0 ] = ( fcomplex< __fx64 >( 1 , 0 ) + p1 ) / ( fcomplex< __fx64 >( 1 , 0 ) - p1 );
            pbs[ j + 1 ] = ( fcomplex< __fx64 >( 1 , 0 ) + p2 ) / ( fcomplex< __fx64 >( 1 , 0 ) - p2 );

            // digital filter coefficients computation:

            // numerator:
            cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
            cfnum[ 3 * ( j + 0 ) + 1 ] = 2 * ( w0 - 1 ) / ( w0 + 1 );
            cfnum[ 3 * ( j + 0 ) + 2 ] = 1;
            cfnum[ 3 * ( j + 1 ) + 0 ] = 1;
            cfnum[ 3 * ( j + 1 ) + 1 ] = 2 * ( w0 - 1 ) / ( w0 + 1 );
            cfnum[ 3 * ( j + 1 ) + 2 ] = 1;

            // denominator:
            cfden[ 3 * ( j + 0 ) + 0 ] = 1;
            cfden[ 3 * ( j + 0 ) + 1 ] = -__realf__( pbs[j] + __conjf__( pbs[j] ) );
            cfden[ 3 * ( j + 0 ) + 2 ] = +__realf__( pbs[j] * __conjf__( pbs[j] ) );
            cfden[ 3 * ( j + 1 ) + 0 ] = 1;
            cfden[ 3 * ( j + 1 ) + 1 ] = -__realf__( pbs[j+1] + __conjf__( pbs[j+1] ) );
            cfden[ 3 * ( j + 1 ) + 2 ] = +__realf__( pbs[j+1] * __conjf__( pbs[j+1] ) );

            // fcomplex conjugate quadratic sections gains computation:
            fcomplex< __fx64 > gain0 = fcomplex< __fx64 >(1,0);
            fcomplex< __fx64 > gain1 = p1 * __conjf__( p1 );
            fcomplex< __fx64 > gain2 = p2 * __conjf__( p2 );
            fcomplex< __fx64 > gain3 = (fcomplex< __fx64 >(1,0) - p1)*(fcomplex< __fx64 >(1,0) - __conjf__(p1));
            fcomplex< __fx64 > gain4 = (fcomplex< __fx64 >(1,0) - p2)*(fcomplex< __fx64 >(1,0) - __conjf__(p2));
            fcomplex< __fx64 > gain5 = gain0 * gain1 * gain2 / gain3 / gain4 / w0 / w0 * ( 1 + w0 ) * ( 1 + w0 );
            gain5 = __csqrtf__( gain5 );
            gains[j+0] = __realf__(gain5);
            gains[j+1] = gains[j+0];
        }

        // real odd pole:
        if( R == 1 )
        {
            // poles transformation by means of square equation solve:
            fcomplex< __fx64 > a = fcomplex< __fx64 >( -1 / w0 , 0 );
            fcomplex< __fx64 > b = fcomplex< __fx64 >( dW , 0 ) / plp[N-1] / w0;
            fcomplex< __fx64 > c = fcomplex< __fx64 >( -1 , 0 );
            fcomplex< __fx64 > D = b * b - a * c * (__fx64)4;
            fcomplex< __fx64 > p1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            fcomplex< __fx64 > p2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // zeros and poles bilinear transform:
            zbs[ j + 0 ] = fcomplex< __fx64 >( +1 , 0 );
            zbs[ j + 1 ] = fcomplex< __fx64 >( -1 , 0 );
            pbs[ j + 0 ] = ( fcomplex< __fx64 >( 1 , 0 ) + p1 ) / ( fcomplex< __fx64 >( 1 , 0 ) - p1 );
            pbs[ j + 1 ] = ( fcomplex< __fx64 >( 1 , 0 ) + p2 ) / ( fcomplex< __fx64 >( 1 , 0 ) - p2 );

            // digital filter coefficients computation:

            // numerator:
            cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
            cfnum[ 3 * ( j + 0 ) + 1 ] = 2 * ( w0 - 1 ) / ( w0 + 1 );
            cfnum[ 3 * ( j + 0 ) + 2 ] = 1;

            // denominator:
            cfden[ 3 * ( j + 0 ) + 0 ] = 1;
            cfden[ 3 * ( j + 0 ) + 1 ] = -__realf__( pbs[j]+pbs[j+1] );
            cfden[ 3 * ( j + 0 ) + 2 ] = +__realf__( pbs[j]*pbs[j+1] );

            // fcomplex conjugate quadratic sections gains computation:
            fcomplex< __fx64 > gain0 = glp[N-1];
            fcomplex< __fx64 > gain1 = p1 * p2;
            fcomplex< __fx64 > gain2 = fcomplex< __fx64 >(1,0) / plp[N-1] / (__fx64)(-1);
            fcomplex< __fx64 > gain3 = (fcomplex< __fx64 >(1,0) - p1)*(fcomplex< __fx64 >(1,0) - p2);
            fcomplex< __fx64 > gain4 = gain0 * gain1 * gain2 / gain3 / w0 * ( 1 + w0 );
            gains[j+0] = __realf__(gain4);
        }

        // setting filter output gain:
        gains[ 2 * L + R ] = __realf__(glp[ N ]);

        // memory free:
        free( plp );
        free( zlp );
        free( glp );
        free( pbs );
        free( zbs );
        free( gbs );

        return cf< T >{ cfnum , cfden , gains , 2 * L , R , 2 * L + R };
    }


    /*!
     * \brief Chebyshev II or Elliptic digital lowpass filter coefficients computation function
     * \param[Fs]     sampling frequency , Hz
     * \param[Fc]     cut-off frequency , Hz
     * \param[order]  filter order
     * \param[type]   filter type ( 0 - Chebyshev II , 1 - Elliptic )
     * \param[g_stop] stopband attenuation , Db
     * \param[g_pass] passband attenuation , Db
     * \return   The function computes Chebyshev II or Elliptic digital lowpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */

    template < typename T > cf<T> __cheb2_ellip_digital_lp__
    (
            __fx64 Fs,
            __fx64 Fc,
            __ix32 order,
            __ix32 type   = 0,
            __fx64 g_pass = 1,
            __fx64 g_stop = 80
    )
    {
        __fx64 w = tan( PI2 * Fc / 2 / Fs );

        // digital lowpass coefficients computation:
        zp< __fx64 > zp = ( !type ) ? __cheb2_zeros_poles_plain__< __fx64 >( order , g_stop ) : __ellip_zeros_poles_plain__< __fx64 >( order , g_pass , g_stop );

        // zeros/poles and coefficients number:
        __ix32 L = zp.L;
        __ix32 R = zp.R;
        __ix32 N = zp.N;

        // zeros / poles plain initialization:
        fcomplex< __fx64 > *plp = zp.plp;
        fcomplex< __fx64 > *zlp = zp.zlp;
        fcomplex< __fx64 > *glp = zp.glp;

        // coefficients matrix computation:
         T  *cfnum = ( T * )calloc( 3 * N , sizeof ( T ) );
         T  *cfden = ( T * )calloc( 3 * N , sizeof ( T ) );
         T  *gains = ( T * )calloc( N + 1 , sizeof ( T ) );

        if( cfnum == 0 || cfden == 0 || gains == 0 ||
            plp == 0 || zlp == 0 || glp == 0 )
        {
            free( cfnum ); free( cfden ); free( gains );
            free( plp )  ; free( zlp )  ; free( glp );
            return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 };
        }

        // fcomplex conjugate pairs:
        for ( __ix32 i = 0 ; i < L ; i++)
        {
            // quadratic sections gains computation:
            fcomplex< __fx64 > gain0 = glp[i];
            fcomplex< __fx64 > gain1 = ( fcomplex< __fx64 >( 1 , 0 ) - zlp[i] * w ) * ( fcomplex< __fx64 >( 1 , 0 ) - __conjf__( zlp[i] ) * w );
            fcomplex< __fx64 > gain2 = ( fcomplex< __fx64 >( 1 , 0 ) - plp[i] * w ) * ( fcomplex< __fx64 >( 1 , 0 ) - __conjf__( plp[i] ) * w );
            gains[ i ] = __realf__( gain0 * gain1 / gain2 );

            // zeros and poles transformation:
            zlp[i] = ( fcomplex< __fx64 >( 1 , 0 ) + zlp[i] * w ) / ( fcomplex< __fx64 >( 1 , 0 ) - zlp[i] * w );
            plp[i] = ( fcomplex< __fx64 >( 1 , 0 ) + plp[i] * w ) / ( fcomplex< __fx64 >( 1 , 0 ) - plp[i] * w );

            // quadratic sections coefficients computation:

            // numerator:
            cfnum[ 3 * i + 0 ] = 1;
            cfnum[ 3 * i + 1 ] = -__realf__( zlp[i] + __conjf__<__fx64>( zlp[i] ) );
            cfnum[ 3 * i + 2 ] = +__realf__( zlp[i] * __conjf__( zlp[i] ) );

            // denominator:
            cfden[ 3 * i + 0 ] = 1;
            cfden[ 3 * i + 1 ] = -__realf__( plp[i] + __conjf__( plp[i] ) );
            cfden[ 3 * i + 2 ] = +__realf__( plp[i] * __conjf__( plp[i] ) );
        }

        // real odd pole:
        if( R >= 1 )
        {
            // quadratic sections gains computation:
            fcomplex< __fx64 > gain0 = glp[ N - 1 ];
            fcomplex< __fx64 > gain1 = fcomplex< __fx64 >( w , 0 );
            fcomplex< __fx64 > gain2 = ( fcomplex< __fx64 >( 1 , 0 ) - plp[ N - 1 ] * w );
            gains[ N - 1 ] = __realf__( gain0 * gain1 / gain2 );

            // zeros and poles transformation:
            zlp[ N - 1 ] = fcomplex< __fx64 >( -1 , 0 );
            plp[ N - 1 ] = ( fcomplex< __fx64 >( 1 , 0 ) + plp[ N - 1 ] * w ) / ( fcomplex< __fx64 >( 1 , 0 ) - plp[ N - 1 ] * w );

            // quadratic sections coefficients computation:

            // numerator:
            cfnum[ 3 * (N - 1) + 0 ] = 1;
            cfnum[ 3 * (N - 1) + 1 ] = -__realf__(zlp[ N - 1 ]);
            cfnum[ 3 * (N - 1) + 2 ] = 0;

            // denominator:
            cfden[ 3 * (N - 1) + 0 ] = 1;
            cfden[ 3 * (N - 1) + 1 ] = -__realf__(plp[ N - 1 ]);
            cfden[ 3 * (N - 1) + 2 ] = 0;

        }

        gains[N] = __realf__(glp[N]);

        // memory deallocation:
        free( plp );
        free( zlp );
        free( glp );

        return cf< T >{ cfnum , cfden , gains , L , R , N };
    }

    /*!
     * \brief Chebyshev II or Elliptic digital highpass filter coefficients computation function
     * \param[Fs]     sampling frequency , Hz
     * \param[Fp]     pass frequency , Hz
     * \param[order]  filter order
     * \param[type]   filter type ( 0 - Chebyshev II , 1 - Elliptic )
     * \param[g_stop] stopband attenuation , Db
     * \param[g_pass] passband attenuation , Db
     * \return   The function computes Chebyshev II or Elliptic digital lowpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */

    template < typename T > cf<T> __cheb2_ellip_digital_hp__
    (
            __fx64 Fs,
            __fx64 Fc,
            __ix32 order,
            __ix32 type   = 0,
            __fx64 g_pass = 1,
            __fx64 g_stop = 80
    )
    {
         __fx64  w = tan( PI2 * Fc / 2 / Fs );

        // digital lowpass coefficients computation:
        zp< __fx64 > zp = ( !type ) ? __cheb2_zeros_poles_plain__< __fx64 >( order , g_stop ) : __ellip_zeros_poles_plain__< __fx64 >( order , g_pass , g_stop );

        // zeros/poles and coefficients number:
        __ix32 L = zp.L;
        __ix32 R = zp.R;
        __ix32 N = zp.N;

        // zeros / poles plain initialization:
        fcomplex< __fx64 > *plp = zp.plp;
        fcomplex< __fx64 > *zlp = zp.zlp;
        fcomplex< __fx64 > *glp = zp.glp;

        // coefficients matrix memory allocation:
         T *cfnum = (T* )calloc( 3 * N , sizeof ( T ) );
         T *cfden = (T* )calloc( 3 * N , sizeof ( T ) );
         T *gains = (T* )calloc( N + 1 , sizeof ( T ) );

         if( cfnum == 0 || cfden == 0 || gains == 0 ||
             plp == 0 || zlp == 0 || glp == 0 )
         {
             free( cfnum ); free( cfden ); free( gains );
             free( plp )  ; free( zlp )  ; free( glp );
             return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 };
         }

        // coefficients matrix computation:

        // fcomplex conjugate pairs:
        for ( __ix32 i = 0 ; i < L ; i++)
        {
            // quadratic sections gains computation:
            fcomplex< __fx64 > gain0 = glp[i];
            fcomplex< __fx64 > gain1 = ( fcomplex< __fx64 >( 1 , 0 ) - zlp[i] / w ) * ( fcomplex< __fx64 >( 1 , 0 ) - __conjf__( zlp[i] ) / w );
            fcomplex< __fx64 > gain2 = ( fcomplex< __fx64 >( 1 , 0 ) - plp[i] / w ) * ( fcomplex< __fx64 >( 1 , 0 ) - __conjf__( plp[i] ) / w );
            gains[ i ] = __realf__( gain0 * gain1 / gain2 );

            // zeros and poles transformation:
            zlp[i] = ( fcomplex< __fx64 >( 1 , 0 ) + zlp[i] / w ) / ( fcomplex< __fx64 >( 1 , 0 ) - zlp[i] / w );
            plp[i] = ( fcomplex< __fx64 >( 1 , 0 ) + plp[i] / w ) / ( fcomplex< __fx64 >( 1 , 0 ) - plp[i] / w );

            // quadratic sections coefficients computation:

            // numerator:
            cfnum[ 3 * i + 0 ] = 1;
            cfnum[ 3 * i + 1 ] = +__realf__( zlp[i] + __conjf__( zlp[i] ) );
            cfnum[ 3 * i + 2 ] = +__realf__( zlp[i] * __conjf__( zlp[i] ) );

            // denominator:
            cfden[ 3 * i + 0 ] = 1;
            cfden[ 3 * i + 1 ] = +__realf__( plp[i] + __conjf__( plp[i] ) );
            cfden[ 3 * i + 2 ] = +__realf__( plp[i] * __conjf__( plp[i] ) );
        }

        // real odd pole:
        if( R >= 1 )
        {
            // quadratic sections gains computation:
            fcomplex< __fx64 > gain0 = glp[ N - 1 ];
            fcomplex< __fx64 > gain1 = fcomplex< __fx64 >( 1 / w , 0 );
            fcomplex< __fx64 > gain2 = ( fcomplex< __fx64 >( 1 , 0 ) - plp[ N - 1 ] / w );
            gains[ N - 1 ] = __realf__( gain0 * gain1 / gain2 );

            // zeros and poles transformation:
            zlp[ N - 1 ] = fcomplex< __fx64 >( -1 , 0 );
            plp[ N - 1 ] = ( fcomplex< __fx64 >( 1 , 0 ) + plp[ N - 1 ] / w ) / ( fcomplex< __fx64 >( 1 , 0 ) - plp[ N - 1 ] / w );

            // quadratic sections coefficients computation:

            // numerator:
            cfnum[ 3 * (N - 1) + 0 ] = 1;
            cfnum[ 3 * (N - 1) + 1 ] = __realf__(zlp[ N - 1 ]);
            cfnum[ 3 * (N - 1) + 2 ] = 0;

            // denominator:
            cfden[ 3 * (N - 1) + 0 ] = 1;
            cfden[ 3 * (N - 1) + 1 ] = __realf__(plp[ N - 1 ]);
            cfden[ 3 * (N - 1) + 2 ] = 0;
        }

        // output gain setting:
        gains[N] = __realf__(glp[N]);

        // memory deallocation:
        free( plp );
        free( zlp );
        free( glp );

        return cf< T >{ cfnum , cfden , gains , L , R , N };
    }

    /*!
     * \brief Chebyshev II or Elliptic digital bandpass filter coefficients computation function
     * \param[Fs]        sampling frequency , Hz
     * \param[Fp]        pass frequency     , Hz
     * \param[BandWidth] passband width     , Hz
     * \param[order]     filter order
     * \param[type]      filter type ( 0 - Chebyshev II , 1 - Elliptic )
     * \param[g_stop]    stopband attenuation , Db
     * \param[g_pass]    passband attenuation , Db
     * \return   The function computes Chebyshev II or Elliptic digital bandpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */

    template < typename T > cf<T> __cheb2_ellip_digital_bp__
    (
            __fx64 Fs,
            __fx64 Fp,
            __fx64 BandWidth,
            __ix32 order,
            __ix32 type   = 0,
            __fx64 g_pass = 1,
            __fx64 g_stop = 80
    )
    {
        order /= 2;

        // frequency deformation coefficient:
        __fx64 w1 = tan( PI2 * Fp / 2 / Fs ) , w2 = tan( PI2 * ( Fp + BandWidth ) / 2 / Fs );

        // allocate zeros and poles arrays:

        // lowpass analogue prototype poles, zeros and gains:
        zp< __fx64 > zp = ( !type ) ? __cheb2_zeros_poles_plain__< __fx64 >( order , g_stop ) : __ellip_zeros_poles_plain__< __fx64 >( order , g_pass , g_stop );
        fcomplex< __fx64 > *plp = zp.plp;
        fcomplex< __fx64 > *glp = zp.glp;
        fcomplex< __fx64 > *zlp = zp.zlp;
        __ix32 L = zp.L , R = zp.R , N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        fcomplex< __fx64 > *pbp = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *zbp = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *gbp = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );

        // coefficients matrix computation:
         T  *cfnum = ( T * )calloc( 3 * (2*L+R) , sizeof (  T  ) );
         T  *cfden = ( T * )calloc( 3 * (2*L+R) , sizeof (  T  ) );
         T  *gains = ( T * )calloc( (2*L+R + 1) , sizeof (  T  ) );

        if( cfnum == 0 || cfden == 0 || gains == 0 ||
            plp == 0 || zlp == 0 || glp == 0 ||
            pbp == 0 || zbp == 0 || gbp == 0)
        {
            free( cfnum ); free( cfden ); free( gains );
            free( plp )  ; free( zlp )  ; free( glp );
            free( pbp )  ; free( zbp )  ; free( gbp );
            return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 };
        }

        // LP-BP BILLINEAR TRANSFORM:

        // poles transformation:
        __fx64 w0 = w1 * w2;
        __fx64 dW = w2 - w1;

        // fcomplex conjugate pairs:
        __ix32 j = 0;

        // auxiliary variables:
        fcomplex< __fx64 > a , b , c , D , z1 , z2 , p1 , p2;

        for( __ix32 i = 0 ; i < L ; i++ , j+=2 )
        {
            // zeros frequency transformation by means of square equation solve:
            a = 1 / w0;
            b = zlp[i] * (__fx64)dW / (__fx64)w0 * (__fx64)(-1);
            c = 1;
            D = b * b - a * c * (__fx64)4;
            z1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            z2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // poles frequency transformation by means of square equation solve:
            a = 1 / w0;
            b = plp[i] * (__fx64)dW / (__fx64)w0 * (__fx64)(-1);
            c = 1;
            D = b * b - a * c * (__fx64)4;
            p1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            p2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // zeros and poles bilinear transform:
            zbp[ j + 0 ] = ( fcomplex< __fx64 >( 1 , 0 ) + z1 ) / ( fcomplex< __fx64 >( 1 , 0 ) - z1 );
            zbp[ j + 1 ] = ( fcomplex< __fx64 >( 1 , 0 ) + z2 ) / ( fcomplex< __fx64 >( 1 , 0 ) - z2 );
            pbp[ j + 0 ] = ( fcomplex< __fx64 >( 1 , 0 ) + p1 ) / ( fcomplex< __fx64 >( 1 , 0 ) - p1 );
            pbp[ j + 1 ] = ( fcomplex< __fx64 >( 1 , 0 ) + p2 ) / ( fcomplex< __fx64 >( 1 , 0 ) - p2 );

            // filter coefficients computation:

            // numerator:
            cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
            cfnum[ 3 * ( j + 0 ) + 1 ] = -__realf__( zbp[j] + __conjf__( zbp[j] ) );
            cfnum[ 3 * ( j + 0 ) + 2 ] = +__realf__( zbp[j] * __conjf__( zbp[j] ) );
            cfnum[ 3 * ( j + 1 ) + 0 ] = 1;
            cfnum[ 3 * ( j + 1 ) + 1 ] = -__realf__( zbp[j+1] + __conjf__( zbp[j+1] ) );
            cfnum[ 3 * ( j + 1 ) + 2 ] = +__realf__( zbp[j+1] * __conjf__( zbp[j+1] ) );

            // denominator:
            cfden[ 3 * ( j + 0 ) + 0 ] = 1;
            cfden[ 3 * ( j + 0 ) + 1 ] = -__realf__( pbp[j] + __conjf__( pbp[j] ) );
            cfden[ 3 * ( j + 0 ) + 2 ] = +__realf__( pbp[j] * __conjf__( pbp[j] ) );
            cfden[ 3 * ( j + 1 ) + 0 ] = 1;
            cfden[ 3 * ( j + 1 ) + 1 ] = -__realf__( pbp[j+1] + __conjf__( pbp[j+1] ) );
            cfden[ 3 * ( j + 1 ) + 2 ] = +__realf__( pbp[j+1] * __conjf__( pbp[j+1] ) );

            // fcomplex conjugate quadratic sections gains computation:
            fcomplex< __fx64 > gain0 = glp[i];
            fcomplex< __fx64 > gain1 = p1 * __conjf__( p1 );
            fcomplex< __fx64 > gain2 = p2 * __conjf__( p2 );
            fcomplex< __fx64 > gain3 = z1 * __conjf__( z1 );
            fcomplex< __fx64 > gain4 = z2 * __conjf__( z2 );
            fcomplex< __fx64 > gain5 = ( fcomplex< __fx64 >(1,0) - z1)*(fcomplex< __fx64 >(1,0) - __conjf__(z1) );
            fcomplex< __fx64 > gain6 = ( fcomplex< __fx64 >(1,0) - z2)*(fcomplex< __fx64 >(1,0) - __conjf__(z2) );
            fcomplex< __fx64 > gain7 = ( fcomplex< __fx64 >(1,0) - p1)*(fcomplex< __fx64 >(1,0) - __conjf__(p1) );
            fcomplex< __fx64 > gain8 = ( fcomplex< __fx64 >(1,0) - p2)*(fcomplex< __fx64 >(1,0) - __conjf__(p2) );

            fcomplex< __fx64 > gain9 = gain0 * gain1 * gain2 / ( gain3 * gain4 ) * ( gain5 * gain6 ) / ( gain7 * gain8 );
            gain9 = __csqrtf__( gain9 );
            gains[j+0] = __realf__(gain9);
            gains[j+1] = gains[j+0];
        }

        // real odd pole:
        if( R == 1 )
        {
            // pole transformation by means of square equation solve:
            fcomplex< __fx64 > a( 1 / w0 , 0 );
            fcomplex< __fx64 > b( -__realf__(plp[ N - 1 ]) * dW / w0 , -__realf__(plp[ N - 1 ]) * dW / w0 );
            fcomplex< __fx64 > c( 1 , 0 );
            fcomplex< __fx64 > D = b * b - a * c * (__fx64)4;
            fcomplex< __fx64 > p1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            fcomplex< __fx64 > p2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // zeros and poles bilinear transform:
            zbp[ j + 0 ] = fcomplex< __fx64 >( +1 , 0 );
            zbp[ j + 1 ] = fcomplex< __fx64 >( -1 , 0 );
            pbp[ j + 0 ] = ( fcomplex< __fx64 >( 1 , 0 ) + p1 ) / ( fcomplex< __fx64 >( 1 , 0 ) - p1 );
            pbp[ j + 1 ] = ( fcomplex< __fx64 >( 1 , 0 ) + p2 ) / ( fcomplex< __fx64 >( 1 , 0 ) - p2 );

            // digital filter coefficients computation:

            // numerator:
            cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
            cfnum[ 3 * ( j + 0 ) + 1 ] = 0;
            cfnum[ 3 * ( j + 0 ) + 2 ] = -1;

            // denominator:
            cfden[ 3 * ( j + 0 ) + 0 ] = 1;
            cfden[ 3 * ( j + 0 ) + 1 ] = -__realf__( pbp[j] + pbp[j+1] );
            cfden[ 3 * ( j + 0 ) + 2 ] = +__realf__( pbp[j] * pbp[j+1] );

            // fcomplex conjugate quadratic sections gains computation:
            fcomplex< __fx64 > gain0 = glp[N-1];
            fcomplex< __fx64 > gain1 = p1 * p2;
            fcomplex< __fx64 > gain2 = (fcomplex< __fx64 >(1,0) - p1)*(fcomplex< __fx64 >(1,0) - p2);
            fcomplex< __fx64 > gain3 = gain0 * gain1 / gain2 * dW / w0;
            gains[j+0] = __realf__(gain3);
        }

        // setting filter output gain:
        gains[ 2*L+R ] = __realf__(glp[N]);

        // memory free:
        free( plp );
        free( zlp );
        free( glp );
        free( pbp );
        free( zbp );
        free( gbp );

        return cf< T >{ cfnum , cfden , gains , 2*L , R , 2*L+R  };
    }

    /*!
     * \brief Chebyshev II or Elliptic digital bandstop filter coefficients computation function
     * \param[Fs]        sampling frequency , Hz
     * \param[Fc]        cut-off frequency  , Hz
     * \param[BandWidth] stopband width     , Hz
     * \param[order]     filter order
     * \param[type]      filter type ( 0 - Chebyshev II , 1 - Elliptic )
     * \param[g_stop]    stopband attenuation , Db
     * \param[g_pass]    passband attenuation , Db
     * \return   The function computes Chebyshev II or Elliptic digital bandstop filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */

    template < typename T > cf<T> __cheb2_ellip_digital_bs__
    (
            __fx64 Fs,
            __fx64 Fc,
            __fx64 BandWidth,
            __ix32 order,
            __ix32 type   = 0,
            __fx64 g_pass = 1,
            __fx64 g_stop = 80
    )
    {
        order /= 2;

        // frequency deformation coefficient:
        __fx64 w1 = tan( PI2 * Fc / 2 / Fs ) , w2 = tan( PI2 * ( Fc + BandWidth ) / 2 / Fs );

        // allocate zeros and poles arrays:

        // lowpass analogue prototype poles, zeros and gains:
        zp < __fx64 > zp = ( !type ) ? __cheb2_zeros_poles_plain__< __fx64 >( order , g_stop ) : __ellip_zeros_poles_plain__< __fx64 >( order , g_pass , g_stop );
        fcomplex< __fx64 > *plp = zp.plp;
        fcomplex< __fx64 > *glp = zp.glp;
        fcomplex< __fx64 > *zlp = zp.zlp;
        __ix32 L = zp.L , R = zp.R , N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        fcomplex< __fx64 > *pbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *zbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *gbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );

        // coefficients matrix computation:
         T  *cfnum = ( T * )calloc( 3 * (2*L+R) , sizeof ( T ) );
         T  *cfden = ( T * )calloc( 3 * (2*L+R) , sizeof ( T ) );
         T  *gains = ( T * )calloc( (2*L+R + 1) , sizeof ( T ) );

         if( cfnum == 0 || cfden == 0 || gains == 0 ||
             plp == 0 || zlp == 0 || glp == 0 ||
             pbs == 0 || zbs == 0 || gbs == 0)
         {
             free( cfnum ); free( cfden ); free( gains );
             free( plp )  ; free( zlp )  ; free( glp );
             free( pbs )  ; free( zbs )  ; free( gbs );
             return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 };
         }

        // LP-BP BILLINEAR TRANSFORM:

        // poles transformation:
        __fx64  w0 = w1 * w2;
        __fx64  dW = w2 - w1;

        // fcomplex conjugate pairs:
        __ix32 j = 0;

        // auxiliary variables:
        fcomplex< __fx64 > a , b , c , D , z1 , z2 , p1 , p2;

        for( __ix32 i = 0 ; i < L ; i++ , j+=2 )
        {
            // zeros frequency transformation by means of square equation solve:
            a = -1 / w0;
            b = fcomplex< __fx64 >( 1 , 0 ) / zlp[i] * dW / w0;
            c = -1;
            D = b * b - a * c * (__fx64)4;
            z1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            z2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // poles frequency transformation by means of square equation solve:
            a = -1 / w0;
            b = fcomplex< __fx64 >( 1 , 0 ) / plp[i] * dW / w0;
            c = -1;
            D = b * b - a * c * (__fx64)4;
            p1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            p2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // zeros and poles bilinear transform:
            zbs[ j + 0 ] = ( fcomplex< __fx64 >( 1 , 0 ) + z1 ) / ( fcomplex< __fx64 >( 1 , 0 ) - z1 );
            zbs[ j + 1 ] = ( fcomplex< __fx64 >( 1 , 0 ) + z2 ) / ( fcomplex< __fx64 >( 1 , 0 ) - z2 );
            pbs[ j + 0 ] = ( fcomplex< __fx64 >( 1 , 0 ) + p1 ) / ( fcomplex< __fx64 >( 1 , 0 ) - p1 );
            pbs[ j + 1 ] = ( fcomplex< __fx64 >( 1 , 0 ) + p2 ) / ( fcomplex< __fx64 >( 1 , 0 ) - p2 );

            // filter coefficients computation:

            // numerator:
            cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
            cfnum[ 3 * ( j + 0 ) + 1 ] = -__realf__( zbs[j] + __conjf__( zbs[j] ) );
            cfnum[ 3 * ( j + 0 ) + 2 ] = +__realf__( zbs[j] * __conjf__( zbs[j] ) );
            cfnum[ 3 * ( j + 1 ) + 0 ] = 1;
            cfnum[ 3 * ( j + 1 ) + 1 ] = -__realf__( zbs[j+1] + __conjf__( zbs[j+1] ) );
            cfnum[ 3 * ( j + 1 ) + 2 ] = +__realf__( zbs[j+1] * __conjf__( zbs[j+1] ) );

            // denominator:
            cfden[ 3 * ( j + 0 ) + 0 ] = 1;
            cfden[ 3 * ( j + 0 ) + 1 ] = -__realf__( pbs[j] + __conjf__( pbs[j] ) );
            cfden[ 3 * ( j + 0 ) + 2 ] = +__realf__( pbs[j] * __conjf__( pbs[j] ) );
            cfden[ 3 * ( j + 1 ) + 0 ] = 1;
            cfden[ 3 * ( j + 1 ) + 1 ] = -__realf__( pbs[j+1] + __conjf__( pbs[j+1] ) );
            cfden[ 3 * ( j + 1 ) + 2 ] = +__realf__( pbs[j+1] * __conjf__( pbs[j+1] ) );

            // fcomplex conjugate quadratic sections gains computation:
            fcomplex< __fx64 > gain0 = p1 * __conjf__( p1 );
            fcomplex< __fx64 > gain1 = p2 * __conjf__( p2 );
            fcomplex< __fx64 > gain2 = z1 * __conjf__( z1 );
            fcomplex< __fx64 > gain3 = z2 * __conjf__( z2 );
            fcomplex< __fx64 > gain4 = ( fcomplex< __fx64 >(1,0) - z1)*(fcomplex< __fx64 >(1,0) - __conjf__(z1) );
            fcomplex< __fx64 > gain5 = ( fcomplex< __fx64 >(1,0) - z2)*(fcomplex< __fx64 >(1,0) - __conjf__(z2) );
            fcomplex< __fx64 > gain6 = ( fcomplex< __fx64 >(1,0) - p1)*(fcomplex< __fx64 >(1,0) - __conjf__(p1) );
            fcomplex< __fx64 > gain7 = ( fcomplex< __fx64 >(1,0) - p2)*(fcomplex< __fx64 >(1,0) - __conjf__(p2) );
            fcomplex< __fx64 > gain8 = ( gain0 * gain1 ) / ( gain2 * gain3 ) * ( gain4 * gain5 ) / ( gain6 * gain7 );
            gain8 = __csqrtf__( gain8 );
            gains[j+0] = __realf__(gain8);
            gains[j+1] = gains[j+0];
        }

        // real odd pole:
        if( R == 1 )
        {
            // poles transformation by means of square equation solve:
            fcomplex< __fx64 > a = fcomplex< __fx64 >( -1 / w0 , 0 );
            fcomplex< __fx64 > b = fcomplex< __fx64 >( dW , 0 ) / plp[N-1] / w0;
            fcomplex< __fx64 > c = fcomplex< __fx64 >( -1 , 0 );
            fcomplex< __fx64 > D = b * b - a * c * (__fx64)4;
            fcomplex< __fx64 > p1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            fcomplex< __fx64 > p2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // zeros and poles bilinear transform:
            zbs[ j + 0 ] = fcomplex< __fx64 >( +1 , 0 );
            zbs[ j + 1 ] = fcomplex< __fx64 >( -1 , 0 );
            pbs[ j + 0 ] = ( fcomplex< __fx64 >( 1 , 0 ) + p1 ) / ( fcomplex< __fx64 >( 1 , 0 ) - p1 );
            pbs[ j + 1 ] = ( fcomplex< __fx64 >( 1 , 0 ) + p2 ) / ( fcomplex< __fx64 >( 1 , 0 ) - p2 );

            // digital filter coefficients computation:

            // numerator:
            cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
            cfnum[ 3 * ( j + 0 ) + 1 ] = 2 * ( w0 - 1 ) / ( w0 + 1 );
            cfnum[ 3 * ( j + 0 ) + 2 ] = 1;

            // denominator:
            cfden[ 3 * ( j + 0 ) + 0 ] = 1;
            cfden[ 3 * ( j + 0 ) + 1 ] = -__realf__( pbs[j]+pbs[j+1] );
            cfden[ 3 * ( j + 0 ) + 2 ] = +__realf__( pbs[j]*pbs[j+1] );

            // fcomplex conjugate quadratic sections gains computation:
            fcomplex< __fx64 > gain0 = glp[N-1];
            fcomplex< __fx64 > gain1 = p1 * p2;
            fcomplex< __fx64 > gain2 = fcomplex< __fx64 >(1,0) / plp[N-1] / (__fx64)(-1);
            fcomplex< __fx64 > gain3 = (fcomplex< __fx64 >(1,0) - p1)*(fcomplex< __fx64 >(1,0) - p2);
            fcomplex< __fx64 > gain4 = gain0 * gain1 * gain2 / gain3 / w0 * ( 1 + w0 );
            gains[j+0] = __realf__(gain4);
        }

        // setting filter output gain:
        gains[ 2*L+R ] = __realf__(glp[N]);

        // memory free:
        free( plp );
        free( zlp );
        free( glp );
        free( pbs );
        free( zbs );
        free( gbs );

        return cf< T >{ cfnum , cfden , gains , 2*L , R , 2*L+R  };
    }

    // FIR coefficients computation functions:

    /*!
      * \brief FIR digital lowpass filter coefficients computation function
      * \param[Fs]     sampling frequency , Hz
      * \param[Fc]     cut-off frequency  , Hz
      * \param[N]      filter order
      * \param[scale]  scaling factor ( scale = 0 - not-scaled coefficients , scale = 1 - scaled coefficients )
      * \param[wind]   input window function object
      * \return The function returns digital lowpass FIR filter coefficients buffer.
    */
    template< typename __type > cf<__type> __fir_wind_digital_lp__
    (
            __fx64 Fs,
            __fx64 Fc,
            __ix32 N,
            bool scale,
            __fx64 *wind
    )
    {
        // coefficients buffer memory allocation:
        __type *cfbuff = ( __type* ) calloc( N , sizeof ( __type ) );

        // coefficients computation:
        __ix32 k = 0;
        Fc /= Fs;

        if ( N % 2 == 0) // even order filter coefficients computation
        {
            for ( __ix32 n = 0; n < N / 2; n++)
            {
                k = abs(n - N / 2);
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

            for ( __ix32 n = 0; n < ceil(N / 2); n++)
            {
                k = abs(n - N / 2);
                cfbuff[k] = 2 * Fc * sin((n + rem) * PI2 * Fc ) / ((n + rem) * PI2 * Fc ) * wind[k];
                cfbuff[ N - k] = cfbuff[k];
            }
        }

        // filter pulse characteristic normalization:
        if ( scale )
        {
            fr fr = __freq_resp__( cfbuff , N , Fs , 0 );
            for ( __ix32 n = 0; n < N; n++) cfbuff[n] /= fr.Km;
        }

        // returning the result
        return cf< __type >{ cfbuff , 0 , 0 , -1 , -1 , N  };
    }

    /*!
      * \brief FIR digital lowpass filter coefficients computation function
      * \param[Fs]     sampling frequency , Hz
      * \param[Fp]     pass-off frequency , Hz
      * \param[N]      filter order
      * \param[scale]  scaling factor ( scale = 0 - not-scaled coefficients , scale = 1 - scaled coefficients )
      * \param[wind]   input window function object
      * \return The function returns digital lowpass FIR filter coefficients buffer.
    */
    template< typename __type > cf<__type> __fir_wind_digital_hp__
    (
            __fx64 Fs,
            __fx64 Fp,
            __ix32 N,
            bool scale,
           __fx64 *wind
    )
    {
        // coefficients buffer memory allocation:
        __type *cfbuff = ( __type* ) calloc( N , sizeof ( __type ) );

        // coefficients computation:
        __ix32 k = 0;
        Fp /= Fs;

        if ( N % 2 == 0) // even order highpass
        {
            for ( __ix32 n = 0; n < N / 2; n++)
            {
                k = abs(n - N / 2);

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
            for ( __ix32 n = 0; n < ceil(N / 2); n++)
            {
                k = abs(n - N / 2);
                cfbuff[k]     = -pow(-1 , n) * 2 * (0.5-Fp) * sin((n + rem) * PI2 * (0.5-Fp) ) / ((n + rem) * PI2 * (0.5 - Fp) )* wind[k];
                cfbuff[N - k] = -cfbuff[k];
            }
        }

        // filter pulse characteristic scaling:
        if ( scale )
        {
            fr fr = __freq_resp__( cfbuff , N , Fs , Fs / 2 );
            for ( __ix32 n = 0; n < N; n++) cfbuff[n] /= fr.Km;
        }

        // returning the result
        return cf< __type >{ cfbuff , 0 , 0 , -1 , -1 , N  };
    }

    /*!
      * \brief FIR digital lowpass filter coefficients computation function
      * \param[Fs]     sampling frequency , Hz
      * \param[Fp]     pass frequency     , Hz
      * \param[BW]     passband width     , Hz
      * \param[N]      filter order
      * \param[scale]  scaling factor ( scale = 0 - not-scaled coefficients , scale = 1 - scaled coefficients )
      * \param[wind]   input window function object
      * \return The function returns digital lowpass FIR filter coefficients buffer.
    */
    template< typename __type > cf<__type> __fir_wind_digital_bp__
    (
            __fx64 Fs,
            __fx64 Fp,
            __fx64 BW,
            __ix32 N,
            bool scale,
            __fx64 *wind
    )
    {
        // coefficients buffer memory allocation:
        __type *cfbuff = ( __type* ) calloc( N , sizeof ( __type ) );

        // coefficients computation:
        __ix32 k = 0;
        __fx64 Fp1 = Fp / Fs , Fp2 = ( Fp + BW )/ Fs;

        if ( N % 2 == 0) // even order bandpass
        {
            for ( __ix32 n = 0; n < N / 2; n++)
            {
                k = abs(n - N / 2);

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

            for ( __ix32 n = 0; n < ceil(N / 2); n++)
            {
                k = abs(n - N / 2);
                cfbuff[k] = 2 * (Fp2 * sin((n + rem) * PI2 * Fp2) / ((n + rem) * PI2 * Fp2) - Fp1 * sin((n + rem) * PI2 * Fp1) / ((n + rem) * PI2 * Fp1)) * wind[k];
                cfbuff[N - k] = cfbuff[k];
            }
        }

        // filter pulse characteristic normalization:
        if ( scale )
        {
            fr fr = __freq_resp__( cfbuff , N , Fs , ( ( Fp1 + 0.5 * ( Fp2 - Fp1 ) ) * Fs ) );
            for ( __ix32 n = 0; n < N; n++) cfbuff[n] /= fr.Km;
        }

        // returning the result:
        return cf< __type >{ cfbuff , 0 , 0 , -1 , -1 , N  };
    }

    /*!
      * \brief FIR digital bandpass filter coefficients computation function
      * \param[Fs]     sampling frequency , Hz
      * \param[Fc]     cut-off frequency  , Hz
      * \param[BW]     stopband width     , Hz
      * \param[N]      filter order
      * \param[scale]  scaling factor ( scale = 0 - not-scaled coefficients , scale = 1 - scaled coefficients )
      * \param[wind]   input window function object
      * \return The function returns digital lowpass FIR filter coefficients buffer.
    */
    template< typename __type > cf<__type> __fir_wind_digital_bs__
    (
            __fx64 Fs,
            __fx64 Fc,
            __fx64 BW,
            __ix32 N,
            bool scale,
            __fx64 *wind
    )
    {
        // coefficients buffer memory allocation:
        __type *cfbuff = ( __type* ) calloc( N , sizeof ( __type ) );

        // coefficients computation:
        __ix32 k = 0;
        __fx64 Fc1 = Fc / Fs , Fc2 = ( Fc + BW ) / Fs;

        for (int n = 0; n < N / 2; n++)
        {
            k = abs(n - N / 2);

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
            fr fr = __freq_resp__( cfbuff , N , Fs , 0 );
            for ( __ix32 n = 0; n < N; n++) cfbuff[n] /= fr.Km;
        }

        // return the result:
        return cf< __type >{ cfbuff , 0 , 0 , -1 , -1 , N  };
    }

    /*! @} */

    /*! \defgroup <DSPB_AbstractModel> ( Digital Signal Processing abstract model )
     *  \ingroup DSPB
     *  \brief The module contains abstract models for DSP basic elements
        @{
    */

    /*!
     *  \class filter_abstract
     *  \brief defines the basic filter abstract structure
    */
    class filter_abstract
    {
    protected:

        /*! \brief filter order */
        __ix32 m_order;
        /*! \brief input signal sampling frequency, Hz */
        __fx64 m_Fs;
        /*! \brief input signal nominal frequency , Hz */
        __fx64 m_Fn;
        /*! \brief input signal sampling period , s */
        __fx64 m_Ts;

        /*! \brief default constructor */
        filter_abstract()
        {
            m_order = 8;
            m_Fs    = 4000;
            m_Fn    = 50;
            m_Ts    = 1 / m_Fs;
        }

        /*!
         *  \brief frequency response computation virtual function
         *  \param[F] input signal frequency, Hz
        */
        virtual fr frequency_response( __fx64 F ) = 0;

        /*! \brief virtual destructor */
        virtual ~filter_abstract(){};

        /*! \brief memory allocation virtual function */
        virtual __ix32 allocate() = 0;

        /*! \brief memory deallocation virtual function */
        virtual __ix32 deallocate() = 0;
    };

    /*!
     *  \class classic_iir_abstract
     *  \brief defines classic IIR filter structure
    */
    template< typename __type > class classic_iir_abstract : public  filter_abstract
    {
        protected:

        /*! \brief filter type( lowpass, highpass , bandpass or bandstop ) */
        filter_type      m_filter_type;
        /*! \brief filter bandwidth, Hz */
        bandwidth        m_bandwidth;
        /*! \brief filter passband/stopband attenuation, Db */
        attenuation      m_attenuation;
        /*! \brief filter input buffers vector */
        delay< __type > *m_buff_sx;
        /*! \brief filter output buffers vector */
        delay< __type > *m_buff_sy;
        /*! \brief filter coefficients matrix */
        cf   < __type >  m_cfmatrix;

        /*! \brief lowpass filter coefficients computation virtual function */
        virtual cf< __type > compute_lowpass () = 0;
        /*! \brief highpass filter coefficients computation virtual function */
        virtual cf< __type > compute_highpass() = 0;
        /*! \brief bandpass filter coefficients computation virtual function */
        virtual cf< __type > compute_bandpass() = 0;
        /*! \brief bandstop filter coefficients computation virtual function */
        virtual cf< __type > compute_bandstop() = 0;

        /*!
         *  \brief filter memory allocation function
         *  \details The function allocates memory and computes filter coefficients
         *           depending on a filter type
        */
        __ix32 allocate() override
        {
            switch ( m_filter_type )
            {
                case filter_type::lowpass:
                m_cfmatrix = compute_lowpass ();
                break;

                case filter_type::highpass:
                m_cfmatrix = compute_highpass();
                break;

                case filter_type::bandpass:
                m_cfmatrix = compute_bandpass();
                break;

                case filter_type::bandstop:
                m_cfmatrix = compute_bandstop();
                break;
            }

            if( m_cfmatrix.cfden != nullptr && m_cfmatrix.cfnum != nullptr  && m_cfmatrix.gains != nullptr )
            {
                m_buff_sx = new delay< __type >[ m_cfmatrix.N ];
                m_buff_sy = new delay< __type >[ m_cfmatrix.N ];
                for( int i = 0 ; i < m_cfmatrix.N ; i++ )
                {
                    m_buff_sx[i].allocate(4);
                    m_buff_sy[i].allocate(3);
                }
            }

            return ( m_buff_sx != nullptr && m_buff_sy != nullptr );
        }

    public:

        /*!
         *  \brief filtering operator
         *  \details The operator calls an appropriate filtering function
        */
        virtual inline __type operator()( __type* _input ) = 0;

        /*!
         *  \brief virtual destructor
         *  \details Calls memory deallocation function
        */
        virtual ~classic_iir_abstract()
        {
            deallocate();
        }

        /*! \brief memory deallocation function */
        __ix32 deallocate()  override
        {
            // input / output buffers memory deallcoation:
            int N = m_cfmatrix.N;
            if( m_buff_sx != nullptr )
            {
                for( int i = 0 ; i < N ; i++ )
                {
                    m_buff_sx[i].deallocate();
                }
                m_buff_sx = nullptr;
            }

            if( m_buff_sy != nullptr )
            {
                for( int i = 0 ; i < N ; i++ )
                {
                    m_buff_sy[i].deallocate();
                }
                m_buff_sy = nullptr;
            }

            // filter coefficient matrix memory deallocation:
            m_cfmatrix = __cf_free__( m_cfmatrix );

            #ifdef DEBUG
                std::cout << "memory deallocation invocation" << "\n";
                std::cout << "m_cfmatrix.cfnum cleand = " << !m_cfmatrix.cfnum << "\n";
                std::cout << "m_cfmatrix.cfden cleand = " << !m_cfmatrix.cfden << "\n";
                std::cout << "m_cfmatrix.gains cleand = " << !m_cfmatrix.gains << "\n";
                std::cout << "m_buff_sx        cleand = " << !m_buff_sx        << "\n";
                std::cout << "m_buff_sy        cleand = " << !m_buff_sy        << "\n";
                std::cout << "\n";
            #endif

            // return the result:
            return ( !m_cfmatrix.cfden && !m_cfmatrix.cfnum && !m_cfmatrix.gains && !m_buff_sx && !m_buff_sy );
        }

        /*!
         *  \brief frequency response computation function
         *  \param[F] input signal frequency, Hz
        */
        fr frequency_response( __fx64 _F ) override
        {
            return __freq_resp__( m_cfmatrix.cfnum , m_cfmatrix.cfden , m_cfmatrix.gains , m_cfmatrix.N , m_Fs , _F );
        }

        /*!
         *  \brief frequency response computation function
         *  \param[_Fs         ] input signal sampling frequency, Hz
         *  \param[_order      ] filter order
         *  \param[_type       ] filter type
         *  \param[_bandwidth  ] filter passband/stopband bandwidth
         *  \param[_attenuation] filter passband/stopband attenuation
         *  \details The function initializes filter and calls memory allocation and coefficients
         *           computation function
        */
        void init( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth , attenuation _attenuation )
        {
            // system fields initialization:
            m_Fs          = _Fs;
            m_filter_type = _type;
            m_order       = _order;
            m_bandwidth   = _bandwidth;
            m_attenuation = _attenuation;

            // memory allocation function call:
            allocate();
        }

        /*! \brief default constructor */
        classic_iir_abstract() : filter_abstract()
        {
            m_buff_sx     = nullptr;
            m_buff_sy     = nullptr;
            m_filter_type = filter_type::lowpass;
            m_bandwidth   = { 100 , 500 };
            m_attenuation = { 80  , 1   };
        }

        /*!
         *  \brief initializing constructor
         *  \param[_Fs         ] input signal sampling frequency, Hz
         *  \param[_order      ] filter order
         *  \param[_type       ] filter type
         *  \param[_bandwidth  ] filter passband/stopband bandwidth
         *  \param[_attenuation] filter passband/stopband attenuation
         *  \details Initializes filter and calls memory allocation and coefficients
         *           computation function
        */
        classic_iir_abstract( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth , attenuation _attenuation )
        {
            init(_Fs, _order,  _type, _bandwidth, _attenuation );
        }

        #ifndef __ALG_PLATFORM
        void show()
        {
            __show__( m_cfmatrix, m_filter_type );
        }
        #endif
    };

    /*!
     *  \class classic_fir_abstract
     *  \brief defines classic IIR filter structure
    */
    template< typename __type > class classic_fir_abstract : public  filter_abstract
    {
    protected:

        /*! \brief filter type( lowpass, highpass , bandpass or bandstop ) */
        filter_type   m_filter_type;
        /*! \brief filter passband/stopband width, Hz */
        bandwidth     m_bandwidth;
        /*! \brief coefficients scaling flag */
        bool          m_scale;
         /*! \brief filter buffer */
        delay<__type> m_buff_sx;
         /*! \brief filter coefficients martix */
        cf<__type>    m_cfmatrix;
         /*! \brief filter coefficeints vector pointer */
        __type       *m_coeffs;

        /*!
         *  \brief filter memory allocation empty function
        */
        __ix32 allocate() override{ return 0; }

        /*!
         *  \brief filter memory allocation function
         *  \param[*window] pointer to the window function coefficients buffer
         *  \details The function computes FIR coefficients using window method.
         *           The input window buffer MUST have size equal to the filter oreder !!!
         *           If the input window buffer is null than Chebyshev window is used.
        */
        __ix32 allocate( __fx64* _window )
        {
            switch ( m_filter_type )
            {
                case filter_type::lowpass:
                m_cfmatrix = __fir_wind_digital_lp__<__type>( m_Fs , m_bandwidth.Fc , m_order , m_scale , _window );
                break;

                case filter_type::highpass:
                m_cfmatrix = __fir_wind_digital_hp__<__type>( m_Fs , m_bandwidth.Fc , m_order , m_scale , _window );
                break;

                case filter_type::bandpass:
                m_cfmatrix = __fir_wind_digital_bp__<__type>( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , m_scale , _window );
                break;

                case filter_type::bandstop:
                m_cfmatrix = __fir_wind_digital_bs__<__type>( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , m_scale , _window );
                break;
            }

            if( m_cfmatrix.cfnum != nullptr )
            {
                m_order  = m_cfmatrix.N;
                m_coeffs = m_cfmatrix.cfnum;
                return m_buff_sx.allocate( m_cfmatrix.N + 1 );
            }
            return 0;
        }

    public:

        /*! \brief filter buffer filling function */
        virtual inline void operator <<  ( __type *input ) = 0;

        /*!
         *  \brief filter filtering operator
         *  \param[*input] input signal samples pointer
        */
        virtual inline __type operator() ( __type *input ) = 0;
        /*! \brief filter filtering operator */
        virtual inline __type operator() () = 0;

        /*!
         *  \brief filter virtual destructor
         *  \details Calls memory deallocation function
        */
        virtual ~classic_fir_abstract()
        {
            deallocate();
        }

        /*! \brief memory deallocation function */
        __ix32 deallocate() override
        {
            // filter data memory deallocation:
            m_buff_sx.deallocate();
            m_cfmatrix = __cf_free__( m_cfmatrix );

            #ifdef DEBUG
                std::cout << "memory deallocation invocation" << "\n";
                std::cout << "m_cfmatrix.cfnum cleand = " << !m_cfmatrix.cfnum << "\n";
                std::cout << "m_cfmatrix.cfden cleand = " << !m_cfmatrix.cfden << "\n";
                std::cout << "m_cfmatrix.gains cleand = " << !m_cfmatrix.gains << "\n";
                std::cout << "\n";
            #endif

            return ( !m_cfmatrix.cfnum && !m_cfmatrix.cfden );
        }

        /*!
         *  \brief Frequency response computation function
         *  \param[F] input signal frequency, Hz
        */
        fr frequency_response( __fx64 F ) override
        {
            return __freq_resp__<__type> ( m_cfmatrix.cfnum , m_order, m_Fs , F  );
        }

        /*!
         *  \brief FIR filter initialization function
         *  \param[ _Fs       ] input signal sampling frequency, Hz
         *  \param[ _order    ] filter order
         *  \param[ _type     ] filter type
         *  \param[ _bandwidth] filter stopband/passband width , Hz
         *  \param[ _window   ] filter window
         *  \param[ _scale    ] filter coefficients scaling flag
         *  \details The function calls memory allocation and coeffcients computation function
        */
        void init( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth , __fx64* _window , bool _scale )
        {
            m_Fs          = _Fs;
            m_filter_type = _type;
            m_order       = _order;
            m_bandwidth   = _bandwidth;
            m_scale       = _scale;

            // memory allocation function call:
            allocate(_window);
        }

        /*! \brief default constructor */
        classic_fir_abstract() : filter_abstract()
        {
            m_filter_type = filter_type::lowpass;
            m_bandwidth   = { 100 , 500 };
        }

        /*!
         *  \brief FIR filter initializing constructor
         *  \param[ _Fs       ] input signal sampling frequency, Hz
         *  \param[ _order    ] filter order
         *  \param[ _type     ] filter type
         *  \param[ _bandwidth] filter stopband/passband width , Hz
         *  \param[ _window   ] filter window
         *  \param[ _scale    ] filter coefficients scaling flag
         *  \details Constructor calls memory allocation and coeffcients computation function
        */
        classic_fir_abstract( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth , __fx64* _window , bool _scale )
        {
            init(_Fs, _order,  _type , _bandwidth , _window , _scale );
        }

        #ifndef __ALG_PLATFORM
        /*! \brief filter coefficicents display function */
        void show()
        {
            __show__( m_coeffs, m_order, m_filter_type );
        }
        #endif
    };

    /*!
     *  \class recursive_fir_abstract
     *  \brief defines recursive Fourier FIR filter
    */
    template< typename __type > class recursive_fir_abstract : public  filter_abstract
    {
    protected:
        /*! \brief recursive FIR filter gain */
        __fx64 m_Gain;
        /*! \brief auxiliary variable */
        __fx64 m_Ks;
        /*! \brief auxiliary variable */
        __fx64 m_Kc;
        /*! \brief computed harmonic number */
        __fx64 m_hnum;
        /*! \brief auxiliary variable */
        __fx64 m_a0;
        /*! \brief harmonic real output component */
        __fx64 m_a;
       /*! \brief harmonic imaginary output component */
        __fx64 m_b;
        /*! \brief recursive Fourier filter buffer */
        delay<__type> m_buffer_sx;

        /*! \brief  recursive Fourier filter memory allocation function
         *  \return the function allocates memory for the recursive Fourier filter buffer
       */
       __ix32  allocate() override
       {
           return ( m_buffer_sx.allocate( m_order + 1 ) );
       }

       /*! \brief  recursive Fourier filter memory deallocation function
        *  \return the function deallocates memory of the recursive Fourier buffer
      */
       __ix32 deallocate() override
       {
           m_buffer_sx.deallocate();
           return 0;
       }

        /*!
         *  \brief  Recursive FIR filter filtering function
         *  \param[input] - pointer to the nput signal frames
         *  \return The function computes real and imaginary harmonic component
        */
        template< typename T > inline harmonic filt ( T *input )
        {
            m_buffer_sx( input );
            m_a0 = m_a + ( ( __fx64 )*input - ( __fx64 )m_buffer_sx[ m_order ] ) * m_Gain;
            m_a  = m_a0 * m_Kc - m_b * m_Ks;
            m_b  = m_a0 * m_Ks + m_b * m_Kc;
            return { m_a , m_b , 0 , m_Fn };
        }

       public:

        /*! \brief  recursive Fourier filter initialization function
         *  \param[Fn  ] - input signal nominal frequency  , Hz
         *  \param[Fs  ] - input signal sampling frequency , hz
         *  \param[hnum] - number of computed harmonic
         *  \return the function initializes recursive Fourier filter
       */
        void init( __fx64 _Fs, __fx64 _Fn, __ix32 _hnum, __ix32 _order_multiplier = 1 )
        {
            // system variables initialization:
            m_Fn      = _Fn;
            m_Fs      = _Fs;
            m_Ts      = 1 / m_Fs;
            m_order   = ceil( _Fs / _Fn ) * _order_multiplier;
            m_Gain    = ( _hnum == 0 ) ? ( 1.0 / (__fx64)m_order ) : ( 2.0 / (__fx64)m_order / sqrt(2) );
            m_hnum    = _hnum;
            m_Ks      = sin( PI2 * (__fx64)m_hnum / (__fx64)m_order );
            m_Kc      = cos( PI2 * (__fx64)m_hnum / (__fx64)m_order );

            // filter output initialization:
            m_a0 = 0;
            m_a  = 0;
            m_b  = 0;

            // memory allocation:
            allocate();
        }

        recursive_fir_abstract() : filter_abstract() {}

        recursive_fir_abstract(__fx64 _Fs, __fx64 _Fn, __ix32 _hnum, __ix32 _order_multiplier = 1 )
        {
            init(_Fs, _Fn, _hnum, _order_multiplier);
        }

        virtual ~recursive_fir_abstract()
        {
            deallocate();
        }

        virtual inline harmonic operator ()(__type  *input ) = 0;

        /*! \brief recursive Fourier filter frequency response computation function
         *  \param[F] - input signal frequency , Hz
         *  \return The function computes phase and amplitude frequency response for the signal having frequency F
        */
        fr frequency_response( __fx64 F ) override
        {
            // complex frequency coeffs:
            __type Re1 = 0;
            __type Im1 = 0;
            __type Re2 = 0;
            __type Im2 = 0;
            __type K = (__fx64)1 / (__fx64)m_order;

            // transfer function:
            Re1 = 1 - cos( -PI2 * F * m_order * m_Ts );
            Im1 = 0 - sin( -PI2 * F * m_order * m_Ts );
            Re2 = 1 - ( cos(-PI2 * F * m_Ts) * m_Kc - sin(-PI2 * F * m_Ts) * m_Ks );
            Im2 = 0 - ( cos(-PI2 * F * m_Ts) * m_Ks + sin(-PI2 * F * m_Ts) * m_Kc );

            // amplitude and phase response:
            if( F == m_Fn * m_hnum )
            {
                return  { 1 , 0 };
            }
            else
            {
                return { sqrt( Re1 * Re1 + Im1 * Im1 ) / sqrt( Re2 * Re2 + Im2 * Im2 ) * K
                            , atan2( Im1 , Re1 ) - atan2( Im2 , Re2 ) };
            }
        }
    };

    /*!
     *  \class comb filter abstract class
     *  \brief defines comb FIR filter
    */
    template < typename T > class fcomb_abstract : public  filter_abstract
    {
        typedef T __type ;
    protected:
        /*! \brief odd flag */
        __ix32 m_odd;
        /*! \brief filter output */
        __fx64 m_out;

        /*! \brief comb filter buffer */
         delay< __type > m_bx;

         /*!
          *  \brief 32-bit floating point filtering function
          *  \param[input] - pointer to the input signal samples buffer
          *  \return The function returns filtering result
         */
         template< typename F > inline __type filt( F *input)
         {
             m_bx( input );
             return ( m_out = (m_odd)? (__fx64)*input + (__fx64)m_bx[ m_order ] : (__fx64)*input - (__fx64)m_bx[ m_order ] );
         }

    public:

        /*!
         *  \brief comb filter initialization function
         *  \param[Fs] - input signal sampling frequency
         *  \param[Fn] - input signal nominal frequency
        */
        __ix32 init( __fx64 _Fs , __fx64 _Fn, __ix32 _odd )
        {
            m_Fs      = _Fs;
            m_Fn      = _Fn;
            m_Ts      = 1 / m_Fs;
            m_odd     = _odd;
            m_order   = m_Fs / m_Fn / 2;
            m_out     = 0;

            // memory allocation:
            allocate();

            return 0;
        }

        /*! \brief memory allocation function */
        __ix32 allocate() override
        {
            return m_bx.allocate( m_order + 1 );
        }

        /*! \brief memory deallocation function */
        __ix32 deallocate() override
        {
            m_bx.deallocate();
            return 0;
        }

        /*! \brief default constructor */
        fcomb_abstract(){}

        /*!
         *  \brief initializinf constructor
         *  \param[Fs] - input signal sampling frequency
         *  \param[Fn] - input signal nominal frequency
        */
        fcomb_abstract( __fx64 _Fs , __fx64 _Fn, __ix32 _odd )
        {
            init( _Fs , _Fn, _odd );
        }

        /*! \brief default destructor */
        virtual ~fcomb_abstract()
        {
            deallocate();
        }

        /*! \brief frequency response computation function */
        fr frequency_response( __fx64 F ) override
        {
            __fx64 Re = 0;
            __fx64 Im = 0;
            if(!m_odd)
            {
                Re = 1 - cos(-PI2  * m_order * F * m_Ts);
                Im = 0 - sin(-PI2  * m_order * F * m_Ts);
            }
            else
            {
                Re = 1 + cos(-PI2  * m_order * F * m_Ts);
                Im = 0 + sin(-PI2  * m_order * F * m_Ts);
            }

            return { sqrt(Re * Re + Im * Im) * 0.5 , atan2(Im, Re) };
        }

        /*!
         *  \brief 32-bit floating point filtering operator
         *  \param[input] - pointer to the input signal samples buffer
         *  \return The operatoe calls the function that returns filtering result
        */
        virtual inline __type operator ()( __type *input ) = 0;
    };

    /*!
     *  \class equalized comb filter abstract class
     *  \brief defines equalized comb FIR filter
    */
    template < typename __type > class fcombeq_abstract : public  filter_abstract
    {
    protected:

        /*! \brief input signal frequency deviation from nominal for which the amplitude frequency response slope compensation is implemented , Hz */
        __fx64 m_dF;
        /*! \brief residual amplitude frequency response slope for the input signal frequency deviation dF from nominal , p.u. */
        __fx64 m_d_Amp;
        /*! \brief auxiliary coefficient K1 */
        __fx64 m_K1;
        /*! \brief auxiliary coefficient K2 */
        __fx64 m_K2;
        /*! \brief auxiliary coefficient ElemNum1 */
        __ix32 m_ElemNum1;
        /*! \brief auxiliary coefficient ElemNum2 */
        __ix32 m_ElemNum2;
        /*! \brief odd flag */
        __ix32 m_odd;

        /*! \brief comb filter buffer */
         delay< __type > m_bx;

         /*!
          *  \brief 32-bit floating point filtering function
          *  \param[input] - pointer to the input signal samples buffer
          *  \return The function returns filtering result
         */
         template< typename F > inline __type filt( F *input )
         {
             m_bx(input);
             return ( m_out = ( m_odd ) ? ( (__fx64)*input * m_K1 - (__fx64)m_bx[m_ElemNum1] - (__fx64)m_bx[m_ElemNum2] * m_K2 ) : ( (__fx64)*input * m_K1 + (__fx64)m_bx[m_ElemNum1] - (__fx64)m_bx[m_ElemNum2] * m_K2 ) );
         }

         /*! \brief filter output */
         __fx64 m_out;

    public:

        /*!
         *  \brief comb filter initialization function
         *  \param[ Fs    ] - input signal sampling frequency
         *  \param[ Fn    ] - input signal nominal frequency
         *  \param[ dF    ] - input signal frequency deviation from nominal for which the amplitude frequency response slope compensation is implemented , Hz
         *  \param[ d_Amp ] - residual amplitude frequency response slope for the input signal frequency deviation dF from nominal , p.u.
        */
        void init( __fx64 _Fs, __fx64 _Fn, __fx64 _dF, __fx64 _d_Amp, __ix32 _odd )
        {
            // system parameters:
            m_Fs       = _Fs;
            m_Fn       = _Fn;
            m_dF       = _dF;
            m_d_Amp	   = _d_Amp;
            m_order    = m_Fs / m_Fn / 2;
            m_ElemNum1 = 1 * m_order;
            m_ElemNum2 = 2 * m_order;
            m_Ts       = 1 / m_Fs;
            m_out      = 0;
            m_odd      = _odd;

            // memory allocation function call:
            allocate();
        }

        /*! \brief default constructor */
        fcombeq_abstract() {}

        /*!
         *  \brief comb filter initialization function
         *  \param[ Fs    ] - input signal sampling frequency
         *  \param[ Fn    ] - input signal nominal frequency
         *  \param[ dF    ] - input signal frequency deviation from nominal for which the amplitude frequency response slope compensation is implemented , Hz
         *  \param[ d_Amp ] - residual amplitude frequency response slope for the input signal frequency deviation dF from nominal , p.u.
        */
        fcombeq_abstract( __fx64 _Fs, __fx64 _Fn, __fx64 _dF, __fx64 _d_Amp, __ix32 _odd )
        {
            init( _Fs , _Fn , _dF , _d_Amp , _odd );
        }

        /*! \brief default destructor */
        virtual ~fcombeq_abstract()
        {
            deallocate();
        }

        /*! \brief memory allocation function */
        __ix32 allocate() override
        {
            // auxiliary coefficients:
            __fx64 A =  0.5 - 0.5 * cos(-PI2 * (m_Fn + m_dF) * (__fx64)m_order * m_Ts );
            __fx64 B =  1.0 - cos(-PI2 * (m_Fn + m_dF) * (__fx64)m_order * 2.0 * m_Ts);
            __fx64 C = -0.5 * sin(-PI2 * (m_Fn + m_dF) * (__fx64)m_order * m_Ts);
            __fx64 D = -sin(-PI2 * (m_Fn + m_dF) * (__fx64)m_order * 2.0 * m_Ts);

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

            return m_bx.allocate( m_ElemNum2 + 1 );
        }

        /*! \brief memory deallocation function */
        __ix32 deallocate() override
        {
            m_bx.deallocate();
            return 0;
        }

        /*! \brief frequency response computation function */
        fr frequency_response( __fx64 F ) override
        {
            __fx64 Re = 0;
            __fx64 Im = 0;

            if( !m_odd ) // even filter
            {
                Re = m_K1 - cos(-PI2 * (__fx64)m_order * F * m_Ts) - m_K2 * cos(-PI2 * 2 * (__fx64)m_order * F * m_Ts);
                Im = 0    - sin(-PI2 * (__fx64)m_order * F * m_Ts) - m_K2 * sin(-PI2 * 2 * (__fx64)m_order * F * m_Ts);
            }
            else // odd filter
            {
                Re = m_K1 + cos(-PI2 * (__fx64)m_order * F * m_Ts) - m_K2 * cos(-PI2 * 2 * (__fx64)m_order * F * m_Ts);
                Im = 0    + sin(-PI2 * (__fx64)m_order * F * m_Ts) - m_K2 * sin(-PI2 * 2 * (__fx64)m_order * F * m_Ts);
            }

            return { sqrt(Re * Re + Im * Im) , atan2(Im, Re) };
        }

        /*!
         *  \brief 32-bit floating point filtering operator
         *  \param[input] - pointer to the input signal samples buffer
         *  \return The operatoe calls the function that returns filtering result
        */
        virtual inline __type operator ()( __type *input ) = 0;
    };

    template< typename __type > class derivative_abstract : public filter_abstract
    {
    protected:
        __fx64 m_T1;
        __type *m_cfnum;
        __type *m_cfden;
        __type  m_Gain;
        delay<__type> m_bx;
        delay<__type> m_by;

        // initialization:
        void init( __fx64 Fs , __fx64 Fn , __fx64 Td )
        {
            m_Fs = Fs;
            m_Ts = 1 / Fs;
            m_Fn = Fn;
            m_T1 = Td;

            // memory allocation function call:
            allocate();
        }

        // memory allocation:
        __ix32 allocate() override
        {
            // transfer function input/output and coefficicents buffers memory allocation:
            m_cfnum = ( __type* )calloc( 2 , sizeof ( __type ) );
            m_cfden = ( __type* )calloc( 2 , sizeof ( __type ) );
            m_bx.allocate(3);
            m_by.allocate(3);

            // coefficients and gain computation:
            m_Gain     = 2 / ( m_Ts * ( 1 + 2 * m_T1 / m_Ts ) );
            m_cfnum[0] = +1;
            m_cfnum[1] = -1;
            m_cfden[0] = +1;
            m_cfden[1] = (1 - 2 * m_T1 / m_Ts) / (1 + 2 * m_T1 / m_Ts);

            return 0;
        }

        __ix32 deallocate() override
        {
            if(m_cfnum!=nullptr)
            {
                free(m_cfnum);
                m_cfnum = nullptr;
            }

            if(m_cfden!=nullptr)
            {
                free(m_cfden);
                m_cfden = nullptr;
            }

            m_bx.deallocate();
            m_by.deallocate();

            return 0;
        }

        /*!
         *  \brief Frequency response computation function
         *  \param[_F] input signal frequency, Hz
        */
        fr frequency_response(__fx64 _F ) override
        {
            return __freq_resp__ (m_cfnum, m_cfden, m_Gain, 2, m_Fs, _F );
        }

        // constructors and destructor:
        derivative_abstract() : filter_abstract(){}

        derivative_abstract( __fx64 Fs , __fx64 Fn , __fx64 Td )
        {
            init( Fs , Fn , Td );
        };

        ~derivative_abstract()
        {
            deallocate();
        };

        /*!
         *  \brief 32-bit floating point filtering operator
         *  \param[input] - pointer to the input signal samples buffer
         *  \return The operatoe calls the function that returns filtering result
        */
        virtual inline __type operator ()( __type *input ) = 0;
    };

    template< typename __type > class aperiodic_abstract : public filter_abstract
    {
    protected:
        __fx64 m_T1;
        __type *m_cfnum;
        __type *m_cfden;
        __type  m_Gain;
        delay<__type> m_bx;
        delay<__type> m_by;

        // initialization function:
        void init( __fx64 Fs , __fx64 Fn , __fx64 Ta )
        {
            m_Fs = Fs;
            m_Ts = 1 / Fs;
            m_Fn = Fn;
            m_T1 = Ta;

            // memory allocation function call:
            allocate();
        }

        // memory allocation function:
        __ix32 allocate() override
        {
            // transfer function input/output and coefficicents buffers memory allocation:
            m_cfnum = ( __type* )calloc( 2 , sizeof ( __type ) );
            m_cfden = ( __type* )calloc( 2 , sizeof ( __type ) );
            m_bx.allocate(3);
            m_by.allocate(3);

            // transfer function coefficients:
            m_Gain     = 1 / (1 + 2 * m_T1 / m_Ts);
            m_cfnum[0] = 1;
            m_cfnum[1] = 1;
            m_cfden[0] = 1;
            m_cfden[1] = (1 - 2 * m_T1 / m_Ts) / (1 + 2 * m_T1 / m_Ts);
            return 0;
        }

        __ix32 deallocate() override
        {
            if(m_cfnum!=nullptr)
            {
                free(m_cfnum);
                m_cfnum = nullptr;
            }

            if(m_cfden!=nullptr)
            {
                free(m_cfden);
                m_cfden = nullptr;
            }

            m_bx.deallocate();
            m_by.deallocate();

            return 0;
        }

        /*!
         *  \brief Frequency response computation function
         *  \param[_F] input signal frequency, Hz
        */
        fr frequency_response(__fx64 _F ) override
        {
            return __freq_resp__ (m_cfnum, m_cfden, m_Gain, 2, m_Fs, _F );
        }

        // constructors and destructors:
        aperiodic_abstract() : filter_abstract(){}
        aperiodic_abstract(__fx64 Fs , __fx64 Fn , __fx64 Ta )
        {
            init( Fs , Fn , Ta );
        }

        ~aperiodic_abstract()
        {
            deallocate();
        };

        /*!
         *  \brief 32-bit floating point filtering operator
         *  \param[input] - pointer to the input signal samples buffer
         *  \return The operatoe calls the function that returns filtering result
        */
        virtual inline __type operator ()( __type *input ) = 0;
    };

    template< typename __type > class integrator_abstract : public filter_abstract
    {
    protected:

        __fx64 m_T1;
        __type *m_cfnum;
        __type *m_cfden;
        __type  m_Gain;
        delay<__type> m_bx;
        delay<__type> m_by;

        // initialization function:
        void init( __fx64 Fs , __fx64 Fn )
        {
            m_Fs = Fs;
            m_Ts = 1 / Fs;
            m_Fn = Fn;

            // memory allocation function call:
            allocate();
        }

        // memory allocation function:
        __ix32 allocate() override
        {
            // transfer function input/output and coefficicents buffers memory allocation:
            m_cfnum = ( __type* )calloc( 2 , sizeof ( __type ) );
            m_cfden = ( __type* )calloc( 2 , sizeof ( __type ) );
            m_bx.allocate(3);
            m_by.allocate(3);

            // transfer function coefficients:
            m_Gain = 0.5 * m_Ts;
            m_cfnum[0] = +1;
            m_cfnum[1] = +1;
            m_cfden[0] = +1;
            m_cfden[1] = -1;
            return 0;
        }

        // memory deallocation function:
        __ix32 deallocate() override
        {
            if(m_cfnum!=nullptr)
            {
                free(m_cfnum);
                m_cfnum = nullptr;
            }

            if(m_cfden!=nullptr)
            {
                free(m_cfden);
                m_cfden = nullptr;
            }

            m_bx.deallocate();
            m_by.deallocate();

            return 0;
        }

        /*!
         *  \brief Frequency response computation function
         *  \param[_F] input signal frequency, Hz
        */
        fr frequency_response(__fx64 _F ) override
        {
            return __freq_resp__ (m_cfnum, m_cfden, m_Gain, 2, m_Fs, _F );
        }

        // constructors and destructor:
        integrator_abstract() : filter_abstract(){}

        integrator_abstract( __fx64 _Fs , __fx64 _Fn )
        {
            init(_Fs , _Fn ); allocate();
        }

        ~integrator_abstract()
        {
            deallocate();
        };

        /*!
         *  \brief 32-bit floating point filtering operator
         *  \param[input] - pointer to the input signal samples buffer
         *  \return The operatoe calls the function that returns filtering result
        */
        virtual inline __type operator ()( __type *input ) = 0;
    };

    template< typename __type > class leadlag_abstract : public filter_abstract
    {
    protected:

        __fx64 m_T1;
        __fx64 m_T2;
        __type *m_cfnum;
        __type *m_cfden;
        __type  m_Gain;
        delay<__type> m_bx;
        delay<__type> m_by;

        // initialization function:
        void init( __fx64 _Fs , __fx64 _Fn , __fx64 _T1 , __fx64 _T2 )
        {
            m_Fs = _Fs;
            m_Ts = 1 / _Fs;
            m_Fn = _Fn;
            m_T1 = _T1;
            m_T2 = _T2;

            // memory allocation function call:
            allocate();
        }

        // memory allocation function:
        __ix32 allocate() override
        {
            // transfer function input/output and coefficicents buffers memory allocation:
            m_cfnum = ( __type* )calloc( 2 , sizeof ( __type ) );
            m_cfden = ( __type* )calloc( 2 , sizeof ( __type ) );
            m_bx.allocate(3);
            m_by.allocate(3);

            // transfer function coefficients:
            m_Gain     = (1 + 2 * m_T1 / m_Ts) / (1 + 2 * m_T2 / m_Ts);
            m_cfnum[0] = 1;
            m_cfnum[1] = (1 - 2 * m_T1 / m_Ts) / (1 + 2 * m_T1 / m_Ts);;
            m_cfden[0] = 1;
            m_cfden[1] = (1 - 2 * m_T2 / m_Ts) / (1 + 2 * m_T2 / m_Ts);
            return 0;
        }

        // memory deallocation function:
        __ix32 deallocate() override
        {
            if(m_cfnum!=nullptr)
            {
                free(m_cfnum);
                m_cfnum = nullptr;
            }

            if(m_cfden!=nullptr)
            {
                free(m_cfden);
                m_cfden = nullptr;
            }

            m_bx.deallocate();
            m_by.deallocate();

            return 0;
        }

        /*!
         *  \brief Frequency response computation function
         *  \param[_F] input signal frequency, Hz
        */
        fr frequency_response(__fx64 _F ) override
        {
            return __freq_resp__ (m_cfnum, m_cfden, m_Gain, 2, m_Fs, _F );
        }

        // constructors and destructor:
        leadlag_abstract() : filter_abstract(){}

        leadlag_abstract(__fx64 _Fs , __fx64 _Fn , __fx64 _T1 , __fx64 _T2 )
        {
            init( _Fs , _Fn , _T1 , _T2 );
        }

        ~leadlag_abstract()
        {
            deallocate();
        };

        /*!
         *  \brief 32-bit floating point filtering operator
         *  \param[input] - pointer to the input signal samples buffer
         *  \return The operatoe calls the function that returns filtering result
        */
        virtual inline __type operator ()( __type *input ) = 0;
    };

    template< typename __type > class second_order_filter_abstract : public filter_abstract
    {
    protected:
        __type       *m_cfnum;
        __type       *m_cfden;
        __type        m_Gain;
        __type        m_Fc;
        __type        m_Kd;
        filter_type   m_type;
        delay<__type> m_bx;
        delay<__type> m_by;

        __ix32 allocate() override
        {
            // transfer function input/output and coefficicents buffers memory allocation:
            m_cfnum = ( __type* )calloc( 3 , sizeof ( __type ) );
            m_cfden = ( __type* )calloc( 3 , sizeof ( __type ) );
            m_bx.allocate(4);
            m_by.allocate(4);

            // auxiliary variables:
            __fx64 omega = tan( ( PI2 * m_Fc ) * m_Ts / 2);
            __fx64 a     = 1;
            __fx64 b     = omega / m_Kd;
            __fx64 c     = omega * omega;
            __fx64 k1    = a + b + c;
            __fx64 k2    = 2 * c - 2 * a;
            __fx64 k3    = a - b + c;

            switch ( m_type )
            {
                case filter_type::lowpass:
                    m_Gain     = omega * omega / k1;
                    m_cfnum[0] = 1;
                    m_cfnum[1] = 2;
                    m_cfnum[2] = 1;
                    m_cfden[0] = 1;
                    m_cfden[1] = k2 / k1;
                    m_cfden[2] = k3 / k1;
                break;

                case filter_type::highpass:
                    m_Gain     = 1 / k1;
                    m_cfnum[0] = +1;
                    m_cfnum[1] = -2;
                    m_cfnum[2] = +1;
                    m_cfden[0] = +1;
                    m_cfden[1] = k2 / k1;
                    m_cfden[2] = k3 / k1;
                break;

                case filter_type::bandpass:
                    m_Gain     = omega / m_Kd / k1;;
                    m_cfnum[0] = +1;
                    m_cfnum[1] = -0;
                    m_cfnum[2] = -1;
                    m_cfden[0] = +1;
                    m_cfden[1] = k2 / k1;
                    m_cfden[2] = k3 / k1;
                break;

                case filter_type::bandstop:
                    m_Gain     = 1 / k1;
                    m_cfnum[0] = 1 + omega * omega;
                    m_cfnum[1] = 2 * omega*omega - 2;
                    m_cfnum[2] = 1 + omega * omega;
                    m_cfden[0] = 1;
                    m_cfden[1] = k2 / k1;
                    m_cfden[2] = k3 / k1;
                break;

                default:
                k1 = -cos(2 * PI0 * m_Fc * m_Ts);
                k2 = (1 - tan(PI0 * m_Kd * m_Ts)) / (1 + tan(PI0 * m_Kd * m_Ts));
                m_Gain     = 0.5 * (1 + k2);
                m_cfnum[0] = 1;
                m_cfnum[1] = 2 * k1;
                m_cfnum[2] = 1;
                m_cfden[0] = 1;
                m_cfden[1] = k1 * (1 + k2);
                m_cfden[2] = k2;
                break;
            }
            return 0;
        }

        // memory deallocation function:
        __ix32 deallocate() override
        {
            if(m_cfnum!=nullptr)
            {
                free(m_cfnum);
                m_cfnum = nullptr;
            }

            if(m_cfden!=nullptr)
            {
                free(m_cfden);
                m_cfden = nullptr;
            }

            m_bx.deallocate();
            m_by.deallocate();

            return 0;
        }

        /*!
         *  \brief Frequency response computation function
         *  \param[_F] input signal frequency, Hz
        */
        fr frequency_response(__fx64 _F ) override
        {
            return __freq_resp__ (m_cfnum, m_cfden, m_Gain, 3, m_Fs, _F );
        }

        void init( __fx64 _Fs , __fx64 _Fn , __fx64 _Fc , __fx64 _Kd , filter_type _type )
        {
            m_Fs   = _Fs;
            m_Ts   = 1 / _Fs;
            m_Fn   = _Fn;
            m_Kd   = _Kd;
            m_Fc   = _Fc;
            m_type = _type;

            // memory allocation function call:
            allocate();
        }

        // constructors and destructor:
        second_order_filter_abstract() : filter_abstract(){}

        second_order_filter_abstract(__fx64 _Fs , __fx64 _Fn , __fx64 _Fc , __fx64 _Kd , filter_type _type )
        {
            init( _Fs, _Fn, _Kd, _Fc, _type );
        }

        ~second_order_filter_abstract()
        {
            deallocate();
        }

        /*!
         *  \brief 32-bit floating point filtering operator
         *  \param[input] - pointer to the input signal samples buffer
         *  \return The operatoe calls the function that returns filtering result
        */
        virtual inline __type operator ()( __type *input ) = 0;
    };

    /*! @} */


    /*! \defgroup <DSPB_Implementation> ( Digital Signal Processing implementation )
     *  \ingroup DSPB
     *  \brief The module contains implementations of Digital Signal Processing base models
        @{
    */


    /*! \defgroup <DSPB_ImplementationContent> (Group of filters to implement within the DSP basics context )
     *  \ingroup DSPB_Implementation
     *  \brief The module contains the schedule of the filters that are implemented within the DSP basics
     *         context
        @{
    */

    /*! \brief Child Butterworth IIR filter template class */
    template< typename __type > class butterworth ;
    /*! \brief Child Chebyshev type I IIR filter template class */
    template< typename __type > class chebyshev_1;
    /*! \brief Child Chebyshev type II IIR filter template class */
    template< typename __type > class chebyshev_2;
    /*! \brief Child Elliptic IIR filter template class */
    template< typename __type > class elliptic;
    /*! \brief Child windowed FIR filter template class */
    template< typename __type > class fir;
    /*! \brief Child recursive Fourier filter template class */
    template< typename __type > class recursive_fourier;
    /*! \brief Child comb filter template class */
    template< typename __type > class fcomb;
    /*! \brief Child combeq filter template class */
    template< typename __type > class fcombeq;
    /*! \brief Child derivative transfer function */
    template< typename __type > class derivative;
    /*! \brief Child aperiodic transfer function */
    template< typename __type > class aperiodic;
    /*! \brief Child integrator transfer function */
    template< typename __type > class integrator;
    /*! \brief Child leadlag transfer function */
    template< typename __type > class leadlag;
    /*! \brief Child second order filter transfer function */
    template< typename __type > class second_order_filter;

    /*! @} */


    /*! \defgroup <DSPB_ImplementationIIR> (IIR filters implementation )
     *  \ingroup DSPB_Implementation
     *  \brief The module contains IIR filters fimplementation
        @{
    */

    // butterworth filter 32-bit realization:
    template<> class butterworth< __fx32 > final : public classic_iir_abstract< __fx32 >
    {
            typedef __fx32 __type;
        public:
        // constructors:
         butterworth< __type >() : classic_iir_abstract< __type >(){}
         butterworth< __type >( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth , attenuation _attenuation )
             : classic_iir_abstract< __type >( _Fs , _order , _type , _bandwidth , _attenuation ) {}
        ~butterworth< __type >(){};

        // base class virtual functions overriding:
        cf< __type > compute_lowpass () override { return __butt_cheb1_digital_lp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 0 , m_attenuation.G1 ); }
        cf< __type > compute_highpass() override { return __butt_cheb1_digital_hp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 0 , m_attenuation.G1 ); }
        cf< __type > compute_bandpass() override { return __butt_cheb1_digital_bp__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 0 , m_attenuation.G1 ); }
        cf< __type > compute_bandstop() override { return __butt_cheb1_digital_bs__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 0 , m_attenuation.G1 ); }

        // filtering operator override:
        inline __type operator()( __type* _input ) override
        {
            return __filt__< __type >( _input , m_cfmatrix.cfnum , m_cfmatrix.cfden , m_cfmatrix.gains  , m_cfmatrix.N ,  m_buff_sx , m_buff_sy );
        }
    };

    // butterworth filter 64-bit realization:
    template<> class butterworth< __fx64 > final : public classic_iir_abstract< __fx64 >
    {
            typedef __fx64 __type;
        public:
        // constructors:
        butterworth< __type >() : classic_iir_abstract< __type >(){}
        butterworth< __type >( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth , attenuation _attenuation )
            : classic_iir_abstract< __type >( _Fs , _order , _type , _bandwidth , _attenuation ) {}
       ~butterworth< __type >(){};

        // base class virtual functions overriding:
        cf< __type > compute_lowpass () override { return __butt_cheb1_digital_lp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 0 , m_attenuation.G1 ); }
        cf< __type > compute_highpass() override { return __butt_cheb1_digital_hp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 0 , m_attenuation.G1 ); }
        cf< __type > compute_bandpass() override { return __butt_cheb1_digital_bp__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 0 , m_attenuation.G1 ); }
        cf< __type > compute_bandstop() override { return __butt_cheb1_digital_bs__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 0 , m_attenuation.G1 ); }

        // filtering operator override:
        inline __type operator()( __type* _input ) override
        {
            return __filt__< __type >( _input , m_cfmatrix.cfnum , m_cfmatrix.cfden , m_cfmatrix.gains  , m_cfmatrix.N ,  m_buff_sx , m_buff_sy );
        }
    };

    // Chebyshev I filter 32-bit realization:
    template<> class chebyshev_1< __fx32 > final : public classic_iir_abstract< __fx32 >
    {
            typedef __fx32 __type;
        public:
        // constructors:
         chebyshev_1< __type >() : classic_iir_abstract< __type >(){}
         chebyshev_1< __type >( __fx64 _Fs , __ix32 _order , filter_type _type, bandwidth _bandwidth , attenuation _attenuation )
             : classic_iir_abstract< __type >(_Fs, _order, _type, _bandwidth, _attenuation ) {}
        ~chebyshev_1< __type >(){};

        // base class virtual functions overriding:
        cf< __type > compute_lowpass () override { return __butt_cheb1_digital_lp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 1 , m_attenuation.G1 ); }
        cf< __type > compute_highpass() override { return __butt_cheb1_digital_hp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 1 , m_attenuation.G1 ); }
        cf< __type > compute_bandpass() override { return __butt_cheb1_digital_bp__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 1 , m_attenuation.G1 ); }
        cf< __type > compute_bandstop() override { return __butt_cheb1_digital_bs__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 1 , m_attenuation.G1 ); }

        // filtering operator override:
        inline __type operator()( __type *input ) override { return __filt__< __type >( input , m_cfmatrix.cfnum , m_cfmatrix.cfden , m_cfmatrix.gains  , m_cfmatrix.N ,  m_buff_sx , m_buff_sy ); }
    };

    // Chebyshev I filter 64-bit realization:
    template<> class chebyshev_1< __fx64 > final : public classic_iir_abstract< __fx64 >
    {
            typedef __fx64 __type;
        public:
        // constructors:
        chebyshev_1< __type >() : classic_iir_abstract< __type >(){}
        chebyshev_1< __type >( __fx64 _Fs , __ix32 _order , filter_type _type, bandwidth _bandwidth , attenuation _attenuation )
            : classic_iir_abstract< __type >(_Fs, _order, _type, _bandwidth, _attenuation ) {}
       ~chebyshev_1< __type >(){};

        // base class virtual functions overriding:
        cf< __type > compute_lowpass () override { return __butt_cheb1_digital_lp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 1 , m_attenuation.G1 ); }
        cf< __type > compute_highpass() override { return __butt_cheb1_digital_hp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 1 , m_attenuation.G1 ); }
        cf< __type > compute_bandpass() override { return __butt_cheb1_digital_bp__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 1 , m_attenuation.G1 ); }
        cf< __type > compute_bandstop() override { return __butt_cheb1_digital_bs__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 1 , m_attenuation.G1 ); }

        // filtering operator override:
        inline __type operator()( __type* _input ) override
        {
            return __filt__< __type >( _input , m_cfmatrix.cfnum , m_cfmatrix.cfden , m_cfmatrix.gains  , m_cfmatrix.N ,  m_buff_sx , m_buff_sy );
        }
    };

    // Chebyshev II filter 32-bit realization:
    template<> class chebyshev_2< __fx32 > final : public classic_iir_abstract< __fx32 >
    {
            typedef __fx32 __type;
        public:
        // constructors:
         chebyshev_2< __type >() : classic_iir_abstract< __type >(){}
         chebyshev_2< __type >( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, attenuation _attenuation )
             : classic_iir_abstract< __type >( _Fs, _order, _type, _bandwidth, _attenuation ) {}
        ~chebyshev_2< __type >(){};

        // base class virtual functions overriding:
        cf< __type > compute_lowpass () override { return __cheb2_ellip_digital_lp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 0 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_highpass() override { return __cheb2_ellip_digital_hp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 0 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_bandpass() override { return __cheb2_ellip_digital_bp__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 0 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_bandstop() override { return __cheb2_ellip_digital_bs__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 0 , m_attenuation.G2 , m_attenuation.G1 ); }

        // filtering operator override:
        inline __type operator()( __type* _input ) override
        {
            return __filt__< __type >(_input , m_cfmatrix.cfnum , m_cfmatrix.cfden , m_cfmatrix.gains  , m_cfmatrix.N ,  m_buff_sx , m_buff_sy );
        }
    };

    // Chebyshev II filter 64-bit realization:
    template<> class chebyshev_2< __fx64 > final : public classic_iir_abstract< __fx64 >
    {
            typedef __fx64 __type;
        public:
        // constructors:
         chebyshev_2< __type >() : classic_iir_abstract< __type >(){}
         chebyshev_2< __type >( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, attenuation _attenuation )
             : classic_iir_abstract< __type >( _Fs, _order, _type, _bandwidth, _attenuation ) {}
        ~chebyshev_2< __type >(){};

        // base class virtual functions overriding:
        cf< __type > compute_lowpass () override { return __cheb2_ellip_digital_lp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 0 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_highpass() override { return __cheb2_ellip_digital_hp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 0 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_bandpass() override { return __cheb2_ellip_digital_bp__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 0 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_bandstop() override { return __cheb2_ellip_digital_bs__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 0 , m_attenuation.G2 , m_attenuation.G1 ); }

        // filtering operator override:
        inline __type operator()( __type* _input ) override
        {
            return __filt__< __type >(_input , m_cfmatrix.cfnum , m_cfmatrix.cfden , m_cfmatrix.gains  , m_cfmatrix.N ,  m_buff_sx , m_buff_sy );
        }
    };

    // Elliptic filter 32-bit realization:
    template<> class elliptic< __fx32 > final : public classic_iir_abstract< __fx32 >
    {
            typedef __fx32 __type;
        public:
        // constructors:
         elliptic< __type >() : classic_iir_abstract< __type >(){}
         elliptic< __type >( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, attenuation _attenuation )
             : classic_iir_abstract< __type >( _Fs, _order, _type, _bandwidth, _attenuation ) {}
        ~elliptic< __type >(){};

        // base class virtual functions overriding:
        cf< __type > compute_lowpass () override { return __cheb2_ellip_digital_lp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 1 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_highpass() override { return __cheb2_ellip_digital_hp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 1 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_bandpass() override { return __cheb2_ellip_digital_bp__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 1 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_bandstop() override { return __cheb2_ellip_digital_bs__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 1 , m_attenuation.G2 , m_attenuation.G1 ); }

        // filtering operator override:
        inline __type operator()( __type* _input ) override
        {
            return __filt__< __type >( _input , m_cfmatrix.cfnum , m_cfmatrix.cfden , m_cfmatrix.gains  , m_cfmatrix.N ,  m_buff_sx , m_buff_sy );
        }
    };

    // Elliptic filter 64-bit realization:
    template<> class elliptic< __fx64 > final : public classic_iir_abstract< __fx64 >
    {
            typedef __fx64 __type;
        public:
        // constructors:
         elliptic< __type >() : classic_iir_abstract< __type >(){}
         elliptic< __type >( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, attenuation _attenuation )
             : classic_iir_abstract< __type >( _Fs, _order, _type, _bandwidth, _attenuation ) {}
        ~elliptic< __type >(){};

        // base class virtual functions overriding:
        cf< __type > compute_lowpass () override { return __cheb2_ellip_digital_lp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 1 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_highpass() override { return __cheb2_ellip_digital_hp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 1 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_bandpass() override { return __cheb2_ellip_digital_bp__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 1 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_bandstop() override { return __cheb2_ellip_digital_bs__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 1 , m_attenuation.G2 , m_attenuation.G1 ); }

        // filtering operator override:
        inline __type operator()( __type* _input ) override
        {
            return __filt__< __type >( _input , m_cfmatrix.cfnum , m_cfmatrix.cfden , m_cfmatrix.gains  , m_cfmatrix.N ,  m_buff_sx , m_buff_sy );
        }
    };

    /*! @} */

    /*! \defgroup <DSPB_ImplementationFIR> (FIR filters implementation )
     *  \ingroup DSPB_Implementation
     *  \brief The module contains FIR filters fimplementation
        @{
    */

    // FIR filter 32-bit realization:
    template<> class fir< __fx32 > final : public classic_fir_abstract< __fx32 >
    {
        typedef __fx32 __type;
    public:
        fir() : classic_fir_abstract< __type >(){}
        fir( __fx64 _Fs , __ix32 _order, filter_type _type, bandwidth _bandwidth, __fx64* _window, bool _scale )
            : classic_fir_abstract< __type >(_Fs, _order, _type, _bandwidth, _window, _scale ){}
       ~fir() {}
        inline __type operator()( __type*  _input ) override { return __filt__< __type , __type >( _input, m_coeffs, m_buff_sx, m_order ); }
        inline __type operator()( __fx64*  _input ) { return __filt__< __fx64  , __type >( _input, m_coeffs, m_buff_sx, m_order ); }
        inline __type operator()( __fxx64* _input ) { return __filt__< __fxx64 , __type >( _input, m_coeffs, m_buff_sx, m_order ); }

        inline __type operator()() override { return __filt__ < __type > ( m_coeffs , m_buff_sx , m_order ); }
        inline void operator << ( __type*  _input ) override { m_buff_sx(_input ); }
        inline void operator << ( __fx64*  _input ) { m_buff_sx(_input ); }
        inline void operator << ( __fxx64* _input ) { m_buff_sx(_input ); }
    };

    // FIR filter 64-bit realization:
    template<> class fir< __fx64 > final : public classic_fir_abstract< __fx64 >
    {
        typedef __fx64 __type;
    public:
        fir() : classic_fir_abstract< __type >(){}
        fir( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth , __fx64* _window , bool _scale )
            : classic_fir_abstract< __type >( _Fs , _order , _type , _bandwidth , _window , _scale ){}
       ~fir() {}
        inline __type operator()( __type*  _input ) override { return __filt__< __type , __type >( _input, m_coeffs, m_buff_sx, m_order ); }
        inline __type operator()( __fxx64* _input ) { return __filt__< __fxx64 , __type >( _input, m_coeffs, m_buff_sx, m_order ); }

        inline __type operator()() override { return __filt__ < __type > ( m_coeffs , m_buff_sx , m_order ); }
        inline void operator << ( __type*  _input ) override { m_buff_sx(_input ); }
        inline void operator << ( __fxx64* _input ) { m_buff_sx(_input ); }
    };

    /*! @} */

    /*! \defgroup <DSPB_ImplementationRecursiveFIR> (Recursive FIR filters implementation )
     *  \ingroup DSPB_Implementation
     *  \brief The module contains FIR filters fimplementation
        @{
    */

    /*! \brief Child recursive Fourier filter 32-bit realization */
    template<> class recursive_fourier<__fx32> final : public recursive_fir_abstract<__fx32>
    {
        typedef __fx32 __type;
    public:
        // construction / destruction:
        recursive_fourier() : recursive_fir_abstract(){}
        recursive_fourier(__fx64 _Fs, __fx64 _Fn, __ix32 _hnum, __ix32 _order_multiplier = 1 )
            : recursive_fir_abstract(_Fs, _Fn, _hnum, _order_multiplier){}
        ~recursive_fourier(){}

        // filtering
        inline harmonic operator ()(__type  *input ) override { return filt<__type> ( input ); }
        inline harmonic operator ()(__fx64  *input ){ return filt<__fx64> ( input ); }
        inline harmonic operator ()(__fxx64 *input ){ return filt<__fxx64>( input ); }
    };

    /*! \brief Child recursive Fourier filter 32-bit realization */
    template<> class recursive_fourier<__fx64> final : public recursive_fir_abstract<__fx64>
    {
        typedef __fx64 __type;
    public:
        // construction / destruction:
        recursive_fourier() : recursive_fir_abstract(){}
        recursive_fourier(__fx64 _Fs, __fx64 _Fn, __ix32 _hnum, __ix32 _order_multiplier = 1 )
            : recursive_fir_abstract(_Fs, _Fn, _hnum, _order_multiplier){}
        ~recursive_fourier(){}

        // filtering
        inline harmonic operator ()(__type  *input ) override { return filt<__type> ( input ); }
        inline harmonic operator ()(__fxx64 *input ){ return filt<__fxx64>( input ); }
    };

    /*! @} */

    /*! \defgroup <DSPB_ImplementationComb> (Comb filters implementation )
     *  \ingroup DSPB_Implementation
     *  \brief The module contains Comb FIR filters fimplementation
        @{
    */

    /*! \brief Child comb FIR filter 32-bit rea;ization */
    template<> class fcomb<__fx32> final : public fcomb_abstract<__fx32>
    {
        typedef __fx32 __type;
    public:
        fcomb() : fcomb_abstract(){}
        fcomb( __fx64 _Fs, __fx64 _Fn , __ix32 _odd ) : fcomb_abstract(_Fs, _Fn , _odd ){}
        ~fcomb(){}

        inline __type operator ()( __type  *input ) override { return filt<__type>(input); }
        inline __type operator ()( __fx64  *input ) { return filt<__fx64 >(input); }
        inline __type operator ()( __fxx64 *input ) { return filt<__fxx64>(input); }

    };

    /*! \brief Child comb FIR filter 64-bit realization */
    template<> class fcomb<__fx64> final : public fcomb_abstract<__fx64>
    {
        typedef __fx64 __type;
    public:
        fcomb() : fcomb_abstract(){}
        fcomb( __fx64 _Fs, __fx64 _Fn , __ix32 _odd ) : fcomb_abstract(_Fs, _Fn , _odd ){}
        ~fcomb(){}

        inline __type operator ()( __type  *input ) override { return filt<__type>(input); }
        inline __type operator ()( __fxx64 *input ) { return filt<__fxx64>(input); }

    };

    /*! \brief Child equilized comb FIR filter 32-bit realization */
    template<> class fcombeq<__fx32> final : public fcombeq_abstract<__fx32>
    {
        typedef __fx32 __type;
    public:
        fcombeq() : fcombeq_abstract(){}
        fcombeq( __fx64 _Fs, __fx64 _Fn, __fx64 _dF, __fx64 _d_Amp, __ix32 _odd )
            : fcombeq_abstract(_Fs, _Fn, _dF, _d_Amp, _odd ){}
        ~fcombeq(){}

        inline __type operator ()( __type  *input ) override { return filt<__type>(input); }
        inline __type operator ()( __fx64  *input ) { return filt<__fx64 >(input); }
        inline __type operator ()( __fxx64 *input ) { return filt<__fxx64>(input); }

    };

    /*! \brief Child equilized comb FIR filter 64-bit realization */
    template<> class fcombeq<__fx64> final : public fcombeq_abstract<__fx64>
    {
        typedef __fx64 __type;
    public:
        fcombeq() : fcombeq_abstract(){}
        fcombeq( __fx64 _Fs, __fx64 _Fn, __fx64 _dF, __fx64 _d_Amp, __ix32 _odd )
            : fcombeq_abstract(_Fs, _Fn, _dF, _d_Amp, _odd ){}
        ~fcombeq(){}

        inline __type operator ()( __type  *input ) override { return filt<__type>(input); }
        inline __type operator ()( __fxx64 *input ) { return filt<__fxx64>(input); }
    };

    /*! @} */


    //------------------------------------------------------------------------------------------------------------------------------------------
    template<> class derivative<__fx32> final : public derivative_abstract<__fx32>
    {
        typedef __fx32 __type;
        derivative() : derivative_abstract(){}
        derivative(__fx64 _Fs , __fx64 _Fn , __fx64 _Td ) : derivative_abstract( _Fs , _Fn , _Td ){}
        ~derivative();

        inline __type operator ()( __type  *input ) override
        {
            return __filt__<__type>(input, m_cfnum, m_cfden, m_Gain, 2, 1, m_bx, m_by );
        }
    };

    template<> class derivative<__fx64> final : public derivative_abstract<__fx64>
    {
        typedef __fx64 __type;
        derivative() : derivative_abstract(){}
        derivative(__fx64 _Fs , __fx64 _Fn , __fx64 _Td )
            : derivative_abstract( _Fs , _Fn , _Td ){}
        ~derivative();

        inline __type operator ()( __type *input ) override
        {
            return __filt__<__type>(input, m_cfnum, m_cfden, m_Gain, 2, 1, m_bx, m_by );
        }
    };

    template<> class integrator<__fx32> final : public integrator_abstract<__fx32>
    {
        typedef __fx32 __type;
        integrator() : integrator_abstract(){}
        integrator(__fx64 _Fs , __fx64 _Fn ) : integrator_abstract( _Fs , _Fn ){}
        ~integrator();

        inline __type operator ()( __type  *input ) override
        {
            return __filt__<__type>(input, m_cfnum, m_cfden, m_Gain, 2, 1, m_bx, m_by );
        }
    };

    template<> class integrator<__fx64> final : public integrator_abstract<__fx64>
    {
        typedef __fx64 __type;
        integrator() : integrator_abstract(){}
        integrator(__fx64 _Fs , __fx64 _Fn ) : integrator_abstract( _Fs , _Fn ){}
        ~integrator();

        inline __type operator ()( __type  *input ) override
        {
            return __filt__<__type>(input, m_cfnum, m_cfden, m_Gain, 2, 1, m_bx, m_by );
        }
    };

    template<> class leadlag<__fx32> final : public leadlag_abstract<__fx32>
    {
        typedef __fx32 __type;
        leadlag() : leadlag_abstract(){}
        leadlag(__fx64 _Fs, __fx64 _Fn, __fx64 _T1, __fx64 _T2 ) : leadlag_abstract(_Fs, _Fn, _T1, _T2 ){}
        ~leadlag();

        inline __type operator ()( __type  *input ) override
        {
            return __filt__<__type>(input, m_cfnum, m_cfden, m_Gain, 2, 1, m_bx, m_by );
        }
    };

    template<> class leadlag<__fx64> final : public leadlag_abstract<__fx64>
    {
        typedef __fx64 __type;
        leadlag() : leadlag_abstract(){}
        leadlag(__fx64 _Fs, __fx64 _Fn, __fx64 _T1, __fx64 _T2 ) : leadlag_abstract(_Fs, _Fn, _T1, _T2 ){}
        ~leadlag();

        inline __type operator ()( __type  *input ) override
        {
            return __filt__<__type>(input, m_cfnum, m_cfden, m_Gain, 2, 1, m_bx, m_by );
        }
    };

    template<> class second_order_filter<__fx32> final : public second_order_filter_abstract<__fx32>
    {
        typedef __fx32 __type;
        second_order_filter() : second_order_filter_abstract(){}
        second_order_filter(__fx64 _Fs , __fx64 _Fn , __fx64 _Fc , __fx64 _Kd , filter_type _type)
            : second_order_filter_abstract(_Fs , _Fn, _Fc, _Kd, _type ){}
        ~second_order_filter();

        inline __type operator ()( __type  *input ) override
        {
            return __filt__<__type>(input, m_cfnum, m_cfden, m_Gain, 3, 2, m_bx, m_by );
        }
    };

    template<> class second_order_filter<__fx64> final : public second_order_filter_abstract<__fx64>
    {
        typedef __fx64 __type;
        second_order_filter() : second_order_filter_abstract(){}
        second_order_filter(__fx64 _Fs , __fx64 _Fn , __fx64 _Fc , __fx64 _Kd , filter_type _type)
            : second_order_filter_abstract(_Fs , _Fn, _Fc, _Kd, _type ){}
        ~second_order_filter();

        inline __type operator ()( __type  *input ) override
        {
            return __filt__<__type>(input, m_cfnum, m_cfden, m_Gain, 3, 2, m_bx, m_by );
        }
    };
    //------------------------------------------------------------------------------------------------------------------------------------------


    /*! @} */

};

/*! @} */

#endif // DSP_H
