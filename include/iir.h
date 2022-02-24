/*!
 * \file
 * \brief   IIR filters
 * \authors A.Tykvinskiy
 * \date    21.12.2021
 * \version 1.0
 *
 * The header declares IIR filters
*/

#ifndef IIR_H
#define IIR_H

#ifndef DEBUGGING
//#define DEBUGGING
#endif

/*! \brief defines if the compilation is implemented for Prosoft-Systems IDE */
#ifndef __ALG_PLATFORM
#include "cmath"
#include "math.h"
#include <iostream>
#include <cstring>
#endif

/*! \defgroup <IIR_FILTERS> ( IIR filters )
 *  \brief the module contains IIR filter template class and it's auxiliary functions
    @{
*/

#include "buffer.h"
#include "fcomplex.h"
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

namespace IIR
{

/*! \brief template IIR filter zeros / poles data structure */
template < typename T > struct zp;

/*! \brief template IIR filter coefficients matrix data structure */
template < typename T > struct cf;

/*! \brief template IIR filter second order sections input / output buffers */
template < typename T > struct bf;

/*! \brief template IIR filter frequency response data structure */
template < typename T > struct fr;

/*!
 *  \brief 32-bit floating point IIR filter zeros / poles data structure
 *  \param[plp] pointer to the IIR filter nomrmalized analogue lowpass prototype  poles pairs
 *  \param[zlp] pointer to the IIR filter nomrmalized analogue lowpass prototype  zeros pairs
 *  \param[glp] pointer to the IIR filter nomrmalized analogue lowpass prototype  second order sections gains ( size is N + 1 , N-th element is the output gain )
 *  \param[L]   number of the complex conjugate pairs of poles
 *  \param[R]   real odd pole existance flag
 *  \param[N]   number of the IIR filter coefficients
*/
template<> struct zp< __fx32  > { complex< __fx32  > *plp , *zlp , *glp; __ix32 L , R , N; };
/*!
 *  \brief 64-bit floating point IIR filter zeros / poles data structure
 *  \param[plp] pointer to the IIR filter nomrmalized analogue lowpass prototype  poles pairs
 *  \param[zlp] pointer to the IIR filter nomrmalized analogue lowpass prototype  zeros pairs
 *  \param[glp] pointer to the IIR filter nomrmalized analogue lowpass prototype  second order sections gains ( size is N + 1 , N-th element is the output gain )
 *  \param[L]   number of the complex conjugate pairs of poles
 *  \param[R]   real odd pole existance flag
 *  \param[N]   number of the IIR filter coefficients
*/
template<> struct zp< __fx64  > { complex< __fx64  > *plp , *zlp , *glp; __ix32 L , R , N; };
/*!
 *  \brief extended 64-bit floating point IIR filter zeros / poles data structure
 *  \param[plp] pointer to the IIR filter nomrmalized analogue lowpass prototype poles pairs
 *  \param[zlp] pointer to the IIR filter nomrmalized analogue lowpass prototype  zeros pairs
 *  \param[glp] pointer to the IIR filter nomrmalized analogue lowpass prototype  second order sections gains ( size is N + 1 , N-th element is the output gain )
 *  \param[L]   number of the complex conjugate pairs of poles
 *  \param[R]   real odd pole existance flag
 *  \param[N]   number of the IIR filter coefficients
*/
template<> struct zp< __fxx64 > { complex< __fxx64 > *plp , *zlp , *glp; __ix32 L , R , N; };

/*!
 *  \brief 32-bit floating point IIR filter coefficients matrix data structure
 *  \param[cfnum] pointer to the IIR filter numerator coefficicents matrix
 *  \param[cfden] pointer to the IIR filter denominator coefficicents matrix
 *  \param[gains] pointer to the IIR filter second order sections gains ( size is N + 1 , N-th element is the output gain )
 *  \param[L]   number of the complex conjugate pairs of poles
 *  \param[R]   real odd pole existance flag
 *  \param[N]   number of the IIR filter coefficients
*/
template<> struct cf< __fx32  > { __fx32  *cfnum , *cfden , *gains; __ix32 L , R , N; };
/*!
 *  \brief 64-bit floating point IIR filter coefficients matrix data structure
 *  \param[cfnum] pointer to the IIR filter numerator coefficicents matrix
 *  \param[cfden] pointer to the IIR filter denominator coefficicents matrix
 *  \param[gains] pointer to the IIR filter second order sections gains ( size is N + 1 , N-th element is the output gain )
 *  \param[L]   number of the complex conjugate pairs of poles
 *  \param[R]   real odd pole existance flag
 *  \param[N]   number of the IIR filter coefficients
*/
template<> struct cf< __fx64  > { __fx64  *cfnum , *cfden , *gains; __ix32 L , R , N; };
/*!
 *  \brief extended 64-bit floating point IIR filter coefficients matrix data structure
 *  \param[cfnum] pointer to the IIR filter numerator coefficicents matrix
 *  \param[cfden] pointer to the IIR filter denominator coefficicents matrix
 *  \param[gains] pointer to the IIR filter second order sections gains ( size is N + 1 , N-th element is the output gain )
 *  \param[L]   number of the complex conjugate pairs of poles
 *  \param[R]   real odd pole existance flag
 *  \param[N]   number of the IIR filter coefficients
*/
template<> struct cf< __fxx64 > { __fxx64 *cfnum , *cfden , *gains; __ix32 L , R , N; };

/*!
 *  \brief 32-bit floating point IIR filter coefficients matrix data structure
 *  \param[bx] pointer to the IIR filter second order sections input buffers
 *  \param[by] pointer to the IIR filter second order sections output buffers
 *  \param[N]  number of the IIR filter coefficients
*/
template<> struct bf< __fx32  >{ mirror_ring_buffer< __fx32  > *bx , *by; __ix32 N; };
/*!
 *  \brief 64-bit floating point IIR filter coefficients matrix data structure
 *  \param[bx] pointer to the IIR filter second order sections input buffers
 *  \param[by] pointer to the IIR filter second order sections output buffers
 *  \param[N]  number of the IIR filter coefficients
*/
template<> struct bf< __fx64  >{ mirror_ring_buffer< __fx64  > *bx , *by; __ix32 N; };
/*!
 *  \brief extended 64-bit floating point IIR filter coefficients matrix data structure
 *  \param[bx] pointer to the IIR filter second order sections input buffers
 *  \param[by] pointer to the IIR filter second order sections output buffers
 *  \param[N]  number of the IIR filter coefficients
*/
template<> struct bf< __fxx64 >{ mirror_ring_buffer< __fxx64 > *bx , *by; __ix32 N; };

/*!
 *  \brief 32-bit floating point IIR filter frequency response data structure
 *  \param[Km] IIR filter amplitude response , p.u.
 *  \param[pH] IIR filter phase response
*/
template<> struct fr< __fx32  > { __fx32  Km , pH; };
/*!
 *  \brief 64-bit floating point IIR filter frequency response data structure
 *  \param[Km] IIR filter amplitude response , p.u.
 *  \param[pH] IIR filter phase response
*/
template<> struct fr< __fx64  > { __fx64  Km , pH; };
/*!
 *  \brief extended 64-bit floating point IIR filter frequency response data structure
 *  \param[Km] IIR filter amplitude response , p.u.
 *  \param[pH] IIR filter phase response
*/
template<> struct fr< __fxx64 > { __fxx64 Km , pH; };

/*!
 *  \brief IIR filter specification data structure
 *  \param[Fs]      samping frequency       , Hz
 *  \param[Ts]      sampling period         , s
 *  \param[Fn]      nominal frequency       , Hz
 *  \param[Fc]      cut-off frequency       , Hz
 *  \param[BW]      bandpass/bandstop width , Hz
 *  \param[Gs]      bandstop attenuation    , Db
 *  \param[Gp]      bandpass attenuation    , Db
 *  \param[order]   filter order
 *  \param[type]    dilter type( 0 - lowpass , 1 - highpass , 2 - bandpass , 3 - bandstop )
*/
struct sp { __fx64 Fs , Ts , Fn , Fc , BW , Gs , Gp; __ix32 order , type; };

/*!
 * \brief Butterworth lowpass analogue prototype zeros/poles plain computation function
 * \param[g_stop] stopband attenuation , Db
 * \param[order]  filter order
 * \return   The function computes Butterworth zeros/poles pairs of the Butterworth lowpass
 *           analogue prototype. It also compute zero frequency gains. All the data is stored
 *           within zp data structure and returned.
*/
template< typename T > zp< T > __butt_zeros_poles_plain__ ( __ix32 order , __fx64 g_stop )
{
    // number of zeros, poles, coeffs:
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // stopband attenuation:
    T epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

    // memory allocation for the lowpass analogue prototype poles, zeros and gains:
    complex<T> *plp = ( complex<T>* ) calloc( N+0 , sizeof ( complex<T> ) );
    complex<T> *zlp = ( complex<T>* ) calloc( N+0 , sizeof ( complex<T> ) );
    complex<T> *glp = ( complex<T>* ) calloc( N+1 , sizeof ( complex<T> ) );

    if( plp == 0 || zlp == 0 || glp == 0 ) return zp<T>{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // complex-conjugate pairs:
    T alpha = 0 , betta = 1 / sqrt( pow( epsilon_stop , 1 / order ) );

    for( __ix32 i = 0 ; i < L ; i++ )
    {
        alpha = (2 * (i + 1) - 1) * PI0 / (2 * order);
        plp[i] = complex<T>( ( -1 / betta * sin(alpha) ) , ( +1 / betta * cos(alpha) ) );
        glp[i] = plp[i] * __conjf__( plp[i] ) ;
    }

    // real odd pole:
    if( R == 1 )
    {
        plp[ N - 1 ] = complex< T >( ( -1 / betta ) , 0 );
        glp[ N - 1 ] = -plp[ N - 1 ].m_re;
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
template< typename T > zp< T > __cheb1_zeros_poles_plain__( __ix32 order , __fx64 g_stop )
{
    // number of zeros, poles, coeffs:
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // stopband attenuation:
    T epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

    // memory allocation for the lowpass analogue prototype poles, zeros and gains:
    complex< T >  *plp = ( complex<T>* ) calloc( N   , sizeof ( complex< T > ) );
    complex< T >  *zlp = ( complex<T>* ) calloc( N   , sizeof ( complex< T > ) );
    complex< T >  *glp = ( complex<T>* ) calloc( N+1 , sizeof ( complex< T > ) );

    if( plp == 0 || zlp == 0 || glp == 0 ) return zp< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // complex conjugate pairs:
    T alpha = 0 , betta = asinh( 1 / epsilon_stop ) / order;
    for ( __ix32 i = 0; i < L; i++)
    {
        alpha = (2 * (i + 1) - 1) * PI0 / (2 * order);
        plp[i] = complex< T >( -sin( alpha ) * sinh( betta ) , +cos( alpha ) * cosh( betta ) );
        glp[i] = plp[i] * __conjf__( plp[i] );
    }

    // real odd pole:
    if ( R == 1 )
    {
        plp[ N - 1 ] = complex< T >( -sinh( betta ) , 0 );
        glp[ N - 1 ] = -plp[ N - 1 ].m_re;
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
template< typename T > zp< T > __cheb2_zeros_poles_plain__( __ix32 order , __fx64 g_stop )
{
    // stopband attenuation:
    __fx64 epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

    // identify the number of zeros and poles:
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // allocate zeros and poles arrays:
    complex< T > *plp = ( complex< T >* ) calloc( N   , sizeof ( complex< T > ) );
    complex< T > *zlp = ( complex< T >* ) calloc( N   , sizeof ( complex< T > ) );
    complex< T > *glp = ( complex< T >* ) calloc( N+1 , sizeof ( complex< T > ) );

    if( plp == 0 || zlp == 0 || glp == 0 ) return zp< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // ZEROS AND POLES COMPUTATION:

    // complex conjugate pairs:

    // auxiliary variables:
    T alpha , betta = asinh( epsilon_stop ) / order , re , im;

    // zeros , poles, gains computation:
    for ( __ix32 i = 0 ; i < L ; i++ )
    {
        alpha = (2 * ( i + 1 ) - 1) * PI0 / ( 2 * order );

        // zeros:
        zlp[i] = complex< T >( 0 , 1 / cos( alpha ) );

        // poles:
        re = -( sin( alpha ) * sinh( betta ) ) / ( cos( alpha ) * cos( alpha ) * cosh( betta ) * cosh( betta ) + sin( alpha ) * sin( alpha ) * sinh( betta ) * sinh( betta ) );
        im = +( cos( alpha ) * cosh( betta ) ) / ( cos( alpha ) * cos( alpha ) * cosh( betta ) * cosh( betta ) + sin( alpha ) * sin( alpha ) * sinh( betta ) * sinh( betta ) );
        plp[i] = complex< T >( re , im );

        // gains:
        glp[i] = ( zlp[i] * __conjf__( zlp[i] ) ) / ( plp[i] * __conjf__( plp[i] ) );
        glp[i] = complex< T >( 1 , 0 ) / glp[i];
    }

    // real odd pole:
    if( R >= 1 )
    {
        plp[ N - 1 ] = complex< T >( -1 / sinh( betta ) , 0 );
        glp[ N - 1 ] = -plp[N-1].m_re;
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
template< typename T > zp< T > __ellip_zeros_poles_plain__( __ix32 order ,  __fx64 g_pass , __fx64 g_stop )
{
    // INITIALIZATION:

    // stopband and passband attenuation:
    __fx64 epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);
    __fx64 epsilon_pass = sqrt(pow(10, g_pass / 10) - 1);

    // identify the number of zeros and poles:
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // allocate zeros and poles arrays:
    complex< T > *plp = ( complex< T >* ) calloc( N   , sizeof ( complex< T > ) );
    complex< T > *zlp = ( complex< T >* ) calloc( N   , sizeof ( complex< T > ) );
    complex< T > *glp = ( complex< T >* ) calloc( N+1 , sizeof ( complex< T > ) );

    if( plp == 0 || zlp == 0 || glp == 0 ) return zp< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

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
            zlp[i] = complex< T >( 0 , 1 /( Kw * SN ) );
        }
    }
    else if( order % 2 != 0 ) // odd order filter
    {
        for ( __ix32 i = 0; i < L; i++)
        {
            alpha = (2 * i + 2) / ( ( T )order) * KE;
            SN = __sn__( alpha , Kw );
            zlp[i] = complex< T >( 0 , 1 /( Kw * SN ) );
        }
    }

    // POLES COMPUTATION:
    T V0 = 0 , A = 0 , B = 0 , C = 0 , D = 0 , E = 0 , F = 0;
    V0 = -__ellip_k__( Kw ) * __isc__(1 / epsilon_pass, sqrt(1 - Ke * Ke)) / ( __ellip_k__( Ke ) * ( ( T ) order ) );
    KE =  __ellip_k__( Kw );

    // complex conjugate pairs:
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
        plp[i].m_re = A * B * C * D / (1 - E * F);

        // compute auxiliary variables:
        A = __sn__( alpha, Kw );
        B = __dn__( V0 , sqrt(1 - Kw * Kw ) );
        C = __dn__( alpha, Kw );
        D = __sn__( V0, sqrt( 1 - Kw * Kw ) );
        E = C * C;
        F = D * D;

        // pole imaginary part:
        plp[i].m_im = A * B / (1 - E * F);
    }

    // real odd pole:
    if( R >= 1 )
    {
        // compute auxiliary variables:
        A = __sn__( V0 , sqrt( 1 - Kw * Kw ) );
        B = __cn__( V0 , sqrt( 1 - Kw * Kw ) );
        C = A * A;

        // compute the real odd pole:
        plp[ N - 1 ] = complex< T >( A * B / ( 1 - C )  , 0 );
    }

    // GAINS COMPUTATION:

    // complex conjugate pairs:
    if( R >= 1 )
    {
        // complex conjugate pairs:
        for ( __ix32 i = 0; i < L; i++)
        {
            glp[i] = ( zlp[i] * __conjf__( zlp[i] ) ) / ( plp[i] * __conjf__( plp[i] ) );
            glp[i] = complex< T >( 1 , 0 ) / glp[i];
        }

        // real odd pole:
        glp[ N - 1 ] = -plp[ N - 1 ].m_re;

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

template < typename T > cf< T > __butt_cheb1_digital_lp__( __fx64 Fs , __fx64 Fc , __ix32 order , __ix32 type = 0 , __fx64 g_stop = 1 )
{
    // COMPUTE LOWPASS ANALOGUE PROTOTYPE ZEROS, POLES AND GAINS:
    zp< __fx64 > zp = ( !type ) ? __butt_zeros_poles_plain__< __fx64 >( order , g_stop ) : __cheb1_zeros_poles_plain__< __fx64 >( order , g_stop );

    // allocate zeros and poles arrays:
    complex< __fx64 > *plp = zp.plp;
    complex< __fx64 > *zlp = zp.zlp;
    complex< __fx64 > *glp = zp.glp;
    __ix32 L = zp.L , R = zp.R , N = L + R;

    if( plp == 0 || zlp == 0 || glp == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // frequency deformation coefficient:
    __fx64 K = tan( PI2 * Fc / 2 / Fs );

    // coefficients matrix computation:
    T *cfnum = ( T* )calloc( 3 * N , sizeof ( T ) );
    T *cfden = ( T* )calloc( 3 * N , sizeof ( T ) );
    T *gains = ( T* )calloc( N + 1 , sizeof ( T ) );

    if( cfnum == 0 || cfden == 0 || gains == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // BILLINEAR LP-LP TRANSFORM:

    // complex-conjugate pairs:
    for( __ix32 i = 0 ; i < L ; i++ )
    {
        // quadratic section gain:
        complex< __fx64 > gain0 = glp[i];
        complex< __fx64 > gain1 = complex< __fx64 >( K * K , 0 );
        complex< __fx64 > gain2 = ( complex< __fx64 >( 1 , 0 ) - plp[i] * K ) * ( complex< __fx64 >( 1 , 0 ) - __conjf__( plp[i] ) * K );
        gains[i] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[i] = complex< __fx64 >( -1 , 0 );
        plp[i] = ( complex< __fx64 >( 1 , 0 ) + plp[i] * K ) / ( complex< __fx64 >( 1 , 0 ) - plp[i] * K );

        // quadratic section numerator coefficients:
        cfnum[ 3 * i + 0 ] = +1;
        cfnum[ 3 * i + 1 ] = +2;
        cfnum[ 3 * i + 2 ] = +1;

        // quadratic section denumerator coefficients:
        cfden[ 3 * i + 0 ] = 1;
        cfden[ 3 * i + 1 ] = -( plp[i] + __conjf__( plp[i] ) ).m_re;
        cfden[ 3 * i + 2 ] = +( plp[i] * __conjf__( plp[i] ) ).m_re;
    }

    // real odd pole:
    if ( R == 1 )
    {
        // quadratic section gain:
        complex< __fx64 > gain0 = glp[N-1];
        complex< __fx64 > gain1 = complex< __fx64 >( K , 0 );
        complex< __fx64 > gain2 = ( complex< __fx64 >( 1 , 0 ) - plp[N-1] * K );
        gains[N-1] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[ N - 1 ] = complex< __fx64 >( -1 , 0 );
        plp[ N - 1 ] = ( complex< __fx64 >( 1 , 0 ) + plp[N-1] * K ) / ( complex< __fx64 >( 1 , 0 ) - plp[N-1] * K );

        // real odd pole section coefficients computation:

        // numerator:
        cfnum[ 3 * ( N - 1 ) + 0 ] = 1;
        cfnum[ 3 * ( N - 1 ) + 1 ] = - zlp[ N - 1 ].m_re;
        cfnum[ 3 * ( N - 1 ) + 2 ] = 0;

        // denominator:
        cfden[ 3 * ( N - 1 ) + 0 ] = 1;
        cfden[ 3 * ( N - 1 ) + 1 ] = -plp[N-1].m_re;
        cfden[ 3 * ( N - 1 ) + 2 ] = 0;
    }

    // setting filter output gain:
    gains[N] = glp[N].m_re;


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

template < typename T > cf< T > __butt_cheb1_digital_hp__( __fx64 Fs , __fx64 Fp , __ix32 order , __ix32 type = 0 , __fx64 g_stop = 1 )
{
    // INITIALIZATION:
    zp < __fx64 > zp = ( !type ) ? __butt_zeros_poles_plain__< __fx64 >( order , g_stop ) : __cheb1_zeros_poles_plain__< __fx64 >( order , g_stop );
    complex< __fx64 > *plp = zp.plp;
    complex< __fx64 > *zlp = zp.zlp;
    complex< __fx64 > *glp = zp.glp;
    __ix32 L = zp.L , R = zp.R , N = L + R;

    if( plp == 0 || zlp == 0 || glp == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // frequency deformation coefficient:
    T w = tan( PI2 * Fp / 2 / Fs );

    // coefficients matrix computation:
    T *cfnum = ( T* )calloc( 3 * N , sizeof ( T ) );
    T *cfden = ( T* )calloc( 3 * N , sizeof ( T ) );
    T *gains = ( T* )calloc( N + 1 , sizeof ( T ) );

    if( cfnum == 0 || cfden == 0 || gains == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // BILLINEAR LP-HP TRANSFORM:

    // complex conjugate pairs:
    for ( __ix32 i = 0; i < L; i++)
    {
        // gains compputation:
        complex< __fx64 > gain0 = glp[i];
        complex< __fx64 > gain1 = ( complex< __fx64 >( 1 , 0 ) - plp[i] / w ) * ( complex< __fx64 >( 1 , 0 ) - __conjf__( plp[i] ) / w );
        gains[i] = ( gain0 / gain1 / w / w).m_re;

        // zeros and poles transformation:
        zlp[i] = complex< __fx64 >( 1 , 0 );
        plp[i] = ( complex< __fx64 >( 1 , 0 ) + plp[i] / w ) / ( complex< __fx64 >( 1 , 0 ) - plp[i] / w ) * (-1);

        // digital highpass coefficients computation:

        // numerator:
        cfnum[3*i+0] = +1;
        cfnum[3*i+1] = -2;
        cfnum[3*i+2] = +1;

        // denominator:
        cfden[3*i+0] = 1;
        cfden[3*i+1] = -( plp[i] + __conjf__(plp[i]) ).m_re;
        cfden[3*i+2] = +( plp[i] * __conjf__(plp[i]) ).m_re;
    }

    // real odd pole:
    if ( R == 1 )
    {
        // gains compputation:
        complex< __fx64 > gain0 = glp[N-1];
        complex< __fx64 > gain1 = ( complex< __fx64 >( 1 , 0 ) - plp[N-1] / w );
        gains[N-1] = ( gain0 / gain1 / w ).m_re;

        // zeros and poles transformation:
        zlp[ N - 1 ] = complex< __fx64 >( 1 , 0 );
        plp[ N - 1 ] = ( complex< __fx64 >( 1 , 0 ) + plp[ N - 1 ] / w ) / ( complex< __fx64 >( 1 , 0 ) - plp[ N - 1 ] / w ) * (-1);

        // digital highpass coefficients computation:

        // numerator:
        cfnum[3*(N-1)+0] = +1;
        cfnum[3*(N-1)+1] = -2;
        cfnum[3*(N-1)+2] = +1;

        // denominator:
        cfden[3*(N-1)+0] = 1;
        cfden[3*(N-1)+1] = -plp[ N - 1 ].m_re;
        cfden[3*(N-1)+2] = 0;
    }

    // setting filter output gain:
    gains[N] = glp[N].m_re;

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

template < typename T > cf<T> __butt_cheb1_digital_bp__( __fx64 Fs , __fx64 Fp , __fx64 BandWidth , __ix32 order , __ix32 type = 0 , __fx64 g_stop = 1 )
{
    order /= 2;

    // frequency deformation coefficient:
    __fx64 w1 = tan( PI2 * Fp / 2 / Fs ) , w2 = tan( PI2 * ( Fp + BandWidth ) / 2 / Fs );

    // allocate zeros and poles arrays:

    // lowpass analogue prototype poles, zeros and gains:
    zp < __fx64 > zp = ( !type ) ? __butt_zeros_poles_plain__< __fx64 >( order , g_stop ) : __cheb1_zeros_poles_plain__< __fx64 >( order , g_stop );
    complex< __fx64 > *plp = zp.plp;
    complex< __fx64 > *glp = zp.glp;
    complex< __fx64 > *zlp = zp.zlp;
    __ix32 L = zp.L , R = zp.R , N = L + R;

    if( plp == 0 || zlp == 0 || glp == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // bandpass digital prototype poles, zeros and gains:
    complex< __fx64 > *pbp = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );
    complex< __fx64 > *zbp = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );
    complex< __fx64 > *gbp = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );

    // coefficients matrix computation:
    T *cfnum = ( T* )calloc( 3 * (2*L+R) , sizeof ( T ) );
    T *cfden = ( T* )calloc( 3 * (2*L+R) , sizeof ( T ) );
    T *gains = ( T* )calloc( (2*L+R + 1) , sizeof ( T ) );

    if( pbp   == 0 || zbp   == 0 || gbp   == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...
    if( cfnum == 0 || cfden == 0 || gains == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // LP-BP BILLINEAR TRANSFORM:

    // poles transformation:
    __fx64 w0 = w1 * w2;
    __fx64 dW = w2 - w1;

    // complex conjugate pairs:
    __ix32 j = 0;
    for( __ix32 i = 0 ; i < L ; i++ , j+=2 )
    {
        // poles transformation by means of square equation solve:
        complex< __fx64 > a( 1 / w0 , 0 );
        complex< __fx64 > b( -plp[i].m_re * dW / w0 , -plp[i].m_im * dW / w0 );
        complex< __fx64 > c( 1 , 0 );
        complex< __fx64 > D = b * b - a * c * 4;
        complex< __fx64 > p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        complex< __fx64 > p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbp[ j + 0 ] = complex< __fx64 >( +1 , 0 );
        zbp[ j + 1 ] = complex< __fx64 >( -1 , 0 );
        pbp[ j + 0 ] = ( complex< __fx64 >( 1 , 0 ) + p1 ) / ( complex< __fx64 >( 1 , 0 ) - p1 );
        pbp[ j + 1 ] = ( complex< __fx64 >( 1 , 0 ) + p2 ) / ( complex< __fx64 >( 1 , 0 ) - p2 );

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
        cfden[ 3 * ( j + 0 ) + 1 ] = -( pbp[j] + __conjf__( pbp[j] ) ).m_re;
        cfden[ 3 * ( j + 0 ) + 2 ] = +( pbp[j] * __conjf__( pbp[j] ) ).m_re;
        cfden[ 3 * ( j + 1 ) + 0 ] = 1;
        cfden[ 3 * ( j + 1 ) + 1 ] = -( pbp[j+1] + __conjf__( pbp[j+1] ) ).m_re;
        cfden[ 3 * ( j + 1 ) + 2 ] = +( pbp[j+1] * __conjf__( pbp[j+1] ) ).m_re;

        // complex conjugate quadratic sections gains computation:
        complex< __fx64 > gain0 = glp[i];
        complex< __fx64 > gain1 = p1 * __conjf__( p1 );
        complex< __fx64 > gain2 = p2 * __conjf__( p2 );
        complex< __fx64 > gain3 = (complex< __fx64 >(1,0) - p1)*(complex< __fx64 >(1,0) - __conjf__(p1));
        complex< __fx64 > gain4 = (complex< __fx64 >(1,0) - p2)*(complex< __fx64 >(1,0) - __conjf__(p2));
        complex< __fx64 > gain5 = gain0 * gain1 * gain2 / gain3 / gain4 * dW * dW / w0 / w0;
        gain5 = __sqrtf__( gain5 );
        gains[j+0] = gain5.m_re;
        gains[j+1] = gain5.m_re;
    }

    // real odd pole:
    if( R == 1 )
    {
        // pole transformation by means of square equation solve:
        complex< __fx64 > a( 1 / w0 , 0 );
        complex< __fx64 > b( -plp[ N - 1 ].m_re * dW / w0 , -plp[ N - 1 ].m_im * dW / w0 );
        complex< __fx64 > c( 1 , 0 );
        complex< __fx64 > D = b * b - a * c * 4;
        complex< __fx64 > p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        complex< __fx64 > p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbp[ j + 0 ] = complex< __fx64 >( +1 , 0 );
        zbp[ j + 1 ] = complex< __fx64 >( -1 , 0 );
        pbp[ j + 0 ] = ( complex< __fx64 >( 1 , 0 ) + p1 ) / ( complex< __fx64 >( 1 , 0 ) - p1 );
        pbp[ j + 1 ] = ( complex< __fx64 >( 1 , 0 ) + p2 ) / ( complex< __fx64 >( 1 , 0 ) - p2 );

        // digital filter coefficients computation:

        // numerator:
        cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
        cfnum[ 3 * ( j + 0 ) + 1 ] = 0;
        cfnum[ 3 * ( j + 0 ) + 2 ] = -1;

        // denominator:
        cfden[ 3 * ( j + 0 ) + 0 ] = 1;
        cfden[ 3 * ( j + 0 ) + 1 ] = -( pbp[j] + pbp[j+1] ).m_re;
        cfden[ 3 * ( j + 0 ) + 2 ] = +( pbp[j] * pbp[j+1] ).m_re;

        // complex conjugate quadratic sections gains computation:
        complex< __fx64 > gain0 = glp[N-1];
        complex< __fx64 > gain1 = p1 * p2;
        complex< __fx64 > gain2 = (complex< __fx64 >(1,0) - p1)*(complex< __fx64 >(1,0) - p2);
        complex< __fx64 > gain3 = gain0 * gain1 / gain2 * dW / w0;
        gains[j+0] = gain3.m_re;
    }

    // setting filter output gain:
    gains[ 2*L+R ] = glp[N].m_re;

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

template < typename T > cf<T> __butt_cheb1_digital_bs__( __fx64 Fs , __fx64 Fc , __fx64 BandWidth , __ix32 order , __fx32 type = 0 , __fx64 g_stop = 1 )
{

   // INITIALIZATION:
   order /= 2;

   // frequency deformation coefficient:
   __fx64 w1 = tan( PI2 * Fc / 2 / Fs ) , w2 = tan( PI2 * ( Fc + BandWidth ) / 2 / Fs );

   // allocate zeros and poles arrays:

   // lowpass analogue prototype poles, zeros and gains:
   zp < __fx64 > zp = ( !type ) ? __butt_zeros_poles_plain__< __fx64 >( order , g_stop ) : __cheb1_zeros_poles_plain__< __fx64 >( order , g_stop );
   complex< __fx64 > *plp = zp.plp;
   complex< __fx64 > *glp = zp.glp;
   complex< __fx64 > *zlp = zp.zlp;
   __ix32 L = zp.L , R = zp.R , N = L + R;

   if( plp == 0 || zlp == 0 || glp == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // bandpass digital prototype poles, zeros and gains:
    complex< __fx64 > *pbs = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );
    complex< __fx64 > *zbs = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );
    complex< __fx64 > *gbs = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );

    // coefficients matrix computation:
    T *cfnum = (T* )calloc( 3 * (2*L+R) , sizeof ( T ) );
    T *cfden = (T* )calloc( 3 * (2*L+R) , sizeof ( T ) );
    T *gains = (T* )calloc( (2*L+R+1)   , sizeof ( T ) );

    if( pbs   == 0 || pbs   == 0 || pbs   == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...
    if( cfnum == 0 || cfden == 0 || gains == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // LP-BS BILLINEAR TRANSFORM:

    // poles transformation:
    __fx64 w0 = w1 * w2;
    __fx64 dW = w2 - w1;

    // complex conjugate pairs:
    __ix32 j = 0;
    for( __ix32 i = 0 ; i < L ; i++ , j+=2 )
    {
        // poles transformation by means of square equation solve:
        complex< __fx64 > a = complex< __fx64 >( -1 / w0 , 0 );
        complex< __fx64 > b = complex< __fx64 >( dW , 0 ) / plp[i] / w0;
        complex< __fx64 > c = complex< __fx64 >( -1 , 0 );
        complex< __fx64 > D = b * b - a * c * 4;
        complex< __fx64 > p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        complex< __fx64 > p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbs[ j + 0 ] = complex< __fx64 >( +1 , 0 );
        zbs[ j + 1 ] = complex< __fx64 >( -1 , 0 );
        pbs[ j + 0 ] = ( complex< __fx64 >( 1 , 0 ) + p1 ) / ( complex< __fx64 >( 1 , 0 ) - p1 );
        pbs[ j + 1 ] = ( complex< __fx64 >( 1 , 0 ) + p2 ) / ( complex< __fx64 >( 1 , 0 ) - p2 );

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
        cfden[ 3 * ( j + 0 ) + 1 ] = -( pbs[j] + __conjf__( pbs[j] ) ).m_re;
        cfden[ 3 * ( j + 0 ) + 2 ] = +( pbs[j] * __conjf__( pbs[j] ) ).m_re;
        cfden[ 3 * ( j + 1 ) + 0 ] = 1;
        cfden[ 3 * ( j + 1 ) + 1 ] = -( pbs[j+1] + __conjf__( pbs[j+1] ) ).m_re;
        cfden[ 3 * ( j + 1 ) + 2 ] = +( pbs[j+1] * __conjf__( pbs[j+1] ) ).m_re;

        // complex conjugate quadratic sections gains computation:
        complex< __fx64 > gain0 = complex< __fx64 >(1,0);
        complex< __fx64 > gain1 = p1 * __conjf__( p1 );
        complex< __fx64 > gain2 = p2 * __conjf__( p2 );
        complex< __fx64 > gain3 = (complex< __fx64 >(1,0) - p1)*(complex< __fx64 >(1,0) - __conjf__(p1));
        complex< __fx64 > gain4 = (complex< __fx64 >(1,0) - p2)*(complex< __fx64 >(1,0) - __conjf__(p2));
        complex< __fx64 > gain5 = gain0 * gain1 * gain2 / gain3 / gain4 / w0 / w0 * ( 1 + w0 ) * ( 1 + w0 );
        gain5 = __sqrtf__( gain5 );
        gains[j+0] = gain5.m_re;
        gains[j+1] = gains[j+0];
    }

    // real odd pole:
    if( R == 1 )
    {
        // poles transformation by means of square equation solve:
        complex< __fx64 > a = complex< __fx64 >( -1 / w0 , 0 );
        complex< __fx64 > b = complex< __fx64 >( dW , 0 ) / plp[N-1] / w0;
        complex< __fx64 > c = complex< __fx64 >( -1 , 0 );
        complex< __fx64 > D = b * b - a * c * 4;
        complex< __fx64 > p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        complex< __fx64 > p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbs[ j + 0 ] = complex< __fx64 >( +1 , 0 );
        zbs[ j + 1 ] = complex< __fx64 >( -1 , 0 );
        pbs[ j + 0 ] = ( complex< __fx64 >( 1 , 0 ) + p1 ) / ( complex< __fx64 >( 1 , 0 ) - p1 );
        pbs[ j + 1 ] = ( complex< __fx64 >( 1 , 0 ) + p2 ) / ( complex< __fx64 >( 1 , 0 ) - p2 );

        // digital filter coefficients computation:

        // numerator:
        cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
        cfnum[ 3 * ( j + 0 ) + 1 ] = 2 * ( w0 - 1 ) / ( w0 + 1 );
        cfnum[ 3 * ( j + 0 ) + 2 ] = 1;

        // denominator:
        cfden[ 3 * ( j + 0 ) + 0 ] = 1;
        cfden[ 3 * ( j + 0 ) + 1 ] = -( pbs[j]+pbs[j+1] ).m_re;
        cfden[ 3 * ( j + 0 ) + 2 ] = +( pbs[j]*pbs[j+1] ).m_re;

        // complex conjugate quadratic sections gains computation:
        complex< __fx64 > gain0 = glp[N-1];
        complex< __fx64 > gain1 = p1 * p2;
        complex< __fx64 > gain2 = complex< __fx64 >(1,0) / plp[N-1] / (-1);
        complex< __fx64 > gain3 = (complex< __fx64 >(1,0) - p1)*(complex< __fx64 >(1,0) - p2);
        complex< __fx64 > gain4 = gain0 * gain1 * gain2 / gain3 / w0 * ( 1 + w0 );
        gains[j+0] = gain4.m_re;
    }

    // setting filter output gain:
    gains[ 2 * L + R ] = glp[ N ].m_re;

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

template < typename T > cf<T> __cheb2_ellip_digital_lp__( __fx64 Fs , __fx64 Fc , __ix32 order , __ix32 type = 0 , __fx64 g_pass = 1 , __fx64 g_stop = 80 )
{
    __fx64 w = tan( PI2 * Fc / 2 / Fs );

    // digital lowpass coefficients computation:
    zp< __fx64 > zp = ( !type ) ? __cheb2_zeros_poles_plain__< __fx64 >( order , g_stop ) : __ellip_zeros_poles_plain__< __fx64 >( order , g_pass , g_stop );

    // zeros/poles and coefficients number:
    __ix32 L = zp.L;
    __ix32 R = zp.R;
    __ix32 N = zp.N;

    // zeros / poles plain initialization:
    complex< __fx64 > *plp = zp.plp;
    complex< __fx64 > *zlp = zp.zlp;
    complex< __fx64 > *glp = zp.glp;

    if( plp == 0 || zlp == 0 || glp == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // coefficients matrix computation:
     T  *cfnum = ( T * )calloc( 3 * N , sizeof ( T ) );
     T  *cfden = ( T * )calloc( 3 * N , sizeof ( T ) );
     T  *gains = ( T * )calloc( N + 1 , sizeof ( T ) );

    if( cfnum == 0 || cfden == 0 || gains == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // complex conjugate pairs:
    for ( __ix32 i = 0 ; i < L ; i++)
    {
        // quadratic sections gains computation:
        complex< __fx64 > gain0 = glp[i];
        complex< __fx64 > gain1 = ( complex< __fx64 >( 1 , 0 ) - zlp[i] * w ) * ( complex< __fx64 >( 1 , 0 ) - __conjf__( zlp[i] ) * w );
        complex< __fx64 > gain2 = ( complex< __fx64 >( 1 , 0 ) - plp[i] * w ) * ( complex< __fx64 >( 1 , 0 ) - __conjf__( plp[i] ) * w );
        gains[ i ] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[i] = ( complex< __fx64 >( 1 , 0 ) + zlp[i] * w ) / ( complex< __fx64 >( 1 , 0 ) - zlp[i] * w );
        plp[i] = ( complex< __fx64 >( 1 , 0 ) + plp[i] * w ) / ( complex< __fx64 >( 1 , 0 ) - plp[i] * w );

        // quadratic sections coefficients computation:

        // numerator:
        cfnum[ 3 * i + 0 ] = 1;
        cfnum[ 3 * i + 1 ] = -( zlp[i] + __conjf__( zlp[i] ) ).m_re;
        cfnum[ 3 * i + 2 ] = +( zlp[i] * __conjf__( zlp[i] ) ).m_re;

        // denominator:
        cfden[ 3 * i + 0 ] = 1;
        cfden[ 3 * i + 1 ] = -( plp[i] + __conjf__( plp[i] ) ).m_re;
        cfden[ 3 * i + 2 ] = +( plp[i] * __conjf__( plp[i] ) ).m_re;
    }

    // real odd pole:
    if( R >= 1 )
    {
        // quadratic sections gains computation:
        complex< __fx64 > gain0 = glp[ N - 1 ];
        complex< __fx64 > gain1 = complex< __fx64 >( w , 0 );
        complex< __fx64 > gain2 = ( complex< __fx64 >( 1 , 0 ) - plp[ N - 1 ] * w );
        gains[ N - 1 ] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[ N - 1 ] = complex< __fx64 >( -1 , 0 );
        plp[ N - 1 ] = ( complex< __fx64 >( 1 , 0 ) + plp[ N - 1 ] * w ) / ( complex< __fx64 >( 1 , 0 ) - plp[ N - 1 ] * w );

        // quadratic sections coefficients computation:

        // numerator:
        cfnum[ 3 * (N - 1) + 0 ] = 1;
        cfnum[ 3 * (N - 1) + 1 ] = -zlp[ N - 1 ].m_re;
        cfnum[ 3 * (N - 1) + 2 ] = 0;

        // denominator:
        cfden[ 3 * (N - 1) + 0 ] = 1;
        cfden[ 3 * (N - 1) + 1 ] = -plp[ N - 1 ].m_re;
        cfden[ 3 * (N - 1) + 2 ] = 0;

    }

    gains[N] = glp[N].m_re;

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

template < typename T > cf<T> __cheb2_ellip_digital_hp__( __fx64 Fs , __fx64 Fc , __ix32 order , __ix32 type = 0 , __fx64 g_pass = 1 , __fx64 g_stop = 80 )
{
     __fx64  w = tan( PI2 * Fc / 2 / Fs );

    // digital lowpass coefficients computation:
    zp< __fx64 > zp = ( !type ) ? __cheb2_zeros_poles_plain__< __fx64 >( order , g_stop ) : __ellip_zeros_poles_plain__< __fx64 >( order , g_pass , g_stop );

    // zeros/poles and coefficients number:
    __ix32 L = zp.L;
    __ix32 R = zp.R;
    __ix32 N = zp.N;

    // zeros / poles plain initialization:
    complex< __fx64 > *plp = zp.plp;
    complex< __fx64 > *zlp = zp.zlp;
    complex< __fx64 > *glp = zp.glp;

    if( plp == 0 || zlp == 0 || glp == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // coefficients matrix memory allocation:
     T *cfnum = (T* )calloc( 3 * N , sizeof ( T ) );
     T *cfden = (T* )calloc( 3 * N , sizeof ( T ) );
     T *gains = (T* )calloc( N + 1 , sizeof ( T ) );

    if( cfnum == 0 || cfden == 0 || gains == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // coefficients matrix computation:

    // complex conjugate pairs:
    for ( __ix32 i = 0 ; i < L ; i++)
    {
        // quadratic sections gains computation:
        complex< __fx64 > gain0 = glp[i];
        complex< __fx64 > gain1 = ( complex< __fx64 >( 1 , 0 ) - zlp[i] / w ) * ( complex< __fx64 >( 1 , 0 ) - __conjf__( zlp[i] ) / w );
        complex< __fx64 > gain2 = ( complex< __fx64 >( 1 , 0 ) - plp[i] / w ) * ( complex< __fx64 >( 1 , 0 ) - __conjf__( plp[i] ) / w );
        gains[ i ] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[i] = ( complex< __fx64 >( 1 , 0 ) + zlp[i] / w ) / ( complex< __fx64 >( 1 , 0 ) - zlp[i] / w );
        plp[i] = ( complex< __fx64 >( 1 , 0 ) + plp[i] / w ) / ( complex< __fx64 >( 1 , 0 ) - plp[i] / w );

        // quadratic sections coefficients computation:

        // numerator:
        cfnum[ 3 * i + 0 ] = 1;
        cfnum[ 3 * i + 1 ] = +( zlp[i] + __conjf__( zlp[i] ) ).m_re;
        cfnum[ 3 * i + 2 ] = +( zlp[i] * __conjf__( zlp[i] ) ).m_re;

        // denominator:
        cfden[ 3 * i + 0 ] = 1;
        cfden[ 3 * i + 1 ] = +( plp[i] + __conjf__( plp[i] ) ).m_re;
        cfden[ 3 * i + 2 ] = +( plp[i] * __conjf__( plp[i] ) ).m_re;
    }

    // real odd pole:
    if( R >= 1 )
    {
        // quadratic sections gains computation:
        complex< __fx64 > gain0 = glp[ N - 1 ];
        complex< __fx64 > gain1 = complex< __fx64 >( 1 / w , 0 );
        complex< __fx64 > gain2 = ( complex< __fx64 >( 1 , 0 ) - plp[ N - 1 ] / w );
        gains[ N - 1 ] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[ N - 1 ] = complex< __fx64 >( -1 , 0 );
        plp[ N - 1 ] = ( complex< __fx64 >( 1 , 0 ) + plp[ N - 1 ] / w ) / ( complex< __fx64 >( 1 , 0 ) - plp[ N - 1 ] / w );

        // quadratic sections coefficients computation:

        // numerator:
        cfnum[ 3 * (N - 1) + 0 ] = 1;
        cfnum[ 3 * (N - 1) + 1 ] = zlp[ N - 1 ].m_re;
        cfnum[ 3 * (N - 1) + 2 ] = 0;

        // denominator:
        cfden[ 3 * (N - 1) + 0 ] = 1;
        cfden[ 3 * (N - 1) + 1 ] = plp[ N - 1 ].m_re;
        cfden[ 3 * (N - 1) + 2 ] = 0;
    }

    // output gain setting:
    gains[N] = glp[N].m_re;

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

template < typename T > cf<T> __cheb2_ellip_digital_bp__( __fx64 Fs , __fx64 Fp , __fx64 BandWidth , __ix32 order , __ix32 type = 0 , __fx64 g_pass = 1 , __fx64 g_stop = 80 )
{
    order /= 2;

    // frequency deformation coefficient:
    __fx64 w1 = tan( PI2 * Fp / 2 / Fs ) , w2 = tan( PI2 * ( Fp + BandWidth ) / 2 / Fs );

    // allocate zeros and poles arrays:

    // lowpass analogue prototype poles, zeros and gains:
    zp< __fx64 > zp = ( !type ) ? __cheb2_zeros_poles_plain__< __fx64 >( order , g_stop ) : __ellip_zeros_poles_plain__< __fx64 >( order , g_pass , g_stop );
    complex< __fx64 > *plp = zp.plp;
    complex< __fx64 > *glp = zp.glp;
    complex< __fx64 > *zlp = zp.zlp;
    __ix32 L = zp.L , R = zp.R , N = L + R;

    if( plp == 0 || zlp == 0 || glp == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // bandpass digital prototype poles, zeros and gains:
    complex< __fx64 > *pbp = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );
    complex< __fx64 > *zbp = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );
    complex< __fx64 > *gbp = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );

    // coefficients matrix computation:
     T  *cfnum = ( T * )calloc( 3 * (2*L+R) , sizeof (  T  ) );
     T  *cfden = ( T * )calloc( 3 * (2*L+R) , sizeof (  T  ) );
     T  *gains = ( T * )calloc( (2*L+R + 1) , sizeof (  T  ) );

    if( pbp   == 0 || zbp   == 0 || gbp   == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...
    if( cfnum == 0 || cfden == 0 || gains == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // LP-BP BILLINEAR TRANSFORM:

    // poles transformation:
    __fx64 w0 = w1 * w2;
    __fx64 dW = w2 - w1;

    // complex conjugate pairs:
    __ix32 j = 0;

    // auxiliary variables:
    complex< __fx64 > a , b , c , D , z1 , z2 , p1 , p2;

    for( __ix32 i = 0 ; i < L ; i++ , j+=2 )
    {
        // zeros frequency transformation by means of square equation solve:
        a = 1 / w0;
        b = zlp[i] * dW / w0 * (-1);
        c = 1;
        D = b * b - a * c * 4;
        z1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        z2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // poles frequency transformation by means of square equation solve:
        a = 1 / w0;
        b = plp[i] * dW / w0 * (-1);
        c = 1;
        D = b * b - a * c * 4;
        p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbp[ j + 0 ] = ( complex< __fx64 >( 1 , 0 ) + z1 ) / ( complex< __fx64 >( 1 , 0 ) - z1 );
        zbp[ j + 1 ] = ( complex< __fx64 >( 1 , 0 ) + z2 ) / ( complex< __fx64 >( 1 , 0 ) - z2 );
        pbp[ j + 0 ] = ( complex< __fx64 >( 1 , 0 ) + p1 ) / ( complex< __fx64 >( 1 , 0 ) - p1 );
        pbp[ j + 1 ] = ( complex< __fx64 >( 1 , 0 ) + p2 ) / ( complex< __fx64 >( 1 , 0 ) - p2 );

        // filter coefficients computation:

        // numerator:
        cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
        cfnum[ 3 * ( j + 0 ) + 1 ] = -( zbp[j] + __conjf__( zbp[j] ) ).m_re;
        cfnum[ 3 * ( j + 0 ) + 2 ] = +( zbp[j] * __conjf__( zbp[j] ) ).m_re;
        cfnum[ 3 * ( j + 1 ) + 0 ] = 1;
        cfnum[ 3 * ( j + 1 ) + 1 ] = -( zbp[j+1] + __conjf__( zbp[j+1] ) ).m_re;
        cfnum[ 3 * ( j + 1 ) + 2 ] = +( zbp[j+1] * __conjf__( zbp[j+1] ) ).m_re;

        // denominator:
        cfden[ 3 * ( j + 0 ) + 0 ] = 1;
        cfden[ 3 * ( j + 0 ) + 1 ] = -( pbp[j] + __conjf__( pbp[j] ) ).m_re;
        cfden[ 3 * ( j + 0 ) + 2 ] = +( pbp[j] * __conjf__( pbp[j] ) ).m_re;
        cfden[ 3 * ( j + 1 ) + 0 ] = 1;
        cfden[ 3 * ( j + 1 ) + 1 ] = -( pbp[j+1] + __conjf__( pbp[j+1] ) ).m_re;
        cfden[ 3 * ( j + 1 ) + 2 ] = +( pbp[j+1] * __conjf__( pbp[j+1] ) ).m_re;

        // complex conjugate quadratic sections gains computation:
        complex< __fx64 > gain0 = glp[i];
        complex< __fx64 > gain1 = p1 * __conjf__( p1 );
        complex< __fx64 > gain2 = p2 * __conjf__( p2 );
        complex< __fx64 > gain3 = z1 * __conjf__( z1 );
        complex< __fx64 > gain4 = z2 * __conjf__( z2 );
        complex< __fx64 > gain5 = ( complex< __fx64 >(1,0) - z1)*(complex< __fx64 >(1,0) - __conjf__(z1) );
        complex< __fx64 > gain6 = ( complex< __fx64 >(1,0) - z2)*(complex< __fx64 >(1,0) - __conjf__(z2) );
        complex< __fx64 > gain7 = ( complex< __fx64 >(1,0) - p1)*(complex< __fx64 >(1,0) - __conjf__(p1) );
        complex< __fx64 > gain8 = ( complex< __fx64 >(1,0) - p2)*(complex< __fx64 >(1,0) - __conjf__(p2) );

        complex< __fx64 > gain9 = gain0 * gain1 * gain2 / ( gain3 * gain4 ) * ( gain5 * gain6 ) / ( gain7 * gain8 );
        gain9 = __sqrtf__( gain9 );
        gains[j+0] = gain9.m_re;
        gains[j+1] = gains[j+0];
    }

    // real odd pole:
    if( R == 1 )
    {
        // pole transformation by means of square equation solve:
        complex< __fx64 > a( 1 / w0 , 0 );
        complex< __fx64 > b( -plp[ N - 1 ].m_re * dW / w0 , -plp[ N - 1 ].m_im * dW / w0 );
        complex< __fx64 > c( 1 , 0 );
        complex< __fx64 > D = b * b - a * c * 4;
        complex< __fx64 > p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        complex< __fx64 > p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbp[ j + 0 ] = complex< __fx64 >( +1 , 0 );
        zbp[ j + 1 ] = complex< __fx64 >( -1 , 0 );
        pbp[ j + 0 ] = ( complex< __fx64 >( 1 , 0 ) + p1 ) / ( complex< __fx64 >( 1 , 0 ) - p1 );
        pbp[ j + 1 ] = ( complex< __fx64 >( 1 , 0 ) + p2 ) / ( complex< __fx64 >( 1 , 0 ) - p2 );

        // digital filter coefficients computation:

        // numerator:
        cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
        cfnum[ 3 * ( j + 0 ) + 1 ] = 0;
        cfnum[ 3 * ( j + 0 ) + 2 ] = -1;

        // denominator:
        cfden[ 3 * ( j + 0 ) + 0 ] = 1;
        cfden[ 3 * ( j + 0 ) + 1 ] = -( pbp[j] + pbp[j+1] ).m_re;
        cfden[ 3 * ( j + 0 ) + 2 ] = +( pbp[j] * pbp[j+1] ).m_re;

        // complex conjugate quadratic sections gains computation:
        complex< __fx64 > gain0 = glp[N-1];
        complex< __fx64 > gain1 = p1 * p2;
        complex< __fx64 > gain2 = (complex< __fx64 >(1,0) - p1)*(complex< __fx64 >(1,0) - p2);
        complex< __fx64 > gain3 = gain0 * gain1 / gain2 * dW / w0;
        gains[j+0] = gain3.m_re;
    }

    // setting filter output gain:
    gains[ 2*L+R ] = glp[N].m_re;

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

template < typename T > cf<T> __cheb2_ellip_digital_bs__( __fx64 Fs , __fx64 Fc , __fx64 BandWidth , __ix32 order , __ix32 type = 0 , __fx64 g_pass = 1 , __fx64 g_stop = 80 )
{
    order /= 2;

    // frequency deformation coefficient:
    __fx64 w1 = tan( PI2 * Fc / 2 / Fs ) , w2 = tan( PI2 * ( Fc + BandWidth ) / 2 / Fs );

    // allocate zeros and poles arrays:

    // lowpass analogue prototype poles, zeros and gains:
    zp < __fx64 > zp = ( !type ) ? __cheb2_zeros_poles_plain__< __fx64 >( order , g_stop ) : __ellip_zeros_poles_plain__< __fx64 >( order , g_pass , g_stop );
    complex< __fx64 > *plp = zp.plp;
    complex< __fx64 > *glp = zp.glp;
    complex< __fx64 > *zlp = zp.zlp;
    __ix32 L = zp.L , R = zp.R , N = L + R;

    if( plp == 0 || zlp == 0 || glp == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // bandpass digital prototype poles, zeros and gains:
    complex< __fx64 > *pbs = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );
    complex< __fx64 > *zbs = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );
    complex< __fx64 > *gbs = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );

    // coefficients matrix computation:
     T  *cfnum = ( T * )calloc( 3 * (2*L+R) , sizeof ( T ) );
     T  *cfden = ( T * )calloc( 3 * (2*L+R) , sizeof ( T ) );
     T  *gains = ( T * )calloc( (2*L+R + 1) , sizeof ( T ) );

    if( pbs   == 0 || zbs   == 0 || gbs   == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...
    if( cfnum == 0 || cfden == 0 || gains == 0 ) return cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // LP-BP BILLINEAR TRANSFORM:

    // poles transformation:
    __fx64  w0 = w1 * w2;
    __fx64  dW = w2 - w1;

    // complex conjugate pairs:
    __ix32 j = 0;

    // auxiliary variables:
    complex< __fx64 > a , b , c , D , z1 , z2 , p1 , p2;

    for( __ix32 i = 0 ; i < L ; i++ , j+=2 )
    {
        // zeros frequency transformation by means of square equation solve:
        a = -1 / w0;
        b = complex< __fx64 >( 1 , 0 ) / zlp[i] * dW / w0;
        c = -1;
        D = b * b - a * c * 4;
        z1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        z2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // poles frequency transformation by means of square equation solve:
        a = -1 / w0;
        b = complex< __fx64 >( 1 , 0 ) / plp[i] * dW / w0;
        c = -1;
        D = b * b - a * c * 4;
        p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbs[ j + 0 ] = ( complex< __fx64 >( 1 , 0 ) + z1 ) / ( complex< __fx64 >( 1 , 0 ) - z1 );
        zbs[ j + 1 ] = ( complex< __fx64 >( 1 , 0 ) + z2 ) / ( complex< __fx64 >( 1 , 0 ) - z2 );
        pbs[ j + 0 ] = ( complex< __fx64 >( 1 , 0 ) + p1 ) / ( complex< __fx64 >( 1 , 0 ) - p1 );
        pbs[ j + 1 ] = ( complex< __fx64 >( 1 , 0 ) + p2 ) / ( complex< __fx64 >( 1 , 0 ) - p2 );

        // filter coefficients computation:

        // numerator:
        cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
        cfnum[ 3 * ( j + 0 ) + 1 ] = -( zbs[j] + __conjf__( zbs[j] ) ).m_re;
        cfnum[ 3 * ( j + 0 ) + 2 ] = +( zbs[j] * __conjf__( zbs[j] ) ).m_re;
        cfnum[ 3 * ( j + 1 ) + 0 ] = 1;
        cfnum[ 3 * ( j + 1 ) + 1 ] = -( zbs[j+1] + __conjf__( zbs[j+1] ) ).m_re;
        cfnum[ 3 * ( j + 1 ) + 2 ] = +( zbs[j+1] * __conjf__( zbs[j+1] ) ).m_re;

        // denominator:
        cfden[ 3 * ( j + 0 ) + 0 ] = 1;
        cfden[ 3 * ( j + 0 ) + 1 ] = -( pbs[j] + __conjf__( pbs[j] ) ).m_re;
        cfden[ 3 * ( j + 0 ) + 2 ] = +( pbs[j] * __conjf__( pbs[j] ) ).m_re;
        cfden[ 3 * ( j + 1 ) + 0 ] = 1;
        cfden[ 3 * ( j + 1 ) + 1 ] = -( pbs[j+1] + __conjf__( pbs[j+1] ) ).m_re;
        cfden[ 3 * ( j + 1 ) + 2 ] = +( pbs[j+1] * __conjf__( pbs[j+1] ) ).m_re;

        // complex conjugate quadratic sections gains computation:
        complex< __fx64 > gain0 = p1 * __conjf__( p1 );
        complex< __fx64 > gain1 = p2 * __conjf__( p2 );
        complex< __fx64 > gain2 = z1 * __conjf__( z1 );
        complex< __fx64 > gain3 = z2 * __conjf__( z2 );
        complex< __fx64 > gain4 = ( complex< __fx64 >(1,0) - z1)*(complex< __fx64 >(1,0) - __conjf__(z1) );
        complex< __fx64 > gain5 = ( complex< __fx64 >(1,0) - z2)*(complex< __fx64 >(1,0) - __conjf__(z2) );
        complex< __fx64 > gain6 = ( complex< __fx64 >(1,0) - p1)*(complex< __fx64 >(1,0) - __conjf__(p1) );
        complex< __fx64 > gain7 = ( complex< __fx64 >(1,0) - p2)*(complex< __fx64 >(1,0) - __conjf__(p2) );
        complex< __fx64 > gain8 = ( gain0 * gain1 ) / ( gain2 * gain3 ) * ( gain4 * gain5 ) / ( gain6 * gain7 );
        gain8 = __sqrtf__( gain8 );
        gains[j+0] = gain8.m_re;
        gains[j+1] = gains[j+0];
    }

    // real odd pole:
    if( R == 1 )
    {
        // poles transformation by means of square equation solve:
        complex< __fx64 > a = complex< __fx64 >( -1 / w0 , 0 );
        complex< __fx64 > b = complex< __fx64 >( dW , 0 ) / plp[N-1] / w0;
        complex< __fx64 > c = complex< __fx64 >( -1 , 0 );
        complex< __fx64 > D = b * b - a * c * 4;
        complex< __fx64 > p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        complex< __fx64 > p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbs[ j + 0 ] = complex< __fx64 >( +1 , 0 );
        zbs[ j + 1 ] = complex< __fx64 >( -1 , 0 );
        pbs[ j + 0 ] = ( complex< __fx64 >( 1 , 0 ) + p1 ) / ( complex< __fx64 >( 1 , 0 ) - p1 );
        pbs[ j + 1 ] = ( complex< __fx64 >( 1 , 0 ) + p2 ) / ( complex< __fx64 >( 1 , 0 ) - p2 );

        // digital filter coefficients computation:

        // numerator:
        cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
        cfnum[ 3 * ( j + 0 ) + 1 ] = 2 * ( w0 - 1 ) / ( w0 + 1 );
        cfnum[ 3 * ( j + 0 ) + 2 ] = 1;

        // denominator:
        cfden[ 3 * ( j + 0 ) + 0 ] = 1;
        cfden[ 3 * ( j + 0 ) + 1 ] = -( pbs[j]+pbs[j+1] ).m_re;
        cfden[ 3 * ( j + 0 ) + 2 ] = +( pbs[j]*pbs[j+1] ).m_re;

        // complex conjugate quadratic sections gains computation:
        complex< __fx64 > gain0 = glp[N-1];
        complex< __fx64 > gain1 = p1 * p2;
        complex< __fx64 > gain2 = complex< __fx64 >(1,0) / plp[N-1] / (-1);
        complex< __fx64 > gain3 = (complex< __fx64 >(1,0) - p1)*(complex< __fx64 >(1,0) - p2);
        complex< __fx64 > gain4 = gain0 * gain1 * gain2 / gain3 / w0 * ( 1 + w0 );
        gains[j+0] = gain4.m_re;
    }

    // setting filter output gain:
    gains[ 2*L+R ] = glp[N].m_re;

    // memory free:
    free( plp );
    free( zlp );
    free( glp );
    free( pbs );
    free( zbs );
    free( gbs );

    return cf< T >{ cfnum , cfden , gains , 2*L , R , 2*L+R  };
}

/*!
 * \brief    IIR filter input/output buffers memory allocation function
 * \param[N] IIR filer input/output buffers size
 * \return   The function allocates memory for IIR filter input/output buffers
*/

template< typename T > bf<T> __bf_alloc__( __ix32 N )
{
    mirror_ring_buffer<T> *bx = new mirror_ring_buffer<T> [ N ];
    mirror_ring_buffer<T> *by = new mirror_ring_buffer<T> [ N ];

    for( __ix32 i = 0 ; i < N ; i++ )
    {
        bx[i].allocate( 4 );
        by[i].allocate( 3 );
    }

    return bf< T >{ bx , by , N };
}

/*!
 * \brief     IIR filter input/output buffers memory deallocation function
 * \param[bf] IIR filer input/output buffers data structure
 * \return    The function deallocates memory for IIR filter input/output buffers
*/

template< typename T > bf<T> __bf_free__( bf<T> bf )
{
    mirror_ring_buffer<T> *bx = bf.bx , *by = bf.by;
    __ix32 N = bf.N;

    if( bx != 0 )
    {
        for( __ix32 i = 0 ; i < N ; i++ ) bx[i].deallocate();
        delete[] bx;
        bx = 0;
    }

    if( by != 0 )
    {
        for( __ix32 i = 0 ; i < N ; i++ ) by[i].deallocate();
        delete[] by;
        by = 0;
    }

    return { 0 , 0 , -1 };
}

/*!
 * \brief     IIR filter zeros/poles plain memory allocation function
 * \param[zp] IIR filer zeros/poles plain data structure
 * \return    The function allocates memory for IIR filter zeros/poles plain
*/

template< typename T > zp< T > __zp_free__( zp< T > zp )
{
    if( zp.glp ) { free( zp.glp );  }
    if( zp.zlp ) { free( zp.zlp );  }
    if( zp.plp ) { free( zp.plp );  }
    zp.N = zp.L = zp.R = -1;
    return { 0 , 0 , 0 , -1 , -1 , -1 };
}

/*!
 * \brief     IIR filter coefficients memory allocation function
 * \param[cf] IIR filer coefficients data structure
 * \return    The function allocates memory for IIR filter coefficients
*/

template< typename T > cf< T > __cf_free__( cf< T > cf )
{
    if( cf.cfnum ) { free( cf.cfnum );  }
    if( cf.cfden ) { free( cf.cfden );  }
    if( cf.gains ) { free( cf.gains );  }
    cf.N = cf.L = cf.R = -1;
    return { 0 , 0 , 0 , -1 , -1 , -1 };
}

/*!
 * \brief     IIR filter template filtering function
 * \param[input  ] - input samples
 * \param[cfnum  ] - IIR numerator   second order sections coefficients
 * \param[cfden  ] - IIR denominator second order sections coefficients
 * \param[gains  ] - IIR second order sections gains
 * \param[N      ] - IIR order
 * \param[buff_sx] - IIR second order sections input  buffers
 * \param[buff_sy] - IIR second order sections output buffers
 * \return    The function implements input signal filtering using the IIR second order sections coefficients, gains and input/output buffers
*/

template< typename T > extern inline __attribute__( (always_inline) ) T __filt__( T *input , T *cfnum , T *cfden , T *gains  , __ix32 N ,  mirror_ring_buffer< T > *buff_sx , mirror_ring_buffer< T > *buff_sy )
{
    if( !cfnum || !cfden || !gains || !input || !buff_sx || !buff_sy || ( N < 0 ) ) return 0;

    // initialization:
    T sum_num = 0 , sum_den = 0 , out = 0;

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
 * \brief     IIR filter template frequency response computation function
 * \param[input  ] - input samples
 * \param[cfnum  ] - IIR numerator   second order sections coefficients
 * \param[cfden  ] - IIR denominator second order sections coefficients
 * \param[gains  ] - IIR second order sections gains
 * \param[N      ] - IIR order
 * \param[Fs     ] - sampling frequency     , Hz
 * \param[F      ] - input signal frequency , Hz
 * \return           The function implements IIR filter frequency response for the predefined input signal frequency and sampling rate.
 *                   The function operates using IIR second order sections coefficients and gains. Frequency resopnse computation result
 *                   is stored within fr data structure.
*/

template< typename T > fr< __fx64 > __freq_resp__( T *cfnum , T *cfden , T *gains , __ix32 N , __fx64 Fs , __fx64 F )
{
    // sampling period:
    __fx64 Ts = 1 / Fs;

    // transfer function initialization:
    complex< __fx64 > tsf = complex< __fx64 >( 1, 0 );

    for( __ix32 i = 0 ; i < N ; i++ )
    {
        // complex exponents:
        complex< __fx64 > z0 = complex<__fx64>( cos( -PI2 * F * Ts * 0 ) , sin( -PI2 * F * Ts * 0 ) );
        complex< __fx64 > z1 = complex<__fx64>( cos( -PI2 * F * Ts * 1 ) , sin( -PI2 * F * Ts * 1 ) );
        complex< __fx64 > z2 = complex<__fx64>( cos( -PI2 * F * Ts * 2 ) , sin( -PI2 * F * Ts * 2 ) );

        // transfer function:
        complex< __fx64 > num =  z0 * cfnum[ 3 * i + 0 ] + z1 * cfnum[ 3 * i + 1 ] + z2 * cfnum[ 3 * i + 2 ];
        complex< __fx64 > den =  z0 * cfden[ 3 * i + 0 ] + z1 * cfden[ 3 * i + 1 ] + z2 * cfden[ 3 * i + 2 ];
        tsf *= num / den * gains[i];
    }

    // multiply by an output gain:
    tsf *= gains[N];

    // output:
    return fr< __fx64 >{ __absf__( tsf ) , __argf__( tsf ) };
}

#ifndef __ALG_PLATFORM

/*!
 * \brief             IIR filter coefficients show function
 * \param[coeffs  ] - IIR coefficients data structure
 * \return           The function implements IIR filter frequency response for the predefined input signal frequency and sampling rate.
 *                   The function outputs IIR filter second order sections coefficicents on the console.
*/
template < typename T > void __show_coeffs__( cf< T > coeffs )
{
    __ix32 L = coeffs.L , R = coeffs.R , N = L + R;
    __fx64 *cfnum = coeffs.cfnum;
    __fx64 *cfden = coeffs.cfden;
    __fx64 *gains = coeffs.gains;
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

#endif

/*! \brief IIR filter abstract class */
template< typename T >  class iir_abstract
{
    typedef T __type ;
protected:

    /*!
     *  \brief IIR types enumeration
     *  \param[lowpass ] lowpass  IIR
     *  \param[highpass] highpass IIR
     *  \param[bandpass] bandpass IIR
     *  \param[bandstop] bandstop IIR
    */
    enum type { lowpass  , highpass , bandpass , bandstop };

    /*! \brief IIR filter coefficients */
    cf< __type > m_cf;
    /*! \brief IIR filter I/O buffers data structure */
    bf< __type > m_bf;
    /*! \brief IIR filter specification data structure */
    sp m_sp;
    /*! \brief IIR filter name */
    char m_name[256];

    /*! \brief lowpass IIR filter coefficients virtual computation function */
    virtual cf< __type > compute_lowpass () = 0;
    /*! \brief highpass IIR filter coefficients virtual computation function */
    virtual cf< __type > compute_highpass() = 0;
    /*! \brief bandpass IIR filter coefficients virtual computation function */
    virtual cf< __type > compute_bandpass() = 0;
    /*! \brief bandstop IIR filter coefficients virtual computation function */
    virtual cf< __type > compute_bandstop() = 0;

public:

    /*! \brief filter output */
    __type m_out;

    /*!
     *  \brief lowpass IIR filter initialization function
     *  \param[Fs   ] - input signal sampling frequency , Hz
     *  \param[Fn   ] - input signal nominal  frequency , Hz
     *  \param[Fc   ] - IIR filter cut-off frequency    , Hz
     *  \param[order] - IIR filter order
     *  \param[Gs   ] - IIR filter bandstop attenuation , Db
     *  \param[Gp   ] - IIR filter bandpass attenuation , Db
    */
    void lp_init( __fx64 Fs , __fx64 Fn , __fx64 Fc ,  __ix32 order , __fx64 Gs , __fx64 Gp = 1 ) { m_sp = sp{ Fs , 1 / Fs , Fn , Fc , -1 , Gs , Gp , order , type::lowpass  }; };

    /*!
     *  \brief highpass IIR filter initialization function
     *  \param[Fs   ] - input signal sampling frequency , Hz
     *  \param[Fn   ] - input signal nominal  frequency , Hz
     *  \param[Fc   ] - IIR filter pass frequency       , Hz
     *  \param[order] - IIR filter order
     *  \param[Gs   ] - IIR filter bandstop attenuation , Db
     *  \param[Gp   ] - IIR filter bandpass attenuation , Db
    */
    void hp_init( __fx64 Fs , __fx64 Fn , __fx64 Fc ,  __ix32 order , __fx64 Gs , __fx64 Gp = 1 ) { m_sp = sp{ Fs , 1 / Fs , Fn , Fc , -1 , Gs , Gp , order , type::highpass }; };

    /*!
     *  \brief bandpass IIR filter initialization function
     *  \param[Fs   ] - input signal sampling frequency , Hz
     *  \param[Fn   ] - input signal nominal  frequency , Hz
     *  \param[Fc   ] - IIR filter cut-off frequency    , Hz
     *  \param[BW   ] - IIR filter passband width       , Hz
     *  \param[order] - IIR filter order
     *  \param[Gs   ] - IIR filter bandstop attenuation , Db
     *  \param[Gp   ] - IIR filter bandpass attenuation , Db
    */
    void bp_init( __fx64 Fs , __fx64 Fn , __fx64 Fc , __fx64 BW ,  __ix32 order , __fx64 Gs , __fx64 Gp = 1 ) { m_sp = sp{ Fs , 1 / Fs , Fn , Fc , BW , Gs , Gp , order , type::bandpass }; };

    /*!
     *  \brief bandstop IIR filter initialization function
     *  \param[Fs   ] - input signal sampling frequency , Hz
     *  \param[Fn   ] - input signal nominal  frequency , Hz
     *  \param[Fc   ] - IIR filter cut-off frequency    , Hz
     *  \param[BW   ] - IIR filter passband width       , Hz
     *  \param[order] - IIR filter order
     *  \param[Gs   ] - IIR filter bandstop attenuation , Db
     *  \param[Gp   ] - IIR filter bandpass attenuation , Db
    */
    void bs_init( __fx64 Fs , __fx64 Fn , __fx64 Fc , __fx64 BW ,  __ix32 order , __fx64 Gs , __fx64 Gp = 1 ) { m_sp = sp{ Fs , 1 / Fs , Fn , Fc , BW , Gs , Gp , order , type::bandstop }; };

    /*! \brief memory allocation function */
    __ix32 allocate()
    {
        switch ( m_sp.type )
        {
            case type::lowpass:
            m_cf = compute_lowpass ();
            break;

            case type::highpass:
            m_cf = compute_highpass();
            break;

            case type::bandpass:
            m_cf = compute_bandpass();
            break;

            case type::bandstop:
            m_cf = compute_bandstop();
            break;
        }

        m_bf = ( ( m_cf.cfnum != 0 ) && ( m_cf.cfden != 0 ) && ( m_cf.gains != 0 ) ) ? __bf_alloc__< __type >( m_cf.N ) : bf< __type >{ 0 , 0 , -1 } ;
        return ( m_bf.bx != 0 && m_bf.by != 0 );
    }

    /*! \brief memory deallocation function */
    void deallocate()
    {
        m_bf = __bf_free__< __type >( m_bf );
        m_cf = __cf_free__< __type >( m_cf );
    }

    /*! \brief default constructor */
    iir_abstract()
    {
        m_cf = cf< __type >{ 0 , 0 , 0 , -1 , -1 , -1 };
        m_bf = bf< __type >{ 0 , 0 , -1 };
        m_sp = sp{ 4000 , 1 / 4000 , 50 , 100 , -1 , 1 , 80 , 4 , 0 };
    }

    /*!
     *  \brief frequency response computation function
     *  \param[ F ] - input signal frequency , Hz
    */
    fr< __fx64 > freq_resp( __fx64 F ) { return __freq_resp__( m_cf.cfnum , m_cf.cfden , m_cf.gains , m_cf.N , m_sp.Fs , F ); }

    /*!
     *  \brief filtering function
     *  \param[ input ] - pointer to the input signal samples
    */
    __type filt( __type *input ) { return ( m_out = __filt__< __type >( input , m_cf.cfnum , m_cf.cfden , m_cf.gains , m_cf.N , m_bf.bx , m_bf.by ) ); }

    /*!
     *  \brief filtering operator
     *  \param[ input ] - pointer to the input signal samples
    */
    inline __type operator () (  __type *input  ) { return filt( input ); }

    /*! \brief virtual destructor */
    virtual ~iir_abstract() { deallocate(); }

    #ifndef __ALG_PLATFORM

    /*!
     * \brief         IIR filter specification and coefficients show function
     * \param[sp   ] - IIR specification data structure
     * \param[sp   ] - IIR coefficients data structure
     * \param[name ] - IIR name
     * \return           The function outputs IIR filter specification and coefficients on the console
    */
    template < typename F > void __show_iir__( sp sp , cf < F > cf , const char *name )
    {
        printf( "%s " , name );

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
        if( sp.Gs > 0    ) { printf( "Gs    = %.6f Db \n"   , sp.Gs    ); }
        if( sp.Gp > 0    ) { printf( "Gp    = %.6f Db \n"   , sp.Gp    ); }
        if( sp.order > 0 ) { printf( "order = %d      \n"   , sp.order ); }
        if( cf.N > 0     ) { printf( "N     = %d      \n"   , cf.N     ); }
        if( cf.L > 0     ) { printf( "L     = %d      \n"   , cf.L     ); }
        if( cf.R > 0     ) { printf( "R     = %d      \n\n" , cf.R     ); }

        __fx64 fgain = 1;
        if( cf.cfden != 0 && cf.cfnum != 0 && cf.gains != 0 )
        {
            printf( "coefficients: \n\n" );
            for( __ix32 i = 0 ; i < cf.N ; i++ )
            {
                printf( " section %d : \n" , i );
                printf( " numerator  : %.12f %.12f %.12f \n" , cf.cfnum[ 3 * i + 0 ] , cf.cfnum[ 3 * i + 1 ]  , cf.cfnum[ 3 * i + 2 ] );
                printf( " denominator: %.12f %.12f %.12f \n" , cf.cfden[ 3 * i + 0 ] , cf.cfden[ 3 * i + 1 ]  , cf.cfden[ 3 * i + 2 ] );
                printf( " gain       : %.12f \n\n" , cf.gains[i] );
                fgain *= cf.gains[i];
            }
            printf( "output gain: %.12f \n" , cf.gains[ cf.N ]         );
            printf( "filter gain: %.12f \n\n" , fgain * cf.gains[ cf.N ] );
        }
        else
        {
            printf( "coefficients : coefficients have not been computed yet... \n" );
        }
    }

    /*! \brief debugging function */
    void show_properties() { __show_iir__< __type >( m_sp , m_cf , m_name ); }
    #endif
};

/*! \brief transfer function abstract class */
template< typename T = __fx32 > class transfer_function_abstract
{
    typedef T __type;
protected:
    __fx64 m_Fs;
    __fx64 m_Fn;
    __fx64 m_Ts;
    __fx64 m_T1;
    __fx64 m_T2;
    __fx64 m_Kd;
    __fx64 m_Fc;
    __fx64 m_Gain;

    mirror_ring_buffer< __type > m_bx;
    mirror_ring_buffer< __type > m_by;
    __type *m_cfnum;
    __type *m_cfden;

    inline __type __tf_filt__( __type *input , __type *cfnum , __type *cfden , __type gain , __ix32 Nx , __ix32 Ny , mirror_ring_buffer<__type> &bx , mirror_ring_buffer<__type> &by )
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

public:

    /*!
     *  \brief IIR types enumeration
     *  \param[lowpass ] lowpass  IIR
     *  \param[highpass] highpass IIR
     *  \param[bandpass] bandpass IIR
     *  \param[bandstop] bandstop IIR
    */
    enum type { lowpass  , highpass , bandpass , bandstop1 , bandstop2 };

    __type m_out;

    virtual __ix32 allocate() = 0;
    virtual __type operator() ( __type *input ) = 0;

    void deallocate()
    {
        m_bx.deallocate();
        m_by.deallocate();
        if( m_cfnum != 0 ) { free( m_cfnum ); m_cfnum = 0; }
        if( m_cfden != 0 ) { free( m_cfden ); m_cfden = 0; }
    }

    transfer_function_abstract()
    {
        m_Fs    = 4000;
        m_Fn    = 50;
        m_Ts    = 1 / m_Fs;
        m_T1    = 0.01;
        m_T2    = 0.02;
        m_Kd    = 0.707;
        m_Fc    = 120;
        m_Gain  = 1;
        m_cfden = 0;
        m_cfnum = 0;
        m_out   = 0;
    };
    virtual ~transfer_function_abstract(){ deallocate(); };
};

/*! \brief Child differentiator template class */
template< typename T > class differentiator;
/*! \brief Child aperiodic ( delay ) template class */
template< typename T > class aperiodic;
/*! \brief Child integrator template class */
template< typename T > class integrator;
/*! \brief Child leadlag template class */
template< typename T > class leadlag;
/*! \brief Child second order filter template class */
template< typename T > class second_order_filter;
/*! \brief Child Butterworth IIR filter template class */
template< typename T > class butterworth;
/*! \brief Child Chebyshev type I IIR filter template class */
template< typename T > class chebyshev_1;
/*! \brief Child Chebyshev type II IIR filter template class */
template< typename T > class chebyshev_2;
/*! \brief Child Elliptic IIR filter template class */
template< typename T > class elliptic;

/*! \brief Child differentiator 32-bit realization */
template<> class differentiator< __fx32 > final : public transfer_function_abstract< __fx32 >
{
    typedef __fx32 __type;
public:

     // initialization:
     void init( __fx64 Fs , __fx64 Fn , __fx64 Td )
     {
         m_Fs = Fs;
         m_Ts = 1 / Fs;
         m_Fn = Fn;
         m_T1 = Td;
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

     // constructors and destructor:
     differentiator() : transfer_function_abstract(){}
     differentiator( __fx64 Fs , __fx64 Fn , __fx64 Td ){ init( Fs , Fn , Td ); allocate(); };
     ~differentiator(){};

     // filtering operator:
     inline __type operator()( __type *input ) override { return ( m_out = __tf_filt__( input , m_cfnum , m_cfden , m_Gain , 2 , 1 , m_bx , m_by ) ); }
};

/*! \brief Child differentiator 64-bit realization */
template<> class differentiator< __fx64 > final : public transfer_function_abstract< __fx64 >
{
    typedef __fx64 __type;
public:

     // initialization:
     void init( __fx64 Fs , __fx64 Fn , __fx64 Td )
     {
         m_Fs = Fs;
         m_Ts = 1 / Fs;
         m_Fn = Fn;
         m_T1 = Td;
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

     // constructors and destructor:
     differentiator() : transfer_function_abstract(){}
     differentiator( __fx64 Fs , __fx64 Fn , __fx64 Td ){ init( Fs , Fn , Td ); allocate(); };
     ~differentiator(){};

     // filtering operator:
     inline __type operator()( __type *input ) override { return ( m_out = __tf_filt__( input , m_cfnum , m_cfden , m_Gain , 2 , 1 , m_bx , m_by ) ); }
};

/*! \brief Child aperiodic ( delay ) 32-bit realization */
template<> class aperiodic< __fx32 > final : public transfer_function_abstract< __fx32 >
{
    typedef __fx32 __type;
public:

    // initialization function:
    void init( __fx64 Fs , __fx64 Fn , __fx64 Ta )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_T1 = Ta;
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

    // constructors and destructors:
    aperiodic() : transfer_function_abstract(){}
    aperiodic(__fx64 Fs , __fx64 Fn , __fx64 Ta ){ init( Fs , Fn , Ta ); }
    ~aperiodic(){};

    // filtering operator:
    inline __type operator() ( __type *input ) override { return ( m_out = __tf_filt__( input , m_cfnum , m_cfden , m_Gain , 2 , 1 , m_bx , m_by ) ); }
};

/*! \brief Child aperiodic ( delay ) 32-bit realization */
template<> class aperiodic< __fx64 > final : public transfer_function_abstract< __fx64 >
{
    typedef __fx64 __type;
public:

    // initialization function:
    void init( __fx64 Fs , __fx64 Fn , __fx64 Ta )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_T1 = Ta;
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

    // constructors and destructors:
    aperiodic() : transfer_function_abstract(){}
    aperiodic(__fx64 Fs , __fx64 Fn , __fx64 Ta ){ init( Fs , Fn , Ta ); allocate(); }
    ~aperiodic(){};

    // filtering operator:
    inline __type operator() ( __type *input ) override { return ( m_out = __tf_filt__( input , m_cfnum , m_cfden , m_Gain , 2 , 1 , m_bx , m_by ) ); }
};

/*! \brief Child integrator 32-bit realization */
template<> class integrator< __fx32 > final : public transfer_function_abstract< __fx32 >
{
    typedef __fx32 __type;
public:

    // initialization function:
    void init( __fx64 Fs , __fx64 Fn )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
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

    // constructors and destructor:
    integrator() : transfer_function_abstract(){}
    integrator( __fx64 Fs , __fx64 Fn ){ init( Fs , Fn ); allocate(); }
    ~integrator(){};

    // filtering operator:
    inline __type operator()( __type *input ) override { return ( m_out = __tf_filt__( input , m_cfnum , m_cfden , m_Gain , 2 , 1 , m_bx , m_by ) ); }
};

/*! \brief Child integrator 64-bit realization */
template<> class integrator< __fx64 > final : public transfer_function_abstract< __fx64 >
{
    typedef __fx64 __type;
public:

    // initialization function:
    void init( __fx64 Fs , __fx64 Fn )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
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

    // constructors and destructor:
    integrator() : transfer_function_abstract(){}
    integrator( __fx64 Fs , __fx64 Fn ){ init( Fs , Fn ); allocate(); }
    ~integrator(){};

    // filtering operator:
    inline __type operator()( __type *input ) override { return ( m_out = __tf_filt__( input , m_cfnum , m_cfden , m_Gain , 2 , 1 , m_bx , m_by ) ); }
};

/*! \brief Child leadlag 32-bit realization */
template<> class leadlag< __fx32 > final : public transfer_function_abstract< __fx32 >
{
    typedef __fx32 __type;
public:

    // initialization function:
    void init( __fx64 Fs , __fx64 Fn , __fx64 T1 , __fx64 T2 )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_T1 = T1;
        m_T2 = T2;
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

    // constructors and destructor:
    leadlag() : transfer_function_abstract(){}
    leadlag( __fx64 Fs , __fx64 Fn , __fx64 T1 , __fx64 T2 ){ init( Fs , Fn , T1 , T2 ); }
    ~leadlag(){};

    // filtering operator:
    inline __type operator()( __type *input ) override { return ( m_out = __tf_filt__( input , m_cfnum , m_cfden , m_Gain , 2 , 1 , m_bx , m_by ) ); }
};

/*! \brief Child leadlag 64-bit realization */
template<> class leadlag< __fx64 > final : public transfer_function_abstract< __fx64 >
{
    typedef __fx64 __type;
public:

    // initialization function:
    void init( __fx64 Fs , __fx64 Fn , __fx64 T1 , __fx64 T2 )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_T1 = T1;
        m_T2 = T2;
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

    // constructors and destructor:
    leadlag() : transfer_function_abstract(){}
    leadlag( __fx64 Fs , __fx64 Fn , __fx64 T1 , __fx64 T2 ){ init( Fs , Fn , T1 , T2 ); }
    ~leadlag(){};

    // filtering operator:
    inline __type operator()( __type *input ) override { return ( m_out = __tf_filt__( input , m_cfnum , m_cfden , m_Gain , 2 , 1 , m_bx , m_by ) ); }
};

/*! \brief Child second order filter 32-bit realization */
template<> class second_order_filter< __fx32 > final : public transfer_function_abstract< __fx32 >
{
    typedef __fx32 __type;
    __ix32 m_type;
public:

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
            case type::lowpass:
                m_Gain     = omega * omega / k1;
                m_cfnum[0] = 1;
                m_cfnum[1] = 2;
                m_cfnum[2] = 1;
                m_cfden[0] = 1;
                m_cfden[1] = k2 / k1;
                m_cfden[2] = k3 / k1;
            break;

            case type::highpass:
                m_Gain     = 1 / k1;
                m_cfnum[0] = +1;
                m_cfnum[1] = -2;
                m_cfnum[2] = +1;
                m_cfden[0] = +1;
                m_cfden[1] = k2 / k1;
                m_cfden[2] = k3 / k1;
            break;

            case type::bandpass:
                m_Gain     = omega / m_Kd / k1;;
                m_cfnum[0] = +1;
                m_cfnum[1] = -0;
                m_cfnum[2] = -1;
                m_cfden[0] = +1;
                m_cfden[1] = k2 / k1;
                m_cfden[2] = k3 / k1;
            break;

            case type::bandstop1:
                m_Gain     = 1 / k1;
                m_cfnum[0] = 1 + omega * omega;
                m_cfnum[1] = 2 * omega*omega - 2;
                m_cfnum[2] = 1 + omega * omega;
                m_cfden[0] = 1;
                m_cfden[1] = k2 / k1;
                m_cfden[2] = k3 / k1;
            break;

            case type::bandstop2:
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

    void init( __fx64 Fs , __fx64 Fn , __fx64 Fc , __fx64 Kd , type tp )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_Kd = Kd;
        m_Fc = Fc;
        m_type = tp;
    }

    // constructors and destructor:
    second_order_filter() : transfer_function_abstract(){ m_type = type :: lowpass; }
    second_order_filter( __fx64 Fs , __fx64 Fn , __fx64 Kd , __fx64 Fc , type tp ){ init( Fs , Fn , Kd , Fc , tp ); }
    ~second_order_filter(){};

    // filtering operator:
    inline __type operator () ( __type *input ) override { return ( m_out = __tf_filt__( input , m_cfnum , m_cfden , m_Gain , 3 , 2 , m_bx , m_by ) ); }
};

/*! \brief Child second order filter 32-bit realization */
template<> class second_order_filter< __fx64 > final : public transfer_function_abstract< __fx64 >
{
    typedef __fx64 __type;
    __ix32 m_type;
public:

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
            case type::lowpass:
                m_Gain     = omega * omega / k1;
                m_cfnum[0] = 1;
                m_cfnum[1] = 2;
                m_cfnum[2] = 1;
                m_cfden[0] = 1;
                m_cfden[1] = k2 / k1;
                m_cfden[2] = k3 / k1;
            break;

            case type::highpass:
                m_Gain     = 1 / k1;
                m_cfnum[0] = +1;
                m_cfnum[1] = -2;
                m_cfnum[2] = +1;
                m_cfden[0] = +1;
                m_cfden[1] = k2 / k1;
                m_cfden[2] = k3 / k1;
            break;

            case type::bandpass:
                m_Gain     = omega / m_Kd / k1;;
                m_cfnum[0] = +1;
                m_cfnum[1] = -0;
                m_cfnum[2] = -1;
                m_cfden[0] = +1;
                m_cfden[1] = k2 / k1;
                m_cfden[2] = k3 / k1;
            break;

            case type::bandstop1:
                m_Gain     = 1 / k1;
                m_cfnum[0] = 1 + omega * omega;
                m_cfnum[1] = 2 * omega*omega - 2;
                m_cfnum[2] = 1 + omega * omega;
                m_cfden[0] = 1;
                m_cfden[1] = k2 / k1;
                m_cfden[2] = k3 / k1;
            break;

            case type::bandstop2:
                k1 = -cos(2 * PI0 * m_Fc * m_Ts);
                k2 = (1 - tan(PI0 * m_Kd * m_Ts)) / (1 + tan(PI0 * m_Kd * m_Ts));
                m_Gain = 0.5 * (1 + k2);
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

    void init( __fx64 Fs , __fx64 Fn , __fx64 Fc , __fx64 Kd , type tp )
    {
        m_Fs   = Fs;
        m_Ts   = 1 / Fs;
        m_Fn   = Fn;
        m_Kd   = Kd;
        m_Fc   = Fc;
        m_type = tp;
    }

    // constructors and destructor:
    second_order_filter() : transfer_function_abstract(){ m_type = type :: lowpass; }
    second_order_filter( __fx64 Fs , __fx64 Fn , __fx64 Kd , __fx64 Fc , type tp ){ init( Fs , Fn , Kd , Fc , tp ); }
    ~second_order_filter(){};

    // filtering operator:
    inline __type operator () ( __type *input ) override { return ( m_out = __tf_filt__( input , m_cfnum , m_cfden , m_Gain , 3 , 2 , m_bx , m_by ) ); }
};

/*! \brief Child Butterworth IIR filter 32-bit realization */
template<> class butterworth < __fx32 > final : public iir_abstract< __fx32 >
{
    typedef __fx32 __type ;
private:

    // base class virtual functions overriding:
    cf< __type > compute_lowpass () override { return __butt_cheb1_digital_lp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , 0 , m_sp.Gs ); }
    cf< __type > compute_highpass() override { return __butt_cheb1_digital_hp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , 0 , m_sp.Gs ); }
    cf< __type > compute_bandpass() override { return __butt_cheb1_digital_bp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 0 , m_sp.Gs ); }
    cf< __type > compute_bandstop() override { return __butt_cheb1_digital_bs__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 0 , m_sp.Gs ); }

public:

    // default constructor and destructor:
      butterworth() : iir_abstract() { strcpy( m_name , "Butterworth" ); }
      // default destructor:
     ~butterworth(){};
};

/*! \brief Child Butterworth IIR filter 64-bit realization */
template<> class butterworth < __fx64 > final : public iir_abstract< __fx64 >
{
    typedef __fx64 __type ;
private:

    // base class virtual functions overriding:
    cf< __type > compute_lowpass () override { return __butt_cheb1_digital_lp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , 0 , m_sp.Gs ); }
    cf< __type > compute_highpass() override { return __butt_cheb1_digital_hp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , 0 , m_sp.Gs ); }
    cf< __type > compute_bandpass() override { return __butt_cheb1_digital_bp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 0 , m_sp.Gs ); }
    cf< __type > compute_bandstop() override { return __butt_cheb1_digital_bs__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 0 , m_sp.Gs ); }

public:

    // default constructor and destructor:
    butterworth() : iir_abstract() { strcpy( m_name , "Butterworth" ); }
    // default destructor:
    ~butterworth(){};
};

/*! \brief Child Chebyshev type I IIR filter 32-bit realization */
template<> class chebyshev_1 < __fx32 > final : public iir_abstract< __fx32 >
{
    typedef __fx32 __type ;
private:

    // base class virtual functions overriding:
    cf< __type > compute_lowpass () override { return __butt_cheb1_digital_lp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , 1 , m_sp.Gs ); }
    cf< __type > compute_highpass() override { return __butt_cheb1_digital_hp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , 1 , m_sp.Gs ); }
    cf< __type > compute_bandpass() override { return __butt_cheb1_digital_bp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 1 , m_sp.Gs ); }
    cf< __type > compute_bandstop() override { return __butt_cheb1_digital_bs__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 1 , m_sp.Gs ); }

public:

    // default constructor and destructor:
      chebyshev_1() : iir_abstract() { strcpy( m_name , "Chebyshev_I" ); }

      // default destructor:
     ~chebyshev_1(){};
};

/*! \brief Child Chebyshev type I IIR filter 64-bit realization */
template<> class chebyshev_1 < __fx64 > final : public iir_abstract< __fx64 >
{
    typedef __fx64 __type ;
private:

    // base class virtual functions overriding:
    cf< __type > compute_lowpass () override { return __butt_cheb1_digital_lp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , 1 , m_sp.Gs ); }
    cf< __type > compute_highpass() override { return __butt_cheb1_digital_hp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , 1 , m_sp.Gs ); }
    cf< __type > compute_bandpass() override { return __butt_cheb1_digital_bp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 1 , m_sp.Gs ); }
    cf< __type > compute_bandstop() override { return __butt_cheb1_digital_bs__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 1 , m_sp.Gs ); }

public:

    // default constructor and destructor:
      chebyshev_1() : iir_abstract() { strcpy( m_name , "Chebyshev_I" ); }

      // default destructor:
     ~chebyshev_1(){};
};

/*! \brief Child Chebyshev type II IIR filter 32-bit realization */
template<> class chebyshev_2 < __fx32 > final : public iir_abstract< __fx32 >
{
    typedef __fx32 __type ;
private:

    // base class virtual functions overriding:
    cf< __type > compute_lowpass () override { return __cheb2_ellip_digital_lp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , 0 , m_sp.Gp , m_sp.Gs ); }
    cf< __type > compute_highpass() override { return __cheb2_ellip_digital_hp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , 0 , m_sp.Gp , m_sp.Gs ); }
    cf< __type > compute_bandpass() override { return __cheb2_ellip_digital_bp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 0 , m_sp.Gp , m_sp.Gs ); }
    cf< __type > compute_bandstop() override { return __cheb2_ellip_digital_bs__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 0 , m_sp.Gp , m_sp.Gs ); }

public:

    // default constructor and destructor:
    chebyshev_2() : iir_abstract() { strcpy( m_name , "Chebyshev_II" ); }

    // default destructor:
    ~chebyshev_2(){};
};

/*! \brief Child Chebyshev type I IIR filter 64-bit realization */
template<> class chebyshev_2 < __fx64 > final : public iir_abstract< __fx64 >
{
    typedef __fx64 __type ;
private:

    // base class virtual functions overriding:
    cf< __type > compute_lowpass () override { return __cheb2_ellip_digital_lp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , 0 , m_sp.Gp , m_sp.Gs ); }
    cf< __type > compute_highpass() override { return __cheb2_ellip_digital_hp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , 0 , m_sp.Gp , m_sp.Gs ); }
    cf< __type > compute_bandpass() override { return __cheb2_ellip_digital_bp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 0 , m_sp.Gp , m_sp.Gs ); }
    cf< __type > compute_bandstop() override { return __cheb2_ellip_digital_bs__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 0 , m_sp.Gp , m_sp.Gs ); }

