//------------------------------------------------------------------------------------------------------------
// A.Tykvinskiy , 23.12.2021
//------------------------------------------------------------------------------------------------------------
// INFINITE IMPULSE RESPONSE FILTERS
//------------------------------------------------------------------------------------------------------------
/*
    Naming conventions:
    plp - poles of the normalized lowpass analogue prototype
    zlp - zeros of the normalized lowpass analogue prototype
    glp - gains of the normalized lowpass analogue prototype
    pbp - poles of the bandpass analogue/digital prototype
    zbp - zeros of the bandpass analogue/digital prototype
    gbp - gains of the bandpass analogue/digital prototype
    pbs - poles of the bandstop analogue/digital prototype
    zbs - zeros of the bandstop analogue/digital prototype
    gbs - gains of the bandstop analogue/digital prototype
*/

#ifndef IIR_H
#define IIR_H

#ifndef __ALG_PLATFORM // identify if the compilation is for ProsoftSystems IDE
#include "cmath"
#include <iostream>

#define DEBUG

#endif

#include "complex.h"
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

// customized PI:
#ifndef PI0
#define PI0 3.1415926535897932384626433832795
#endif

#ifndef PI2
#define PI2 6.283185307179586476925286766559
#endif

// UNIT NUMERATOR IIR FILTER COEFFICIENTS COMPUTATION FUNCTIONS:

// Butterworth or Chebyshev type I digital lowpass computation function:
extern void __butt_cheb1_digital_lp__( __fx64 Fs , __fx64 Fstop , __ix32 order , __fx64 *coeffs_num , __fx64 *coeffs_den , __fx64 *gains , __ix32 type = 0 , __fx64 g_stop = 1 )
{
    // Fs           - sampling frequency
    // Fstop        - cutoff frequency
    // order        - filter order
    // coeffs_num   - quadratic sections numerator coefficients
    // coeffs_den   - quadratic sections denominator coefficients
    // gains        - quadratic sections gains
    // type         - filter type ( 0 - Butterworth , 1 - Chebyshev )
    // g_stop       - stopband attenuation in Db


    // INITIALIZATION:

    // stopband attenuation:
    __fx64 epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

    // identify the number of zeros and poles:
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // frequency deformation coefficient:
    __fx64 K = tan( PI2 * Fstop / 2 / Fs );

    // allocate zeros and poles arrays:
    complex<__fx64> *plp = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *zlp = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *glp = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );

    // coefficients matrix computation:
    coeffs_num = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );
    coeffs_den = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );
    gains      = (__fx64* )calloc( N + 1 , sizeof ( __fx64 ) );

    // COMPUTE LOWPASS ANALOGUE PROTOTYPE ZEROS, POLES AND GAINS:

    if(!type) // Butterworth approximation
    {
        // complex-conjugate pairs:
        __fx64 alpha = 0 , betta = 1 / sqrt( pow( epsilon_stop , 1 / order ) );
        for( __ix32 i = 0 ; i < L ; i++ )
        {
            alpha = (2 * (i + 1) - 1) * PI0 / (2 * order);
            plp[i] = complex<__fx64>( ( -1 / betta * sin(alpha) ) , ( +1 / betta * cos(alpha) ) );
            glp[i] = plp[i] * __conjf__( plp[i] ) ;
        }

        // real odd pole:
        if( R == 1 )
        {
            plp[ N - 1 ] = complex<__fx64>( ( -1 / betta ) , 0 );
            glp[ N - 1 ] = -plp[ N - 1 ].m_re;
        }
    }
    else // Chebyshev type I approximation
    {
        // complex conjugate pairs:
        __fx64 alpha = 0 , betta = asinh( 1 / epsilon_stop ) / order;
        for ( __ix32 i = 0; i < L; i++)
        {
                alpha = (2 * (i + 1) - 1) * PI0 / (2 * order);
                plp[i] = complex<__fx64>( -sin( alpha ) * sinh( betta ) , +cos( alpha ) * cosh( betta ) );
                glp[i] = plp[i] * __conjf__( plp[i] );
        }

        // real odd pole:
        if ( R == 1 )
        {
            plp[ N - 1 ] = complex<__fx64>( -sinh( betta ) , 0 );
            glp[ N - 1 ] = -plp[ N - 1 ].m_re;
        }
    }

    // BILLINEAR LP-LP TRANSFORM:

    // complex-conjugate pairs:
    for( __ix32 i = 0 ; i < L ; i++ )
    {
        // quadratic section gain:
        complex<__fx64> gain0 = glp[i];
        complex<__fx64> gain1 = complex<__fx64>( K * K , 0 );
        complex<__fx64> gain2 = ( complex<__fx64>( 1 , 0 ) - plp[i] * K ) * ( complex<__fx64>( 1 , 0 ) - __conjf__( plp[i] ) * K );
        gains[i] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[i] = complex<__fx64>( -1 , 0 );
        plp[i] = ( complex<__fx64>( 1 , 0 ) + plp[i] * K ) / ( complex<__fx64>( 1 , 0 ) - plp[i] * K );

        // quadratic section numerator coefficients:
        coeffs_num[ 3 * i + 0 ] = +1;
        coeffs_num[ 3 * i + 1 ] = +2;
        coeffs_num[ 3 * i + 2 ] = +1;

        // quadratic section denumerator coefficients:
        coeffs_den[ 3 * i + 0 ] = 1;
        coeffs_den[ 3 * i + 1 ] = -( plp[i] + __conjf__( plp[i] ) ).m_re;
        coeffs_den[ 3 * i + 2 ] = +( plp[i] * __conjf__( plp[i] ) ).m_re;
    }

    // real odd pole:
    if ( R == 1 )
    {
        // quadratic section gain:
        complex<__fx64> gain0 = glp[N-1];
        complex<__fx64> gain1 = complex<__fx64>( K , 0 );
        complex<__fx64> gain2 = ( complex<__fx64>( 1 , 0 ) - plp[N-1] * K );
        gains[N-1] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[ N - 1 ] = complex<__fx64>( -1 , 0 );
        plp[ N - 1 ] = ( complex<__fx64>( 1 , 0 ) + plp[N-1] * K ) / ( complex<__fx64>( 1 , 0 ) - plp[N-1] * K );

        // real odd pole section coefficients computation:

        // numerator:
        coeffs_num[ 3 * ( N - 1 ) + 0 ] = 1;
        coeffs_num[ 3 * ( N - 1 ) + 1 ] = - zlp[ N - 1 ].m_re;
        coeffs_num[ 3 * ( N - 1 ) + 2 ] = 0;

        // denominator:
        coeffs_den[ 3 * ( N - 1 ) + 0 ] = 1;
        coeffs_den[ 3 * ( N - 1 ) + 1 ] = -plp[N-1].m_re;
        coeffs_den[ 3 * ( N - 1 ) + 2 ] = 0;
    }

    // setting filter output gain:
    gains[N] = ( type && ( R < 1 ) ) ? ( sqrt(1 / (1 + epsilon_stop * epsilon_stop) ) ) : 1;

    // debugging output:
    #ifdef DEBUG
    if( !type ) { printf( "Butterworth \n\n " ); }
    else        { printf( "Chebyshev I \n\n " ); }
    for( __ix32 i = 0 ; i < N ; i++ )
    {
        printf( "section: %d \n " , i + 1 );
        printf( "num: %.12f %.12f %.12f \n " , coeffs_num[ 3 * i + 0 ] , coeffs_num[ 3 * i + 1 ] , coeffs_num[ 3 * i + 2 ] );
        printf( "den: %.12f %.12f %.12f \n " , coeffs_den[ 3 * i + 0 ] , coeffs_den[ 3 * i + 1 ] , coeffs_den[ 3 * i + 2 ] );
        printf( "gf0: %.12f \n\n " , gains[i] );
    }
    printf( "output gain = : %.12f \n " , gains[N] );
    #endif

    // memory free:
    free( plp  );
    free( zlp  );
    free( glp  );
}

