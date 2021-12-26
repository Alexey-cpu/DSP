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

// IIR zeros/poles container:
struct iir_zp
{
    // plp - lowpass normalized analogue poles
    // zlp - lowpass normalized analogue zeros
    // glp - lowpass normalized analogue gains
    // L   - lowpass normalized analogue poles number
    // R   - lowpass normalized analogue odd poles
    // N   - lowpass normalized analogue order
    complex<__fx64> *plp;
    complex<__fx64> *zlp;
    complex<__fx64> *glp;
    __ix32 L;
    __ix32 R;
    __ix32 N;
};

struct iir_cf
{
    // cfnum - lowpass analogue normalized quadratic sections numerator coeffs
    // cfden - lowpass analogue normalized quadratic sections denominator coeffs
    // gains - lowpass analogue normalized quadratic sections gains
    // L     - lowpass analogue normalized poles number
    // R     - lowpass analogue normalized odd poles
    // N     - lowpass analogue normalized order
    __fx64 *cfnum;
    __fx64 *cfden;
    __fx64 *gains;
    __ix32 L;
    __ix32 R;
    __ix32 N;
};

struct iir_fr
{
    // Km - amplitude frequency response
    // pH - phase frequency response
    __fx64 Km;
    __fx64 pH;
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// IIR ZEROS / POLES PLAIN COMPUTATION FUNCTIONS:

// Butterworth lowpass analogue normalized zeros/poles plain computation:
extern iir_zp __butt_zeros_poles_plain__ ( __ix32 order , __fx64 g_stop )
{
    // number of zeros, poles, coeffs:
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // stopband attenuation:
    __fx64 epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

    // memory allocation for the lowpass analogue prototype poles, zeros and gains:
    complex<__fx64> *plp = ( complex<__fx64>* ) calloc( N+0 , sizeof ( complex<__fx64> ) );
    complex<__fx64> *zlp = ( complex<__fx64>* ) calloc( N+0 , sizeof ( complex<__fx64> ) );
    complex<__fx64> *glp = ( complex<__fx64>* ) calloc( N+1 , sizeof ( complex<__fx64> ) );

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_zp{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

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

    // setting the output gain:
    glp[ N ] = 1;

    return iir_zp{ plp , zlp , glp , L , R , N };
}

// Chebyshev type I lowpass analogue normalized zeros/poles plain computation:
extern iir_zp __cheb1_zeros_poles_plain__( __ix32 order , __fx64 g_stop )
{
    // number of zeros, poles, coeffs:
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // stopband attenuation:
    __fx64 epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

    // memory allocation for the lowpass analogue prototype poles, zeros and gains:
    complex<__fx64>  *plp = ( complex<__fx64>* ) calloc( N   , sizeof ( complex<__fx64> ) );
    complex<__fx64>  *zlp = ( complex<__fx64>* ) calloc( N   , sizeof ( complex<__fx64> ) );
    complex<__fx64>  *glp = ( complex<__fx64>* ) calloc( N+1 , sizeof ( complex<__fx64> ) );

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_zp{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

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

    // setting the output gain:
    glp[N] = ( R < 1 ) ? ( sqrt(1 / (1 + epsilon_stop * epsilon_stop) ) ) : 1;

    // end the computation and return the filter info:
    return iir_zp{ plp , zlp , glp , L , R , N };
}

// Chebyshev type II lowpass analogue normalized zeros/poles plain computation:
extern iir_zp __cheb2_zeros_poles_plain__( __ix32 order , __fx64 g_stop )
{
    // INITIALIZATION:

    // stopband attenuation:
    __fx64 epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);

    // identify the number of zeros and poles:
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // allocate zeros and poles arrays:
    complex<__fx64> *plp = ( complex<__fx64>* ) calloc( N   , sizeof ( complex<__fx64> ) );
    complex<__fx64> *zlp = ( complex<__fx64>* ) calloc( N   , sizeof ( complex<__fx64> ) );
    complex<__fx64> *glp = ( complex<__fx64>* ) calloc( N+1 , sizeof ( complex<__fx64> ) );

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_zp{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // ZEROS AND POLES COMPUTATION:

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
        glp[i] = ( zlp[i] * __conjf__( zlp[i] ) ) / ( plp[i] * __conjf__( plp[i] ) );
        glp[i] = complex<__fx64>( 1 , 0 ) / glp[i];
    }

    // real odd pole:
    if( R >= 1 )
    {
        plp[ N - 1 ] = complex<__fx64>( -1 / sinh( betta ) , 0 );
        glp[ N - 1 ] = -plp[N-1].m_re;
    }

    // set the output gain:
    glp[N] = 1;

    return iir_zp{ plp , zlp , glp , L , R , N };
}

// Elliptic lowpass analogue normalized zeros/poles plain computation:
extern iir_zp __ellip_zeros_poles_plain__( __ix32 order ,  __fx64 g_pass , __fx64 g_stop )
{
    // INITIALIZATION:

    // stopband and passband attenuation:
    __fx64 epsilon_stop = sqrt(pow(10, g_stop / 10) - 1);
    __fx64 epsilon_pass = sqrt(pow(10, g_pass / 10) - 1);

    // identify the number of zeros and poles:
    __ix32 L = trunc( order / 2 ) , R = order - 2 * L , N = L + R;

    // allocate zeros and poles arrays:
    complex<__fx64> *plp = ( complex<__fx64>* ) calloc( N   , sizeof ( complex<__fx64> ) );
    complex<__fx64> *zlp = ( complex<__fx64>* ) calloc( N   , sizeof ( complex<__fx64> ) );
    complex<__fx64> *glp = ( complex<__fx64>* ) calloc( N+1 , sizeof ( complex<__fx64> ) );

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_zp{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // TRANSIENT SUPPRESSION FACTORS COMPUTATION:

    // auxiliary variables:
    __fx64 SN , KE;
    __fx64 Ke = epsilon_pass / epsilon_stop , Kp = 1;
    __fx64 m = sqrt( 1 - Ke * Ke ) , alpha , Kw;

    // transient suppression factors computation:
    for ( __ix32 i = 0; i < L; i++ )
    {
        alpha = (2 * ( i + 1 ) - 1) / ( (__fx64)order);
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
            alpha = (2 * i + 1) / ( ( __fx64 )order) * KE;
            SN = __sn__( alpha , Kw );
            zlp[i] = complex<__fx64>( 0 , 1 /( Kw * SN ) );
        }
    }
    else if( order % 2 != 0 ) // odd order filter
    {
        for ( __ix32 i = 0; i < L; i++)
        {
            alpha = (2 * i + 2) / ( ( __fx64 )order) * KE;
            SN = __sn__( alpha , Kw );
            zlp[i] = complex<__fx64>( 0 , 1 /( Kw * SN ) );
        }
    }

    // POLES COMPUTATION:
    __fx64 V0 = 0 , A = 0 , B = 0 , C = 0 , D = 0 , E = 0 , F = 0;
    V0 = -__ellip_k__( Kw ) * __isc__(1 / epsilon_pass, sqrt(1 - Ke * Ke)) / ( __ellip_k__( Ke ) * ( ( __fx64 ) order ) );
    KE =  __ellip_k__( Kw );

    // complex conjugate pairs:
    for ( __ix32 i = 0; i < L; i++)
    {
        if( order % 2 == 0 ) // even order filter
        {
            alpha = (2 * i + 1) / ( ( __fx64 ) order ) * KE;
        }
        else // odd order filter
        {
            alpha = (2 * i + 2) / ( ( __fx64 ) order ) * KE;
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
        plp[ N - 1 ] = complex<__fx64>( A * B / ( 1 - C )  , 0 );
    }

    // GAINS COMPUTATION:

    // complex conjugate pairs:
    if( R >= 1 )
    {
        // complex conjugate pairs:
        for ( __ix32 i = 0; i < L; i++)
        {
            glp[i] = ( zlp[i] * __conjf__( zlp[i] ) ) / ( plp[i] * __conjf__( plp[i] ) );
            glp[i] = complex<__fx64>( 1 , 0 ) / glp[i];
        }

        // real odd pole:
        glp[ N - 1 ] = -plp[ N - 1 ].m_re;

        // output gain:
        glp[N] = 1;
    }
    else
    {
        __fx64 a = sqrt( 1 / (1 + epsilon_stop * epsilon_stop) );
        glp[N] = pow ( a , 1 / ( (__fx64) L ) );
        for ( __ix32 i = 0; i < L; i++) glp[i] = glp[N];
    }

    return iir_zp{ plp , zlp , glp , L , R , N };
}

// UNIT NUMERATOR IIR COEFFICIENTS COMPUTATION:

// Butterworth or Chebyshev I digital lowpass coefficients computation:
extern iir_cf __butt_cheb1_digital_lp__( __fx64 Fs , __fx64 Fc , __ix32 order , __ix32 type = 0 , __fx64 g_stop = 1 )
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
    iir_zp zp = ( !type ) ? __butt_zeros_poles_plain__( order , g_stop ) : __cheb1_zeros_poles_plain__( order , g_stop );

    // allocate zeros and poles arrays:
    complex<__fx64> *plp = zp.plp;
    complex<__fx64> *zlp = zp.zlp;
    complex<__fx64> *glp = zp.glp;
    __ix32 L = zp.L , R = zp.R , N = L + R;

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // frequency deformation coefficient:
    __fx64 K = tan( PI2 * Fc / 2 / Fs );

    // coefficients matrix computation:
    __fx64 *cfnum = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );
    __fx64 *cfden = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );
    __fx64 *gains = (__fx64* )calloc( N + 1 , sizeof ( __fx64 ) );

