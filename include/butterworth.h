#ifndef BUTTERWORTH_H
#define BUTTERWORTH_H

// identify if the compilation is for ProsoftSystems IDE
#ifndef __ALG_PLATFORM
#include "cmath"
#include <iostream>
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

// butterworth digital lowpass computation function:
extern void butterworth_digital_lp( __fx64 Fs , __fx64 Fstop , __ix32 order , __fx64 *coeffs_num , __fx64 *coeffs_den , __fx64 g_stop = 1 )
{
    // INITIALIZATION:

    // stopband attenuation:
    __fx64 epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

    // identify the number of zeros and poles:
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // frequency deformation coefficient:
    __fx32 K = tan( PI2 * Fstop / 2 / Fs );

    // allocate zeros and poles arrays:
    complex<__fx64> *poles = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *zeros = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *gaint = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *gainp = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *gainz = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );

    // coefficients matrix computation:
    coeffs_num = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );
    coeffs_den = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );

    // COMPUTE LOWPASS ANALOGUE PROTOTYPE ZEROS, POLES AND GAINS:

    // complex-conjugate pairs:
    __fx64 alpha = 0 , betta = 1 / sqrt( pow( epsilon_stop , 1 / order ) );
    for( __ix32 i = 0 ; i < L ; i++ )
    {
        alpha = (2 * (i + 1) - 1) * PI0 / (2 * order);
        poles[i] = complex<__fx64>( ( -1 / betta * sin(alpha) ) , ( +1 / betta * cos(alpha) ) );
        gaint[i] = poles[i] * __conjf__( poles[i] ) ;
    }

    // real odd pole:
    if( R == 1 )
    {
        poles[ N - 1 ] = complex<__fx64>( ( -1 / betta ) , 0 );
        gaint[ N - 1 ] = -poles[ N - 1 ].m_re;
    }

    // BILLINEAR LP-LP TRANSFORM:

    // complex-conjugate pairs:
    for( __ix32 i = 0 ; i < L ; i++ )
    {
        // zeros and poles gains computation:
        gainz[i] = K * K;
        gainp[i] = ( complex<__fx64>( 1 , 0 ) - poles[i] * K ) * ( complex<__fx64>( 1 , 0 ) - __conjf__( poles[i] ) * K );

        // zeros and poles transformation:
        zeros[i] = complex<__fx64>( -1 , 0 );
        poles[i] = ( complex<__fx64>( 1 , 0 ) + poles[i] * K ) / ( complex<__fx64>( 1 , 0 ) - poles[i] * K );
    }

    // real odd pole:
    if ( R == 1 )
    {
        // zeros and poles gains computation:
        gainz[ N - 1 ] = complex<__fx64>( K , 0 );
        gainp[ N - 1 ] = ( complex<__fx64>( 1 , 0 ) - poles[ N - 1 ] * K );

        // zeros and poles transformation:
        zeros[ N - 1 ] = complex<__fx64>( -1 , 0 );
        poles[ N - 1 ] = ( complex<__fx64>( 1 , 0 ) + poles[N-1] * K ) / ( complex<__fx64>( 1 , 0 ) - poles[N-1] * K );
    }

    // DIGITAL BUTTERWORTH LP COEFFICIENTS COMPUTATION:

    // complex conjugate pairs:
    for( __ix32 i = 0 ; i < L ; i++ )
    {
        // quadratic section gain:
        gaint[i] *= gainz[i] / gainp[i];

        // quadratic section numerator coefficients:
        coeffs_num[ 3 * i + 0 ] = 1;
        coeffs_num[ 3 * i + 1 ] = -2 * zeros[i].m_re;
        coeffs_num[ 3 * i + 2 ] = zeros[i].m_im * zeros[i].m_im + zeros[i].m_re * zeros[i].m_re;

        // quadratic section denumerator coefficients:
        coeffs_den[ 3 * i + 0 ] = 1;
        coeffs_den[ 3 * i + 1 ] = -2 * poles[i].m_re;
        coeffs_den[ 3 * i + 2 ] = poles[i].m_im * poles[i].m_im + poles[i].m_re * poles[i].m_re;
    }

    // the real odd pole:
    if( R == 1 )
    {
        // quadratic sections gains:
        gaint[ N - 1 ] *= gainz[ N - 1 ] / gainp[ N - 1 ];

        // quadratic section numerator coefficients:
        coeffs_num[ 3 * ( N - 1 ) + 0 ] = 1;
        coeffs_num[ 3 * ( N - 1 ) + 1 ] = - zeros[ N - 1 ].m_re;
        coeffs_num[ 3 * ( N - 1 ) + 2 ] = 0;

        // quadratic section denumerator coefficients:
        coeffs_den[ 3 * ( N - 1 ) + 0 ] = 1;
        coeffs_den[ 3 * ( N - 1 ) + 1 ] = - poles[ N - 1 ].m_re;
        coeffs_den[ 3 * ( N - 1 ) + 2 ] = 0;
    }

    // show the result:
    std::cout << "poles:" << "\n\n";
    for( __ix32 i = 0 ; i < N ; i++ ) printf( "%.4f \t %.4f \n" , poles[i].m_re , poles[i].m_im );
    std::cout << "\n";

    std::cout << "zeros:" << "\n\n";
    for( __ix32 i = 0 ; i < N ; i++ ) printf( "%.4f \t %.4f \n" , zeros[i].m_re , zeros[i].m_im );

    std::cout << "coeffs:" << "\n\n";
    for( __ix32 i = 0 ; i < N ; i++ )
    {
        printf( "section: %d \n " , i + 1 );
        printf( "num: %.6f %.4f %.4f \n " , coeffs_num[ 3 * i + 0 ] , coeffs_num[ 3 * i + 1 ] , coeffs_num[ 3 * i + 2 ] );
        printf( "den: %.6f %.4f %.4f \n " , coeffs_den[ 3 * i + 0 ] , coeffs_den[ 3 * i + 1 ] , coeffs_den[ 3 * i + 2 ] );
        printf( "gf0: %.6f \n " , gaint[i].m_re );
        printf( "\n" );
    }

    // memory free:
    free( poles  );
    free( zeros  );
    free( gaint  );
    free( gainp  );
    free( gainz  );
}