//  Butterworth or Chebyshev type I digital highpass computation function:
extern void __butt_cheb1_digital_hp__( __fx64 Fs , __fx64 Fpass , __ix32 order , __fx64 *coeffs_num , __fx64 *coeffs_den , __fx64 *gains , __ix32 type = 0 , __fx64 g_stop = 1 )
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

    // stopband attenuation:
    __fx64 epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

    // identify the number of zeros and poles:
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // frequency deformation coefficient:
    __fx32 w = tan( PI2 * Fpass / 2 / Fs );

    // allocate zeros and poles arrays:
    complex<__fx64> *plp = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *zlp = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *glp = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );

    // coefficients matrix computation:
    coeffs_num = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );
    coeffs_den = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );
    gains      = (__fx64* )calloc( N + 1 , sizeof ( __fx64 ) );

    // COMPUTE LOWPASS ANALOGUE PROTOTYPE ZEROS, POLES AND GAINS:

    if(!type) // Butterworth approximation
    {
        // complex-conjugate pairs:
        __fx64 alpha = 0 , betta = 1 / sqrt( pow( epsilon_stop , 1 / order ) );
        for( __ix32 i = 0 ; i < L ; i++ )
        {
            alpha = (2 * (i + 1) - 1) * PI0 / (2 * order);
            plp[i] = complex<__fx64>( ( -1 / betta * sin(alpha) ) , ( +1 / betta * cos(alpha) ) );
            glp[i] = plp[i] * __conjf__( plp[i] ) ;
        }

        // real odd pole:
        if( R == 1 )
        {
            plp[ N - 1 ] = complex<__fx64>( ( -1 / betta ) , 0 );
            glp[ N - 1 ] = -plp[ N - 1 ].m_re;
        }
    }
    else // Chebyshev type I approximation
    {
        // complex conjugate pairs:
        __fx64 alpha = 0 , betta = asinh( 1 / epsilon_stop ) / order;
        for ( __ix32 i = 0; i < L; i++)
        {
                alpha = (2 * (i + 1) - 1) * PI0 / (2 * order);
                plp[i] = complex<__fx64>( -sin( alpha ) * sinh( betta ) , +cos( alpha ) * cosh( betta ) );
                glp[i] = plp[i] * __conjf__( plp[i] );
        }

        // real odd pole:
        if ( R == 1 )
        {
            plp[ N - 1 ] = complex<__fx64>( -sinh( betta ) , 0 );
            glp[ N - 1 ] = -plp[ N - 1 ].m_re;
        }
    }

    // BILLINEAR LP-HP TRANSFORM:

    // complex conjugate pairs:
    for ( __ix32 i = 0; i < L; i++)
    {
        // gains compputation:
        complex<__fx64> gain0 = glp[i];
        complex<__fx64> gain1 = ( complex<__fx64>( 1 , 0 ) - plp[i] / w ) * ( complex<__fx64>( 1 , 0 ) - __conjf__( plp[i] ) / w );
        gains[i] = ( gain0 / gain1 / w / w).m_re;
        //gains[i] = ( gain0 * gain0 / gain1 / w / w).m_re;

        // zeros and poles transformation:
        zlp[i] = complex<__fx64>( 1 , 0 );
        plp[i] = ( complex<__fx64>( 1 , 0 ) + plp[i] / w ) / ( complex<__fx64>( 1 , 0 ) - plp[i] / w ) * (-1);

        // digital highpass coefficients computation:

        // numerator:
        coeffs_num[3*i+0] = +1;
        coeffs_num[3*i+1] = -2;
        coeffs_num[3*i+2] = +1;

        // denominator:
        coeffs_den[3*i+0] = 1;
        coeffs_den[3*i+1] = -( plp[i] + __conjf__(plp[i]) ).m_re;
        coeffs_den[3*i+2] = +( plp[i] * __conjf__(plp[i]) ).m_re;
    }

    // real odd pole:
    if ( R == 1 )
    {
        // gains compputation:
        complex<__fx64> gain0 = glp[N-1];
        complex<__fx64> gain1 = ( complex<__fx64>( 1 , 0 ) - plp[N-1] / w );
        gains[N-1] = ( gain0 / gain1 / w ).m_re;

        // zeros and poles transformation:
        zlp[ N - 1 ] = complex<__fx64>( 1 , 0 );
        plp[ N - 1 ] = ( complex<__fx64>( 1 , 0 ) + plp[ N - 1 ] / w ) / ( complex<__fx64>( 1 , 0 ) - plp[ N - 1 ] / w ) * (-1);

        // digital highpass coefficients computation:

        // numerator:
        coeffs_num[3*(N-1)+0] = +1;
        coeffs_num[3*(N-1)+1] = -2;
        coeffs_num[3*(N-1)+2] = +1;

        // denominator:
        coeffs_den[3*(N-1)+0] = 1;
        coeffs_den[3*(N-1)+1] = -plp[ N - 1 ].m_re;
        coeffs_den[3*(N-1)+2] = 0;
    }

    // setting filter output gain:
    gains[N] = ( type && ( R < 1 ) ) ? ( sqrt(1 / (1 + epsilon_stop * epsilon_stop) ) ) : 1;

    #ifdef DEBUG
    if( !type ) { printf( "Butterworth \n\n " ); }
    else        { printf( "Chebyshev I \n\n " ); }
    for( __ix32 i = 0 ; i < N ; i++ )
    {
        printf( "section: %d \n " , i + 1 );
        printf( "num: %.12f %.12f %.12f \n " , coeffs_num[ 3 * i + 0 ] , coeffs_num[ 3 * i + 1 ] , coeffs_num[ 3 * i + 2 ] );
        printf( "den: %.12f %.12f %.12f \n " , coeffs_den[ 3 * i + 0 ] , coeffs_den[ 3 * i + 1 ] , coeffs_den[ 3 * i + 2 ] );
        printf( "gf0: %.12f \n\n " , gains[i] );
    }
    printf( "output gain = : %.12f \n " , gains[N] );
    #endif

    // memory free:
    free( plp  );
    free( zlp  );
    free( glp  );
}

