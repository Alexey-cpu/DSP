//------------------------------------------------------------------------------------------------------------
// A.Tykvinskiy , 23.12.2021
//------------------------------------------------------------------------------------------------------------
// INFINITE IMPULSE RESPONSE FILTERS
//------------------------------------------------------------------------------------------------------------
/*
    Naming conventions:

    IIR - infinite impulse response

    plp   - poles of the normalized lowpass analogue prototype
    zlp   - zeros of the normalized lowpass analogue prototype
    glp   - gains of the normalized lowpass analogue prototype
    pbp   - poles of the bandpass analogue/digital prototype
    zbp   - zeros of the bandpass analogue/digital prototype
    gbp   - gains of the bandpass analogue/digital prototype
    pbs   - poles of the bandstop analogue/digital prototype
    zbs   - zeros of the bandstop analogue/digital prototype
    gbs   - gains of the bandstop analogue/digital prototype
    L     - complex conjugate poles / zeros second order sections number
    R     - real odd second order section identifier
    N     - second order sections total numbeer
    Fs    - sampling frequecny        , Hz
    Ts    - sampling period           , s
    Fn    - nominal frequency         , Hz
    Fc    - cutoff frequency          , Hz
    BW    - stopband / passband width , Hz
    Gs    - stopband attenuation      , Db
    Gp    - passband attenuation      , Db
    order - IIR filter order
    type  - IIR filter type ( 0 - lowpass , 1 - highpass , 2 - bandpass , 3 - bandstop )
    cfnum - second order sections numeator coefficients
    cfden - second order sections denominator coefficients
    gains - second order sections gains ( including filter output gain )
    bx    - second order sections input  buffers
    by    - second order sections output buffers
    Km    - filter amplitude frequency response
    pH    - filter phase frequency response

*/

#ifndef IIR_H
#define IIR_H

#ifndef __ALG_PLATFORM // identify if the compilation is for ProsoftSystems IDE
#include "cmath"
#include "math.h"
#include <iostream>

#define DEBUG

#endif

#include "buffer.h"
#include "complex.h"
#include "special_functions.h"

#ifndef __fx32
#define __fx32 float
#endif

#ifndef __fx64
#define __fx64 double
#endif

#ifndef __fxx64
#define __fxx64 long double
#endif

#ifndef __ix32
#define __ix32 int
#endif

// customized PI:
#ifndef PI0
#define PI0 3.1415926535897932384626433832795
#endif

#ifndef PI2
#define PI2 6.283185307179586476925286766559
#endif

// IIR template zeros / poles data structure:
template < typename T > struct iir_zp;
template<> struct iir_zp< __fx32  > { complex< __fx32  > *plp , *zlp , *glp; __ix32 L , R , N; };
template<> struct iir_zp< __fx64  > { complex< __fx64  > *plp , *zlp , *glp; __ix32 L , R , N; };
template<> struct iir_zp< __fxx64 > { complex< __fxx64 > *plp , *zlp , *glp; __ix32 L , R , N; };

// IIR template coefficients and gains data structure:
template < typename T > struct iir_cf;
template<> struct iir_cf< __fx32  > { __fx32  *cfnum , *cfden , *gains; __ix32 L , R , N; };
template<> struct iir_cf< __fx64  > { __fx64  *cfnum , *cfden , *gains; __ix32 L , R , N; };
template<> struct iir_cf< __fxx64 > { __fxx64 *cfnum , *cfden , *gains; __ix32 L , R , N; };

// IIR template buffers data structure:
template< typename T > struct iir_bf;
template<> struct iir_bf< __fx32  >{ mirror_ring_buffer< __fx32  > *bx , *by; __ix32 N; };
template<> struct iir_bf< __fx64  >{ mirror_ring_buffer< __fx64  > *bx , *by; __ix32 N; };
template<> struct iir_bf< __fxx64 >{ mirror_ring_buffer< __fxx64 > *bx , *by; __ix32 N; };

// IIR template frequency response data structure:
template < typename T > struct iir_fr;
template<> struct iir_fr< __fx32  > { __fx32  Km , pH; };
template<> struct iir_fr< __fx64  > { __fx64  Km , pH; };
template<> struct iir_fr< __fxx64 > { __fxx64 Km , pH; };

// IIR specification data structure:
struct iir_sp { __fx64 Fs , Ts , Fn , Fc , BW , Gs , Gp; __ix32 order , type; };

// IIR ZEROS / POLES PLAIN COMPUTATION FUNCTIONS:

// Butterworth lowpass analogue normalized zeros/poles plain computation:
template< typename T > iir_zp< T > __butt_zeros_poles_plain__ ( __ix32 order , __fx64 g_stop )
{
    // number of zeros, poles, coeffs:
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // stopband attenuation:
    T epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

    // memory allocation for the lowpass analogue prototype poles, zeros and gains:
    complex<T> *plp = ( complex<T>* ) calloc( N+0 , sizeof ( complex<T> ) );
    complex<T> *zlp = ( complex<T>* ) calloc( N+0 , sizeof ( complex<T> ) );
    complex<T> *glp = ( complex<T>* ) calloc( N+1 , sizeof ( complex<T> ) );

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_zp<T>{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

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

    return iir_zp< T > { plp , zlp , glp , L , R , N };
}

// Chebyshev type I lowpass analogue normalized zeros/poles plain computation:
template< typename T > iir_zp< T > __cheb1_zeros_poles_plain__( __ix32 order , __fx64 g_stop )
{
    // number of zeros, poles, coeffs:
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // stopband attenuation:
    T epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

    // memory allocation for the lowpass analogue prototype poles, zeros and gains:
    complex< T >  *plp = ( complex<T>* ) calloc( N   , sizeof ( complex< T > ) );
    complex< T >  *zlp = ( complex<T>* ) calloc( N   , sizeof ( complex< T > ) );
    complex< T >  *glp = ( complex<T>* ) calloc( N+1 , sizeof ( complex< T > ) );

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_zp< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

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
    return iir_zp< T >{ plp , zlp , glp , L , R , N };
}

// Chebyshev type II lowpass analogue normalized zeros/poles plain computation:
template< typename T > iir_zp< T > __cheb2_zeros_poles_plain__( __ix32 order , __fx64 g_stop )
{
    // INITIALIZATION:

    // stopband attenuation:
    __fx64 epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

    // identify the number of zeros and poles:
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // allocate zeros and poles arrays:
    complex< T > *plp = ( complex< T >* ) calloc( N   , sizeof ( complex< T > ) );
    complex< T > *zlp = ( complex< T >* ) calloc( N   , sizeof ( complex< T > ) );
    complex< T > *glp = ( complex< T >* ) calloc( N+1 , sizeof ( complex< T > ) );

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_zp< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

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

    return iir_zp< T >{ plp , zlp , glp , L , R , N };
}

// Elliptic lowpass analogue normalized zeros/poles plain computation:
template< typename T > iir_zp< T > __ellip_zeros_poles_plain__( __ix32 order ,  __fx64 g_pass , __fx64 g_stop )
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

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_zp< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

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

    return iir_zp< T >{ plp , zlp , glp , L , R , N };
}

// UNIT NUMERATOR IIR COEFFICIENTS COMPUTATION:

// Butterworth or Chebyshev I digital lowpass coefficients computation:
template < typename T > iir_cf< T > __butt_cheb1_digital_lp__( __fx64 Fs , __fx64 Fc , __ix32 order , __ix32 type = 0 , __fx64 g_stop = 1 )
{
    // Fs           - sampling frequency
    // Fstop        - cutoff frequency
    // order        - filter order
    // coeffs_num   - quadratic sections numerator coefficients
    // coeffs_den   - quadratic sections denominator coefficients
    // gains        - quadratic sections gains
    // type         - filter type ( 0 - Butterworth , 1 - Chebyshev )
    // g_stop       - stopband attenuation in Db

    // COMPUTE LOWPASS ANALOGUE PROTOTYPE ZEROS, POLES AND GAINS:
    iir_zp< T > zp = ( !type ) ? __butt_zeros_poles_plain__< T >( order , g_stop ) : __cheb1_zeros_poles_plain__< T >( order , g_stop );

    // allocate zeros and poles arrays:
    complex< T > *plp = zp.plp;
    complex< T > *zlp = zp.zlp;
    complex< T > *glp = zp.glp;
    __ix32 L = zp.L , R = zp.R , N = L + R;

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // frequency deformation coefficient:
    T K = tan( PI2 * Fc / 2 / Fs );

    // coefficients matrix computation:
    T *cfnum = ( T* )calloc( 3 * N , sizeof ( T ) );
    T *cfden = ( T* )calloc( 3 * N , sizeof ( T ) );
    T *gains = ( T* )calloc( N + 1 , sizeof ( T ) );

    if( cfnum == 0 || cfden == 0 || gains == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // BILLINEAR LP-LP TRANSFORM:

    // complex-conjugate pairs:
    for( __ix32 i = 0 ; i < L ; i++ )
    {
        // quadratic section gain:
        complex< T > gain0 = glp[i];
        complex< T > gain1 = complex< T >( K * K , 0 );
        complex< T > gain2 = ( complex< T >( 1 , 0 ) - plp[i] * K ) * ( complex< T >( 1 , 0 ) - __conjf__( plp[i] ) * K );
        gains[i] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[i] = complex< T >( -1 , 0 );
        plp[i] = ( complex< T >( 1 , 0 ) + plp[i] * K ) / ( complex< T >( 1 , 0 ) - plp[i] * K );

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
        complex< T > gain0 = glp[N-1];
        complex< T > gain1 = complex< T >( K , 0 );
        complex< T > gain2 = ( complex< T >( 1 , 0 ) - plp[N-1] * K );
        gains[N-1] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[ N - 1 ] = complex< T >( -1 , 0 );
        plp[ N - 1 ] = ( complex< T >( 1 , 0 ) + plp[N-1] * K ) / ( complex< T >( 1 , 0 ) - plp[N-1] * K );

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

    return iir_cf< T >{ cfnum , cfden , gains , L , R , N };
}

// Butterworth or Chebyshev I digital highpass coefficients computation:
template < typename T > iir_cf< T > __butt_cheb1_digital_hp__( __fx64 Fs , __fx64 Fp , __ix32 order , __ix32 type = 0 , __fx64 g_stop = 1 )
{
    // Fs           - sampling frequency
    // Fpass        - cutoff frequency
    // order        - filter order
    // coeffs_num   - quadratic sections numerator coefficients
    // coeffs_den   - quadratic sections denominator coefficients
    // gains        - quadratic sections gains
    // type         - filter type ( 0 - Butterworth , 1 - Chebyshev )
    // g_stop       - stopband attenuation in Db

    // INITIALIZATION:
    iir_zp < T > zp = ( !type ) ? __butt_zeros_poles_plain__< T >( order , g_stop ) : __cheb1_zeros_poles_plain__< T >( order , g_stop );
    complex< T > *plp = zp.plp;
    complex< T > *zlp = zp.zlp;
    complex< T > *glp = zp.glp;
    __ix32 L = zp.L , R = zp.R , N = L + R;

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // frequency deformation coefficient:
    T w = tan( PI2 * Fp / 2 / Fs );

    // coefficients matrix computation:
    T *cfnum = ( T* )calloc( 3 * N , sizeof ( T ) );
    T *cfden = ( T* )calloc( 3 * N , sizeof ( T ) );
    T *gains = ( T* )calloc( N + 1 , sizeof ( T ) );

    if( cfnum == 0 || cfden == 0 || gains == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // BILLINEAR LP-HP TRANSFORM:

    // complex conjugate pairs:
    for ( __ix32 i = 0; i < L; i++)
    {
        // gains compputation:
        complex< T > gain0 = glp[i];
        complex< T > gain1 = ( complex< T >( 1 , 0 ) - plp[i] / w ) * ( complex< T >( 1 , 0 ) - __conjf__( plp[i] ) / w );
        gains[i] = ( gain0 / gain1 / w / w).m_re;
        //gains[i] = ( gain0 * gain0 / gain1 / w / w).m_re;

        // zeros and poles transformation:
        zlp[i] = complex< T >( 1 , 0 );
        plp[i] = ( complex< T >( 1 , 0 ) + plp[i] / w ) / ( complex< T >( 1 , 0 ) - plp[i] / w ) * (-1);

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
        complex< T > gain0 = glp[N-1];
        complex< T > gain1 = ( complex< T >( 1 , 0 ) - plp[N-1] / w );
        gains[N-1] = ( gain0 / gain1 / w ).m_re;

        // zeros and poles transformation:
        zlp[ N - 1 ] = complex< T >( 1 , 0 );
        plp[ N - 1 ] = ( complex< T >( 1 , 0 ) + plp[ N - 1 ] / w ) / ( complex< T >( 1 , 0 ) - plp[ N - 1 ] / w ) * (-1);

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

    return iir_cf< T >{ cfnum , cfden , gains , L , R , N };
}

// Butterworth or Chebyshev I digital bandpass coefficients computation:
template < typename T > iir_cf<T> __butt_cheb1_digital_bp__( __fx64 Fs , __fx64 Fp , __fx64 BandWidth , __ix32 order , __ix32 type = 0 , __fx64 g_stop = 1 )
{
    // Fs           - sampling frequency
    // Fpass        - passband frequency
    // BandWidth    - passband withd
    // order        - filter order
    // coeffs_num   - quadratic sections numerator coefficients
    // coeffs_den   - quadratic sections denominator coefficients
    // gains        - quadratic sections gains
    // type         - filter type ( 0 - Butterworth , 1 - Chebyshev )
    // g_stop       - stopband attenuation in Db

     order /= 2;

    // frequency deformation coefficient:
    T w1 = tan( PI2 * Fp / 2 / Fs ) , w2 = tan( PI2 * ( Fp + BandWidth ) / 2 / Fs );

    // allocate zeros and poles arrays:

    // lowpass analogue prototype poles, zeros and gains:
    iir_zp < T > zp = ( !type ) ? __butt_zeros_poles_plain__< T >( order , g_stop ) : __cheb1_zeros_poles_plain__< T >( order , g_stop );
    complex< T > *plp = zp.plp;
    complex< T > *glp = zp.glp;
    complex< T > *zlp = zp.zlp;
    __ix32 L = zp.L , R = zp.R , N = L + R;

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // bandpass digital prototype poles, zeros and gains:
    complex< T > *pbp = ( complex< T >* ) calloc( 2*N , sizeof ( complex< T > ) );
    complex< T > *zbp = ( complex< T >* ) calloc( 2*N , sizeof ( complex< T > ) );
    complex< T > *gbp = ( complex< T >* ) calloc( 2*N , sizeof ( complex< T > ) );

    // coefficients matrix computation:
    T *cfnum = ( T* )calloc( 3 * (2*L+R) , sizeof ( T ) );
    T *cfden = ( T* )calloc( 3 * (2*L+R) , sizeof ( T ) );
    T *gains = ( T* )calloc( (2*L+R + 1) , sizeof ( T ) );

    if( pbp   == 0 || zbp   == 0 || gbp   == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...
    if( cfnum == 0 || cfden == 0 || gains == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // LP-BP BILLINEAR TRANSFORM:

    // poles transformation:
    T w0 = w1 * w2;
    T dW = w2 - w1;

    // complex conjugate pairs:
    __ix32 j = 0;
    for( __ix32 i = 0 ; i < L ; i++ , j+=2 )
    {
        // poles transformation by means of square equation solve:
        complex< T > a( 1 / w0 , 0 );
        complex< T > b( -plp[i].m_re * dW / w0 , -plp[i].m_im * dW / w0 );
        complex< T > c( 1 , 0 );
        complex< T > D = b * b - a * c * 4;
        complex< T > p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        complex< T > p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbp[ j + 0 ] = complex< T >( +1 , 0 );
        zbp[ j + 1 ] = complex< T >( -1 , 0 );
        pbp[ j + 0 ] = ( complex< T >( 1 , 0 ) + p1 ) / ( complex< T >( 1 , 0 ) - p1 );
        pbp[ j + 1 ] = ( complex< T >( 1 , 0 ) + p2 ) / ( complex< T >( 1 , 0 ) - p2 );

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
        complex< T > gain0 = glp[i];
        complex< T > gain1 = p1 * __conjf__( p1 );
        complex< T > gain2 = p2 * __conjf__( p2 );
        complex< T > gain3 = (complex< T >(1,0) - p1)*(complex< T >(1,0) - __conjf__(p1));
        complex< T > gain4 = (complex< T >(1,0) - p2)*(complex< T >(1,0) - __conjf__(p2));
        complex< T > gain5 = gain0 * gain1 * gain2 / gain3 / gain4 * dW * dW / w0 / w0;
        gain5 = __sqrtf__( gain5 );
        gains[j+0] = gain5.m_re;
        gains[j+1] = gain5.m_re;
    }

    // real odd pole:
    if( R == 1 )
    {
        // pole transformation by means of square equation solve:
        complex< T > a( 1 / w0 , 0 );
        complex< T > b( -plp[ N - 1 ].m_re * dW / w0 , -plp[ N - 1 ].m_im * dW / w0 );
        complex< T > c( 1 , 0 );
        complex< T > D = b * b - a * c * 4;
        complex< T > p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        complex< T > p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbp[ j + 0 ] = complex< T >( +1 , 0 );
        zbp[ j + 1 ] = complex< T >( -1 , 0 );
        pbp[ j + 0 ] = ( complex< T >( 1 , 0 ) + p1 ) / ( complex< T >( 1 , 0 ) - p1 );
        pbp[ j + 1 ] = ( complex< T >( 1 , 0 ) + p2 ) / ( complex< T >( 1 , 0 ) - p2 );

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
        complex< T > gain0 = glp[N-1];
        complex< T > gain1 = p1 * p2;
        complex< T > gain2 = (complex< T >(1,0) - p1)*(complex< T >(1,0) - p2);
        complex< T > gain3 = gain0 * gain1 / gain2 * dW / w0;
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

    return iir_cf<  T  >{ cfnum , cfden , gains , 2*L , R , 2*L+R  };
}

//  Butterworth or Chebyshev type I digital bandstop computation:
template < typename T > iir_cf<T> __butt_cheb1_digital_bs__( __fx64 Fs , __fx64 Fc , __fx64 BandWidth , __ix32 order , __fx32 type = 0 , __fx64 g_stop = 1 )
{
    // Fs           - sampling frequency
    // Fstop        - stopband frequency
    // BandWidth    - passband withd
    // order        - filter order
    // coeffs_num   - quadratic sections numerator coefficients
    // coeffs_den   - quadratic sections denominator coefficients
    // gains        - quadratic sections gains
    // type         - filter type ( 0 - Butterworth , 1 - Chebyshev )
    // g_stop       - stopband attenuation in Db

    // INITIALIZATION:

    order /= 2;

   // frequency deformation coefficient:
   T w1 = tan( PI2 * Fc / 2 / Fs ) , w2 = tan( PI2 * ( Fc + BandWidth ) / 2 / Fs );

   // allocate zeros and poles arrays:

   // lowpass analogue prototype poles, zeros and gains:
   iir_zp < T > zp = ( !type ) ? __butt_zeros_poles_plain__< T >( order , g_stop ) : __cheb1_zeros_poles_plain__< T >( order , g_stop );
   complex< T > *plp = zp.plp;
   complex< T > *glp = zp.glp;
   complex< T > *zlp = zp.zlp;
   __ix32 L = zp.L , R = zp.R , N = L + R;

   if( plp == 0 || zlp == 0 || glp == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // bandpass digital prototype poles, zeros and gains:
    complex< T > *pbs = ( complex< T >* ) calloc( 2*N , sizeof ( complex< T > ) );
    complex< T > *zbs = ( complex< T >* ) calloc( 2*N , sizeof ( complex< T > ) );
    complex< T > *gbs = ( complex< T >* ) calloc( 2*N , sizeof ( complex< T > ) );

    // coefficients matrix computation:
    T *cfnum = (T* )calloc( 3 * (2*L+R) , sizeof ( T ) );
    T *cfden = (T* )calloc( 3 * (2*L+R) , sizeof ( T ) );
    T *gains = (T* )calloc( (2*L+R+1)   , sizeof ( T ) );

    if( pbs   == 0 || pbs   == 0 || pbs   == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...
    if( cfnum == 0 || cfden == 0 || gains == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // LP-BS BILLINEAR TRANSFORM:

    // poles transformation:
    T w0 = w1 * w2;
    T dW = w2 - w1;

    // complex conjugate pairs:
    __ix32 j = 0;
    for( __ix32 i = 0 ; i < L ; i++ , j+=2 )
    {
        // poles transformation by means of square equation solve:
        complex< T > a = complex< T >( -1 / w0 , 0 );
        complex< T > b = complex< T >( dW , 0 ) / plp[i] / w0;
        complex< T > c = complex< T >( -1 , 0 );
        complex< T > D = b * b - a * c * 4;
        complex< T > p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        complex< T > p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbs[ j + 0 ] = complex< T >( +1 , 0 );
        zbs[ j + 1 ] = complex< T >( -1 , 0 );
        pbs[ j + 0 ] = ( complex< T >( 1 , 0 ) + p1 ) / ( complex< T >( 1 , 0 ) - p1 );
        pbs[ j + 1 ] = ( complex< T >( 1 , 0 ) + p2 ) / ( complex< T >( 1 , 0 ) - p2 );

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
        //complex<__fx64> gain0 = glp[i];
        complex< T > gain0 = complex< T >(1,0);
        complex< T > gain1 = p1 * __conjf__( p1 );
        complex< T > gain2 = p2 * __conjf__( p2 );
        complex< T > gain3 = (complex< T >(1,0) - p1)*(complex< T >(1,0) - __conjf__(p1));
        complex< T > gain4 = (complex< T >(1,0) - p2)*(complex< T >(1,0) - __conjf__(p2));
        complex< T > gain5 = gain0 * gain1 * gain2 / gain3 / gain4 / w0 / w0 * ( 1 + w0 ) * ( 1 + w0 );
        gain5 = __sqrtf__( gain5 );
        gains[j+0] = gain5.m_re;
        gains[j+1] = gains[j+0];
    }

    // real odd pole:
    if( R == 1 )
    {
        // poles transformation by means of square equation solve:
        complex< T > a = complex< T >( -1 / w0 , 0 );
        complex< T > b = complex< T >( dW , 0 ) / plp[N-1] / w0;
        complex< T > c = complex< T >( -1 , 0 );
        complex< T > D = b * b - a * c * 4;
        complex< T > p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        complex< T > p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbs[ j + 0 ] = complex< T >( +1 , 0 );
        zbs[ j + 1 ] = complex< T >( -1 , 0 );
        pbs[ j + 0 ] = ( complex< T >( 1 , 0 ) + p1 ) / ( complex< T >( 1 , 0 ) - p1 );
        pbs[ j + 1 ] = ( complex< T >( 1 , 0 ) + p2 ) / ( complex< T >( 1 , 0 ) - p2 );

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
        complex< T > gain0 = glp[N-1];
        complex< T > gain1 = p1 * p2;
        complex< T > gain2 = complex< T >(1,0) / plp[N-1] / (-1);
        complex< T > gain3 = (complex< T >(1,0) - p1)*(complex< T >(1,0) - p2);
        complex< T > gain4 = gain0 * gain1 * gain2 / gain3 / w0 * ( 1 + w0 );
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

    return iir_cf< T >{ cfnum , cfden , gains , 2 * L , R , 2 * L + R };
}

// NON-UNIT NUMERATOR IIR COEFFICIENTS COMPUTATION:

// Elliptic or Chebyshev type II digital lowpass computation function:
template < typename T > iir_cf<T> __cheb2_ellip_digital_lp__( __fx64 Fs , __fx64 Fc , __ix32 order , __ix32 type = 0 , __fx64 g_pass = 1 , __fx64 g_stop = 80 )
{
    // Fs           - sampling frequency
    // Fstop        - cutoff frequency
    // order        - filter order
    // coeffs_num   - quadratic sections numerator coefficients
    // coeffs_den   - quadratic sections denominator coefficients
    // gains        - quadratic sections gains
    // type         - filter type ( 0 - Chebyshev type II , 1 - Elliptic )
    // g_stop       - stopband attenuation in Db

    T w = tan( PI2 * Fc / 2 / Fs );

    // digital lowpass coefficients computation:
    iir_zp< T > zp = ( !type ) ? __cheb2_zeros_poles_plain__< T >( order , g_stop ) : __ellip_zeros_poles_plain__< T >( order , g_pass , g_stop );

    // zeros/poles and coefficients number:
    __ix32 L = zp.L;
    __ix32 R = zp.R;
    __ix32 N = zp.N;

    // zeros / poles plain initialization:
    complex< T > *plp = zp.plp;
    complex< T > *zlp = zp.zlp;
    complex< T > *glp = zp.glp;

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // coefficients matrix computation:
     T  *cfnum = ( T * )calloc( 3 * N , sizeof ( T ) );
     T  *cfden = ( T * )calloc( 3 * N , sizeof ( T ) );
     T  *gains = ( T * )calloc( N + 1 , sizeof ( T ) );

    if( cfnum == 0 || cfden == 0 || gains == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // complex conjugate pairs:
    for ( __ix32 i = 0 ; i < L ; i++)
    {
        // quadratic sections gains computation:
        complex< T > gain0 = glp[i];
        complex< T > gain1 = ( complex< T >( 1 , 0 ) - zlp[i] * w ) * ( complex< T >( 1 , 0 ) - __conjf__( zlp[i] ) * w );
        complex< T > gain2 = ( complex< T >( 1 , 0 ) - plp[i] * w ) * ( complex< T >( 1 , 0 ) - __conjf__( plp[i] ) * w );
        gains[ i ] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[i] = ( complex< T >( 1 , 0 ) + zlp[i] * w ) / ( complex< T >( 1 , 0 ) - zlp[i] * w );
        plp[i] = ( complex< T >( 1 , 0 ) + plp[i] * w ) / ( complex< T >( 1 , 0 ) - plp[i] * w );

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
        complex< T > gain0 = glp[ N - 1 ];
        complex< T > gain1 = complex< T >( w , 0 );
        complex< T > gain2 = ( complex< T >( 1 , 0 ) - plp[ N - 1 ] * w );
        gains[ N - 1 ] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[ N - 1 ] = complex< T >( -1 , 0 );
        plp[ N - 1 ] = ( complex< T >( 1 , 0 ) + plp[ N - 1 ] * w ) / ( complex< T >( 1 , 0 ) - plp[ N - 1 ] * w );

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

    return iir_cf< T >{ cfnum , cfden , gains , L , R , N };
}

// Elliptic or Chebyshev type II digital highpass computation function:
template < typename T > iir_cf<T> __cheb2_ellip_digital_hp__( __fx64 Fs , __fx64 Fc , __ix32 order , __ix32 type = 0 , __fx64 g_pass = 1 , __fx64 g_stop = 80 )
{
    // Fs           - sampling frequency
    // Fstop        - cutoff frequency
    // order        - filter order
    // coeffs_num   - quadratic sections numerator coefficients
    // coeffs_den   - quadratic sections denominator coefficients
    // gains        - quadratic sections gains
    // type         - filter type ( 0 - Chebyshev II , 1 - Elliptic )
    // g_stop       - stopband attenuation in Db

     T  w = tan( PI2 * Fc / 2 / Fs );

    // digital lowpass coefficients computation:
    iir_zp< T > zp = ( !type ) ? __cheb2_zeros_poles_plain__< T >( order , g_stop ) : __ellip_zeros_poles_plain__< T >( order , g_pass , g_stop );

    // zeros/poles and coefficients number:
    __ix32 L = zp.L;
    __ix32 R = zp.R;
    __ix32 N = zp.N;

    // zeros / poles plain initialization:
    complex< T > *plp = zp.plp;
    complex< T > *zlp = zp.zlp;
    complex< T > *glp = zp.glp;

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // coefficients matrix memory allocation:
     T *cfnum = (T* )calloc( 3 * N , sizeof ( T ) );
     T *cfden = (T* )calloc( 3 * N , sizeof ( T ) );
     T *gains = (T* )calloc( N + 1 , sizeof ( T ) );

    if( cfnum == 0 || cfden == 0 || gains == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // coefficients matrix computation:

    // complex conjugate pairs:
    for ( __ix32 i = 0 ; i < L ; i++)
    {
        // quadratic sections gains computation:
        complex< T > gain0 = glp[i];
        complex< T > gain1 = ( complex< T >( 1 , 0 ) - zlp[i] / w ) * ( complex< T >( 1 , 0 ) - __conjf__( zlp[i] ) / w );
        complex< T > gain2 = ( complex< T >( 1 , 0 ) - plp[i] / w ) * ( complex< T >( 1 , 0 ) - __conjf__( plp[i] ) / w );
        gains[ i ] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[i] = ( complex< T >( 1 , 0 ) + zlp[i] / w ) / ( complex< T >( 1 , 0 ) - zlp[i] / w );
        plp[i] = ( complex< T >( 1 , 0 ) + plp[i] / w ) / ( complex< T >( 1 , 0 ) - plp[i] / w );

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
        complex< T > gain0 = glp[ N - 1 ];
        complex< T > gain1 = complex< T >( 1 / w , 0 );
        complex< T > gain2 = ( complex< T >( 1 , 0 ) - plp[ N - 1 ] / w );
        gains[ N - 1 ] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[ N - 1 ] = complex< T >( -1 , 0 );
        plp[ N - 1 ] = ( complex< T >( 1 , 0 ) + plp[ N - 1 ] / w ) / ( complex< T >( 1 , 0 ) - plp[ N - 1 ] / w );

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

    return iir_cf< T >{ cfnum , cfden , gains , L , R , N };
}

// Elliptic or Chebyshev II digital bandpass computation function:
template < typename T > iir_cf<T> __cheb2_ellip_digital_bp__( __fx64 Fs , __fx64 Fp , __fx64 BandWidth , __ix32 order , __ix32 type = 0 , __fx64 g_pass = 1 , __fx64 g_stop = 80 )
{
    // Fs           - sampling frequency
    // Fpass        - passband frequency
    // BandWidth    - passband withd
    // order        - filter order
    // coeffs_num   - quadratic sections numerator coefficients
    // coeffs_den   - quadratic sections denominator coefficients
    // gains        - quadratic sections gains
    // type         - filter type ( 0 - Butterworth , 1 - Chebyshev )
    // g_stop       - stopband attenuation in Db

     order /= 2;

    // frequency deformation coefficient:
     T w1 = tan( PI2 * Fp / 2 / Fs ) , w2 = tan( PI2 * ( Fp + BandWidth ) / 2 / Fs );

    // allocate zeros and poles arrays:

    // lowpass analogue prototype poles, zeros and gains:
    iir_zp< T > zp = ( !type ) ? __cheb2_zeros_poles_plain__< T >( order , g_stop ) : __ellip_zeros_poles_plain__< T >( order , g_pass , g_stop );
    complex< T > *plp = zp.plp;
    complex< T > *glp = zp.glp;
    complex< T > *zlp = zp.zlp;
    __ix32 L = zp.L , R = zp.R , N = L + R;

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // bandpass digital prototype poles, zeros and gains:
    complex< T > *pbp = ( complex< T >* ) calloc( 2*N , sizeof ( complex< T > ) );
    complex< T > *zbp = ( complex< T >* ) calloc( 2*N , sizeof ( complex< T > ) );
    complex< T > *gbp = ( complex< T >* ) calloc( 2*N , sizeof ( complex< T > ) );

    // coefficients matrix computation:
     T  *cfnum = ( T * )calloc( 3 * (2*L+R) , sizeof (  T  ) );
     T  *cfden = ( T * )calloc( 3 * (2*L+R) , sizeof (  T  ) );
     T  *gains = ( T * )calloc( (2*L+R + 1) , sizeof (  T  ) );

    if( pbp   == 0 || zbp   == 0 || gbp   == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...
    if( cfnum == 0 || cfden == 0 || gains == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // LP-BP BILLINEAR TRANSFORM:

    // poles transformation:
     T w0 = w1 * w2;
     T dW = w2 - w1;

    // complex conjugate pairs:
    __ix32 j = 0;

    // auxiliary variables:
    complex< T > a , b , c , D , z1 , z2 , p1 , p2;

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
        zbp[ j + 0 ] = ( complex< T >( 1 , 0 ) + z1 ) / ( complex< T >( 1 , 0 ) - z1 );
        zbp[ j + 1 ] = ( complex< T >( 1 , 0 ) + z2 ) / ( complex< T >( 1 , 0 ) - z2 );
        pbp[ j + 0 ] = ( complex< T >( 1 , 0 ) + p1 ) / ( complex< T >( 1 , 0 ) - p1 );
        pbp[ j + 1 ] = ( complex< T >( 1 , 0 ) + p2 ) / ( complex< T >( 1 , 0 ) - p2 );

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
        complex< T > gain0 = glp[i];
        complex< T > gain1 = p1 * __conjf__( p1 );
        complex< T > gain2 = p2 * __conjf__( p2 );
        complex< T > gain3 = z1 * __conjf__( z1 );
        complex< T > gain4 = z2 * __conjf__( z2 );
        complex< T > gain5 = ( complex< T >(1,0) - z1)*(complex< T >(1,0) - __conjf__(z1) );
        complex< T > gain6 = ( complex< T >(1,0) - z2)*(complex< T >(1,0) - __conjf__(z2) );
        complex< T > gain7 = ( complex< T >(1,0) - p1)*(complex< T >(1,0) - __conjf__(p1) );
        complex< T > gain8 = ( complex< T >(1,0) - p2)*(complex< T >(1,0) - __conjf__(p2) );

        complex< T > gain9 = gain0 * gain1 * gain2 / ( gain3 * gain4 ) * ( gain5 * gain6 ) / ( gain7 * gain8 );
        gain9 = __sqrtf__( gain9 );
        gains[j+0] = gain9.m_re;
        gains[j+1] = gains[j+0];
    }

    // real odd pole:
    if( R == 1 )
    {
        // pole transformation by means of square equation solve:
        complex< T > a( 1 / w0 , 0 );
        complex< T > b( -plp[ N - 1 ].m_re * dW / w0 , -plp[ N - 1 ].m_im * dW / w0 );
        complex< T > c( 1 , 0 );
        complex< T > D = b * b - a * c * 4;
        complex< T > p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        complex< T > p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbp[ j + 0 ] = complex< T >( +1 , 0 );
        zbp[ j + 1 ] = complex< T >( -1 , 0 );
        pbp[ j + 0 ] = ( complex< T >( 1 , 0 ) + p1 ) / ( complex< T >( 1 , 0 ) - p1 );
        pbp[ j + 1 ] = ( complex< T >( 1 , 0 ) + p2 ) / ( complex< T >( 1 , 0 ) - p2 );

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
        complex< T > gain0 = glp[N-1];
        complex< T > gain1 = p1 * p2;
        complex< T > gain2 = (complex< T >(1,0) - p1)*(complex< T >(1,0) - p2);
        complex< T > gain3 = gain0 * gain1 / gain2 * dW / w0;
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

    return iir_cf< T >{ cfnum , cfden , gains , 2*L , R , 2*L+R  };
}

// Elliptic or Chebyshev II digital bandstop computation function:
template < typename T > iir_cf<T> __cheb2_ellip_digital_bs__( __fx64 Fs , __fx64 Fp , __fx64 BandWidth , __ix32 order , __ix32 type = 0 , __fx64 g_pass = 1 , __fx64 g_stop = 80 )
{
    // Fs           - sampling frequency
    // Fpass        - passband frequency
    // BandWidth    - passband withd
    // order        - filter order
    // coeffs_num   - quadratic sections numerator coefficients
    // coeffs_den   - quadratic sections denominator coefficients
    // gains        - quadratic sections gains
    // type         - filter type ( 0 - Butterworth , 1 - Chebyshev )
    // g_stop       - stopband attenuation in Db

     order /= 2;

    // frequency deformation coefficient:
    T w1 = tan( PI2 * Fp / 2 / Fs ) , w2 = tan( PI2 * ( Fp + BandWidth ) / 2 / Fs );

    // allocate zeros and poles arrays:

    // lowpass analogue prototype poles, zeros and gains:
    iir_zp < T > zp = ( !type ) ? __cheb2_zeros_poles_plain__< T >( order , g_stop ) : __ellip_zeros_poles_plain__< T >( order , g_pass , g_stop );
    complex< T > *plp = zp.plp;
    complex< T > *glp = zp.glp;
    complex< T > *zlp = zp.zlp;
    __ix32 L = zp.L , R = zp.R , N = L + R;

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // bandpass digital prototype poles, zeros and gains:
    complex< T > *pbs = ( complex< T >* ) calloc( 2*N , sizeof ( complex< T > ) );
    complex< T > *zbs = ( complex< T >* ) calloc( 2*N , sizeof ( complex< T > ) );
    complex< T > *gbs = ( complex< T >* ) calloc( 2*N , sizeof ( complex< T > ) );

    // coefficients matrix computation:
     T  *cfnum = ( T * )calloc( 3 * (2*L+R) , sizeof ( T ) );
     T  *cfden = ( T * )calloc( 3 * (2*L+R) , sizeof ( T ) );
     T  *gains = ( T * )calloc( (2*L+R + 1) , sizeof ( T ) );

    if( pbs   == 0 || zbs   == 0 || gbs   == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...
    if( cfnum == 0 || cfden == 0 || gains == 0 ) return iir_cf< T >{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // LP-BP BILLINEAR TRANSFORM:

    // poles transformation:
     T  w0 = w1 * w2;
     T  dW = w2 - w1;

    // complex conjugate pairs:
    __ix32 j = 0;

    // auxiliary variables:
    complex< T > a , b , c , D , z1 , z2 , p1 , p2;

    for( __ix32 i = 0 ; i < L ; i++ , j+=2 )
    {
        // zeros frequency transformation by means of square equation solve:
        a = -1 / w0;
        b = complex< T >( 1 , 0 ) / zlp[i] * dW / w0;
        c = -1;
        D = b * b - a * c * 4;
        z1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        z2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // poles frequency transformation by means of square equation solve:
        a = -1 / w0;
        b = complex< T >( 1 , 0 ) / plp[i] * dW / w0;
        c = -1;
        D = b * b - a * c * 4;
        p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbs[ j + 0 ] = ( complex< T >( 1 , 0 ) + z1 ) / ( complex< T >( 1 , 0 ) - z1 );
        zbs[ j + 1 ] = ( complex< T >( 1 , 0 ) + z2 ) / ( complex< T >( 1 , 0 ) - z2 );
        pbs[ j + 0 ] = ( complex< T >( 1 , 0 ) + p1 ) / ( complex< T >( 1 , 0 ) - p1 );
        pbs[ j + 1 ] = ( complex< T >( 1 , 0 ) + p2 ) / ( complex< T >( 1 , 0 ) - p2 );

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
        complex< T > gain0 = p1 * __conjf__( p1 );
        complex< T > gain1 = p2 * __conjf__( p2 );
        complex< T > gain2 = z1 * __conjf__( z1 );
        complex< T > gain3 = z2 * __conjf__( z2 );
        complex< T > gain4 = ( complex< T >(1,0) - z1)*(complex< T >(1,0) - __conjf__(z1) );
        complex< T > gain5 = ( complex< T >(1,0) - z2)*(complex< T >(1,0) - __conjf__(z2) );
        complex< T > gain6 = ( complex< T >(1,0) - p1)*(complex< T >(1,0) - __conjf__(p1) );
        complex< T > gain7 = ( complex< T >(1,0) - p2)*(complex< T >(1,0) - __conjf__(p2) );
        complex< T > gain8 = ( gain0 * gain1 ) / ( gain2 * gain3 ) * ( gain4 * gain5 ) / ( gain6 * gain7 );
        gain8 = __sqrtf__( gain8 );
        gains[j+0] = gain8.m_re;
        gains[j+1] = gains[j+0];
    }

    // real odd pole:
    if( R == 1 )
    {
        // poles transformation by means of square equation solve:
        complex< T > a = complex< T >( -1 / w0 , 0 );
        complex< T > b = complex< T >( dW , 0 ) / plp[N-1] / w0;
        complex< T > c = complex< T >( -1 , 0 );
        complex< T > D = b * b - a * c * 4;
        complex< T > p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        complex< T > p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbs[ j + 0 ] = complex< T >( +1 , 0 );
        zbs[ j + 1 ] = complex< T >( -1 , 0 );
        pbs[ j + 0 ] = ( complex< T >( 1 , 0 ) + p1 ) / ( complex< T >( 1 , 0 ) - p1 );
        pbs[ j + 1 ] = ( complex< T >( 1 , 0 ) + p2 ) / ( complex< T >( 1 , 0 ) - p2 );

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
        complex< T > gain0 = glp[N-1];
        complex< T > gain1 = p1 * p2;
        complex< T > gain2 = complex< T >(1,0) / plp[N-1] / (-1);
        complex< T > gain3 = (complex< T >(1,0) - p1)*(complex< T >(1,0) - p2);
        complex< T > gain4 = gain0 * gain1 * gain2 / gain3 / w0 * ( 1 + w0 );
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

    return iir_cf< T >{ cfnum , cfden , gains , 2*L , R , 2*L+R  };
}

// iir buffers memory allocation function:
template< typename T > iir_bf<T> __iir_bf_alloc__( __ix32 N )
{
    mirror_ring_buffer<T> *bx = ( mirror_ring_buffer<T>* ) calloc( N , sizeof ( mirror_ring_buffer<T> ) );
    mirror_ring_buffer<T> *by = ( mirror_ring_buffer<T>* ) calloc( N , sizeof ( mirror_ring_buffer<T> ) );
    for( __ix32 i = 0 ; i < N ; i++ )
    {
        bx[i].allocate( 4 );
        by[i].allocate( 3 );
    }

    return iir_bf< T >{ bx , by , N };
}

// iir buffers memory deallocation function:
template< typename T > void __iir_bf_free__( iir_bf<T> bf )
{
    mirror_ring_buffer<T> *bx = bf.bx , *by = bf.by;
    __ix32 N = bf.N;

    if( bx != 0 )
    {
        for( __ix32 i = 0 ; i < N ; i++ ) bx[i].deallocate();
        free( bx );
        bf.bx = 0;
    }

    if( by != 0 )
    {
        for( __ix32 i = 0 ; i < N ; i++ ) by[i].deallocate();
        free( by );
        bf.by = 0;
    }
    bf.N = -1;
}

// iir zeros / poles plain memory deallocation function:
template< typename T > void __iir_zp_free__( iir_zp< T > zp )
{
    if( zp.glp != 0 ) { free( zp.glp ); zp.glp = 0; }
    if( zp.zlp != 0 ) { free( zp.zlp ); zp.zlp = 0; }
    if( zp.plp != 0 ) { free( zp.plp ); zp.plp = 0; }
    zp.N = zp.L = zp.R = -1;
}

// iir coefficients memory deallocation function:
template< typename T > void __iir_cf_free__( iir_cf< T > cf )
{
    if( cf.cfnum != 0 ) { free( cf.cfnum ); cf.cfnum = 0; }
    if( cf.cfden != 0 ) { free( cf.cfden ); cf.cfden = 0; }
    if( cf.gains != 0 ) { free( cf.gains ); cf.gains = 0; }
    cf.N = cf.L = cf.R = -1;
}

// filtering template function:
template< typename T > extern T __filt__( T *input , T *cfnum , T *cfden , T *gains  , __ix32 N ,  mirror_ring_buffer< T > *buff_sx , mirror_ring_buffer< T > *buff_sy )
{
    // initialization:
    T sum_num = 0 , sum_den = 0 , out = 0;

    // filtering:
    buff_sx[0].fill_buff( input );
    for ( __ix32 i = 0 ; i < N ; i++)
    {
        sum_num = gains[i]*( buff_sx[ i ][ 0 ] * cfnum[ 3 * i + 0 ] + buff_sx[ i ][ 1 ] * cfnum[ 3 * i + 1 ] + buff_sx[ i ][ 2 ] * cfnum[ 3 * i + 2 ] );
        sum_den = ( buff_sy[ i ][ 0 ] * cfden[ 3 * i + 1 ] + buff_sy[ i ][ 1 ] * cfden[ 3 * i + 2 ] );
        out = sum_num - sum_den;
        buff_sy[ i ].fill_buff( &out );
        if( i < N - 1 ) buff_sx[ i + 1 ].fill_buff( &out );
    }
    return out;
}

// frequency response computation function:
template< typename T > iir_fr< T > __freq_resp__( T *cfnum , T *cfden , T *gains , __ix32 N , __fx64 Fs , __fx64 F )
{
    // sampling period:
    T Ts = 1 / Fs;

    // transfer function initialization:
    complex< T > tsf = complex< T >( 1, 0 );

    for( __ix32 i = 0 ; i < N ; i++ )
    {
        // complex exponents:
        complex< T > z0 = complex<__fx64>( cos( -PI2 * F * Ts * 0 ) , sin( -PI2 * F * Ts * 0 ) );
        complex< T > z1 = complex<__fx64>( cos( -PI2 * F * Ts * 1 ) , sin( -PI2 * F * Ts * 1 ) );
        complex< T > z2 = complex<__fx64>( cos( -PI2 * F * Ts * 2 ) , sin( -PI2 * F * Ts * 2 ) );

        // transfer function:
        complex< T > num =  z0 * cfnum[ 3 * i + 0 ] + z1 * cfnum[ 3 * i + 1 ] + z2 * cfnum[ 3 * i + 2 ];
        complex< T > den =  z0 * cfden[ 3 * i + 0 ] + z1 * cfden[ 3 * i + 1 ] + z2 * cfden[ 3 * i + 2 ];
        tsf *= num / den * gains[i];
    }

    // multiply by an output gain:
    tsf *= gains[N];

    // output:
    return iir_fr< T >{ __absf__( tsf ) , __argf__( tsf ) };
}

// DEBUGGING FUNCTIONS:

#ifndef __ALG_PLATFORM

// show coeffs:
template < typename T > void __show_coeffs__( iir_cf< T > coeffs )
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

template < typename T > void __show_iir__( iir_sp sp , iir_cf<T> cf , const char *name )
{
    printf( "%s " , name );

    switch ( sp.type )
    {
        case 0:
        printf( "lowpass specifications: \n" );
        break;

        case 1:
        printf( "highpass specifications: \n" );
        break;

        case 2:
        printf( "bandpass specifications: \n" );
        break;

        case 3:
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

#endif

// template Butterworth filter:
template < typename T > class butterworth
{
public:
    // filter specification , coefficients and buffers:
    iir_sp      m_sp;
    iir_cf< T > m_cf;
    iir_bf< T > m_bf;
public:

    // filter output:
    T m_out , m_Km , m_pH;

    // initializations:
    void lp_init( __fx64 Fs , __fx64 Fn , __fx64 Fc ,  __ix32 order , __fx64 Gs = 1 )             { m_sp = iir_sp{ Fs , 1 / Fs , Fn , Fc , -1 , Gs , -1 , order , 0 }; }
    void hp_init( __fx64 Fs , __fx64 Fn , __fx64 Fc ,  __ix32 order , __fx64 Gs = 1 )             { m_sp = iir_sp{ Fs , 1 / Fs , Fn , Fc , -1 , Gs , -1 , order , 1 }; }
    void bp_init( __fx64 Fs , __fx64 Fn , __fx64 Fc , __fx64 BW ,  __ix32 order , __fx64 Gs = 1 ) { m_sp = iir_sp{ Fs , 1 / Fs , Fn , Fc , BW , Gs , -1 , order , 2 }; }
    void bs_init( __fx64 Fs , __fx64 Fn , __fx64 Fc , __fx64 BW ,  __ix32 order , __fx64 Gs = 1 ) { m_sp = iir_sp{ Fs , 1 / Fs , Fn , Fc , BW , Gs , -1 , order , 3 }; }

    // memory allocation function:
    __ix32 allocate()
    {
        switch ( m_sp.type )
        {
            case 0: // lowpass computation
            m_cf = __butt_cheb1_digital_lp__< T >( m_sp.Fs , m_sp.Fc , m_sp.order , 0 , m_sp.Gs );
            break;

            case 1: // highpass computation:
            m_cf = __butt_cheb1_digital_hp__< T >( m_sp.Fs , m_sp.Fc , m_sp.order , 0 , m_sp.Gs );
            break;

            case 2: // bandpass computation
            m_cf = __butt_cheb1_digital_bp__< T >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 0 , m_sp.Gs );
            break;

            case 3: // bandstop computation
            m_cf = __butt_cheb1_digital_bs__< T >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 0 , m_sp.Gs );
            break;
        }

        m_bf = ( ( m_cf.cfnum != 0 ) && ( m_cf.cfden != 0 ) && ( m_cf.gains != 0 ) ) ? __iir_bf_alloc__< T >( m_cf.N ) : iir_bf< T >{ 0 , 0 , -1 } ;
        return ( m_bf.bx != 0 && m_bf.by != 0 );
    }

    // memory deallocation function:
    void deallocate()
    {
        __iir_bf_free__< T >( m_bf );
        __iir_cf_free__< T >( m_cf );
    }

    // default constructor:
    butterworth()
    {
        m_cf = iir_cf<T>{ 0 , 0 , 0 , -1 , -1 , -1 };
        m_bf = iir_bf<T>{ 0 , 0 , -1 };
        m_sp = iir_sp   { 4000 , 1 / 4000 , 50 , 100 , -1 , 1 , -1 , 4 , 0 };
    }

    // default destructor:
    ~butterworth(){ deallocate(); }

    // coefficients computation virtual function:

    // filtering function:
    T filt( T *input ) { return ( m_out = __filt__< T >( input , m_cf.cfnum , m_cf.cfden , m_cf.gains , m_cf.N , m_bf.bx , m_bf.by ) ); }

    inline T operator () (  T *input  ) { return filt( input ); }

    #ifndef __ALG_PLATFORM
    void show_properties() { __show_iir__< T >( m_sp , m_cf , "Butterworth" ); }
    #endif
};

// template Chebyshev type I filter:
template < typename T > class chebyshev_1
{
public:
    // filter specification , coefficients and buffers:
    iir_sp      m_sp;
    iir_cf< T > m_cf;
    iir_bf< T > m_bf;
public:

    // filter output:
    T m_out , m_Km , m_pH;

    // initializations:
    void lp_init( __fx64 Fs , __fx64 Fn , __fx64 Fc ,  __ix32 order , __fx64 Gs = 1 )             { m_sp = iir_sp{ Fs , 1 / Fs , Fn , Fc , -1 , Gs , -1 , order , 0 }; }
    void hp_init( __fx64 Fs , __fx64 Fn , __fx64 Fc ,  __ix32 order , __fx64 Gs = 1 )             { m_sp = iir_sp{ Fs , 1 / Fs , Fn , Fc , -1 , Gs , -1 , order , 1 }; }
    void bp_init( __fx64 Fs , __fx64 Fn , __fx64 Fc , __fx64 BW ,  __ix32 order , __fx64 Gs = 1 ) { m_sp = iir_sp{ Fs , 1 / Fs , Fn , Fc , BW , Gs , -1 , order , 2 }; }
    void bs_init( __fx64 Fs , __fx64 Fn , __fx64 Fc , __fx64 BW ,  __ix32 order , __fx64 Gs = 1 ) { m_sp = iir_sp{ Fs , 1 / Fs , Fn , Fc , BW , Gs , -1 , order , 3 }; }

    __ix32 allocate()
    {
        switch ( m_sp.type )
        {
            case 0: // lowpass computation
            m_cf = __butt_cheb1_digital_lp__< T >( m_sp.Fs , m_sp.Fc , m_sp.order , 1 , m_sp.Gs );
            break;

            case 1: // highpass computation:
            m_cf = __butt_cheb1_digital_hp__< T >( m_sp.Fs , m_sp.Fc , m_sp.order , 1 , m_sp.Gs );
            break;

            case 2: // bandpass computation
            m_cf = __butt_cheb1_digital_bp__< T >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 1 , m_sp.Gs );
            break;

            case 3: // bandstop computation
            m_cf = __butt_cheb1_digital_bs__< T >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 1 , m_sp.Gs );
            break;
        }

        m_bf = ( ( m_cf.cfnum != 0 ) && ( m_cf.cfden != 0 ) && ( m_cf.gains != 0 ) ) ? __iir_bf_alloc__< T >( m_cf.N ) : iir_bf< T >{ 0 , 0 , -1 } ;
        return ( m_bf.bx != 0 && m_bf.by != 0 );
    }

    void deallocate()
    {
        __iir_bf_free__< T >( m_bf );
        __iir_cf_free__< T >( m_cf );
    }

    // default constructor:
    chebyshev_1()
    {
        m_cf = iir_cf<T>{ 0 , 0 , 0 , -1 , -1 , -1 };
        m_bf = iir_bf<T>{ 0 , 0 , -1 };
        m_sp = iir_sp   { 4000 , 1 / 4000 , 50 , 100 , -1 , 1 , -1 , 4 , 0 };
    }

    // default destructor:
    ~chebyshev_1(){ deallocate(); }

    // coefficients computation virtual function:

    // filtering function:
    T filt( T *input ) { return ( m_out = __filt__< T >( input , m_cf.cfnum , m_cf.cfden , m_cf.gains , m_cf.N , m_bf.bx , m_bf.by ) ); }

    inline T operator () (  T *input  ) { return filt( input ); }

    #ifndef __ALG_PLATFORM
    void show_properties() { __show_iir__< T >( m_sp , m_cf , "Chebyshev_I" ); }
    #endif
};

// template Chebyshev type II filter:
template < typename T > class chebyshev_2
{
public:
    // filter specification , coefficients and buffers:
    iir_sp      m_sp;
    iir_cf< T > m_cf;
    iir_bf< T > m_bf;
public:

    // filter output:
    T m_out , m_Km , m_pH;

    // initializations:
    void lp_init( __fx64 Fs , __fx64 Fn , __fx64 Fc ,  __ix32 order , __fx64 Gp = 1 , __fx64 Gs = 80 )             { m_sp = iir_sp{ Fs , 1 / Fs , Fn , Fc , -1 , Gs , Gp , order , 0 }; }
    void hp_init( __fx64 Fs , __fx64 Fn , __fx64 Fc ,  __ix32 order , __fx64 Gp = 1 , __fx64 Gs = 80 )             { m_sp = iir_sp{ Fs , 1 / Fs , Fn , Fc , -1 , Gs , Gp , order , 1 }; }
    void bp_init( __fx64 Fs , __fx64 Fn , __fx64 Fc , __fx64 BW ,  __ix32 order , __fx64 Gp = 1 , __fx64 Gs = 80 ) { m_sp = iir_sp{ Fs , 1 / Fs , Fn , Fc , BW , Gs , Gp , order , 2 }; }
    void bs_init( __fx64 Fs , __fx64 Fn , __fx64 Fc , __fx64 BW ,  __ix32 order , __fx64 Gp = 1 , __fx64 Gs = 80 ) { m_sp = iir_sp{ Fs , 1 / Fs , Fn , Fc , BW , Gs , Gp , order , 3 }; }

    __ix32 allocate()
    {
        switch ( m_sp.type )
        {
            case 0: // lowpass computation
            m_cf = __cheb2_ellip_digital_lp__< T >( m_sp.Fs , m_sp.Fc , m_sp.order , 0 , m_sp.Gp , m_sp.Gs );
            break;

            case 1: // highpass computation:
            m_cf = __cheb2_ellip_digital_hp__< T >( m_sp.Fs , m_sp.Fc , m_sp.order , 0 , m_sp.Gp , m_sp.Gs );
            break;

            case 2: // bandpass computation
            m_cf = __cheb2_ellip_digital_bp__< T >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 0 , m_sp.Gp , m_sp.Gs );
            break;

            case 3: // bandstop computation
            m_cf = __cheb2_ellip_digital_bs__< T >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 0 , m_sp.Gp , m_sp.Gs );
            break;
        }

        m_bf = ( ( m_cf.cfnum != 0 ) && ( m_cf.cfden != 0 ) && ( m_cf.gains != 0 ) ) ? __iir_bf_alloc__< T >( m_cf.N ) : iir_bf< T >{ 0 , 0 , -1 } ;
        return ( m_bf.bx != 0 && m_bf.by != 0 );
    }

    void deallocate()
    {
        __iir_bf_free__< T >( m_bf );
        __iir_cf_free__< T >( m_cf );
    }

    // default constructor:
    chebyshev_2()
    {
        m_cf = iir_cf<T>{ 0 , 0 , 0 , -1 , -1 , -1 };
        m_bf = iir_bf<T>{ 0 , 0 , -1 };
        m_sp = iir_sp   { 4000 , 1 / 4000 , 50 , 100 , -1 , 1 , 80 , 4 , 0 };
    }

    // default destructor:
    ~chebyshev_2(){ deallocate(); }

    // coefficients computation virtual function:

    // filtering function:
    T filt( T *input ) { return ( m_out = __filt__< T >( input , m_cf.cfnum , m_cf.cfden , m_cf.gains , m_cf.N , m_bf.bx , m_bf.by ) ); }

    inline T operator () (  T *input  ) { return filt( input ); }

    #ifndef __ALG_PLATFORM
    void show_properties() { __show_iir__< T >( m_sp , m_cf , "Chebyshev_II" ); }
    #endif
};

// template Chebyshev type II filter:
template < typename T > class elliptic
{
public:
    // filter specification , coefficients and buffers:
    iir_sp      m_sp;
    iir_cf< T > m_cf;
    iir_bf< T > m_bf;
public:

    // filter output:
    T m_out , m_Km , m_pH;

    // initializations:
    void lp_init( __fx64 Fs , __fx64 Fn , __fx64 Fc ,  __ix32 order , __fx64 Gp = 1 , __fx64 Gs = 80 )             { m_sp = iir_sp{ Fs , 1 / Fs , Fn , Fc , -1 , Gs , Gp , order , 0 }; }
    void hp_init( __fx64 Fs , __fx64 Fn , __fx64 Fc ,  __ix32 order , __fx64 Gp = 1 , __fx64 Gs = 80 )             { m_sp = iir_sp{ Fs , 1 / Fs , Fn , Fc , -1 , Gs , Gp , order , 1 }; }
    void bp_init( __fx64 Fs , __fx64 Fn , __fx64 Fc , __fx64 BW ,  __ix32 order , __fx64 Gp = 1 , __fx64 Gs = 80 ) { m_sp = iir_sp{ Fs , 1 / Fs , Fn , Fc , BW , Gs , Gp , order , 2 }; }
    void bs_init( __fx64 Fs , __fx64 Fn , __fx64 Fc , __fx64 BW ,  __ix32 order , __fx64 Gp = 1 , __fx64 Gs = 80 ) { m_sp = iir_sp{ Fs , 1 / Fs , Fn , Fc , BW , Gs , Gp , order , 3 }; }

    // memory allocation function:
    __ix32 allocate()
    {
        switch ( m_sp.type )
        {
            case 0: // lowpass computation
            m_cf = __cheb2_ellip_digital_lp__< T >( m_sp.Fs , m_sp.Fc , m_sp.order , 1 , m_sp.Gp , m_sp.Gs );
            break;

            case 1: // highpass computation:
            m_cf = __cheb2_ellip_digital_hp__< T >( m_sp.Fs , m_sp.Fc , m_sp.order , 1 , m_sp.Gp , m_sp.Gs );
            break;

            case 2: // bandpass computation
            m_cf = __cheb2_ellip_digital_bp__< T >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 1 , m_sp.Gp , m_sp.Gs );
            break;

            case 3: // bandstop computation
            m_cf = __cheb2_ellip_digital_bs__< T >( m_sp.Fs , m_sp.Fc , m_sp.BW , m_sp.order , 1 , m_sp.Gp , m_sp.Gs );
            break;
        }

        m_bf = ( ( m_cf.cfnum != 0 ) && ( m_cf.cfden != 0 ) && ( m_cf.gains != 0 ) ) ? __iir_bf_alloc__< T >( m_cf.N ) : iir_bf< T >{ 0 , 0 , -1 } ;
        return ( m_bf.bx != 0 && m_bf.by != 0 );
    }

    // memory deallocation function:
    void deallocate()
    {
        __iir_bf_free__< T >( m_bf );
        __iir_cf_free__< T >( m_cf );
    }

    // default constructor:
    elliptic()
    {
        m_cf = iir_cf<T>{ 0 , 0 , 0 , -1 , -1 , -1 };
        m_bf = iir_bf<T>{ 0 , 0 , -1 };
        m_sp = iir_sp   { 4000 , 1 / 4000 , 50 , 100 , -1 , 1 , 80 , 4 , 0 };
    }

    // default destructor:
    ~elliptic(){ deallocate(); }


    // filtering function:
    T filt( T *input ) { return ( m_out = __filt__< T >( input , m_cf.cfnum , m_cf.cfden , m_cf.gains , m_cf.N , m_bf.bx , m_bf.by ) ); }

    inline T operator () (  T *input  ) { return filt( input ); }

    // debugging function:
    #ifndef __ALG_PLATFORM
    void show_properties() { __show_iir__< T >( m_sp , m_cf , "Chebyshev_II" ); }
    #endif
};

// customized macros exclusion to avoid aliases during compilation:
#undef __fx32
#undef __fx64
#undef __ix32
#undef PI0
#undef PI2
#undef DEBUG

#endif