public:

    // default constructor and destructor:
    chebyshev_2() : iir_abstract() { strcpy( m_name , "Chebyshev_II" ); }

    // default destructor:
    ~chebyshev_2(){};
};

/*! \brief Child Elliptic IIR filter 32-bit realization */
template<> class elliptic < __fx32 > final : public iir_abstract< __fx32 >
{
    typedef __fx32 __type ;
private:

    // base class virtual functions overriding:
    cf< __type > compute_lowpass () override { return __cheb2_ellip_digital_lp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , 1 , m_sp.Gp , m_sp.Gs ); }
    cf< __type > compute_highpass() override { return __cheb2_ellip_digital_hp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , 1 , m_sp.Gp , m_sp.Gs ); }
    cf< __type > compute_bandpass() override { return __cheb2_ellip_digital_bp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 1 , m_sp.Gp , m_sp.Gs ); }
    cf< __type > compute_bandstop() override { return __cheb2_ellip_digital_bs__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 1 , m_sp.Gp , m_sp.Gs ); }

public:

    // default constructor and destructor:
    elliptic() : iir_abstract(){ strcpy( m_name , "Elliptic" ); }

    // default destructor:
    ~elliptic(){};
};

/*! \brief Child Elliptic IIR filter 64-bit realization */
template<> class elliptic < __fx64 > final : public iir_abstract< __fx64 >
{
    typedef __fx64 __type ;
private:

    // base class virtual functions overriding:
    cf< __type > compute_lowpass () override { return __cheb2_ellip_digital_lp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , 1 , m_sp.Gp , m_sp.Gs ); }
    cf< __type > compute_highpass() override { return __cheb2_ellip_digital_hp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.order , 1 , m_sp.Gp , m_sp.Gs ); }
    cf< __type > compute_bandpass() override { return __cheb2_ellip_digital_bp__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 1 , m_sp.Gp , m_sp.Gs ); }
    cf< __type > compute_bandstop() override { return __cheb2_ellip_digital_bs__< __type >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 1 , m_sp.Gp , m_sp.Gs ); }

public:

    // default constructor and destructor:
    elliptic() : iir_abstract(){ strcpy( m_name , "Elliptic" ); }

    // default destructor:
    ~elliptic(){};
};

};

/*! @} */

// customized macros exclusion to avoid aliases during compilation:
#undef __fx32
#undef __fx64
#undef __ix32
#undef PI0
#undef PI2
#undef DEBUGGING

#endif