//  Butterworth or Chebyshev type I digital bandpass computation:
extern void __butt_cheb1_digital_bp__( __fx64 Fs , __fx64 Fpass , __fx64 BandWidth , __ix32 order , __fx64 *coeffs_num , __fx64 *coeffs_den , __fx64 *gains , __ix32 type = 1 , __fx64 g_stop = 1 )
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


    // INITIALIZATION:

    // stopband attenuation:
    __fx64 epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

    // identify the number of zeros and poles:
    order /= 2;
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // frequency deformation coefficient:
    __fx64 w1 = tan( PI2 * Fpass / 2 / Fs ) , w2 = tan( PI2 * ( Fpass + BandWidth ) / 2 / Fs );

    // allocate zeros and poles arrays:

    // lowpass analogue prototype poles, zeros and gains:
    complex<__fx64> *plp = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *glp = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );

    // bandpass digital prototype poles, zeros and gains:
    complex<__fx64> *pbp = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *zbp = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *gbp = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );

    // coefficients matrix computation:
    coeffs_num = (__fx64* )calloc( 3 * (2*L+R) , sizeof ( __fx64 ) );
    coeffs_den = (__fx64* )calloc( 3 * (2*L+R) , sizeof ( __fx64 ) );
    gains      = (__fx64* )calloc( (2*L+R + 1) , sizeof ( __fx64 ) );

    // COMPUTE LOWPASS ANALOGUE PROTOTYPE ZEROS, POLES AND GAINS:

    if(!type) // Butterworth approximation
    {
        // complex-conjugate pairs:
        __fx64 alpha = 0 , betta = 1 / sqrt( pow( epsilon_stop , 1 / order ) );
        for( __ix32 i = 0 ; i < L ; i++ )
        {
            alpha = (2 * (i + 1) - 1) * PI0 / (2 * order);
            plp[i] = complex<__fx64>( ( -1 / betta * sin(alpha) ) , ( +1 / betta * cos(alpha) ) );
            glp[i] = plp[i] * __conjf__( plp[i] ) ;
        }

        // real odd pole:
        if( R == 1 )
        {
            plp[ N - 1 ] = complex<__fx64>( ( -1 / betta ) , 0 );
            glp[ N - 1 ] = -plp[ N - 1 ].m_re;
        }
    }
    else // Chebyshev type I approximation
    {
        // complex conjugate pairs:
        __fx64 alpha = 0 , betta = asinh( 1 / epsilon_stop ) / order;
        for ( __ix32 i = 0; i < L; i++)
        {
                alpha = (2 * (i + 1) - 1) * PI0 / (2 * order);
                plp[i] = complex<__fx64>( -sin( alpha ) * sinh( betta ) , +cos( alpha ) * cosh( betta ) );
                glp[i] = plp[i] * __conjf__( plp[i] );
        }

        // real odd pole:
        if ( R == 1 )
        {
            plp[ N - 1 ] = complex<__fx64>( -sinh( betta ) , 0 );
            glp[ N - 1 ] = -plp[ N - 1 ].m_re;
        }
    }

    // LP-BP BILLINEAR TRANSFORM:

    // poles transformation:
    __fx64 w0 = w1 * w2;
    __fx64 dW = w2 - w1;

    // complex conjugate pairs:
    __ix32 j = 0;
    for( __ix32 i = 0 ; i < L ; i++ , j+=2 )
    {
        // poles transformation by means of square equation solve:
        complex<__fx64> a( 1 / w0 , 0 );
        complex<__fx64> b( -plp[i].m_re * dW / w0 , -plp[i].m_im * dW / w0 );
        complex<__fx64> c( 1 , 0 );
        complex<__fx64> D = b * b - a * c * 4;
        complex<__fx64> p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        complex<__fx64> p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbp[ j + 0 ] = complex<__fx64>( +1 , 0 );
        zbp[ j + 1 ] = complex<__fx64>( -1 , 0 );
        pbp[ j + 0 ] = ( complex<__fx64>( 1 , 0 ) + p1 ) / ( complex<__fx64>( 1 , 0 ) - p1 );
        pbp[ j + 1 ] = ( complex<__fx64>( 1 , 0 ) + p2 ) / ( complex<__fx64>( 1 , 0 ) - p2 );

        // digital filter coefficients computation:

        // numerator:
        coeffs_num[ 3 * ( j + 0 ) + 0 ] = 1;
        coeffs_num[ 3 * ( j + 0 ) + 1 ] = 0;
        coeffs_num[ 3 * ( j + 0 ) + 2 ] = -1;
        coeffs_num[ 3 * ( j + 1 ) + 0 ] = 1;
        coeffs_num[ 3 * ( j + 1 ) + 1 ] = 0;
        coeffs_num[ 3 * ( j + 1 ) + 2 ] = -1;

        // denominator:
        coeffs_den[ 3 * ( j + 0 ) + 0 ] = 1;
        coeffs_den[ 3 * ( j + 0 ) + 1 ] = -( pbp[j] + __conjf__( pbp[j] ) ).m_re;
        coeffs_den[ 3 * ( j + 0 ) + 2 ] = +( pbp[j] * __conjf__( pbp[j] ) ).m_re;
        coeffs_den[ 3 * ( j + 1 ) + 0 ] = 1;
        coeffs_den[ 3 * ( j + 1 ) + 1 ] = -( pbp[j+1] + __conjf__( pbp[j+1] ) ).m_re;
        coeffs_den[ 3 * ( j + 1 ) + 2 ] = +( pbp[j+1] * __conjf__( pbp[j+1] ) ).m_re;

        // complex conjugate quadratic sections gains computation:
        complex<__fx64> gain0 = glp[i];
        complex<__fx64> gain1 = p1 * __conjf__( p1 );
        complex<__fx64> gain2 = p2 * __conjf__( p2 );
        complex<__fx64> gain3 = (complex<__fx64>(1,0) - p1)*(complex<__fx64>(1,0) - __conjf__(p1));
        complex<__fx64> gain4 = (complex<__fx64>(1,0) - p2)*(complex<__fx64>(1,0) - __conjf__(p2));
        complex<__fx64> gain5 = gain0 * gain1 * gain2 / gain3 / gain4 * dW * dW / w0 / w0;
        gain5 = __sqrtf__( gain5 );
        gains[j+0] = gain5.m_re;
        gains[j+1] = gain5.m_re;
    }

    // real odd pole:
    if( R == 1 )
    {
        // pole transformation by means of square equation solve:
        complex<__fx64> a( 1 / w0 , 0 );
        complex<__fx64> b( -plp[ N - 1 ].m_re * dW / w0 , -plp[ N - 1 ].m_im * dW / w0 );
        complex<__fx64> c( 1 , 0 );
        complex<__fx64> D = b * b - a * c * 4;
        complex<__fx64> p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        complex<__fx64> p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbp[ j + 0 ] = complex<__fx64>( +1 , 0 );
        zbp[ j + 1 ] = complex<__fx64>( -1 , 0 );
        pbp[ j + 0 ] = ( complex<__fx64>( 1 , 0 ) + p1 ) / ( complex<__fx64>( 1 , 0 ) - p1 );
        pbp[ j + 1 ] = ( complex<__fx64>( 1 , 0 ) + p2 ) / ( complex<__fx64>( 1 , 0 ) - p2 );

        // digital filter coefficients computation:

        // numerator:
        coeffs_num[ 3 * ( j + 0 ) + 0 ] = 1;
        coeffs_num[ 3 * ( j + 0 ) + 1 ] = 0;
        coeffs_num[ 3 * ( j + 0 ) + 2 ] = -1;

        // denominator:
        coeffs_den[ 3 * ( j + 0 ) + 0 ] = 1;
        coeffs_den[ 3 * ( j + 0 ) + 1 ] = -( pbp[j] + pbp[j+1] ).m_re;
        coeffs_den[ 3 * ( j + 0 ) + 2 ] = +( pbp[j] * pbp[j+1] ).m_re;

        // complex conjugate quadratic sections gains computation:
        complex<__fx64> gain0 = glp[N-1];
        complex<__fx64> gain1 = p1 * p2;
        complex<__fx64> gain2 = (complex<__fx64>(1,0) - p1)*(complex<__fx64>(1,0) - p2);
        complex<__fx64> gain3 = gain0 * gain1 / gain2 * dW / w0;
        gains[j+0] = gain3.m_re;
    }

    // setting filter output gain:
    gains[ 2*L+R ] = ( type && ( R < 1 ) ) ? ( sqrt(1 / (1 + epsilon_stop * epsilon_stop) ) ) : 1;

    #ifdef DEBUG

    if( !type ) { printf( "Butterworth \n\n " ); }
    else        { printf( "Chebyshev I \n\n " ); }
    for( __ix32 i = 0 ; i < 2*L+R ; i++ )
    {
        printf( "section: %d \n " , i + 1 );
        printf( "num: %.12f %.12f %.12f \n " , coeffs_num[ 3 * i + 0 ] , coeffs_num[ 3 * i + 1 ] , coeffs_num[ 3 * i + 2 ] );
        printf( "den: %.12f %.12f %.12f \n " , coeffs_den[ 3 * i + 0 ] , coeffs_den[ 3 * i + 1 ] , coeffs_den[ 3 * i + 2 ] );
        printf( "gf0: %.12f \n\n " , gains[i] );
    }
    printf( "output gain = : %.12f \n " , gains[2*L+R] );

    #endif

    // memory free:
    free( plp );
    free( glp );
    free( pbp );
    free( zbp );
    free( gbp );
}

