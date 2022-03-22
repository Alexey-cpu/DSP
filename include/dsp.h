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
        bandstop,
        other
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
    template<typename __type> cf<__type> __cf_free__( cf<__type> _cfmatrix )
    {
        if( _cfmatrix.cfden != nullptr ) free( _cfmatrix.cfden );
        if( _cfmatrix.cfnum != nullptr ) free( _cfmatrix.cfnum );
        if( _cfmatrix.gains != nullptr ) free( _cfmatrix.gains );
        return { nullptr , nullptr , nullptr , -1 , -1 , -1 };
    }

    /*!
     * \brief Biquadratic form IIR filter frequency response computation function
     * \param[ _cfnum ] IIR filter numerator quadratic sections coefficients matrix
     * \param[ _cfden ] IIR filter denominator quadratic sections coefficients matrix
     * \param[ _gains ] IIR filter quadratic sections gains vector
     * \param[ _N     ] IIR filter filter order
     * \param[ _Fs    ] sampling frequency, Hz
     * \param[ _F     ] input signal frequency, Hz
     * \return The function computes IIR filter transfer function frequency response:
     * \f[
     *      W(j*2*\pi*f) = \prod_{i=0}^N \frac{ a_{0i} + a_{1i} * e^{-j*2*\pi*f*Ts} + a_{2i} * e^{-j*4*\pi*f*Ts} }
     *                                        { b_{0i} + b_{1i} * e^{-j*2*\pi*f*Ts} + b_{2i} * e^{-j*4*\pi*f*Ts} }
     * \f]
    */
    template<typename __type> fr __freq_resp__
    (
            __type *_cfnum ,
            __type *_cfden ,
            __type *_gains ,
            __ix32 _N ,
            __fx64 _Fs ,
            __fx64 _F
    )
    {
        // check the input:
        if( !_cfnum || !_cfden || !_gains ) return { -1 , -1 };

        // sampling period:
        __fx64 Ts = 1 / _Fs;

        // transfer function initialization:
        fcomplex< __fx64 > tsf = fcomplex< __fx64 >( 1, 0 );

        for( __ix32 i = 0 ; i < _N ; i++ )
        {
            // fcomplex exponents:
            fcomplex< __fx64 > z0 = fcomplex<__fx64>( cos( -PI2 * _F * Ts * 0 ) , sin( -PI2 * _F * Ts * 0 ) );
            fcomplex< __fx64 > z1 = fcomplex<__fx64>( cos( -PI2 * _F * Ts * 1 ) , sin( -PI2 * _F * Ts * 1 ) );
            fcomplex< __fx64 > z2 = fcomplex<__fx64>( cos( -PI2 * _F * Ts * 2 ) , sin( -PI2 * _F * Ts * 2 ) );

            // transfer function:
            fcomplex< __fx64 > num =  z0 * (__fx64)_cfnum[ 3 * i + 0 ] + z1 * (__fx64)_cfnum[ 3 * i + 1 ] + z2 * (__fx64)_cfnum[ 3 * i + 2 ];
            fcomplex< __fx64 > den =  z0 * (__fx64)_cfden[ 3 * i + 0 ] + z1 * (__fx64)_cfden[ 3 * i + 1 ] + z2 * (__fx64)_cfden[ 3 * i + 2 ];
            tsf *= num / den * (__fx64)_gains[i];
        }

        // multiply by an output gain:
        tsf *= (__fx64)_gains[_N];

        // output:
        return fr{ __cabsf__<__fx64>( tsf ) , __cargf__<__fx64>( tsf ) };
    }


    /*!
     * \brief  Polynominal IIR filter frequency response computation function
     * \param[ cfnum ] IIR filter numerator quadratic sections coefficients matrix
     * \param[ cfden ] IIR filter denominator quadratic sections coefficients matrix
     * \param[ gains ] IIR filter quadratic sections gains vector
     * \param[ N     ] IIR filter filter order
     * \param[ Fs    ] sampling frequency, Hz
     * \param[ F     ] input signal frequency, Hz
     * \return The function computes IIR filter transfer function frequency response:
     * \f[
     *      W(j*2*\pi*f) = \frac{ \sum_{i=0}^N a_{i} * e^{-j*2*\pi*f*Ts } }
     *                          { \sum_{k=0}^M b_{k} * e^{-j*2*\pi*f*Ts s} }
     * \f]
    */
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
     * \brief FIR filter frequency response computation function
     * \param[ cfbuff ] FIR filter coefficients buffer
     * \param[ N      ] FIR filter order
     * \param[ Fs     ] sampling frequency
     * \param[ F      ] input signal frequency
     * \f[
     *      W(j*2*\pi*f) = \sum_{i=0}^N a_{i} * e^{-j*2*\pi*f*Ts }
     * \f]
    */
    template< typename __type > fr __freq_resp__
    (
            __type *_cfbuff ,
            __ix32 _N,
            __fx64 _Fs,
            __fx64 _F
    )
    {
        __type           Ts = 1 / _Fs;
        fcomplex<__fx64> Wz(0,0);
        for ( __ix32 i = 0; i < _N; i++) Wz += fcomplex<__fx64>(cos(-PI2 * i * _F * Ts), sin(-PI2 * i * _F * Ts))*_cfbuff[ i ];
        return { __cabsf__<__fx64>(Wz) , __cargf__<__fx64>(Wz) };
    }

    /*!
     * \brief Biquadratic IIR filter filtering function
     * \param[ *_input   ] pointer to the input signal samples buffer
     * \param[ *_cfnum   ] IIR filter numerator quadratic sections coefficients matrix
     * \param[ *_cfden   ] IIR filter denominator quadratic sections coefficients matrix
     * \param[ *_gains   ] IIR filter quadratic sections gains vector
     * \param[ _N        ] IIR filter filter order
     * \param[ *_buff_sx ] IIR filter quadratic sections input  buffers vector
     * \param[ *_buff_sy ] IIR filter quadratic sections output buffers vector
     * \returns   The function implements input signal filtering using
     *           IIR filter coefficients matrix, gains and I/O buffers vectors.
     *           The input signal is going trough the group of the second order filters cascade:
     *           \f[
     *
     *              y_{0}(k) = gain_{0} * [ a_{00} * x(k-1) + a_{10} * x(k-2) + a_{20} * x(k-3) ] -
     *                      [ b_{10} * y_{0}(k-1)+ b_{20} * y_{0}(k-2) ] \newline
     *              y_{1}(k) = gain_{1} * [ a_{0i} * y_{0}(k-1) + a_{11} * y_{0}(k-2) + a_{21} * y_{0}(k-3) ] -
     *                      [ b_{11} * y_{1}(k-1) + b_{21} * y_{1}(k-2) ] \newline
     *                      \dots \dots \dots \dots \dots \newline
     *              y_{n}(k) = gain_{1} * [ a_{0i} * y_{n-1}(k-1) + a_{11} * y_{n-1}(k-2) + a_{21} * y_{n-1}(k-3) ] -
     *                      [ b_{11} * y_{n}(k-1) + b_{21} * y_{n}(k-2) ]
     *           \f]
    */
    template< typename __type >
    inline __type __filt__
    (
            __type *_input,
            __type *_cfnum,
            __type *_cfden,
            __type *_gains,
            __ix32 _N,
            delay< __type > *_buff_sx,
            delay< __type > *_buff_sy
    )
    {
        if( !_cfnum || !_cfden || !_gains || !_input || !_buff_sx || !_buff_sy || (_N < 0 ) ) return 0;

        // initialization:
        __type sum_num = 0;
        __type sum_den = 0;
        __type out     = 0;

        // filtering:
        _buff_sx[0](_input );
        for ( __ix32 i = 0 ; i < _N ; i++)
        {
            sum_num = _gains[i]*( _buff_sx[ i ][ 0 ] * _cfnum[ 3 * i + 0 ] + _buff_sx[ i ][ 1 ] * _cfnum[ 3 * i + 1 ] + _buff_sx[ i ][ 2 ] * _cfnum[ 3 * i + 2 ] );
            sum_den = ( _buff_sy[ i ][ 0 ] * _cfden[ 3 * i + 1 ] + _buff_sy[ i ][ 1 ] * _cfden[ 3 * i + 2 ] );
            _buff_sy[ i ]( &( out = sum_num - sum_den ) );
            if( i < _N - 1 ) _buff_sx[ i + 1 ]( &out );
        }
        return out;
    }

    /*!
     * \brief Unrwapped IIR filter filtering function
     * \param[ *input   ] pointer to the input signal samples buffer
     * \param[ *cfnum   ] IIR filter numerator quadratic sections coefficients vector
     * \param[ *cfden   ] IIR filter denominator quadratic sections coefficients vector
     * \param[ *gains   ] IIR filter quadratic sections gains vector
     * \param[ Nx       ] IIR filter numerator order
     * \param[ Nx       ] IIR filter denominator order
     * \param[ *buff_sx ] IIR filter quadratic sections input  buffers vector
     * \param[ *buff_sy ] IIR filter quadratic sections output buffers vector
     * \returns   The function implements input signal filtering using
     *           IIR filter coefficients, gains and I/O buffers vectors
     *
     *           \f[
     *              y(k) = \left( \sum_{i=0}^N gain_{i} * [ a_{0i} * x(k-1) + a_{1i} * x(k-2) + a_{1i} * x(k-3) ] \right) -
     *                     \left( \sum_{i=0}^N b_{1i} * y(k-1) + b_{2i} * y(k-2) \right)
     *           \f]
    */
    template< typename __type >
    inline __type __filt__
    (
            __type *_input,
            __type *_cfnum,
            __type *_cfden,
            __type _gain,
            __ix32 _Nx,
            __ix32 _Ny,
            delay<__type> &_bx,
            delay<__type> &_by
    )
    {
        __type sum_num = 0 , sum_den = 0 , out = 0;
        _bx( _input );
        for ( __ix32 m = 0 ; m < _Nx ; m++)
        {
            sum_num += _gain * _bx[m] * _cfnum[m];
            if ( m < _Ny ) sum_den += _by[m] * _cfden[m + 1];
        }
        _by( &( out = sum_num - sum_den ) );
        return out;
    }

    /*!
     * \brief FIR filter filtering function
     * \param[ *_input   ] pointer to the input signal samples buffer
     * \param[ *_coeffs  ] FIR filter coefficitns vector
     * \param[ *_buff_sx ] FIR filter buffer
     * \param[ _N        ] FIR filter filter order
     * \return   The function implements input signal filtering using
     *           FIR filter coefficients vector and buffer:
     *           \f[
     *              y(k) = \sum_{i=0}^N a_{i} * x_{k-i}
     *           \f]
    */
    template< typename CoefficientsType , typename BufferType >
    inline  BufferType __filt__
    (
            CoefficientsType    *_input,
            BufferType          *_coeffs,
            delay< BufferType > &_buff_sx,
            __ix32 _N
    )
    {
        _buff_sx( _input );
        BufferType out = 0;
        for ( __ix32 n = _N-1 ; n >= 0; n--) out += _buff_sx[ n ] * _coeffs[n];
        return out;
    }

    /*!
     * \brief FIR filter filtering function
     * \param[ *_coeffs  ] FIR filter coefficitns vector
     * \param[ *_buff_sx ] FIR filter buffer
     * \param[ _N        ] FIR filter filter order
     * \return   The function implements input signal filtering using
     *           FIR filter coefficients vector and ALREADY FILLED buffer
     *           \f[
     *              y(k) = \sum_{i=0}^N a_{i} * x_{k-i}
     *           \f]
    */
    template< typename __type >
    inline  __type __filt__
    (
            __type *_coeffs,
            delay< __type > &_buff_sx ,
            __ix32 _N
    )
    {
        __type out = 0;
        for ( __ix32 n = _N-1 ; n >= 0; n--) out += _buff_sx[ n ] * _coeffs[n];
        return out;
    }

    #ifndef __ALG_PLATFORM
    /*!
     *  \brief FIR filter display function
     *  \param[ _coeffs ] FIR filter coefficients vector
     *  \param[ _N      ] FIR filter order
     *  \param[ _type   ] FIR filter type (lowpass, highpass, bandpass )
     */
    template<typename __type>
    void __show__
    (
            __type _coeffs,
            __ix32 _N,
            filter_type _type
    )
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

                case filter_type::other:
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
    template < typename __type >
    void __show__
    (
            cf< __type > _coeffs,
            filter_type _type
    )
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

                case filter_type::other:
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
             printf("The filter has been destryed or it's coeffcients have not been computed yet...\n");
        }
    }

    #endif

    /*!
     * \brief Butterworth lowpass analogue prototype zeros/poles plain computation function
     * \param[_g_stop] stopband attenuation, Db
     * \param[_order]  filter order
     * \return   The function computes zeros/poles pairs of the Butterworth lowpass
     *           analogue prototype represented as:
     *
     *           \f[
     *              W(s) = \frac{ 1 }{ ( s - p_r ) } * \sum_{i=0}^L \frac{ 1 }{ ( s - p_i ) * ( s - conj(p_i) ) }
     *           \f]
     *
     *           All the data is stored within zp data structure and returned.
     *           The computation is implemented according the followoing formulas:
     *
     *           Complex conjugate pairs:
     *
     *           \f[
     *             p_i             = -\frac{1}{\beta*sin(\alpha)} + j*\frac{1}{\beta*cos(\alpha)} \newline
     *             z_i             = 0                                                            \newline
     *             g_i             = p_i * conj( p_i )                                            \newline
     *             \beta           = \frac{1}{ \sqrt{ \epsilon_{stop}^{ \frac{1}{order} } } }     \newline
     *             \alpha          = \frac{ ( 2*(i+1)-1 ) * \pi }{2*order}                        \newline
     *             \epsilon_{stop} = \sqrt{ 10^{ \frac{ g_{stop} }{ 10 } } - 1 }                  \newline
     *             i \in 0 \dots trunc\left( \frac{order}{2} \right)                              \newline
     *           \f]
     *
     *           Real odd pole computation:
     *
     *           \f[
     *             p_r        = -\frac{1}{\beta} \newline
     *             g_r        = -p_r             \newline
     *             g_{output} = 1
     *           \f]
    */
    template< typename __type >
    zp< __type > __butt_zeros_poles_plain__
    (
            __ix32 _order,
            __fx64 _g_stop
    )
    {
        // number of zeros, poles, coeffs:
        __ix32 L = trunc( _order / 2 ) , R = _order - 2 * L , N = L + R;

        // stopband attenuation:
        __type epsilon_stop = sqrt(pow(10, _g_stop / 10) - 1);

        // memory allocation for the lowpass analogue prototype poles, zeros and gains:
        fcomplex<__type> *plp = ( fcomplex<__type>* ) calloc( N+0 , sizeof ( fcomplex<__type> ) );
        fcomplex<__type> *zlp = ( fcomplex<__type>* ) calloc( N+0 , sizeof ( fcomplex<__type> ) );
        fcomplex<__type> *glp = ( fcomplex<__type>* ) calloc( N+1 , sizeof ( fcomplex<__type> ) );

        if( plp == 0 || zlp == 0 || glp == 0 )
        {
            free( plp );
            free( zlp );
            free( glp );
            return zp<__type>{ 0 , 0 , 0 , -1 , -1 , -1 };
        }

        // fcomplex-conjugate pairs:
        __type alpha = 0 , beta = 1 / sqrt( pow( epsilon_stop , 1 / _order ) );

        for( __ix32 i = 0 ; i < L ; i++ )
        {
            alpha = (2 * (i + 1) - 1) * PI0 / (2 * _order);
            plp[i] = fcomplex<__type>( ( -1 / beta * sin(alpha) ) , ( +1 / beta * cos(alpha) ) );
            glp[i] = plp[i] * __conjf__( plp[i] ) ;
        }

        // real odd pole:
        if( R == 1 )
        {
            plp[ N - 1 ] = fcomplex< __type >( ( -1 / beta ) , 0 );
            glp[ N - 1 ] = -__realf__<__fx64>( plp[ N - 1 ] );
        }

        // setting the output gain:
        glp[ N ] = 1;

        return zp< __type > { plp , zlp , glp , L , R , N };
    }

    /*!
     * \brief Chebyshev I lowpass analogue prototype zeros/poles plain computation function
     * \param[_g_stop] stopband attenuation , Db
     * \param[_order]  filter order
     * \return   The function computes zeros/poles pairs of  Chebyshev I lowpass
     *           analogue prototype represented as:
     *
     *           \f[
     *              W(s) = \frac{ 1 }{ ( s - p_r ) } * \sum_{i=0}^L \frac{ 1 }{ ( s - p_i ) * ( s - conj(p_i) ) }
     *           \f]
     *
     *           All the data is stored within zp data structure and returned.
     *           The computation is done according the following formulas:
     *           Complex conjugate pairs:
     *
     *           \f[
     *             p_i             = -sin(\alpha)*sinh(\beta) + j*cos(\alpha)*cosh(\beta)              \newline
     *             z_i             = 0                                                                 \newline
     *             g_i             = p_i * conj( p_i )                                                 \newline
     *             \beta           = \frac{ asinh\left(  \frac{1}{\epsilon_{stop} } \right) }{ order } \newline
     *             \alpha          = \frac{ ( 2*(i+1)-1 ) * \pi }{2*order}                             \newline
     *             \epsilon_{stop} = \sqrt{ 10^{ \frac{ g_{stop} }{ 10 } } - 1 }                       \newline
     *             i \in 0 \dots trunc\left( \frac{order}{2} \right)                                   \newline
     *           \f]
     *
     *           Real odd pole computation:
     *
     *           \f[
     *             p_r        = -sinh( \beta )   \newline
     *             g_r        = -p_r             \newline
     *             g_{output} = \begin{cases}
     *                          \sqrt{ \frac{1}{1+\epsilon_{stop}^2} }, \quad order-2*L = 0
     *                          \\
     *                          1
     *                          \end{cases}
     *           \f]
    */
    template< typename __type >
    zp< __type > __cheb1_zeros_poles_plain__
    (
            __ix32 _order,
            __fx64 _g_stop
    )
    {
        // number of zeros, poles, coeffs:
        __ix32 L = trunc( _order / 2 ) , R = _order - 2 * L , N = L + R;

        // stopband attenuation:
        __type epsilon_stop = sqrt(pow(10, _g_stop / 10) - 1);

        // memory allocation for the lowpass analogue prototype poles, zeros and gains:
        fcomplex< __type >  *plp = ( fcomplex<__type>* ) calloc( N   , sizeof ( fcomplex< __type > ) );
        fcomplex< __type >  *zlp = ( fcomplex<__type>* ) calloc( N   , sizeof ( fcomplex< __type > ) );
        fcomplex< __type >  *glp = ( fcomplex<__type>* ) calloc( N+1 , sizeof ( fcomplex< __type > ) );

        if( plp == 0 || zlp == 0 || glp == 0 )
        {
            free( plp );
            free( zlp );
            free( glp );
            return zp< __type >{ 0 , 0 , 0 , -1 , -1 , -1 };
        }

        // fcomplex conjugate pairs:
        __type alpha = 0 , betta = asinh( 1 / epsilon_stop ) / _order;
        for ( __ix32 i = 0; i < L; i++)
        {
            alpha = (2 * (i + 1) - 1) * PI0 / (2 * _order);
            plp[i] = fcomplex< __type >( -sin( alpha ) * sinh( betta ) , +cos( alpha ) * cosh( betta ) );
            glp[i] = plp[i] * __conjf__( plp[i] );
        }

        // real odd pole:
        if ( R == 1 )
        {
            plp[ N - 1 ] = fcomplex< __type >( -sinh( betta ) , 0 );
            glp[ N - 1 ] = -__realf__<__fx64>( plp[ N - 1 ] );
        }

        // setting the output gain:
        glp[N] = ( R < 1 ) ? ( sqrt(1 / (1 + epsilon_stop * epsilon_stop) ) ) : 1;

        // end the computation and return the filter info:
        return zp< __type >{ plp , zlp , glp , L , R , N };
    }

    /*!
     * \brief Chebyshev II lowpass analogue prototype zeros/poles plain computation function
     * \param[_g_stop] stopband attenuation , Db
     * \param[_order]  filter order
     * \return   The function computes zeros/poles pairs of the Chebyshev II analogue lowpass
     *           prototype represented as:
     *
     *           \f[
     *              W(s) = \frac{1}{ ( s - p_r ) } * \sum_{i=0}^L \frac{ ( s - z_i ) * ( s - conj(z_i) ) }{ ( s - p_i ) * ( s - conj(p_i) ) }
     *           \f]
     *
     *           All the data is stored within zp data structure and returned. The computation is done according the
     *           following formulas:
     *
     *          Complex conjugate pairs:
     *
     *           \f[
     *              z_i    = 0 + j*\frac{1}{cos(\alpha)}                                                                                                   \newline
     *              p_i    = \frac{ sin(\alpha) * sinh(\beta) + j*cos(\alpha) * cosh(\beta) }{ cos(\alpha)^2*cosh(\beta^2) + sin(\alpha)^2*sinh(\beta^2) } \newline
     *              g_i    = \frac{ p_i * conj(p_i) }{ z_i * conj(z_i) }                                                                                   \newline
     *              \alpha = \frac{ ( 2*(i+1)-1 ) * \pi }{ 2 * order }                                                                                     \newline
     *              \beta  = \frac{ asinh( \epsilon_{stop} ) }{ order }                                                                                    \newline
     *              i \in 0 \dots trunc\left( \frac{order}{2} \right)                                                                                      \newline
     *           \f]
     *
     *           Real odd pole:
     *
     *           \f[
     *              p_r        = -\frac{1}{ sinh( \beta ) }
     *              g_r        = -p_r
     *              g_{output} = 1
     *           \f]
    */
    template< typename __type >
    zp< __type > __cheb2_zeros_poles_plain__
    (
            __ix32 _order,
            __fx64 _g_stop
    )
    {
        // stopband attenuation:
        __fx64 epsilon_stop = sqrt(pow(10, _g_stop / 10) - 1);

        // identify the number of zeros and poles:
        __ix32 L = trunc( _order / 2 ) , R = _order - 2 * L , N = L + R;

        // allocate zeros and poles arrays:
        fcomplex< __type > *plp = ( fcomplex< __type >* ) calloc( N   , sizeof ( fcomplex< __type > ) );
        fcomplex< __type > *zlp = ( fcomplex< __type >* ) calloc( N   , sizeof ( fcomplex< __type > ) );
        fcomplex< __type > *glp = ( fcomplex< __type >* ) calloc( N+1 , sizeof ( fcomplex< __type > ) );

        if( plp == 0 || zlp == 0 || glp == 0 )
        {
            free( plp );
            free( zlp );
            free( glp );
            return zp< __type >{ 0 , 0 , 0 , -1 , -1 , -1 };
        }

        // ZEROS AND POLES COMPUTATION:

        // fcomplex conjugate pairs:

        // auxiliary variables:
        __type alpha , betta = asinh( epsilon_stop ) / _order , re , im;

        // zeros , poles, gains computation:
        for ( __ix32 i = 0 ; i < L ; i++ )
        {
            alpha = (2 * ( i + 1 ) - 1) * PI0 / ( 2 * _order );

            // zeros:
            zlp[i] = fcomplex< __type >( 0 , 1 / cos( alpha ) );

            // poles:
            re = -( sin( alpha ) * sinh( betta ) ) / ( cos( alpha ) * cos( alpha ) * cosh( betta ) * cosh( betta ) + sin( alpha ) * sin( alpha ) * sinh( betta ) * sinh( betta ) );
            im = +( cos( alpha ) * cosh( betta ) ) / ( cos( alpha ) * cos( alpha ) * cosh( betta ) * cosh( betta ) + sin( alpha ) * sin( alpha ) * sinh( betta ) * sinh( betta ) );
            plp[i] = fcomplex< __type >( re , im );

            // gains:
            glp[i] = ( zlp[i] * __conjf__( zlp[i] ) ) / ( plp[i] * __conjf__( plp[i] ) );
            glp[i] = fcomplex< __type >( 1 , 0 ) / glp[i];
        }

        // real odd pole:
        if( R >= 1 )
        {
            plp[ N - 1 ] = fcomplex< __type >( -1 / sinh( betta ) , 0 );
            glp[ N - 1 ] = -__realf__<__fx64>( plp[ N - 1 ] );
        }

        // set the output gain:
        glp[N] = 1;

        return zp< __type >{ plp , zlp , glp , L , R , N };
    }


    /*!
     * \brief Elliptic lowpass analogue prototype zeros/poles plain computation function
     * \param[_g_stop] stopband attenuation , Db
     * \param[_g_pass] passband attenuation , Db
     * \param[_order]  filter order
     * \return   The function computes Elliptic zeros/poles pairs of the Butterworth lowpass
     *           analogue prototype. It also compute zero frequency gains. All the data is stored
     *           within zp data structure and returned.
     *
     *           Attenuation normalizing:
     *           \f[
     *              \epsilon_{ stop } = \sqrt{ 10^{ \frac{ g_{stop} } { 10 } } } \newline
     *              \epsilon_{ pass } = \sqrt{ 10^{ \frac{ g_{stop} } { 10 } } } \newline
     *           \f]
     *
     *           Analyzling the zeros/poles pattern:
     *
     *           \f[
     *              L = trunc( order / 2 ) \newline
     *              R = order - 2 * L      \newline
     *           \f]
     *
     *           Transient suppression factors computation - Kp, Kw:
     *
     *           \f[
     *              K_e    = \frac{ \epsilon_{ pass } }{ \epsilon_{ stop } } \newline
     *              m      = \sqrt{ 1 - K_e^2 }                              \newline
     *              \alpha = \frac{ 2 *( i + 1 ) - 1 }{ order }              \newline
     *              KE     = ellip_k( m )                                    \newline
     *              K_p    = Kp * sn( \alpha * KE , m )^4                    \newline
     *              i \in 0 \dots L                                          \newline
     *              K_p = m^{order} * Kp                                     \newline
     *              K_w = \sqrt{ 1 - k_p^2 }                                 \newline
     *           \f]
     *
     *           Zeros computation:
     *           \f[
     *              KE     = ellip_k( K_w ) \newline
     *
     *              \alpha = \begin{cases}
     *                       \frac{ 2 * i + 1 } { order } * KE, \quad mod(order,2) = 0
     *                       \\
     *                       \alpha = \frac{ 2 * i + 2 } { order } * KE
     *                       \end{cases}
     *                       \newline
     *
     *              z_i    = 0 + j*\frac{ 1 }{ K_w * sn( \alpha , K_w ) } \newline
     *              i \in 0 \dots L
     *           \f]
     *
     *           Complex conjugate poles pairs computaion:
     *           \f[
     *              V_0 = -\frac{ ellip_k( Kw ) * isc\left( \frac{ 1 }{ epsilon_{pass} } , \sqrt{ 1 - K_e * K_e} \right) } { ellip_k( K_e ) * _order } \newline
     *              KE  = ellip_k( Kw )                                                                                                                \newline
     *              \alpha = \begin{cases}
     *                       \frac{ 2 * i + 1 } { order } * KE, \quad mod(order,2) = 0
     *                       \\
     *                       \alpha = \frac{ 2 * i + 2 } { order } * KE
     *                       \end{cases}
     *                       \newline
     *
     *              A  = cn( \alpha , K_w )             \newline
     *              B  = dn( \alpha , K_w )             \newline
     *              C  = sn( V0, \sqrt{ 1 - Kw^2 } )    \newline
     *              D  = \sqrt{1 - C^2}                 \newline
     *              E  = B^2                            \newline
     *              F  = C^2                            \newline
     *              RE = A * B * C * D / (1 - E * F)    \newline
     *
     *              A = sn( \alpha, K_w )               \newline
     *              B = dn( V0 , \sqrt{1 - K_w^2 } )    \newline
     *              C = dn( \alpha, K_w )               \newline
     *              D = sn( V0, sqrt{ 1 - K_w } )       \newline
     *              E = C^2                             \newline
     *              F = D^2                             \newline
     *              IM = A * B / (1 - E * F)            \newline
     *              p_i = RE + j*IM                     \newline
     *              i \in 0 \dots L                     \newline
     *        \f]
     *
     *        Real odd pole computation:
     *        \f[
     *          A   = sn( V0 , sqrt( 1 - K_w^2 ) )
     *          B   = cn( V0 , sqrt( 1 - K_w^2 ) )
     *          C   = A^2
     *          p_r = \frac{ A * B }{ 1 - C }
     *        \f]
     *
     *        Gains computation:
     *        \f[
     *
     *              g_i = \begin{cases}
     *                    \frac{ p_i * conj(p_i) } {  z_i * conj(z_i) }
     *                    \\
     *                    \sqrt{ \frac{1} { 1 + \epsilon_{stop}^2 } }^{ \frac{1}{L} }
     *                    \end{cases}
     *                    \newline
     *
     *              g_r = \begin{cases}
     *                    -p_r
     *                    \\
     *                    \sqrt{ \frac{1} { 1 + \epsilon_{stop}^2 } }^{ \frac{1}{L} }
     *                    \end{cases}
     *        \f]
     *
    */
    template< typename __type >
    zp< __type > __ellip_zeros_poles_plain__
    (
            __ix32 _order,
            __fx64 _g_pass,
            __fx64 _g_stop
    )
    {
        // INITIALIZATION:

        // stopband and passband attenuation:
        __fx64 epsilon_stop = sqrt(pow(10, _g_stop / 10) - 1);
        __fx64 epsilon_pass = sqrt(pow(10, _g_pass / 10) - 1);

        // identify the number of zeros and poles:
        __ix32 L = trunc( _order / 2 ) , R = _order - 2 * L , N = L + R;

        // allocate zeros and poles arrays:
        fcomplex< __type > *plp = ( fcomplex< __type >* ) calloc( N   , sizeof ( fcomplex< __type > ) );
        fcomplex< __type > *zlp = ( fcomplex< __type >* ) calloc( N   , sizeof ( fcomplex< __type > ) );
        fcomplex< __type > *glp = ( fcomplex< __type >* ) calloc( N+1 , sizeof ( fcomplex< __type > ) );

        if( plp == 0 || zlp == 0 || glp == 0 )
        {
            free( plp );
            free( zlp );
            free( glp );
            return zp< __type >{ 0 , 0 , 0 , -1 , -1 , -1 };
        }

        // TRANSIENT SUPPRESSION FACTORS COMPUTATION:

        // auxiliary variables:
        __type SN , KE;
        __type Ke = epsilon_pass / epsilon_stop , Kp = 1;
        __type m = sqrt( 1 - Ke * Ke ) , alpha , Kw;

        // transient suppression factors computation:
        for ( __ix32 i = 0; i < L; i++ )
        {
            alpha = (2 * ( i + 1 ) - 1) / ( ( __type )_order);
            KE    = __ellip_k__( m );
            SN    = __sn__( alpha * KE , m );
            Kp    = Kp * SN*SN*SN*SN;
        }

        Kp = pow( m , _order) * Kp;
        Kw = sqrt(1 - Kp * Kp);

        // ZEROS COMPUTATION:

        KE = __ellip_k__( Kw );
        if( _order % 2 == 0 ) // even order filter
        {
            for ( __ix32 i = 0; i < L; i++)
            {
                alpha  = (2 * i + 1) / ( ( __type )_order) * KE;
                zlp[i] = fcomplex< __type >( 0 , 1 /( Kw * __sn__( alpha , Kw ) ) );
            }
        }
        else if( _order % 2 != 0 ) // odd order filter
        {
            for ( __ix32 i = 0; i < L; i++)
            {
                alpha  = (2 * i + 2) / ( ( __type )_order) * KE;
                zlp[i] = fcomplex< __type >( 0 , 1 /( Kw * __sn__( alpha , Kw ) ) );
            }
        }

        // POLES COMPUTATION:
        __type V0 = 0 , A = 0 , B = 0 , C = 0 , D = 0 , E = 0 , F = 0;
        V0        = -__ellip_k__( Kw ) * __isc__(1 / epsilon_pass, sqrt(1 - Ke * Ke)) / ( __ellip_k__( Ke ) * ( ( __type )_order ) );
        KE        =  __ellip_k__( Kw );

        // fcomplex conjugate pairs:
        for ( __ix32 i = 0; i < L; i++)
        {
            alpha = ( _order % 2 == 0 ) ? (2 * i + 1) / ( ( __type )_order ) * KE
                                        : (2 * i + 2) / ( ( __type )_order ) * KE;

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
            plp[ N - 1 ] = fcomplex< __type >( A * B / ( 1 - C )  , 0 );
        }

        // GAINS COMPUTATION:
        if( R >= 1 )
        {
            // fcomplex conjugate pairs:
            for ( __ix32 i = 0; i < L; i++)
            {
                glp[i] = ( zlp[i] * __conjf__( zlp[i] ) ) / ( plp[i] * __conjf__( plp[i] ) );
                glp[i] = fcomplex< __type >( 1 , 0 ) / glp[i];
            }

            // real odd pole:
            glp[ N - 1 ] = -__realf__<__fx64>( plp[ N - 1 ] );
        }
        else
        {
            for ( __ix32 i = 0; i < L; i++) glp[i] = pow ( sqrt( 1 / (1 + epsilon_stop * epsilon_stop) ) , 1 / ( ( __type ) L ) );
        }

        // output gain:
        glp[N] = 1;

        return zp< __type >{ plp , zlp , glp , L , R , N };
    }

    // IIR coefficients computation functions:
    /*!
     * \brief Butterworth or Chebyshev I digital lowpass filter coefficients computation function
     * \param[_Fs     ] sampling frequency , Hz
     * \param[_Fc     ] cut-off frequency , Hz
     * \param[_order  ] filter order
     * \param[_type   ] filter type ( 0 - Butterworth , 1 - Chebyshev_I )
     * \param[_g_stop ] stopband attenuation , Db
     * \return   The function computes Butterworth or Chebyshev I digital lowpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */

    template < typename __type >
    cf< __type > __butt_cheb1_digital_lp__
    (
            __fx64 _Fs,
            __fx64 _Fc,
            __ix32 _order,
            __ix32 _type   = 0,
            __fx64 _g_stop = 1
    )
    {
        // COMPUTE LOWPASS ANALOGUE PROTOTYPE ZEROS, POLES AND GAINS:
        zp< __fx64 > zp = ( !_type ) ? __butt_zeros_poles_plain__ < __fx64 >( _order , _g_stop )
                                     : __cheb1_zeros_poles_plain__< __fx64 >( _order , _g_stop );

        // allocate zeros and poles arrays:
        fcomplex< __fx64 > *plp = zp.plp;
        fcomplex< __fx64 > *zlp = zp.zlp;
        fcomplex< __fx64 > *glp = zp.glp;
        __ix32 L = zp.L , R = zp.R , N = L + R;

        // frequency deformation coefficient:
        __fx64 K = tan( PI2 * _Fc / 2 / _Fs );

        // coefficients matrix computation:
        __type *cfnum = ( __type* )calloc( 3 * N , sizeof ( __type ) );
        __type *cfden = ( __type* )calloc( 3 * N , sizeof ( __type ) );
        __type *gains = ( __type* )calloc( N + 1 , sizeof ( __type ) );

        if( cfnum == 0 || cfden == 0 || gains == 0 || plp == 0 || zlp == 0 || glp == 0 )
        {
            free( cfnum ); free( cfden ); free( gains );
            free( plp )  ; free( zlp )  ; free( glp );
            return cf< __type >{ 0 , 0 , 0 , -1 , -1 , -1 };
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

        return cf< __type >{ cfnum , cfden , gains , L , R , N };
    }

    /*!
     * \brief Butterworth or Chebyshev I digital highpass filter coefficients computation function
     * \param[ _Fs     ] sampling frequency , Hz
     * \param[ _Fp     ] pass frequency , Hz
     * \param[ _order  ] filter order
     * \param[ _type   ] filter type ( 0 - Butterworth , 1 - Chebyshev_I )
     * \param[ _g_stop ] stopband attenuation , Db
     * \return   The function computes Butterworth or Chebyshev I digital highpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */

    template < typename __type > cf< __type >
    __butt_cheb1_digital_hp__
    (
            __fx64 _Fs,
            __fx64 _Fp,
            __ix32 _order,
            __ix32 _type   = 0,
            __fx64 _g_stop = 1
    )
    {
        // INITIALIZATION:
        zp < __fx64 > zp = ( !_type ) ? __butt_zeros_poles_plain__ < __fx64 >( _order , _g_stop )
                                      : __cheb1_zeros_poles_plain__< __fx64 >( _order , _g_stop );

        fcomplex< __fx64 > *plp = zp.plp;
        fcomplex< __fx64 > *zlp = zp.zlp;
        fcomplex< __fx64 > *glp = zp.glp;
        __ix32 L = zp.L , R = zp.R , N = L + R;

        // frequency deformation coefficient:
        __type w = tan( PI2 * _Fp / 2 / _Fs );

        // coefficients matrix computation:
        __type *cfnum = ( __type* )calloc( 3 * N , sizeof ( __type ) );
        __type *cfden = ( __type* )calloc( 3 * N , sizeof ( __type ) );
        __type *gains = ( __type* )calloc( N + 1 , sizeof ( __type ) );

        if( cfnum == 0 || cfden == 0 || gains == 0 || plp == 0 || zlp == 0 || glp == 0 )
        {
            free( cfnum ); free( cfden ); free( gains );
            free( plp )  ; free( zlp )  ; free( glp );
            return cf< __type >{ 0 , 0 , 0 , -1 , -1 , -1 };
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

        return cf< __type >{ cfnum , cfden , gains , L , R , N };
    }

    /*!
     * \brief Butterworth or Chebyshev I digital bandpass filter coefficients computation function
     * \param[ _Fs        ] sampling frequency , Hz
     * \param[ _Fp        ] pass frequency , Hz
     * \param[ _BandWidth ] passband width , Hz
     * \param[ _order     ] filter order
     * \param[ _type      ] filter type ( 0 - Butterworth , 1 - Chebyshev_I )
     * \param[ _g_stop    ] stopband attenuation , Db
     * \return   The function computes Butterworth or Chebyshev I digital bandpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */

    template < typename __type > cf<__type>
    __butt_cheb1_digital_bp__
    (
            __fx64 _Fs,
            __fx64 _Fp,
            __fx64 _BandWidth,
            __ix32 _order,
            __ix32 _type   = 0,
            __fx64 _g_stop = 1
    )
    {
        _order /= 2;

        // frequency deformation coefficient:
        __fx64 w1 = tan( PI2 * _Fp / 2 / _Fs ) , w2 = tan( PI2 * ( _Fp + _BandWidth ) / 2 / _Fs );

        // allocate zeros and poles arrays:

        // lowpass analogue prototype poles, zeros and gains:
        zp < __fx64 > zp = ( !_type ) ? __butt_zeros_poles_plain__ < __fx64 >( _order, _g_stop )
                                      : __cheb1_zeros_poles_plain__< __fx64 >( _order, _g_stop );

        fcomplex< __fx64 > *plp = zp.plp;
        fcomplex< __fx64 > *glp = zp.glp;
        fcomplex< __fx64 > *zlp = zp.zlp;
        __ix32 L = zp.L , R = zp.R , N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        fcomplex< __fx64 > *pbp = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *zbp = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *gbp = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );

        // coefficients matrix computation:
        __type *cfnum = ( __type* )calloc( 3 * (2*L+R) , sizeof ( __type ) );
        __type *cfden = ( __type* )calloc( 3 * (2*L+R) , sizeof ( __type ) );
        __type *gains = ( __type* )calloc( (2*L+R + 1) , sizeof ( __type ) );

        if( cfnum == 0 || cfden == 0 || gains == 0 ||
            plp == 0 || zlp == 0 || glp == 0 ||
            pbp == 0 || zbp == 0 || gbp == 0)
        {
            free( cfnum ); free( cfden ); free( gains );
            free( plp )  ; free( zlp )  ; free( glp );
            free( pbp )  ; free( zbp )  ; free( gbp );
            return cf< __type >{ 0 , 0 , 0 , -1 , -1 , -1 };
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

        return cf< __type >{ cfnum , cfden , gains , 2*L , R , 2*L+R  };
    }

    /*!
     * \brief Butterworth or Chebyshev I digital bandstop filter coefficients computation function
     * \param[ _Fs        ] sampling frequency , Hz
     * \param[ _Fc        ] cut-off frequency  , Hz
     * \param[ _BandWidth ] cut-off bandwidth  , Hz
     * \param[ _order     ] filter order
     * \param[ _type      ] filter type ( 0 - Butterworth , 1 - Chebyshev_I )
     * \param[ _g_stop    ] stopband attenuation , Db
     * \return   The function computes Butterworth or Chebyshev I digital bandstop filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */

    template < typename __type >
    cf<__type> __butt_cheb1_digital_bs__
    (
            __fx64 _Fs,
            __fx64 _Fc,
            __fx64 _BandWidth,
            __ix32 _order,
            __fx32 _type   = 0,
            __fx64 _g_stop = 1
    )
    {

       // INITIALIZATION:
       _order /= 2;

       // frequency deformation coefficient:
       __fx64 w1 = tan( PI2 * _Fc / 2 / _Fs ) , w2 = tan( PI2 * ( _Fc + _BandWidth ) / 2 / _Fs );

       // allocate zeros and poles arrays:

       // lowpass analogue prototype poles, zeros and gains:
       zp < __fx64 > zp = ( !_type ) ? __butt_zeros_poles_plain__ < __fx64 >( _order, _g_stop )
                                     : __cheb1_zeros_poles_plain__< __fx64 >( _order, _g_stop );

       fcomplex< __fx64 > *plp = zp.plp;
       fcomplex< __fx64 > *glp = zp.glp;
       fcomplex< __fx64 > *zlp = zp.zlp;
       __ix32 L = zp.L , R = zp.R , N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        fcomplex< __fx64 > *pbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *zbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *gbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );

        // coefficients matrix computation:
        __type *cfnum = (__type* )calloc( 3 * (2*L+R) , sizeof ( __type ) );
        __type *cfden = (__type* )calloc( 3 * (2*L+R) , sizeof ( __type ) );
        __type *gains = (__type* )calloc( (2*L+R+1)   , sizeof ( __type ) );

        if( cfnum == 0 || cfden == 0 || gains == 0 ||
            plp == 0 || zlp == 0 || glp == 0 ||
            pbs == 0 || zbs == 0 || gbs == 0)
        {
            free( cfnum ); free( cfden ); free( gains );
            free( plp )  ; free( zlp )  ; free( glp );
            free( pbs )  ; free( zbs )  ; free( gbs );
            return cf< __type >{ 0 , 0 , 0 , -1 , -1 , -1 };
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

        return cf< __type >{ cfnum , cfden , gains , 2 * L , R , 2 * L + R };
    }


    /*!
     * \brief Chebyshev II or Elliptic digital lowpass filter coefficients computation function
     * \param[ _Fs     ] sampling frequency , Hz
     * \param[ _Fc     ] cut-off frequency , Hz
     * \param[ _order  ] filter order
     * \param[ _type   ] filter type ( 0 - Chebyshev II , 1 - Elliptic )
     * \param[ _g_stop ] stopband attenuation , Db
     * \param[ _g_pass ] passband attenuation , Db
     * \return   The function computes Chebyshev II or Elliptic digital lowpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */

    template < typename __type >
    cf< __type > __cheb2_ellip_digital_lp__
    (
            __fx64 _Fs,
            __fx64 _Fc,
            __ix32 _order,
            __ix32 _type   = 0,
            __fx64 _g_pass = 1,
            __fx64 _g_stop = 80
    )
    {
        __fx64 w = tan( PI2 * _Fc / 2 / _Fs );

        // digital lowpass coefficients computation:
        zp< __fx64 > zp = ( !_type ) ? __cheb2_zeros_poles_plain__< __fx64 >( _order, _g_stop )
                                     : __ellip_zeros_poles_plain__< __fx64 >( _order , _g_pass , _g_stop );

        // zeros/poles and coefficients number:
        __ix32 L = zp.L;
        __ix32 R = zp.R;
        __ix32 N = zp.N;

        // zeros / poles plain initialization:
        fcomplex< __fx64 > *plp = zp.plp;
        fcomplex< __fx64 > *zlp = zp.zlp;
        fcomplex< __fx64 > *glp = zp.glp;

        // coefficients matrix computation:
         __type *cfnum = ( __type* )calloc( 3 * N , sizeof ( __type ) );
         __type *cfden = ( __type* )calloc( 3 * N , sizeof ( __type ) );
         __type *gains = ( __type* )calloc( N + 1 , sizeof ( __type ) );

        if( cfnum == 0 || cfden == 0 || gains == 0 ||
            plp == 0 || zlp == 0 || glp == 0 )
        {
            free( cfnum ); free( cfden ); free( gains );
            free( plp )  ; free( zlp )  ; free( glp );
            return cf< __type >{ 0 , 0 , 0 , -1 , -1 , -1 };
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

        return cf< __type >{ cfnum , cfden , gains , L , R , N };
    }

    /*!
     * \brief Chebyshev II or Elliptic digital highpass filter coefficients computation function
     * \param[ _Fs     ] sampling frequency , Hz
     * \param[ _Fp     ] pass frequency , Hz
     * \param[ _order  ] filter order
     * \param[ _type   ] filter type ( 0 - Chebyshev II , 1 - Elliptic )
     * \param[ _g_stop ] stopband attenuation , Db
     * \param[ _g_pass ] passband attenuation , Db
     * \return   The function computes Chebyshev II or Elliptic digital lowpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */

    template < typename __type >
    cf< __type > __cheb2_ellip_digital_hp__
    (
            __fx64 _Fs,
            __fx64 _Fc,
            __ix32 _order,
            __ix32 _type   = 0,
            __fx64 _g_pass = 1,
            __fx64 _g_stop = 80
    )
    {
         __fx64  w = tan( PI2 * _Fc / 2 / _Fs );

        // digital lowpass coefficients computation:
        zp< __fx64 > zp = ( !_type ) ?__cheb2_zeros_poles_plain__< __fx64 >(_order, _g_stop )
                                     :__ellip_zeros_poles_plain__< __fx64 >(_order, _g_pass, _g_stop );

        // zeros/poles and coefficients number:
        __ix32 L = zp.L;
        __ix32 R = zp.R;
        __ix32 N = zp.N;

        // zeros / poles plain initialization:
        fcomplex< __fx64 > *plp = zp.plp;
        fcomplex< __fx64 > *zlp = zp.zlp;
        fcomplex< __fx64 > *glp = zp.glp;

        // coefficients matrix memory allocation:
         __type *cfnum = (__type* )calloc( 3 * N , sizeof ( __type ) );
         __type *cfden = (__type* )calloc( 3 * N , sizeof ( __type ) );
         __type *gains = (__type* )calloc( N + 1 , sizeof ( __type ) );

         if( cfnum == 0 || cfden == 0 || gains == 0 ||
             plp == 0 || zlp == 0 || glp == 0 )
         {
             free( cfnum ); free( cfden ); free( gains );
             free( plp )  ; free( zlp )  ; free( glp );
             return cf< __type >{ 0 , 0 , 0 , -1 , -1 , -1 };
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

        return cf< __type >{ cfnum , cfden , gains , L , R , N };
    }

    /*!
     * \brief Chebyshev II or Elliptic digital bandpass filter coefficients computation function
     * \param[ _Fs        ] sampling frequency , Hz
     * \param[ _Fp        ] pass frequency     , Hz
     * \param[ _BandWidth ] passband width     , Hz
     * \param[ _order     ] filter order
     * \param[ _type      ] filter type ( 0 - Chebyshev II , 1 - Elliptic )
     * \param[ _g_stop    ] stopband attenuation , Db
     * \param[ _g_pass    ] passband attenuation , Db
     * \return   The function computes Chebyshev II or Elliptic digital bandpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */

    template < typename __type >
    cf<__type> __cheb2_ellip_digital_bp__
    (
            __fx64 _Fs,
            __fx64 _Fp,
            __fx64 _BandWidth,
            __ix32 _order,
            __ix32 _type   = 0,
            __fx64 _g_pass = 1,
            __fx64 _g_stop = 80
    )
    {
        _order /= 2;

        // frequency deformation coefficient:
        __fx64 w1 = tan( PI2 * _Fp / 2 / _Fs ) , w2 = tan( PI2 * ( _Fp + _BandWidth ) / 2 / _Fs );

        // allocate zeros and poles arrays:

        // lowpass analogue prototype poles, zeros and gains:
        zp< __fx64 > zp = ( !_type ) ?__cheb2_zeros_poles_plain__< __fx64 >(_order, _g_stop )
                                     :__ellip_zeros_poles_plain__< __fx64 >(_order, _g_pass, _g_stop );

        fcomplex< __fx64 > *plp = zp.plp;
        fcomplex< __fx64 > *glp = zp.glp;
        fcomplex< __fx64 > *zlp = zp.zlp;
        __ix32 L = zp.L , R = zp.R , N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        fcomplex< __fx64 > *pbp = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *zbp = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *gbp = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );

        // coefficients matrix computation:
         __type *cfnum = ( __type* )calloc( 3 * (2*L+R) , sizeof (__type ) );
         __type *cfden = ( __type* )calloc( 3 * (2*L+R) , sizeof (__type ) );
         __type *gains = ( __type* )calloc( (2*L+R + 1) , sizeof (__type ) );

        if( cfnum == 0 || cfden == 0 || gains == 0 ||
            plp == 0 || zlp == 0 || glp == 0 ||
            pbp == 0 || zbp == 0 || gbp == 0)
        {
            free( cfnum ); free( cfden ); free( gains );
            free( plp )  ; free( zlp )  ; free( glp );
            free( pbp )  ; free( zbp )  ; free( gbp );
            return cf< __type >{ 0 , 0 , 0 , -1 , -1 , -1 };
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

        return cf< __type >{ cfnum , cfden , gains , 2*L , R , 2*L+R  };
    }

    /*!
     * \brief Chebyshev II or Elliptic digital bandstop filter coefficients computation function
     * \param[ _Fs        ] sampling frequency , Hz
     * \param[ _Fc        ] cut-off frequency  , Hz
     * \param[ _BandWidth ] stopband width     , Hz
     * \param[ _order     ] filter order
     * \param[ _type      ] filter type ( 0 - Chebyshev II , 1 - Elliptic )
     * \param[ _g_stop    ] stopband attenuation , Db
     * \param[ _g_pass    ] passband attenuation , Db
     * \return   The function computes Chebyshev II or Elliptic digital bandstop filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */

    template < typename __type >
    cf< __type > __cheb2_ellip_digital_bs__
    (
            __fx64 _Fs,
            __fx64 _Fc,
            __fx64 _BandWidth,
            __ix32 _order,
            __ix32 _type   = 0,
            __fx64 _g_pass = 1,
            __fx64 _g_stop = 80
    )
    {
        _order /= 2;

        // frequency deformation coefficient:
        __fx64 w1 = tan( PI2 * _Fc / 2 / _Fs ) , w2 = tan( PI2 * (_Fc + _BandWidth ) / 2 / _Fs );

        // allocate zeros and poles arrays:

        // lowpass analogue prototype poles, zeros and gains:
        zp < __fx64 > zp = ( !_type ) ?__cheb2_zeros_poles_plain__< __fx64 >(_order, _g_stop )
                                      :__ellip_zeros_poles_plain__< __fx64 >(_order, _g_pass, _g_stop );

        fcomplex< __fx64 > *plp = zp.plp;
        fcomplex< __fx64 > *glp = zp.glp;
        fcomplex< __fx64 > *zlp = zp.zlp;
        __ix32 L = zp.L , R = zp.R , N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        fcomplex< __fx64 > *pbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *zbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *gbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );

        // coefficients matrix computation:
         __type *cfnum = ( __type* )calloc( 3 * (2*L+R) , sizeof ( __type ) );
         __type *cfden = ( __type* )calloc( 3 * (2*L+R) , sizeof ( __type ) );
         __type *gains = ( __type* )calloc( (2*L+R + 1) , sizeof ( __type ) );

         if( cfnum == 0 || cfden == 0 || gains == 0 ||
             plp == 0 || zlp == 0 || glp == 0 ||
             pbs == 0 || zbs == 0 || gbs == 0)
         {
             free( cfnum ); free( cfden ); free( gains );
             free( plp )  ; free( zlp )  ; free( glp );
             free( pbs )  ; free( zbs )  ; free( gbs );
             return cf< __type >{ 0 , 0 , 0 , -1 , -1 , -1 };
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

        return cf< __type >{ cfnum , cfden , gains , 2*L , R , 2*L+R  };
    }

    /*!
      * \brief FIR digital lowpass filter coefficients computation function
      * \param[ _Fs    ] sampling frequency , Hz
      * \param[ _Fc    ] cut-off frequency  , Hz
      * \param[ _N     ] filter order
      * \param[ _scale ] scaling factor ( scale = 0 - not-scaled coefficients , scale = 1 - scaled coefficients )
      * \param[ _wind  ] input window function object
      * \return The function returns digital lowpass FIR filter coefficients buffer.
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
    */
    template< typename __type > cf<__type> __fir_wind_digital_lp__
    (
            __fx64 _Fs,
            __fx64 _Fc,
            __ix32 _N,
            __ix32 _scale,
            __fx64 *_wind
    )
    {
        // coefficients buffer memory allocation:
        __type *cfbuff = ( __type* ) calloc(_N , sizeof ( __type ) );

        // coefficients computation:
        __ix32 k = 0;
        _Fc /= _Fs;

        if ( _N % 2 == 0) // even order filter coefficients computation
        {
            for ( __ix32 n = 0; n < _N / 2; n++)
            {
                k = abs(n - _N / 2);
                if (n == 0)
                {
                    cfbuff[k] = 2 * _Fc * _wind[k];
                }
                else
                {
                    cfbuff[k] = 2 * _Fc * sin(n * PI2 * _Fc ) / (n * PI2 * _Fc ) * _wind[k];
                    cfbuff[_N - k] = cfbuff[k];
                }
            }
        }
        else if (_N % 2 != 0) // odd order filter coefficients computation
        {
            __type rem = ceil( (__type)_N / 2) - (__type)_N / 2;

            for ( __ix32 n = 0; n < ceil(_N / 2); n++)
            {
                k = abs(n - _N / 2);
                cfbuff[k] = 2 * _Fc * sin((n + rem) * PI2 * _Fc ) / ((n + rem) * PI2 * _Fc ) * _wind[k];
                cfbuff[_N - k] = cfbuff[k];
            }
        }

        // filter pulse characteristic normalization:
        if (_scale )
        {
            fr fr = __freq_resp__( cfbuff, _N, _Fs, 0 );
            for ( __ix32 n = 0; n < _N; n++) cfbuff[n] /= fr.Km;
        }

        // returning the result
        return cf< __type >{ cfbuff , 0 , 0 , -1 , -1 , _N };
    }

    /*!
      * \brief FIR digital lowpass filter coefficients computation function
      * \param[ _Fs    ] sampling frequency , Hz
      * \param[ _Fp    ] pass-off frequency , Hz
      * \param[ _N     ] filter order
      * \param[ _scale ] scaling factor ( scale = 0 - not-scaled coefficients , scale = 1 - scaled coefficients )
      * \param[ _wind  ] input window function object
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
    template< typename __type > cf<__type> __fir_wind_digital_hp__
    (
            __fx64 _Fs,
            __fx64 _Fp,
            __ix32 _N,
            __ix32 _scale,
            __fx64 *_wind
    )
    {
        // coefficients buffer memory allocation:
        __type *cfbuff = ( __type* ) calloc( _N , sizeof ( __type ) );

        // coefficients computation:
        __ix32 k = 0;
        _Fp /= _Fs;

        if ( _N % 2 == 0) // even order highpass
        {
            for ( __ix32 n = 0; n < _N / 2; n++)
            {
                k = abs(n - _N / 2);

                if (n == 0)
                {
                    cfbuff[k] = (1 - 2 * _Fp) * _wind[k];
                }
                else
                {
                    cfbuff[k] = -2 * _Fp * sin(n * PI2 * _Fp ) / (n * PI2 * _Fp ) * _wind[k];
                    cfbuff[_N - k] = cfbuff[k];
                }
            }
        }
        else if (_N % 2 != 0) // odd order highpass
        {
            __type rem = ceil( (__type)_N / 2) - (__type)_N / 2;
            for ( __ix32 n = 0; n < ceil(_N / 2); n++)
            {
                k = abs(n - _N / 2);
                cfbuff[k]     = -pow(-1 , n) * 2 * (0.5-_Fp) * sin((n + rem) * PI2 * (0.5-_Fp) ) / ((n + rem) * PI2 * (0.5 - _Fp) )* _wind[k];
                cfbuff[_N - k] = -cfbuff[k];
            }
        }

        // filter pulse characteristic scaling:
        if (_scale )
        {
            fr fr = __freq_resp__( cfbuff, _N, _Fs, _Fs / 2 );
            for ( __ix32 n = 0; n < _N; n++) cfbuff[n] /= fr.Km;
        }

        // returning the result
        return cf< __type >{ cfbuff , 0 , 0 , -1 , -1 , _N };
    }

    /*!
      * \brief FIR digital lowpass filter coefficients computation function
      * \param[ _Fs    ] sampling frequency , Hz
      * \param[ _Fp    ] pass frequency     , Hz
      * \param[ _BW    ] passband width     , Hz
      * \param[ _N     ] filter order
      * \param[ _scale ] scaling factor ( scale = 0 - not-scaled coefficients , scale = 1 - scaled coefficients )
      * \param[ _wind  ] input window function object
      * \return The function returns digital lowpass FIR filter coefficients buffer.
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
    template< typename __type > cf<__type> __fir_wind_digital_bp__
    (
            __fx64 _Fs,
            __fx64 _Fp,
            __fx64 _BW,
            __ix32 _N,
            __ix32 _scale,
            __fx64 *_wind
    )
    {
        // coefficients buffer memory allocation:
        __type *cfbuff = ( __type* ) calloc(_N , sizeof ( __type ) );

        // coefficients computation:
        __ix32 k = 0;
        __fx64 Fp1 = _Fp / _Fs , Fp2 = (_Fp + _BW)/ _Fs;

        if ( _N % 2 == 0) // even order bandpass
        {
            for ( __ix32 n = 0; n < _N / 2; n++)
            {
                k = abs(n - _N / 2);

                if (n == 0)
                {
                    cfbuff[k] = 2 * ( Fp2 - Fp1) * _wind[k];
                }
                else
                {
                    cfbuff[k] = 2 * (Fp2 * sin(n * PI2 * Fp2) / (n * PI2 * Fp2) - Fp1 * sin(n * PI2 * Fp1) / (n * PI2 * Fp1)) * _wind[k];
                    cfbuff[_N - k] = cfbuff[k];
                }
            }
        }
        else if (_N % 2 != 0) // odd order bandpass
        {
            __type rem = ceil((__type)_N / 2) - (__type)_N / 2;

            for ( __ix32 n = 0; n < ceil(_N / 2); n++)
            {
                k = abs(n - _N / 2);
                cfbuff[k] = 2 * (Fp2 * sin((n + rem) * PI2 * Fp2) / ((n + rem) * PI2 * Fp2) - Fp1 * sin((n + rem) * PI2 * Fp1) / ((n + rem) * PI2 * Fp1)) * _wind[k];
                cfbuff[_N - k] = cfbuff[k];
            }
        }

        // filter pulse characteristic normalization:
        if (_scale )
        {
            fr fr = __freq_resp__( cfbuff , _N , _Fs , ( ( Fp1 + 0.5 * ( Fp2 - Fp1 ) ) * _Fs ) );
            for ( __ix32 n = 0; n < _N; n++) cfbuff[n] /= fr.Km;
        }

        // returning the result:
        return cf< __type >{ cfbuff , 0 , 0 , -1 , -1 , _N };
    }

    /*!
      * \brief FIR digital bandpass filter coefficients computation function
      * \param[ _Fs    ] sampling frequency , Hz
      * \param[ _Fc    ] cut-off frequency  , Hz
      * \param[ _BW    ] stopband width     , Hz
      * \param[ _N     ] filter order
      * \param[ _scale ] scaling factor ( scale = 0 - not-scaled coefficients , scale = 1 - scaled coefficients )
      * \param[ _wind  ] input window function object
      * \return The function returns digital lowpass FIR filter coefficients buffer.
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
    template< typename __type > cf<__type> __fir_wind_digital_bs__
    (
            __fx64 _Fs,
            __fx64 _Fc,
            __fx64 _BW,
            __ix32 _N,
            __ix32 _scale,
            __fx64 *_wind
    )
    {
        // coefficients buffer memory allocation:
        __type *cfbuff = ( __type* ) calloc(_N , sizeof ( __type ) );

        // coefficients computation:
        __ix32 k = 0;
        __fx64 Fc1 = _Fc / _Fs , Fc2 = (_Fc + _BW ) / _Fs;

        for (__ix32 n = 0; n < _N / 2; n++)
        {
            k = abs(n - _N / 2);

            if (n == 0)
            {
                cfbuff[k] = 1 - 2 * ( Fc2 - Fc1 ) * _wind[k];
            }
            else
            {
                cfbuff[k] = 2 * ( Fc1 * sin(n * PI2 * Fc1) / (n * PI2 * Fc1 ) - Fc2 * sin(n * PI2 * Fc2) / (n * PI2 * Fc2) ) * _wind[k];
                cfbuff[_N - k] = cfbuff[k];
            }
        }

        // filter pulse characteristic normalization:
        if (_scale )
        {
            fr fr = __freq_resp__(cfbuff, _N, _Fs , 0 );
            for ( __ix32 n = 0; n < _N; n++) cfbuff[n] /= fr.Km;
        }

        // return the result:
        return cf< __type >{ cfbuff , 0 , 0 , -1 , -1 , _N };
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
     *  \details Defines classic IIR filter represented in a biquadratic form:
     *  \f[
     *      W(z) = \prod_{i=0}^N gain_{i} * \frac{ a_{0i} + a_{1i} * z^{-1} + a_{2i} * z^{-2} }
     *                                           { b_{0i} + b_{1i} * z^{-1} + b_{2i} * z^{-2} }
     *  \f]
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

                default:
                m_cfmatrix = compute_lowpass ();
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

        #ifndef __ALG_PLATFORM
        void show()
        {
            __show__( m_cfmatrix, m_filter_type );
        }
        #endif
    };

    /*!
     *  \class classic_fir_abstract
     *  \details Defines classic IIR filter with a transfer function:
     *  \f[
     *      W(z) = \sum_{i=0}^N a_{i} * z^{-i}
     *  \f]
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

                default:
                m_cfmatrix = __fir_wind_digital_lp__<__type>( m_Fs , m_bandwidth.Fc , m_order , m_scale , _window );
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
     *  \details Defines recursive FIR filter having the following transfer function:
     *   \f[
     *      W(z) = \sum_{i=0}^N a_{i} * z^{-i}
     *  \f]
     *
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
     *  \class fcomb_abstract
     *  \details defines comb FIR filter having the following transfer function:
     *  \f[
     *      W(z) = \begin{cases}
     *             1 + z^{Fs/Fn/2}, \quad odd = 1
     *             \\
     *             1 - z^{Fs/Fn/2}, \quad ddd = 0
     *             \end{cases}
     *  \f]
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

         /*! \brief memory allocation function */
         __ix32 allocate() override
         {
             return m_bx.allocate( m_order + 1 );
         }

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
        void init( __fx64 _Fs , __fx64 _Fn, __ix32 _odd )
        {
            m_Fs      = _Fs;
            m_Fn      = _Fn;
            m_Ts      = (__fx64)1 / m_Fs;
            m_odd     = _odd;
            m_order   = m_Fs / m_Fn / 2;
            m_out     = 0;

            // memory allocation:
            allocate();
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
            fcomplex<__fx64> Wz;
            if(!m_odd)
            {
                Wz(1 - cos(-PI2  * (__fx64)m_order * F * m_Ts), -sin(-PI2  * (__fx64)m_order * F * m_Ts) );
            }
            else
            {
                Wz(1 + cos(-PI2  * (__fx64)m_order * F * m_Ts), +sin(-PI2  * (__fx64)m_order * F * m_Ts) );
            }

            return { __cabsf__(Wz) , __cargf__(Wz) };
        }

        /*!
         *  \brief 32-bit floating point filtering operator
         *  \param[input] - pointer to the input signal samples buffer
         *  \return The operatoe calls the function that returns filtering result
        */
        virtual inline __type operator ()( __type *input ) = 0;
    };

    /*!
     *  \class fcombeq_abstract
     *  \details defines equalized comb FIR filter having the following transfer function:
     *  \f[
     *      W(z) = \begin{cases}
     *             1 * K1 - z^{Fs/Fn} - K2 * z^{2*Fs/Fn} , \quad if the filter is odd
     *             \\
     *             1 * K1 + z^{Fs/Fn} - K2 * z^{2*Fs/Fn} , \quad if the filter is not odd
     *             \end{cases}
     *  \f]
     *
     *  K1, K2 are the roots of the following square equation: \newline
     *  \f[
     *  \f]
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
             return ( m_out = ( !m_odd ) ? ( (__fx64)*input * m_K1 - (__fx64)m_bx[m_ElemNum1] - (__fx64)m_bx[m_ElemNum2] * m_K2 )
                                         : ( (__fx64)*input * m_K1 + (__fx64)m_bx[m_ElemNum1] - (__fx64)m_bx[m_ElemNum2] * m_K2 ) );
         }

         /*! \brief memory allocation function */
         __ix32 allocate() override
         {
             // Complex Transfer functions:
             fcomplex<__fx64> WZ1(0.5 - 0.5 * cos(-PI2 * (m_Fn + m_dF) * (__fx64)m_order * m_Ts), -0.5 * sin(-PI2 * (m_Fn + m_dF) * (__fx64)m_order * m_Ts) );
             fcomplex<__fx64> WZ2(1.0 - cos(-PI2 * (m_Fn + m_dF) * (__fx64)m_order * 2.0 * m_Ts), -sin(-PI2 * (m_Fn + m_dF) * (__fx64)m_order * 2.0 * m_Ts) );

             // square equation coefficients:
             __fx64 aa = 1;
             __fx64 bb = __realf__( ( WZ1 * __conjf__(WZ2) + WZ2*__conjf__(WZ1) ) / WZ2 / __conjf__(WZ2) );
             __fx64 cc = __realf__( ( WZ1 * __conjf__(WZ1) - fcomplex<__fx64>((1 + m_d_Amp / 100) * (1 + m_d_Amp / 100) , 0 ) ) / WZ2 / __conjf__(WZ2) );
             __fx64 DD = bb*bb - 4*aa*cc;

             if( DD > (__fx64)0 )
             {
                 __fx64 maxroot = fmax( ( -bb - sqrt(DD) ) * (__fx64)0.5, ( -bb + sqrt(DD) ) * (__fx64)0.5 ) * (__fx64)2;
                 m_K1 = ( 1 + maxroot );
                 m_K2 = maxroot;
             }
             else
             {
                 m_K1 = 1;
                 m_K2 = 0;
             }

             return m_bx.allocate( m_ElemNum2 + 1 );
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

        /*! \brief memory deallocation function */
        __ix32 deallocate() override
        {
            m_bx.deallocate();
            return 0;
        }

        /*! \brief frequency response computation function */
        fr frequency_response( __fx64 F ) override
        {
            fcomplex<__fx64> Wz;
            if( !m_odd ) // even filter
            {

                Wz( m_K1 - cos(-PI2 * ( __fx64)m_order * F * m_Ts) - m_K2 * cos(-PI2 * 2 * (__fx64)m_order * F * m_Ts),
                                                                    -sin(-PI2 * (__fx64)m_order * F * m_Ts) - m_K2 * sin(-PI2 * 2 * (__fx64)m_order * F * m_Ts));
            }
            else // odd filter
            {
                Wz( m_K1 + cos(-PI2 * ( __fx64)m_order * F * m_Ts) - m_K2 * cos(-PI2 * 2 * (__fx64)m_order * F * m_Ts),
                                                                    +sin(-PI2 * (__fx64)m_order * F * m_Ts) - m_K2 * sin(-PI2 * 2 * (__fx64)m_order * F * m_Ts));
            }
            return { __cabsf__(Wz) , __cargf__(Wz) };
        }

        /*!
         *  \brief 32-bit floating point filtering operator
         *  \param[input] - pointer to the input signal samples buffer
         *  \return The operatoe calls the function that returns filtering result
        */
        virtual inline __type operator ()( __type *input ) = 0;
    };

    /*!
     *  \class derivative transfer function abstract class
     *  \details defines derivative transfer function class
    */
    template< typename __type > class derivative_abstract : public filter_abstract
    {
    protected:
        /*! \brief derivative time constant */
        __fx64  m_T1;
        /*! \brief transfer function numerator coefficients vector*/
        __type *m_cfnum;
        __type *m_cfden;
        __type  m_Gain;
        delay<__type> m_bx;
        delay<__type> m_by;

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

    public:
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

        virtual ~derivative_abstract()
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

    public:

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

        virtual ~aperiodic_abstract()
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

    public:

        // initialization function:
        void init( __fx64 Fs , __fx64 Fn )
        {
            m_Fs = Fs;
            m_Ts = 1 / Fs;
            m_Fn = Fn;

            // memory allocation function call:
            allocate();
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

        virtual ~integrator_abstract()
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

    public:

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

        virtual ~leadlag_abstract()
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

                case filter_type::other:
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

    public:

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

        // constructors and destructor:
        second_order_filter_abstract() : filter_abstract(){}

        second_order_filter_abstract(__fx64 _Fs , __fx64 _Fn , __fx64 _Fc , __fx64 _Kd , filter_type _type )
        {
            init( _Fs, _Fn, _Kd, _Fc, _type );
        }

        virtual ~second_order_filter_abstract()
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
         butterworth< __type >( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth , attenuation _attenuation ) { init(_Fs, _order,  _type, _bandwidth, _attenuation ); }
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
        butterworth< __type >( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth , attenuation _attenuation ) { init(_Fs, _order,  _type, _bandwidth, _attenuation ); }
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
         chebyshev_1< __type >( __fx64 _Fs , __ix32 _order , filter_type _type, bandwidth _bandwidth , attenuation _attenuation ) { init(_Fs, _order,  _type, _bandwidth, _attenuation ); }
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
        chebyshev_1< __type >( __fx64 _Fs , __ix32 _order , filter_type _type, bandwidth _bandwidth , attenuation _attenuation ) { init(_Fs, _order,  _type, _bandwidth, _attenuation ); }
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
         chebyshev_2< __type >( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, attenuation _attenuation ) { init(_Fs, _order,  _type, _bandwidth, _attenuation ); }
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
         chebyshev_2< __type >( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, attenuation _attenuation ) { init(_Fs, _order,  _type, _bandwidth, _attenuation ); }
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
         elliptic< __type >( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, attenuation _attenuation ) { init(_Fs, _order,  _type, _bandwidth, _attenuation ); }
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
         elliptic< __type >( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, attenuation _attenuation ) { init(_Fs, _order,  _type, _bandwidth, _attenuation ); }
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


    /*! @} */

    /*! \defgroup <DSPB_ImplementationTransferFunctions> (Elementary transfer functions implementation )
     *  \ingroup DSPB_Implementation
     *  \brief The module contains elementary transfer functions fimplementation
        @{
    */

    /*! \brief Child derivative transfer function 32-bit realization */
    template<> class derivative<__fx32> final : public derivative_abstract<__fx32>
    {
        typedef __fx32 __type;
    public:
        derivative() : derivative_abstract(){}
        derivative(__fx64 _Fs , __fx64 _Fn , __fx64 _Td ) : derivative_abstract( _Fs , _Fn , _Td ){}
        ~derivative(){};

        inline __type operator ()( __type  *input ) override
        {
            return __filt__<__type>(input, m_cfnum, m_cfden, m_Gain, 2, 1, m_bx, m_by );
        }
    };

    /*! \brief Child derivative transfer function 64-bit realization */
    template<> class derivative<__fx64> final : public derivative_abstract<__fx64>
    {
        typedef __fx64 __type;
    public:
        derivative() : derivative_abstract(){}
        derivative(__fx64 _Fs , __fx64 _Fn , __fx64 _Td )
            : derivative_abstract( _Fs , _Fn , _Td ){}
        ~derivative(){};

        inline __type operator ()( __type *input ) override
        {
            return __filt__<__type>(input, m_cfnum, m_cfden, m_Gain, 2, 1, m_bx, m_by );
        }
    };

     /*! \brief Child integrator transfer function 32-bit realization */
    template<> class integrator<__fx32> final : public integrator_abstract<__fx32>
    {
        typedef __fx32 __type;
    public:
        integrator() : integrator_abstract(){}
        integrator(__fx64 _Fs , __fx64 _Fn ) : integrator_abstract( _Fs , _Fn ){}
        ~integrator(){};

        inline __type operator ()( __type  *input ) override
        {
            return __filt__<__type>(input, m_cfnum, m_cfden, m_Gain, 2, 1, m_bx, m_by );
        }
    };

    /*! \brief Child integrator transfer function 64-bit realization */
    template<> class integrator<__fx64> final : public integrator_abstract<__fx64>
    {
        typedef __fx64 __type;
    public:
        integrator() : integrator_abstract(){}
        integrator(__fx64 _Fs , __fx64 _Fn ) : integrator_abstract( _Fs , _Fn ){}
        ~integrator(){};

        inline __type operator ()( __type  *input ) override
        {
            return __filt__<__type>(input, m_cfnum, m_cfden, m_Gain, 2, 1, m_bx, m_by );
        }
    };

    /*! \brief Child lead-lag transfer function 32-bit realization */
    template<> class leadlag<__fx32> final : public leadlag_abstract<__fx32>
    {
        typedef __fx32 __type;
    public:
        leadlag() : leadlag_abstract(){}
        leadlag(__fx64 _Fs, __fx64 _Fn, __fx64 _T1, __fx64 _T2 ) : leadlag_abstract(_Fs, _Fn, _T1, _T2 ){}
        ~leadlag(){};

        inline __type operator ()( __type  *input ) override
        {
            return __filt__<__type>(input, m_cfnum, m_cfden, m_Gain, 2, 1, m_bx, m_by );
        }
    };

    /*! \brief Child lead-lag transfer function 64-bit realization */
    template<> class leadlag<__fx64> final : public leadlag_abstract<__fx64>
    {
        typedef __fx64 __type;
    public:
        leadlag() : leadlag_abstract(){}
        leadlag(__fx64 _Fs, __fx64 _Fn, __fx64 _T1, __fx64 _T2 ) : leadlag_abstract(_Fs, _Fn, _T1, _T2 ){}
        ~leadlag(){};

        inline __type operator ()( __type  *input ) override
        {
            return __filt__<__type>(input, m_cfnum, m_cfden, m_Gain, 2, 1, m_bx, m_by );
        }
    };

    /*! \brief Child second order filter transfer function 32-bit realization */
    template<> class second_order_filter<__fx32> final : public second_order_filter_abstract<__fx32>
    {
        typedef __fx32 __type;
    public:
        second_order_filter() : second_order_filter_abstract(){}
        second_order_filter(__fx64 _Fs , __fx64 _Fn , __fx64 _Fc , __fx64 _Kd , filter_type _type)
            : second_order_filter_abstract(_Fs , _Fn, _Fc, _Kd, _type ){}
        ~second_order_filter(){};

        inline __type operator ()( __type  *input ) override
        {
            return __filt__<__type>(input, m_cfnum, m_cfden, m_Gain, 3, 2, m_bx, m_by );
        }
    };

    /*! \brief Child second order filter transfer function 64-bit realization */
    template<> class second_order_filter<__fx64> final : public second_order_filter_abstract<__fx64>
    {
        typedef __fx64 __type;
    public:
        second_order_filter() : second_order_filter_abstract(){}
        second_order_filter(__fx64 _Fs , __fx64 _Fn , __fx64 _Fc , __fx64 _Kd , filter_type _type)
            : second_order_filter_abstract(_Fs , _Fn, _Fc, _Kd, _type ){}
        ~second_order_filter(){};

        inline __type operator ()( __type  *input ) override
        {
            return __filt__<__type>(input, m_cfnum, m_cfden, m_Gain, 3, 2, m_bx, m_by );
        }
    };
    /*! @} */


    /*! @} */

};

/*! @} */

#endif // DSP_H