    if( cfnum == 0 || cfden == 0 || gains == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

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
        complex<__fx64> gain0 = glp[N-1];
        complex<__fx64> gain1 = complex<__fx64>( K , 0 );
        complex<__fx64> gain2 = ( complex<__fx64>( 1 , 0 ) - plp[N-1] * K );
        gains[N-1] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[ N - 1 ] = complex<__fx64>( -1 , 0 );
        plp[ N - 1 ] = ( complex<__fx64>( 1 , 0 ) + plp[N-1] * K ) / ( complex<__fx64>( 1 , 0 ) - plp[N-1] * K );

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

    return iir_cf{ cfnum , cfden , gains , L , R , N };
}

// Butterworth or Chebyshev I digital highpass coefficients computation:
extern iir_cf __butt_cheb1_digital_hp__( __fx64 Fs , __fx64 Fp , __ix32 order , __ix32 type = 0 , __fx64 g_stop = 1 )
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
    iir_zp zp = ( !type ) ? __butt_zeros_poles_plain__( order , g_stop ) : __cheb1_zeros_poles_plain__( order , g_stop );
    complex<__fx64> *plp = zp.plp;
    complex<__fx64> *zlp = zp.zlp;
    complex<__fx64> *glp = zp.glp;
    __ix32 L = zp.L , R = zp.R , N = L + R;

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // frequency deformation coefficient:
    __fx32 w = tan( PI2 * Fp / 2 / Fs );