//  Butterworth or Chebyshev type I digital bandstop computation:
extern void __butt_cheb1_digital_bs__( __fx64 Fs , __fx64 Fstop , __fx64 BandWidth , __ix32 order , __fx64 *coeffs_num , __fx64 *coeffs_den , __fx64 *gains , __fx32 type = 1 , __fx64 g_stop = 1 )
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

    // stopband attenuation:
    __fx64 epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

    // identify the number of zeros and poles:
    order /= 2;
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // frequency deformation coefficient:
    __fx64 w1 = tan( PI2 * Fstop / 2 / Fs ) , w2 = tan( PI2 * ( Fstop + BandWidth ) / 2 / Fs );

    // allocate zeros and poles arrays:

    // lowpass analogue prototype poles, zeros and gains:
    complex<__fx64> *plp = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *glp = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );

    // bandpass digital prototype poles, zeros and gains:
    complex<__fx64> *pbs = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *zbs = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *gbs = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );

    // coefficients matrix computation:
    coeffs_num = (__fx64* )calloc( 3 * (2*L+R+0) , sizeof ( __fx64 ) );
    coeffs_den = (__fx64* )calloc( 3 * (2*L+R+0) , sizeof ( __fx64 ) );
    gains      = (__fx64* )calloc( (2*L+R+1) , sizeof ( __fx64 ) );

    // COMPUTE LOWPASS ANALOGUE PROTOTYPE ZEROS, POLES AND GAINS:

    if(!type) // Butterworth approximation
    {
        // complex-conjugate pairs:
        __fx64 alpha = 0 , betta = 1 / sqrt( pow( epsilon_stop , 1 / order ) );
        for( __ix32 i = 0 ; i < L ; i++ )
        {
            alpha = (2 * (i + 1) - 1) * PI0 / (2 * order);
            plp[i] = complex<__fx64>( ( -1 / betta * sin(alpha) ) , ( +1 / betta * cos(alpha) ) );
            glp[i] = plp[i] * __conjf__( plp[i] ) ;
        }

        // real odd pole:
        if( R == 1 )
        {
            plp[ N - 1 ] = complex<__fx64>( ( -1 / betta ) , 0 );
            glp[ N - 1 ] = -plp[ N - 1 ].m_re;
        }
    }
    else // Chebyshev type I approximation
    {
        // complex conjugate pairs:
        __fx64 alpha = 0 , betta = asinh( 1 / epsilon_stop ) / order;
        for ( __ix32 i = 0; i < L; i++)
        {
                alpha = (2 * (i + 1) - 1) * PI0 / (2 * order);
                plp[i] = complex<__fx64>( -sin( alpha ) * sinh( betta ) , +cos( alpha ) * cosh( betta ) );
                glp[i] = plp[i] * __conjf__( plp[i] );
        }

        // real odd pole:
        if ( R == 1 )
        {
            plp[ N - 1 ] = complex<__fx64>( -sinh( betta ) , 0 );
            glp[ N - 1 ] = -plp[ N - 1 ].m_re;
        }
    }

    // LP-BS BILLINEAR TRANSFORM:

    // poles transformation:
    __fx64 w0 = w1 * w2;
    __fx64 dW = w2 - w1;

    // complex conjugate pairs:
    __ix32 j = 0;
    for( __ix32 i = 0 ; i < L ; i++ , j+=2 )
    {
        // poles transformation by means of square equation solve:
        complex<__fx64> a = complex<__fx64>( -1 / w0 , 0 );
        complex<__fx64> b = complex<__fx64>( dW , 0 ) / plp[i] / w0;
        complex<__fx64> c = complex<__fx64>( -1 , 0 );
        complex<__fx64> D = b * b - a * c * 4;
        complex<__fx64> p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        complex<__fx64> p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbs[ j + 0 ] = complex<__fx64>( +1 , 0 );
        zbs[ j + 1 ] = complex<__fx64>( -1 , 0 );
        pbs[ j + 0 ] = ( complex<__fx64>( 1 , 0 ) + p1 ) / ( complex<__fx64>( 1 , 0 ) - p1 );
        pbs[ j + 1 ] = ( complex<__fx64>( 1 , 0 ) + p2 ) / ( complex<__fx64>( 1 , 0 ) - p2 );

        // digital filter coefficients computation:

        // numerator:
        coeffs_num[ 3 * ( j + 0 ) + 0 ] = 1;
        coeffs_num[ 3 * ( j + 0 ) + 1 ] = 2 * ( w0 - 1 ) / ( w0 + 1 );
        coeffs_num[ 3 * ( j + 0 ) + 2 ] = 1;
        coeffs_num[ 3 * ( j + 1 ) + 0 ] = 1;
        coeffs_num[ 3 * ( j + 1 ) + 1 ] = 2 * ( w0 - 1 ) / ( w0 + 1 );
        coeffs_num[ 3 * ( j + 1 ) + 2 ] = 1;

        // denominator:
        coeffs_den[ 3 * ( j + 0 ) + 0 ] = 1;
        coeffs_den[ 3 * ( j + 0 ) + 1 ] = -( pbs[j] + __conjf__( pbs[j] ) ).m_re;
        coeffs_den[ 3 * ( j + 0 ) + 2 ] = +( pbs[j] * __conjf__( pbs[j] ) ).m_re;
        coeffs_den[ 3 * ( j + 1 ) + 0 ] = 1;
        coeffs_den[ 3 * ( j + 1 ) + 1 ] = -( pbs[j+1] + __conjf__( pbs[j+1] ) ).m_re;
        coeffs_den[ 3 * ( j + 1 ) + 2 ] = +( pbs[j+1] * __conjf__( pbs[j+1] ) ).m_re;

        // complex conjugate quadratic sections gains computation:
        //complex<__fx64> gain0 = glp[i];
        complex<__fx64> gain0 = complex<__fx64>(1,0);
        complex<__fx64> gain1 = p1 * __conjf__( p1 );
        complex<__fx64> gain2 = p2 * __conjf__( p2 );
        complex<__fx64> gain3 = (complex<__fx64>(1,0) - p1)*(complex<__fx64>(1,0) - __conjf__(p1));
        complex<__fx64> gain4 = (complex<__fx64>(1,0) - p2)*(complex<__fx64>(1,0) - __conjf__(p2));
        complex<__fx64> gain5 = gain0 * gain1 * gain2 / gain3 / gain4 / w0 / w0 * ( 1 + w0 ) * ( 1 + w0 );
        gain5 = __sqrtf__( gain5 );
        gains[j+0] = gain5.m_re;
        gains[j+1] = gains[j+0];
    }

    // real odd pole:
    if( R == 1 )
    {
        // poles transformation by means of square equation solve:
        complex<__fx64> a = complex<__fx64>( -1 / w0 , 0 );
        complex<__fx64> b = complex<__fx64>( dW , 0 ) / plp[N-1] / w0;
        complex<__fx64> c = complex<__fx64>( -1 , 0 );
        complex<__fx64> D = b * b - a * c * 4;
        complex<__fx64> p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        complex<__fx64> p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbs[ j + 0 ] = complex<__fx64>( +1 , 0 );
        zbs[ j + 1 ] = complex<__fx64>( -1 , 0 );
        pbs[ j + 0 ] = ( complex<__fx64>( 1 , 0 ) + p1 ) / ( complex<__fx64>( 1 , 0 ) - p1 );
        pbs[ j + 1 ] = ( complex<__fx64>( 1 , 0 ) + p2 ) / ( complex<__fx64>( 1 , 0 ) - p2 );

        // digital filter coefficients computation:

        // numerator:
        coeffs_num[ 3 * ( j + 0 ) + 0 ] = 1;
        coeffs_num[ 3 * ( j + 0 ) + 1 ] = 2 * ( w0 - 1 ) / ( w0 + 1 );
        coeffs_num[ 3 * ( j + 0 ) + 2 ] = 1;

        // denominator:
        coeffs_den[ 3 * ( j + 0 ) + 0 ] = 1;
        coeffs_den[ 3 * ( j + 0 ) + 1 ] = -( pbs[j]+pbs[j+1] ).m_re;
        coeffs_den[ 3 * ( j + 0 ) + 2 ] = +( pbs[j]*pbs[j+1] ).m_re;

        // complex conjugate quadratic sections gains computation:
        complex<__fx64> gain0 = glp[N-1];
        complex<__fx64> gain1 = p1 * p2;
        complex<__fx64> gain2 = complex<__fx64>(1,0) / plp[N-1] / (-1);
        complex<__fx64> gain3 = (complex<__fx64>(1,0) - p1)*(complex<__fx64>(1,0) - p2);
        complex<__fx64> gain4 = gain0 * gain1 * gain2 / gain3 / w0 * ( 1 + w0 );
        gains[j+0] = gain4.m_re;
    }

    // setting filter output gain:
    gains[ 2 * L + R ] = ( type && ( R < 1 ) ) ? ( sqrt(1 / (1 + epsilon_stop * epsilon_stop) ) ) : 1;

    // debugging output:
    #ifdef DEBUG

    if( !type ) { printf( "Butterworth bandstop \n\n " ); }
    else        { printf( "Chebyshev I bandstop \n\n " ); }
    for( __ix32 i = 0 ; i < 2*L+R ; i++ )
    {
        printf( "section: %d \n " , i + 1 );
        printf( "num: %.12f %.12f %.12f \n " , coeffs_num[ 3 * i + 0 ] , coeffs_num[ 3 * i + 1 ] , coeffs_num[ 3 * i + 2 ] );
        printf( "den: %.12f %.12f %.12f \n " , coeffs_den[ 3 * i + 0 ] , coeffs_den[ 3 * i + 1 ] , coeffs_den[ 3 * i + 2 ] );
        printf( "gf0: %.12f \n\n " , gains[i] );
    }
    printf( "output gain = : %.12f \n " , gains[2*L+R] );

    #endif

    // memory free:
    free( plp );
    free( glp );
    free( pbs );
    free( zbs );
    free( gbs );
}