// butterworth digital highpass computation function:
extern void butterworth_digital_hp( __fx64 Fs , __fx64 Fstop , __ix32 order , __fx64 *coeffs_num , __fx64 *coeffs_den , __fx64 g_stop = 1 )
{
    // INITIALIZATION:

    // stopband attenuation:
    __fx64 epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

    // identify the number of zeros and poles:
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // frequency deformation coefficient:
    __fx32 K = tan( PI2 * Fstop / 2 / Fs );

    // allocate zeros and poles arrays:
    complex<__fx64> *poles = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *zeros = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *gaint = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *gainp = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *gainz = ( complex<__fx64>* ) calloc( N , sizeof ( complex<__fx64> ) );

    // coefficients matrix computation:
    coeffs_num = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );
    coeffs_den = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );

    // COMPUTE LOWPASS ANALOGUE PROTOTYPE ZEROS, POLES AND GAINS:

    // complex-conjugate pairs:
    __fx64 alpha = 0 , betta = 1 / sqrt( pow( epsilon_stop , 1 / order ) );
    for( __ix32 i = 0 ; i < L ; i++ )
    {
        alpha = (2 * (i + 1) - 1) * PI0 / (2 * order);
        poles[i] = complex<__fx64>( ( -1 / betta * sin(alpha) ) , ( +1 / betta * cos(alpha) ) );
        gaint[i] = poles[i] * __conjf__( poles[i] ) ;
    }

    // real odd pole:
    if( R == 1 )
    {
        poles[ N - 1 ] = complex<__fx64>( ( -1 / betta ) , 0 );
        gaint[ N - 1 ] = -poles[ N - 1 ].m_re;
    }

    // BILLINEAR LP-HP TRANSFORM:

    // complex conjugate pairs:
    for ( __ix32 i = 0; i < L; i++)
    {
        // zeros and poles transformation:
        zeros[i] = complex<__fx64>( 1 , 0 );
        poles[i] = ( complex<__fx64>( 1 , 0 ) + poles[i] / K ) / ( complex<__fx64>( 1 , 0 ) - poles[i] / K ) * (-1);

        // zeros and pole gains compiutation:
        gainz[i] = complex<__fx64>( ( 1 / ( K * K ) )  , 0 );
        gainp[i] = ( complex<__fx64>( 1 , 0 ) - poles[i] / K ) * ( complex<__fx64>( 1 , 0 ) - __conjf__( poles[i] ) / K );
    }

    // real odd pole:
    if ( R == 1 )
    {
        // zeros and poles transformation:
        zeros[ N - 1 ] = complex<__fx64>( 1 , 0 );
        poles[ N - 1 ] = ( complex<__fx64>( 1 , 0 ) + poles[ N - 1 ] / K ) / ( complex<__fx64>( 1 , 0 ) - poles[ N - 1 ] / K ) * (-1);

        // zeros and poles gains computation:
        gainz[ N - 1 ] = complex<__fx64>( 1 / K , 0 );
        gainp[ N - 1 ] = ( complex<__fx64>( 1 , 0 ) - poles[ N - 1 ] / K );
    }

    // show the result:
    std::cout << "poles:" << "\n\n";
    for( __ix32 i = 0 ; i < N ; i++ ) printf( "%.4f \t %.4f \n" , poles[i].m_re , poles[i].m_im );
    std::cout << "\n";

    std::cout << "zeros:" << "\n\n";
    for( __ix32 i = 0 ; i < N ; i++ ) printf( "%.4f \t %.4f \n" , zeros[i].m_re , zeros[i].m_im );

    // memory free:
    free( poles  );
    free( zeros  );
    free( gaint  );
    free( gainp  );
    free( gainz  );
}

#undef __fx32
#undef __fx64
#undef __ix32
#undef PI0
#undef PI2

#endif // BUTTERWORTH_H