    // coefficients matrix computation:
    __fx64 *cfnum = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );
    __fx64 *cfden = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );
    __fx64 *gains = (__fx64* )calloc( N + 1 , sizeof ( __fx64 ) );

    if( cfnum == 0 || cfden == 0 || gains == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

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
        complex<__fx64> gain0 = glp[N-1];
        complex<__fx64> gain1 = ( complex<__fx64>( 1 , 0 ) - plp[N-1] / w );
        gains[N-1] = ( gain0 / gain1 / w ).m_re;

        // zeros and poles transformation:
        zlp[ N - 1 ] = complex<__fx64>( 1 , 0 );
        plp[ N - 1 ] = ( complex<__fx64>( 1 , 0 ) + plp[ N - 1 ] / w ) / ( complex<__fx64>( 1 , 0 ) - plp[ N - 1 ] / w ) * (-1);

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

    return iir_cf{ cfnum , cfden , gains , L , R , N };
}

// Butterworth or Chebyshev I digital bandpass coefficients computation:
extern iir_cf __butt_cheb1_digital_bp__( __fx64 Fs , __fx64 Fp , __fx64 BandWidth , __ix32 order , __ix32 type = 0 , __fx64 g_stop = 1 )
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
    __fx64 w1 = tan( PI2 * Fp / 2 / Fs ) , w2 = tan( PI2 * ( Fp + BandWidth ) / 2 / Fs );

    // allocate zeros and poles arrays:

    // lowpass analogue prototype poles, zeros and gains:
    iir_zp zp = ( !type ) ? __butt_zeros_poles_plain__( order , g_stop ) : __cheb1_zeros_poles_plain__( order , g_stop );
    complex<__fx64> *plp = zp.plp;
    complex<__fx64> *glp = zp.glp;
    complex<__fx64> *zlp = zp.zlp;
    __ix32 L = zp.L , R = zp.R , N = L + R;

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // bandpass digital prototype poles, zeros and gains:
    complex<__fx64> *pbp = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *zbp = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *gbp = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );

    // coefficients matrix computation:
    __fx64 *cfnum = (__fx64* )calloc( 3 * (2*L+R) , sizeof ( __fx64 ) );
    __fx64 *cfden = (__fx64* )calloc( 3 * (2*L+R) , sizeof ( __fx64 ) );
    __fx64 *gains = (__fx64* )calloc( (2*L+R + 1) , sizeof ( __fx64 ) );

    if( pbp   == 0 || zbp   == 0 || gbp   == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...
    if( cfnum == 0 || cfden == 0 || gains == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

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
        cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
        cfnum[ 3 * ( j + 0 ) + 1 ] = 0;
        cfnum[ 3 * ( j + 0 ) + 2 ] = -1;

        // denominator:
        cfden[ 3 * ( j + 0 ) + 0 ] = 1;
        cfden[ 3 * ( j + 0 ) + 1 ] = -( pbp[j] + pbp[j+1] ).m_re;
        cfden[ 3 * ( j + 0 ) + 2 ] = +( pbp[j] * pbp[j+1] ).m_re;

        // complex conjugate quadratic sections gains computation:
        complex<__fx64> gain0 = glp[N-1];
        complex<__fx64> gain1 = p1 * p2;
        complex<__fx64> gain2 = (complex<__fx64>(1,0) - p1)*(complex<__fx64>(1,0) - p2);
        complex<__fx64> gain3 = gain0 * gain1 / gain2 * dW / w0;
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

    return iir_cf{ cfnum , cfden , gains , 2*L , R , 2*L+R  };
}

//  Butterworth or Chebyshev type I digital bandstop computation:
extern iir_cf __butt_cheb1_digital_bs__( __fx64 Fs , __fx64 Fc , __fx64 BandWidth , __ix32 order , __fx32 type = 0 , __fx64 g_stop = 1 )
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
   __fx64 w1 = tan( PI2 * Fc / 2 / Fs ) , w2 = tan( PI2 * ( Fc + BandWidth ) / 2 / Fs );

   // allocate zeros and poles arrays:

   // lowpass analogue prototype poles, zeros and gains:
   iir_zp zp = ( !type ) ? __butt_zeros_poles_plain__( order , g_stop ) : __cheb1_zeros_poles_plain__( order , g_stop );
   complex<__fx64> *plp = zp.plp;
   complex<__fx64> *glp = zp.glp;
   complex<__fx64> *zlp = zp.zlp;
   __ix32 L = zp.L , R = zp.R , N = L + R;

   if( plp == 0 || zlp == 0 || glp == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // bandpass digital prototype poles, zeros and gains:
    complex<__fx64> *pbs = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *zbs = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *gbs = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );

    // coefficients matrix computation:
    __fx64 *cfnum = (__fx64* )calloc( 3 * (2*L+R) , sizeof ( __fx64 ) );
    __fx64 *cfden = (__fx64* )calloc( 3 * (2*L+R) , sizeof ( __fx64 ) );
    __fx64 *gains = (__fx64* )calloc( (2*L+R+1)   , sizeof ( __fx64 ) );

    if( pbs   == 0 || pbs   == 0 || pbs   == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...
    if( cfnum == 0 || cfden == 0 || gains == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

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
        cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
        cfnum[ 3 * ( j + 0 ) + 1 ] = 2 * ( w0 - 1 ) / ( w0 + 1 );
        cfnum[ 3 * ( j + 0 ) + 2 ] = 1;

        // denominator:
        cfden[ 3 * ( j + 0 ) + 0 ] = 1;
        cfden[ 3 * ( j + 0 ) + 1 ] = -( pbs[j]+pbs[j+1] ).m_re;
        cfden[ 3 * ( j + 0 ) + 2 ] = +( pbs[j]*pbs[j+1] ).m_re;

        // complex conjugate quadratic sections gains computation:
        complex<__fx64> gain0 = glp[N-1];
        complex<__fx64> gain1 = p1 * p2;
        complex<__fx64> gain2 = complex<__fx64>(1,0) / plp[N-1] / (-1);
        complex<__fx64> gain3 = (complex<__fx64>(1,0) - p1)*(complex<__fx64>(1,0) - p2);
        complex<__fx64> gain4 = gain0 * gain1 * gain2 / gain3 / w0 * ( 1 + w0 );
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

    return iir_cf{ cfnum , cfden , gains , 2 * L , R , 2 * L + R };
}

// NON-UNIT NUMERATOR IIR COEFFICIENTS COMPUTATION:

// Elliptic or Chebyshev type II digital lowpass computation function:
extern iir_cf __cheb2_ellip_digital_lp__( __fx64 Fs , __fx64 Fc , __ix32 order , __ix32 type = 0 , __fx64 g_pass = 1 , __fx64 g_stop = 80 )
{
    // Fs           - sampling frequency
    // Fstop        - cutoff frequency
    // order        - filter order
    // coeffs_num   - quadratic sections numerator coefficients
    // coeffs_den   - quadratic sections denominator coefficients
    // gains        - quadratic sections gains
    // type         - filter type ( 0 - Butterworth , 1 - Chebyshev )
    // g_stop       - stopband attenuation in Db

    __fx64 w = tan( PI2 * Fc / 2 / Fs );

    // digital lowpass coefficients computation:
    iir_zp zp = ( !type ) ? __cheb2_zeros_poles_plain__( order , g_stop ) : __ellip_zeros_poles_plain__( order , g_pass , g_stop );

    // zeros/poles and coefficients number:
    __ix32 L = zp.L;
    __ix32 R = zp.R;
    __ix32 N = zp.N;

    // zeros / poles plain initialization:
    complex<__fx64> *plp = zp.plp;
    complex<__fx64> *zlp = zp.zlp;
    complex<__fx64> *glp = zp.glp;

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // coefficients matrix computation:
    __fx64 *cfnum = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );
    __fx64 *cfden = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );
    __fx64 *gains = (__fx64* )calloc( N + 1 , sizeof ( __fx64 ) );

    if( cfnum == 0 || cfden == 0 || gains == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // complex conjugate pairs:
    for ( __ix32 i = 0 ; i < L ; i++)
    {
        // quadratic sections gains computation:
        complex<__fx64> gain0 = glp[i];
        complex<__fx64> gain1 = ( complex<__fx64>( 1 , 0 ) - zlp[i] * w ) * ( complex<__fx64>( 1 , 0 ) - __conjf__( zlp[i] ) * w );
        complex<__fx64> gain2 = ( complex<__fx64>( 1 , 0 ) - plp[i] * w ) * ( complex<__fx64>( 1 , 0 ) - __conjf__( plp[i] ) * w );
        gains[ i ] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[i] = ( complex<__fx64>( 1 , 0 ) + zlp[i] * w ) / ( complex<__fx64>( 1 , 0 ) - zlp[i] * w );
        plp[i] = ( complex<__fx64>( 1 , 0 ) + plp[i] * w ) / ( complex<__fx64>( 1 , 0 ) - plp[i] * w );

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
        complex<__fx64> gain0 = glp[ N - 1 ];
        complex<__fx64> gain1 = complex<__fx64>( w , 0 );
        complex<__fx64> gain2 = ( complex<__fx64>( 1 , 0 ) - plp[ N - 1 ] * w );
        gains[ N - 1 ] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[ N - 1 ] = complex<__fx64>( -1 , 0 );
        plp[ N - 1 ] = ( complex<__fx64>( 1 , 0 ) + plp[ N - 1 ] * w ) / ( complex<__fx64>( 1 , 0 ) - plp[ N - 1 ] * w );

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

    return iir_cf{ cfnum , cfden , gains , L , R , N };
}

// Elliptic or Chebyshev type II digital highpass computation function:
extern iir_cf __cheb2_ellip_digital_hp__( __fx64 Fs , __fx64 Fc , __ix32 order , __ix32 type = 0 , __fx64 g_pass = 1 , __fx64 g_stop = 80 )
{
    // Fs           - sampling frequency
    // Fstop        - cutoff frequency
    // order        - filter order
    // coeffs_num   - quadratic sections numerator coefficients
    // coeffs_den   - quadratic sections denominator coefficients
    // gains        - quadratic sections gains
    // type         - filter type ( 0 - Chebyshev II , 1 - Elliptic )
    // g_stop       - stopband attenuation in Db

    __fx64 w = tan( PI2 * Fc / 2 / Fs );

    // digital lowpass coefficients computation:
    iir_zp zp = ( !type ) ? __cheb2_zeros_poles_plain__( order , g_stop ) : __ellip_zeros_poles_plain__( order , g_pass , g_stop );

    // zeros/poles and coefficients number:
    __ix32 L = zp.L;
    __ix32 R = zp.R;
    __ix32 N = zp.N;

    // zeros / poles plain initialization:
    complex<__fx64> *plp = zp.plp;
    complex<__fx64> *zlp = zp.zlp;
    complex<__fx64> *glp = zp.glp;

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // coefficients matrix memory allocation:
    __fx64 *cfnum = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );
    __fx64 *cfden = (__fx64* )calloc( 3 * N , sizeof ( __fx64 ) );
    __fx64 *gains = (__fx64* )calloc( N + 1 , sizeof ( __fx64 ) );

    if( cfnum == 0 || cfden == 0 || gains == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // coefficients matrix computation:

    // complex conjugate pairs:
    for ( __ix32 i = 0 ; i < L ; i++)
    {
        // quadratic sections gains computation:
        complex<__fx64> gain0 = glp[i];
        complex<__fx64> gain1 = ( complex<__fx64>( 1 , 0 ) - zlp[i] / w ) * ( complex<__fx64>( 1 , 0 ) - __conjf__( zlp[i] ) / w );
        complex<__fx64> gain2 = ( complex<__fx64>( 1 , 0 ) - plp[i] / w ) * ( complex<__fx64>( 1 , 0 ) - __conjf__( plp[i] ) / w );
        gains[ i ] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[i] = ( complex<__fx64>( 1 , 0 ) + zlp[i] / w ) / ( complex<__fx64>( 1 , 0 ) - zlp[i] / w );
        plp[i] = ( complex<__fx64>( 1 , 0 ) + plp[i] / w ) / ( complex<__fx64>( 1 , 0 ) - plp[i] / w );

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
        complex<__fx64> gain0 = glp[ N - 1 ];
        complex<__fx64> gain1 = complex<__fx64>( 1 / w , 0 );
        complex<__fx64> gain2 = ( complex<__fx64>( 1 , 0 ) - plp[ N - 1 ] / w );
        gains[ N - 1 ] = ( gain0 * gain1 / gain2 ).m_re;

        // zeros and poles transformation:
        zlp[ N - 1 ] = complex<__fx64>( -1 , 0 );
        plp[ N - 1 ] = ( complex<__fx64>( 1 , 0 ) + plp[ N - 1 ] / w ) / ( complex<__fx64>( 1 , 0 ) - plp[ N - 1 ] / w );

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

    return iir_cf{ cfnum , cfden , gains , L , R , N };
}

// Elliptic or Chebyshev II digital bandpass computation function:
extern iir_cf __cheb2_ellip_digital_bp__( __fx64 Fs , __fx64 Fp , __fx64 BandWidth , __ix32 order , __ix32 type = 0 , __fx64 g_pass = 1 , __fx64 g_stop = 80 )
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
    __fx64 w1 = tan( PI2 * Fp / 2 / Fs ) , w2 = tan( PI2 * ( Fp + BandWidth ) / 2 / Fs );

    // allocate zeros and poles arrays:

    // lowpass analogue prototype poles, zeros and gains:
    iir_zp zp = ( !type ) ? __cheb2_zeros_poles_plain__( order , g_stop ) : __ellip_zeros_poles_plain__( order , g_pass , g_stop );
    complex<__fx64> *plp = zp.plp;
    complex<__fx64> *glp = zp.glp;
    complex<__fx64> *zlp = zp.zlp;
    __ix32 L = zp.L , R = zp.R , N = L + R;

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // bandpass digital prototype poles, zeros and gains:
    complex<__fx64> *pbp = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *zbp = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *gbp = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );

    // coefficients matrix computation:
    __fx64 *cfnum = (__fx64* )calloc( 3 * (2*L+R) , sizeof ( __fx64 ) );
    __fx64 *cfden = (__fx64* )calloc( 3 * (2*L+R) , sizeof ( __fx64 ) );
    __fx64 *gains = (__fx64* )calloc( (2*L+R + 1) , sizeof ( __fx64 ) );

    if( pbp   == 0 || zbp   == 0 || gbp   == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...
    if( cfnum == 0 || cfden == 0 || gains == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // LP-BP BILLINEAR TRANSFORM:

    // poles transformation:
    __fx64 w0 = w1 * w2;
    __fx64 dW = w2 - w1;

    // complex conjugate pairs:
    __ix32 j = 0;

    // auxiliary variables:
    complex<__fx64> a , b , c , D , z1 , z2 , p1 , p2;

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
        zbp[ j + 0 ] = ( complex<__fx64>( 1 , 0 ) + z1 ) / ( complex<__fx64>( 1 , 0 ) - z1 );
        zbp[ j + 1 ] = ( complex<__fx64>( 1 , 0 ) + z2 ) / ( complex<__fx64>( 1 , 0 ) - z2 );
        pbp[ j + 0 ] = ( complex<__fx64>( 1 , 0 ) + p1 ) / ( complex<__fx64>( 1 , 0 ) - p1 );
        pbp[ j + 1 ] = ( complex<__fx64>( 1 , 0 ) + p2 ) / ( complex<__fx64>( 1 , 0 ) - p2 );

        printf( " z1 = %.4f %.4f \t  z2 = %.4f %.4f \n" , zbp[j+0].m_re , zbp[j+0].m_im , zbp[j+1].m_re , zbp[j+1].m_im );
        printf( " p1 = %.4f %.4f \t  p2 = %.4f %.4f \n" , pbp[j+0].m_re , pbp[j+0].m_im , pbp[j+1].m_re , pbp[j+1].m_im );

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
        complex<__fx64> gain0 = glp[i];
        complex<__fx64> gain1 = p1 * __conjf__( p1 );
        complex<__fx64> gain2 = p2 * __conjf__( p2 );
        complex<__fx64> gain3 = z1 * __conjf__( z1 );
        complex<__fx64> gain4 = z2 * __conjf__( z2 );
        complex<__fx64> gain5 = ( complex<__fx64>(1,0) - z1)*(complex<__fx64>(1,0) - __conjf__(z1) );
        complex<__fx64> gain6 = ( complex<__fx64>(1,0) - z2)*(complex<__fx64>(1,0) - __conjf__(z2) );
        complex<__fx64> gain7 = ( complex<__fx64>(1,0) - p1)*(complex<__fx64>(1,0) - __conjf__(p1) );
        complex<__fx64> gain8 = ( complex<__fx64>(1,0) - p2)*(complex<__fx64>(1,0) - __conjf__(p2) );

        complex<__fx64> gain9 = gain0 * gain1 * gain2 / ( gain3 * gain4 ) * ( gain5 * gain6 ) / ( gain7 * gain8 );
        gain9 = __sqrtf__( gain9 );
        gains[j+0] = gain9.m_re;
        gains[j+1] = gains[j+0];
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
        cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
        cfnum[ 3 * ( j + 0 ) + 1 ] = 0;
        cfnum[ 3 * ( j + 0 ) + 2 ] = -1;

        // denominator:
        cfden[ 3 * ( j + 0 ) + 0 ] = 1;
        cfden[ 3 * ( j + 0 ) + 1 ] = -( pbp[j] + pbp[j+1] ).m_re;
        cfden[ 3 * ( j + 0 ) + 2 ] = +( pbp[j] * pbp[j+1] ).m_re;

        // complex conjugate quadratic sections gains computation:
        complex<__fx64> gain0 = glp[N-1];
        complex<__fx64> gain1 = p1 * p2;
        complex<__fx64> gain2 = (complex<__fx64>(1,0) - p1)*(complex<__fx64>(1,0) - p2);
        complex<__fx64> gain3 = gain0 * gain1 / gain2 * dW / w0;
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

    return iir_cf{ cfnum , cfden , gains , 2*L , R , 2*L+R  };
}

// Elliptic or Chebyshev II digital bandstop computation function:
extern iir_cf __cheb2_ellip_digital_bs__( __fx64 Fs , __fx64 Fp , __fx64 BandWidth , __ix32 order , __ix32 type = 0 , __fx64 g_pass = 1 , __fx64 g_stop = 80 )
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
    __fx64 w1 = tan( PI2 * Fp / 2 / Fs ) , w2 = tan( PI2 * ( Fp + BandWidth ) / 2 / Fs );

    // allocate zeros and poles arrays:

    // lowpass analogue prototype poles, zeros and gains:
    iir_zp zp = ( !type ) ? __cheb2_zeros_poles_plain__( order , g_stop ) : __ellip_zeros_poles_plain__( order , g_pass , g_stop );
    complex<__fx64> *plp = zp.plp;
    complex<__fx64> *glp = zp.glp;
    complex<__fx64> *zlp = zp.zlp;
    __ix32 L = zp.L , R = zp.R , N = L + R;

    if( plp == 0 || zlp == 0 || glp == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // bandpass digital prototype poles, zeros and gains:
    complex<__fx64> *pbs = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *zbs = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );
    complex<__fx64> *gbs = ( complex<__fx64>* ) calloc( 2*N , sizeof ( complex<__fx64> ) );

    // coefficients matrix computation:
    __fx64 *cfnum = (__fx64* )calloc( 3 * (2*L+R) , sizeof ( __fx64 ) );
    __fx64 *cfden = (__fx64* )calloc( 3 * (2*L+R) , sizeof ( __fx64 ) );
    __fx64 *gains = (__fx64* )calloc( (2*L+R + 1) , sizeof ( __fx64 ) );

    if( pbs   == 0 || zbs   == 0 || gbs   == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...
    if( cfnum == 0 || cfden == 0 || gains == 0 ) return iir_cf{ 0 , 0 , 0 , -1 , -1 , -1 }; // check if the memory is allocated...

    // LP-BP BILLINEAR TRANSFORM:

    // poles transformation:
    __fx64 w0 = w1 * w2;
    __fx64 dW = w2 - w1;

    // complex conjugate pairs:
    __ix32 j = 0;

    // auxiliary variables:
    complex<__fx64> a , b , c , D , z1 , z2 , p1 , p2;

    for( __ix32 i = 0 ; i < L ; i++ , j+=2 )
    {
        // zeros frequency transformation by means of square equation solve:
        a = -1 / w0;
        b = complex<__fx64>( 1 , 0 ) / zlp[i] * dW / w0;
        c = -1;
        D = b * b - a * c * 4;
        z1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        z2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // poles frequency transformation by means of square equation solve:
        a = -1 / w0;
        b = complex<__fx64>( 1 , 0 ) / plp[i] * dW / w0;
        c = -1;
        D = b * b - a * c * 4;
        p1 = ( b*(-1) - __sqrtf__( D ) ) / 2 / a;
        p2 = ( b*(-1) + __sqrtf__( D ) ) / 2 / a;

        // zeros and poles bilinear transform:
        zbs[ j + 0 ] = ( complex<__fx64>( 1 , 0 ) + z1 ) / ( complex<__fx64>( 1 , 0 ) - z1 );
        zbs[ j + 1 ] = ( complex<__fx64>( 1 , 0 ) + z2 ) / ( complex<__fx64>( 1 , 0 ) - z2 );
        pbs[ j + 0 ] = ( complex<__fx64>( 1 , 0 ) + p1 ) / ( complex<__fx64>( 1 , 0 ) - p1 );
        pbs[ j + 1 ] = ( complex<__fx64>( 1 , 0 ) + p2 ) / ( complex<__fx64>( 1 , 0 ) - p2 );

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
        complex<__fx64> gain0 = p1 * __conjf__( p1 );
        complex<__fx64> gain1 = p2 * __conjf__( p2 );
        complex<__fx64> gain2 = z1 * __conjf__( z1 );
        complex<__fx64> gain3 = z2 * __conjf__( z2 );
        complex<__fx64> gain4 = ( complex<__fx64>(1,0) - z1)*(complex<__fx64>(1,0) - __conjf__(z1) );
        complex<__fx64> gain5 = ( complex<__fx64>(1,0) - z2)*(complex<__fx64>(1,0) - __conjf__(z2) );
        complex<__fx64> gain6 = ( complex<__fx64>(1,0) - p1)*(complex<__fx64>(1,0) - __conjf__(p1) );
        complex<__fx64> gain7 = ( complex<__fx64>(1,0) - p2)*(complex<__fx64>(1,0) - __conjf__(p2) );
        complex<__fx64> gain8 = ( gain0 * gain1 ) / ( gain2 * gain3 ) * ( gain4 * gain5 ) / ( gain6 * gain7 );
        gain8 = __sqrtf__( gain8 );
        gains[j+0] = gain8.m_re;
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
        cfnum[ 3 * ( j + 0 ) + 0 ] = 1;
        cfnum[ 3 * ( j + 0 ) + 1 ] = 2 * ( w0 - 1 ) / ( w0 + 1 );
        cfnum[ 3 * ( j + 0 ) + 2 ] = 1;

        // denominator:
        cfden[ 3 * ( j + 0 ) + 0 ] = 1;
        cfden[ 3 * ( j + 0 ) + 1 ] = -( pbs[j]+pbs[j+1] ).m_re;
        cfden[ 3 * ( j + 0 ) + 2 ] = +( pbs[j]*pbs[j+1] ).m_re;

        // complex conjugate quadratic sections gains computation:
        complex<__fx64> gain0 = glp[N-1];
        complex<__fx64> gain1 = p1 * p2;
        complex<__fx64> gain2 = complex<__fx64>(1,0) / plp[N-1] / (-1);
        complex<__fx64> gain3 = (complex<__fx64>(1,0) - p1)*(complex<__fx64>(1,0) - p2);
        complex<__fx64> gain4 = gain0 * gain1 * gain2 / gain3 / w0 * ( 1 + w0 );
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

    return iir_cf{ cfnum , cfden , gains , 2*L , R , 2*L+R  };
}

// DEBUGGING FUNCTIONS:

// show coeffs:
extern void __show_coeffs__( iir_cf coeffs )
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

// frequency response computation function:
template< typename CT > extern iir_fr __freq_resp__( CT *cfnum , CT *cfden , CT *gains , __ix32 N , __fx64 Fs , __fx64 F )
{
    // sampling period:
    __fx64 Ts = 1 / Fs;

    // transfer function initialization:
    complex< __fx64 > tsf = complex< __fx64 >( 1, 0 );

    for( __ix32 i = 0 ; i < N ; i++ )
    {
        // complex exponents:
        complex<__fx64> z0 = complex<__fx64>( cos( -PI2 * F * Ts * 0 ) , sin( -PI2 * F * Ts * 0 ) );
        complex<__fx64> z1 = complex<__fx64>( cos( -PI2 * F * Ts * 1 ) , sin( -PI2 * F * Ts * 1 ) );
        complex<__fx64> z2 = complex<__fx64>( cos( -PI2 * F * Ts * 2 ) , sin( -PI2 * F * Ts * 2 ) );

        // transfer function:
        complex< __fx64 > num =  z0 * cfnum[ 3 * i + 0 ] + z1 * cfnum[ 3 * i + 1 ] + z2 * cfnum[ 3 * i + 2 ];
        complex< __fx64 > den =  z0 * cfden[ 3 * i + 0 ] + z1 * cfden[ 3 * i + 1 ] + z2 * cfden[ 3 * i + 2 ];
        tsf *= num / den * gains[i];
    }

    // multiply by an output gain:
    tsf *= gains[N];

    // output:
    return iir_fr{ __absf__( tsf ) , __argf__( tsf ) };
}

// filtering template function:

// float x64 input:
template< typename DT , typename CT > extern DT __filt__( DT *input , CT *cfnum , CT *cfden , CT *gains  , __ix32 N ,  mirror_ring_buffer< DT > *buff_sx , mirror_ring_buffer< DT > *buff_sy )
{
    // initialization:
    DT sum_num = 0 , sum_den = 0 , out = 0;

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

// parent IIR filter class:
template < typename T > class iir
{
protected:

    // filter parameters:
    __ix32 m_N , m_L , m_R , m_Fc , m_BW , m_Gs , m_Gp , m_Fs , m_Ts , m_Fn , m_order;

    // filter coefficients matrix:
    T *m_cfnum , *m_cfden , *m_gains;

    // filter input / output samples buffers:
    mirror_ring_buffer<T> *m_buff_sx , *m_buff_sy;

public:

    // filter output:
    T m_out , m_Km , m_pH;

    // coefficients computation virtual function:
    virtual __ix32 coeff_calc() { return 0; }

    // lowpass / highpass iir filters initialization functions:
    void init( __fx64 Fs , __fx64 Fn , __fx64 Fc ,  __ix32 order , __fx64 Gs = 1 )
    {
        m_Fs    = Fs;
        m_Fn    = Fn;
        m_Fc    = Fc;
        m_order = order;
        m_Gs    = Gs;
    }

    void init( __fx64 Fs , __fx64 Fn , __fx64 Fc ,  __ix32 order , __fx64 Gp = 1 , __fx64 Gs = 80 )
    {
        m_Fs    = Fs;
        m_Fn    = Fn;
        m_Fc    = Fc;
        m_order = order;
        m_Gp    = Gp;
        m_Gs    = Gs;
    }

    // bandpass / bandstop iir filters initialization functions:
    void init( __fx64 Fs , __fx64 Fn , __fx64 Fc , __fx64 BW ,  __ix32 order , __fx64 Gs = 1 )
    {
        m_Fs    = Fs;
        m_Fn    = Fn;
        m_Fc    = Fc;
        m_BW    = BW;
        m_order = order;
        m_Gs    = Gs;
    }

    void init( __fx64 Fs , __fx64 Fn , __fx64 Fc , __fx64 BW ,  __ix32 order , __fx64 Gp = 1 , __fx64 Gs = 80 )
    {
        m_Fs    = Fs;
        m_Fn    = Fn;
        m_Fc    = Fc;
        m_BW    = BW;
        m_order = order;
        m_Gp    = Gp;
        m_Gs    = Gs;
    }

    // memory allocation:
    __ix32 allocate()
    {
        // compute coefficients:
        coeff_calc();

        // coefficients matrix memory allocation:
        m_cfden = ( T* ) calloc( 3 * m_N , sizeof ( T ) );
        m_cfnum = ( T* ) calloc( 3 * m_N , sizeof ( T ) );
        m_gains = ( T* ) calloc( m_N + 1 , sizeof ( T ) );

        // input/output samples buffer memory allocation:
        m_buff_sx = ( mirror_ring_buffer<T>* ) calloc( m_N , sizeof ( mirror_ring_buffer<T> ) );
        m_buff_sy = ( mirror_ring_buffer<T>* ) calloc( m_N , sizeof ( mirror_ring_buffer<T> ) );

        for( __ix32 i = 0 ; i < m_N ; i++ )
        {
            m_buff_sx[i].allocate( 4 );
            m_buff_sy[i].allocate( 3 );
        }

        return 1;
    }

    // memory deallocation:
    __fx32 deallocate()
    {
        if( m_cfden != 0 ) { free( m_cfden ); m_cfden = 0; }
        if( m_cfnum != 0 ) { free( m_cfnum ); m_cfnum = 0; }
        if( m_gains != 0 ) { free( m_gains ); m_gains = 0; }

        if( m_buff_sx != 0 )
        {
            for( __ix32 i = 0 ; i < m_N ; i++ ) m_buff_sx[i].deallocate();
            free( m_buff_sx );
            m_buff_sx = 0;
        }

        if( m_buff_sy != 0 )
        {
            for( __ix32 i = 0 ; i < m_N ; i++ ) m_buff_sy[i].deallocate();
            free( m_buff_sy );
            m_buff_sy = 0;
        }

        return 1;
    }


    // default constructor:
    iir()
    {
        // variables initialization:
        m_N     = 4;
        m_L     = trunc( m_N / 2 );
        m_R     = m_N - 2 * m_L;
        m_Fc    = 100;
        m_BW    = 500;
        m_Gs    = 1;
        m_Gs    = 80;
        m_out   = 0;
        m_Km    = 0;
        m_pH    = 0;
        m_Fn    = 50;
        m_Fs    = 4000;
        m_Ts    = 1 / m_Fs;
        m_order = 4;

        // pointers initialization:
        m_cfnum   = 0;
        m_cfden   = 0;
        m_buff_sx = 0;
        m_buff_sy = 0;
    }

    // default destructor:
    ~iir() { deallocate(); }

    // frequency response computation:
    void freq_resp( __fx64 F )
    {
        iir_fr fr = __freq_resp__( m_cfnum , m_cfden , m_gains , m_N , m_Fs , F );
        m_Km = fr.Km;
        m_pH = fr.pH;
    }

    // filtering function:
    T filt( __fx64 *input ) { return __filt__< __fx64 , T >( input , m_cfnum , m_cfden , m_gains , m_N , m_buff_sx , m_buff_sy ); }
    T filt( __fx32 *input ) { return __filt__< __fx64 , T >( input , m_cfnum , m_cfden , m_gains , m_N , m_buff_sx , m_buff_sy ); }

};

// child lowpass filters :
class butterworth_lp_fx64 : iir<__fx64>
{
public:

    // IIR filter coefficients computation function override:
    virtual __ix32 coeff_calc() override
    {
        __butt_cheb1_digital_lp__( m_Fs , m_Fc , m_order , 0 , m_Gs );
        return 1;
    }

    // operators:
    inline __fx64 operator() ( __fx64 *input )
    {
        m_out = filt( input );
        return m_out;
    }
};

class chebyshevt1_lp_fx64 : iir<__fx64> { };
class chebyshevt2_lp_fx64 : iir<__fx64> { };
class elliptic_lp_fx64    : iir<__fx64> { };

// child highpass filters :

// customized macros exclusion to avoid aliases during compilation:
#undef __fx32
#undef __fx64
#undef __ix32
#undef PI0
#undef PI2
#undef DEBUG

#endif