// NON-UNIT NUMERATOR IIR FILTER COEFFICIENTS COMPUTATION FUNCTIONS:

// Elliptic or Chebyshev type II digital lowpass computation function:
extern void __ellip_cheb2_digital_lp__( __fx64 Fs , __fx64 Fstop , __ix32 order , __fx64 *coeffs_num , __fx64 *coeffs_den , __fx64 *gains , __ix32 type = 0 , __fx64 g_pass = 1 , __fx64 g_stop = 1 )
{
    // Fs           - sampling frequency
    // Fstop        - cutoff frequency
    // order        - filter order
    // coeffs_num   - quadratic sections numerator coefficients
    // coeffs_den   - quadratic sections denominator coefficients
    // gains        - quadratic sections gains
    // type         - filter type ( 0 - Butterworth , 1 - Chebyshev )
    // g_stop       - stopband attenuation in Db


    // INITIALIZATION:

    // stopband and passband attenuation:
    __fx64 epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);
    __fx64 epsilon_pass = sqrt(pow(10, g_pass / 10) - 1);

    // identify the number of zeros and poles:
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // frequency deformation coefficient:
    __fx64 K = tan( PI2 * Fstop / 2 / Fs );

    // allocate zeros and poles arrays:
    complex<__fx64> *plp = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *zlp = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *glp = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );

    // coefficients matrix computation:
    coeffs_num = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );
    coeffs_den = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );
    gains      = (__fx64* )calloc( N + 1 , sizeof ( __fx64 ) );

    // ANALOGUE LOWPASS PROTOTYPE ZEROS AND POLES COMPUTATION:

    if( !type ) // Chebyshev type II approximation
    {
        // complex conjugate pairs:

        // auxiliary variables:
        __fx64 alpha , betta = asinh( epsilon_stop ) / order , re , im;

        // zeros , poles, gains computation:
        for ( __ix32 i = 0 ; i < L ; i++ )
        {
            alpha = (2 * ( i + 1 ) - 1) * PI0 / ( 2 * order );

            // zeros:
            zlp[i] = complex<__fx64>( 0 , 1 / cos( alpha ) );

            // poles:
            re = -( sin( alpha ) * sinh( betta ) ) / ( cos( alpha ) * cos( alpha ) * cosh( betta ) * cosh( betta ) + sin( alpha ) * sin( alpha ) * sinh( betta ) * sinh( betta ) );
            im = +( cos( alpha ) * cosh( betta ) ) / ( cos( alpha ) * cos( alpha ) * cosh( betta ) * cosh( betta ) + sin( alpha ) * sin( alpha ) * sinh( betta ) * sinh( betta ) );
            plp[i] = complex<__fx64>( re , im );

            // gains:
            glp[i] = plp[i] * __conjf__( plp[i] ) / zlp[i] * __conjf__( zlp[i] );
        }

        // real odd pole:
        if( R >= 1 )
        {
            plp[ N - 1 ] = complex<__fx64>( -1 / sinh( betta ) , 0 );
            glp[ N - 1 ] = -plp[N-1].m_re;
        }
    }
    else // Elliptic approximation
    {

    }

}

// Elliptic or Chebyshev type II digital highpass computation function:
extern void __ellip_cheb2_digital_hp__();

// Elliptic or Chebyshev type II digital bandpass computation function:
extern void __ellip_cheb2_digital_bp__();

// Elliptic or Chebyshev type II digital bandstop computation function:
extern void __ellip_cheb2_digital_bs__();

// customized macros exclusion to avoid aliases during compilation:
#undef __fx32
#undef __fx64
#undef __ix32
#undef PI0
#undef PI2
#undef DEBUG

#endif
