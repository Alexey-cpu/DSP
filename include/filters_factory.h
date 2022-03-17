#ifndef FILTERS_FACTORY_H
#define FILTERS_FACTORY_H

#ifndef __ALG_PLATFORM
#include "cmath"
#include "math.h"
#include <iostream>
#include <cstring>
#endif

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

namespace DSP
{
    // frequency bandwidth data structure:
    struct bandwidth
    {
        __fx64 Fc;
        __fx64 BW;
    };

    // passband and stopband attenuation data structure:
    struct attenuation
    {
        __fx64 G1;
        __fx64 G2;
    };

    // filter type enumeration:
    enum filter_type
    {
        lowpass,
        highpass,
        bandpass,
        bandstop
    };

    // zeros / poles plain:
    template< typename T > struct zp
    {
        complex< T > *plp;
        complex< T > *zlp;
        complex< T > *glp;
        __ix32 L;
        __ix32 R;
        __ix32 N;
    };

    // coefficients matrix:
    template< typename T > struct cf
    {
        T *cfnum;
        T *cfden;
        T *gains;
        __ix32 L;
        __ix32 R;
        __ix32 N;
    };

    // frequency response data structure:
    struct fr
    {
        __fx64 Km;
        __fx64 pH;
    };

    // memory manipulation functions:
    template<typename __type> cf<__type> __cf_free__( cf<__type> cfmatrix )
    {
        if( cfmatrix.cfden != nullptr ) free( cfmatrix.cfden );
        if( cfmatrix.cfnum != nullptr ) free( cfmatrix.cfnum );
        if( cfmatrix.gains != nullptr ) free( cfmatrix.gains );
        return { nullptr , nullptr , nullptr , -1 , -1 , -1 };
    }

    // frequency response computation function:
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
        return fr{ __absf__( tsf ) , __argf__( tsf ) };
    }

    template< typename T > fr __freq_resp__(  __fx64 Fs , __fx64 F , __ix32 order , T *cfbuff )
    {
        T Re = 0 , Im = 0 , Ts = 1 / Fs;
        for ( __ix32 i = 0; i <= order; i++)
        {
            Re = Re + cos(-PI2 * i * F * Ts) * cfbuff[ i ];
            Im = Im + sin(-PI2 * i * F * Ts) * cfbuff[ i ];
        }
        return { sqrt (Re * Re + Im * Im) , atan2( Im , Re ) };
    }

    // zeros / poles computation functions:

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
        complex<T> *plp = ( complex<T>* ) calloc( N+0 , sizeof ( complex<T> ) );
        complex<T> *zlp = ( complex<T>* ) calloc( N+0 , sizeof ( complex<T> ) );
        complex<T> *glp = ( complex<T>* ) calloc( N+1 , sizeof ( complex<T> ) );

        if( plp == 0 || zlp == 0 || glp == 0 )
        {
            free( plp );
            free( zlp );
            free( glp );
            return zp<T>{ 0 , 0 , 0 , -1 , -1 , -1 };
        }

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
        complex< T >  *plp = ( complex<T>* ) calloc( N   , sizeof ( complex< T > ) );
        complex< T >  *zlp = ( complex<T>* ) calloc( N   , sizeof ( complex< T > ) );
        complex< T >  *glp = ( complex<T>* ) calloc( N+1 , sizeof ( complex< T > ) );

        if( plp == 0 || zlp == 0 || glp == 0 )
        {
            free( plp );
            free( zlp );
            free( glp );
            return zp< T >{ 0 , 0 , 0 , -1 , -1 , -1 };
        }

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
        complex< T > *plp = ( complex< T >* ) calloc( N   , sizeof ( complex< T > ) );
        complex< T > *zlp = ( complex< T >* ) calloc( N   , sizeof ( complex< T > ) );
        complex< T > *glp = ( complex< T >* ) calloc( N+1 , sizeof ( complex< T > ) );

        if( plp == 0 || zlp == 0 || glp == 0 )
        {
            free( plp );
            free( zlp );
            free( glp );
            return zp< T >{ 0 , 0 , 0 , -1 , -1 , -1 };
        }

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
        complex< T > *plp = ( complex< T >* ) calloc( N   , sizeof ( complex< T > ) );
        complex< T > *zlp = ( complex< T >* ) calloc( N   , sizeof ( complex< T > ) );
        complex< T > *glp = ( complex< T >* ) calloc( N+1 , sizeof ( complex< T > ) );

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
        complex< __fx64 > *plp = zp.plp;
        complex< __fx64 > *zlp = zp.zlp;
        complex< __fx64 > *glp = zp.glp;
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
        complex< __fx64 > *plp = zp.plp;
        complex< __fx64 > *zlp = zp.zlp;
        complex< __fx64 > *glp = zp.glp;
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
        complex< __fx64 > *plp = zp.plp;
        complex< __fx64 > *glp = zp.glp;
        complex< __fx64 > *zlp = zp.zlp;
        __ix32 L = zp.L , R = zp.R , N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        complex< __fx64 > *pbp = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );
        complex< __fx64 > *zbp = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );
        complex< __fx64 > *gbp = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );

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
       complex< __fx64 > *plp = zp.plp;
       complex< __fx64 > *glp = zp.glp;
       complex< __fx64 > *zlp = zp.zlp;
       __ix32 L = zp.L , R = zp.R , N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        complex< __fx64 > *pbs = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );
        complex< __fx64 > *zbs = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );
        complex< __fx64 > *gbs = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );

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
        complex< __fx64 > *plp = zp.plp;
        complex< __fx64 > *zlp = zp.zlp;
        complex< __fx64 > *glp = zp.glp;

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
        complex< __fx64 > *plp = zp.plp;
        complex< __fx64 > *zlp = zp.zlp;
        complex< __fx64 > *glp = zp.glp;

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
        complex< __fx64 > *plp = zp.plp;
        complex< __fx64 > *glp = zp.glp;
        complex< __fx64 > *zlp = zp.zlp;
        __ix32 L = zp.L , R = zp.R , N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        complex< __fx64 > *pbp = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );
        complex< __fx64 > *zbp = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );
        complex< __fx64 > *gbp = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );

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
        complex< __fx64 > *plp = zp.plp;
        complex< __fx64 > *glp = zp.glp;
        complex< __fx64 > *zlp = zp.zlp;
        __ix32 L = zp.L , R = zp.R , N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        complex< __fx64 > *pbs = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );
        complex< __fx64 > *zbs = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );
        complex< __fx64 > *gbs = ( complex< __fx64 >* ) calloc( 2*N , sizeof ( complex< __fx64 > ) );

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
            wind_fcn &wind
    )
    {
        // check if the window is ready to use:
        if ( wind.is_ready() == 0 )
        {
            wind.init(N+1);
            wind.Chebyshev(100);
        }

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
            fr fr = __freq_resp__( Fs , 0 , N , cfbuff );
            for ( __ix32 n = 0; n <= N; n++) cfbuff[n] /= fr.Km;
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
            wind_fcn &wind
    )
    {
        // check if the window is ready to use:
        if ( wind.is_ready() == 0 )
        {
            wind.init(N+1);
            wind.Chebyshev(100);
        }

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
            fr fr = __freq_resp__( Fs , Fs / 2 , N , cfbuff );
            for ( __ix32 n = 0; n <= N; n++) cfbuff[n] /= fr.Km;
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
            wind_fcn &wind
    )
    {
        // check if the window is ready to use:
        if ( wind.is_ready() == 0 )
        {
            wind.init(N+1);
            wind.Chebyshev(100);
        }

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
            fr fr = __freq_resp__( Fs , ( ( Fp1 + 0.5 * ( Fp2 - Fp1 ) ) * Fs ) , N , cfbuff );
            for ( __ix32 n = 0; n <= N; n++) cfbuff[n] /= fr.Km;
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
    template< typename T > cf<T> __fir_wind_digital_bs__
    (
            __fx64 Fs,
            __fx64 Fc,
            __fx64 BW,
            __ix32 N,
            bool scale,
            wind_fcn &wind
    )
    {
        typedef T __type;

        // check if the window is ready to use:
        if ( wind.is_ready() == 0 )
        {
            wind.init(N+1);
            wind.Chebyshev(100);
        }

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
            fr fr = __freq_resp__( Fs , 0 , N , cfbuff );
            for ( __ix32 n = 0; n <= N; n++) cfbuff[n] /= fr.Km;
        }

        // return the result:
        return cf< T >{ cfbuff , 0 , 0 , -1 , -1 , N  };
    }


    // IIR filter filtering function:
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

    // transfer function filtering function:
    template< typename __type > inline __attribute__( (always_inline) ) __type __filt__
    (
            __type *input ,
            __type *cfnum ,
            __type *cfden ,
            __type gain ,
            __ix32 Nx ,
            __ix32 Ny ,
            delay<__type> &bx ,
            delay<__type> &by )
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


    // FIR filter filtering function:
    template< typename CT , typename BT > inline  BT filt
    (
            CT *input,
            BT *coeffs,
            delay< BT > &buff_sx,
            __ix32 N
    )
    {
        buff_sx( input );
        BT out = 0;
        for ( __ix32 n = N-1 ; n >= 0; n--) out += buff_sx[ n ] * coeffs[n];
        return out;
    }

    // FIR filter filtering function:
    template< typename __type > inline  __type filt
    (
            __type *coeffs ,
            delay< __type > &buff_sx ,
            __ix32 N
    )
    {
        __type out = 0;
        for ( __ix32 n = N-1 ; n >= 0; n--) out += buff_sx[ n ] * coeffs[n];
        return out;
    }

    //

    // template abstract classes:

    // classic filter base:
    class filter_abstract
    {
    protected:
        __ix32 m_order;
        __fx64 m_Fs;
        __fx64 m_Fn;
        __fx64 m_Ts;

        // default constructor:
        filter_abstract()
        {
            m_order = 8;
            m_Fs    = 4000;
            m_Fn    = 50;
            m_Ts    = 1 / m_Fs;
        }

        // frequency response computation function:
        virtual fr frequency_response( __fx64 F ) = 0;

        // virtual destructor:
        virtual ~filter_abstract(){};

        // memory allocation virtual function:
        virtual __ix32 allocate() = 0;

        // memory deallocation virtual function:
        virtual __ix32 deallocate() = 0;
    };

    // classic IIR filter virtual base:
    template< typename __type > class classic_iir_abstract : public  filter_abstract
    {
        protected:
        filter_type m_filter_type;
        bandwidth   m_bandwidth;
        attenuation m_attenuation;

        // input / output buffers:
        delay< __type > *m_buff_sx;
        delay< __type > *m_buff_sy;

        // coefficients matrix:
        cf< __type > m_cfmatrix;

        // filter coefficients computation functions:
        virtual cf< __type > compute_lowpass () = 0;
        virtual cf< __type > compute_highpass() = 0;
        virtual cf< __type > compute_bandpass() = 0;
        virtual cf< __type > compute_bandstop() = 0;

    public:
        // filtering operator:
        virtual inline __type operator()( __type *input ) = 0;

        // virtual destructor:
        virtual ~classic_iir_abstract()
        {
            deallocate();
        }

        // memory allocation function:
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

        // memory deallocation function:
        __ix32 deallocate()  override
        {
            // input / output buffers memory deallcoation:
            int N = m_cfmatrix.N;
            if( m_buff_sx != nullptr ) { for( int i = 0 ; i < N ; i++ ) { m_buff_sx[i].deallocate(); } m_buff_sx = nullptr; }
            if( m_buff_sy != nullptr ) { for( int i = 0 ; i < N ; i++ ) { m_buff_sy[i].deallocate(); } m_buff_sy = nullptr; }

            // filter coefficient matrix memory deallocation:
            m_cfmatrix = __cf_free__( m_cfmatrix );

            // return the result:
            return ( !m_cfmatrix.cfden && !m_cfmatrix.cfnum && !m_cfmatrix.gains && !m_buff_sx && !m_buff_sy );
        }

        // frequecny response function override:
        fr frequency_response( __fx64 F ) override { return __freq_resp__( m_cfmatrix.cfnum , m_cfmatrix.cfden , m_cfmatrix.gains , m_cfmatrix.N , m_Fs , F ); }

        void init( __fx64 Fs , __ix32 order , filter_type type = filter_type::lowpass , bandwidth BW = { 100 , 400 } , attenuation AT = { 80 , 1 } )
        {
            m_Fs    = Fs;
            m_filter_type    = type;
            m_order = order;
            m_bandwidth    = BW;
            m_attenuation    = AT;
        }

        // default constructor:
        classic_iir_abstract() : filter_abstract()
        {
            m_buff_sx = nullptr;
            m_buff_sy = nullptr;
            m_filter_type = filter_type::lowpass;
            m_bandwidth = { 100 , 500 };
            m_attenuation = { 80  , 1   };
        }

        // initializing constructor:
        classic_iir_abstract( __fx64 Fs , __ix32 order , filter_type type = filter_type::lowpass , bandwidth BW = { 100 , 400 } , attenuation AT = { 1 , 80 } ) { init( Fs , order ,  type , BW ,  AT ); }
    };

    // classic FIR filter virtual base:
    template< typename __type > class classic_fir_abstract : public  filter_abstract
    {
            typedef filter_type __ftp;
    public:
        wind_fcn    m_wind;

    protected:
        __ftp m_ft;
        bandwidth   m_bandwidth;
        bool m_scale;

        // input buffer:
        delay<__type> m_buff_sx;

        // coefficients matrix:
        cf<__type> m_cfmatrix;
        __type *m_coeffs;

    public:

        void show_coeffs()
        {
            for( int i = 0 ; i <= m_order ; i++ )
            {
                std::cout << "a[" << i << "] = " << m_coeffs[i] << "\n";
            }
        }

        // filtering operator:
        virtual inline void operator <<  ( __type *input ) = 0;
        virtual inline __type operator() ( __type *input ) = 0;
        virtual inline __type operator() () = 0;

        // virtual destructor:
        virtual ~classic_fir_abstract(){ deallocate(); };

        // memory allocation function:
        __ix32 allocate() override
        {
            switch ( m_ft )
            {
                case filter_type::lowpass:
                m_cfmatrix = __fir_wind_digital_lp__<__type>( m_Fs , m_bandwidth.Fc , m_order , m_scale , m_wind );
                break;

                case filter_type::highpass:
                m_cfmatrix = __fir_wind_digital_hp__<__type>( m_Fs , m_bandwidth.Fc , m_order , m_scale , m_wind );
                break;

                case filter_type::bandpass:
                m_cfmatrix = __fir_wind_digital_bp__<__type>( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , m_scale , m_wind );
                break;

                case filter_type::bandstop:
                m_cfmatrix = __fir_wind_digital_bs__<__type>( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , m_scale , m_wind );
                break;
            }

            if( m_cfmatrix.cfnum != nullptr )
            {
                m_order = m_cfmatrix.N;
                m_coeffs = m_cfmatrix.cfnum;
                return m_buff_sx.allocate( m_cfmatrix.N + 1 );
            }
            return 0;
        }

        // memory deallocation function:
        __ix32 deallocate() override
        {
            // window function memory deallcation:
            m_wind.deallocate();

            // filter data memory deallocation:
            m_buff_sx.deallocate();
            m_cfmatrix = __cf_free__( m_cfmatrix );
            return ( !m_cfmatrix.cfnum && !m_cfmatrix.cfden );
        }

        // frequency response computation function:
        fr frequency_response( __fx64 F ) override
        {
            return __freq_resp__<__type> ( m_Fs , F , m_order , m_cfmatrix.cfnum );
        }

        void init( __fx64 Fs , __ix32 order , filter_type type , bandwidth BW , bool scale )
        {
            m_Fs        = Fs;
            m_ft        = type;
            m_order     = order;
            m_bandwidth = BW;
            m_scale     = scale;

            // window function initialization:
            m_wind.init(m_order+1);
        }

        // default constructor:
        classic_fir_abstract() : filter_abstract()
        {
            m_ft = filter_type::lowpass;
            m_bandwidth = { 100 , 500 };
        }

        // initializing constructor:
        classic_fir_abstract( __fx64 Fs , __ix32 order , filter_type type , bandwidth BW , bool scale )
        {
            init( Fs , order ,  type , BW , scale );
        }
    };

    // Classic filters templates:

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

    //--------------------------------------------------------------------------------------------------------------------
    // not implemented yet:
    //--------------------------------------------------------------------------------------------------------------------
    template< typename __type > class recursive_fourier;
    template< typename __type > class recursive_mean;
    template< typename __type > class recursive_rms;
    //--------------------------------------------------------------------------------------------------------------------

    // IIR filters realizations:

    // butterworth filter 32-bit realization:
    template<> class butterworth< __fx32 > final : public classic_iir_abstract< __fx32 >
    {
            typedef __fx32 __type;
        public:
        // constructors:
         butterworth< __type >() : classic_iir_abstract< __type >(){}
         butterworth< __type >( __fx64 Fs , __ix32 order , filter_type type , bandwidth bandwidth , attenuation attenuation )
             : classic_iir_abstract< __type >( Fs , order , type , bandwidth , attenuation ) {}
        ~butterworth< __type >(){};

        // base class virtual functions overriding:
        cf< __type > compute_lowpass () override { return __butt_cheb1_digital_lp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 0 , m_attenuation.G1 ); }
        cf< __type > compute_highpass() override { return __butt_cheb1_digital_hp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 0 , m_attenuation.G1 ); }
        cf< __type > compute_bandpass() override { return __butt_cheb1_digital_bp__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 0 , m_attenuation.G1 ); }
        cf< __type > compute_bandstop() override { return __butt_cheb1_digital_bs__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 0 , m_attenuation.G1 ); }

        // filtering operator override:
        inline __type operator()( __type *input ) override { return __filt__< __type >( input , m_cfmatrix.cfnum , m_cfmatrix.cfden , m_cfmatrix.gains  , m_cfmatrix.N ,  m_buff_sx , m_buff_sy ); }
    };

    // butterworth filter 64-bit realization:
    template<> class butterworth< __fx64 > final : public classic_iir_abstract< __fx64 >
    {
            typedef __fx64 __type;
        public:
        // constructors:
         butterworth< __type >() : classic_iir_abstract< __type >(){}
         butterworth< __type >( __fx64 Fs , __ix32 order , filter_type type , bandwidth bandwidth , attenuation attenuation )
             : classic_iir_abstract< __type >( Fs , order , type , bandwidth , attenuation ) {}
        ~butterworth< __type >(){};

        // base class virtual functions overriding:
        cf< __type > compute_lowpass () override { return __butt_cheb1_digital_lp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 0 , m_attenuation.G1 ); }
        cf< __type > compute_highpass() override { return __butt_cheb1_digital_hp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 0 , m_attenuation.G1 ); }
        cf< __type > compute_bandpass() override { return __butt_cheb1_digital_bp__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 0 , m_attenuation.G1 ); }
        cf< __type > compute_bandstop() override { return __butt_cheb1_digital_bs__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 0 , m_attenuation.G1 ); }

        // filtering operator override:
        inline __type operator()( __type *input ) override { return __filt__< __type >( input , m_cfmatrix.cfnum , m_cfmatrix.cfden , m_cfmatrix.gains  , m_cfmatrix.N ,  m_buff_sx , m_buff_sy ); }
    };

    // Chebyshev I filter 32-bit realization:
    template<> class chebyshev_1< __fx32 > final : public classic_iir_abstract< __fx32 >
    {
            typedef __fx32 __type;
        public:
        // constructors:
         chebyshev_1< __type >() : classic_iir_abstract< __type >(){}
         chebyshev_1< __type >( __fx64 Fs , __ix32 order , filter_type type, bandwidth bandwidth , attenuation attenuation )
             : classic_iir_abstract< __type >( Fs , order , type , bandwidth , attenuation ) {}
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
         chebyshev_1< __type >( __fx64 Fs , __ix32 order , filter_type type , bandwidth bandwidth , attenuation attenuation )
             : classic_iir_abstract< __type >( Fs , order , type , bandwidth , attenuation ) {}
        ~chebyshev_1< __type >(){};

        // base class virtual functions overriding:
        cf< __type > compute_lowpass () override { return __butt_cheb1_digital_lp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 1 , m_attenuation.G1 ); }
        cf< __type > compute_highpass() override { return __butt_cheb1_digital_hp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 1 , m_attenuation.G1 ); }
        cf< __type > compute_bandpass() override { return __butt_cheb1_digital_bp__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 1 , m_attenuation.G1 ); }
        cf< __type > compute_bandstop() override { return __butt_cheb1_digital_bs__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 1 , m_attenuation.G1 ); }

        // filtering operator override:
        inline __type operator()( __type *input ) override { return __filt__< __type >( input , m_cfmatrix.cfnum , m_cfmatrix.cfden , m_cfmatrix.gains  , m_cfmatrix.N ,  m_buff_sx , m_buff_sy ); }
    };

    // Chebyshev II filter 32-bit realization:
    template<> class chebyshev_2< __fx32 > final : public classic_iir_abstract< __fx32 >
    {
            typedef __fx32 __type;
        public:
        // constructors:
         chebyshev_2< __type >() : classic_iir_abstract< __type >(){}
         chebyshev_2< __type >( __fx64 Fs , __ix32 order , filter_type type , bandwidth bandwidth , attenuation attenuation )
             : classic_iir_abstract< __type >( Fs , order , type , bandwidth , attenuation ) {}
        ~chebyshev_2< __type >(){};

        // base class virtual functions overriding:
        cf< __type > compute_lowpass () override { return __cheb2_ellip_digital_lp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 0 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_highpass() override { return __cheb2_ellip_digital_hp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 0 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_bandpass() override { return __cheb2_ellip_digital_bp__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 0 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_bandstop() override { return __cheb2_ellip_digital_bs__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 0 , m_attenuation.G2 , m_attenuation.G1 ); }

        // filtering operator override:
        inline __type operator()( __type *input ) override { return __filt__< __type >( input , m_cfmatrix.cfnum , m_cfmatrix.cfden , m_cfmatrix.gains  , m_cfmatrix.N ,  m_buff_sx , m_buff_sy ); }
    };

    // Chebyshev II filter 64-bit realization:
    template<> class chebyshev_2< __fx64 > final : public classic_iir_abstract< __fx64 >
    {
            typedef __fx64 __type;
        public:
        // constructors:
         chebyshev_2< __type >() : classic_iir_abstract< __type >(){}
         chebyshev_2< __type >( __fx64 Fs , __ix32 order , filter_type type , bandwidth bandwidth , attenuation attenuation )
             : classic_iir_abstract< __type >( Fs , order , type , bandwidth , attenuation ) {}
        ~chebyshev_2< __type >(){};

        // base class virtual functions overriding:
        cf< __type > compute_lowpass () override { return __cheb2_ellip_digital_lp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 0 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_highpass() override { return __cheb2_ellip_digital_hp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 0 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_bandpass() override { return __cheb2_ellip_digital_bp__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 0 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_bandstop() override { return __cheb2_ellip_digital_bs__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 0 , m_attenuation.G2 , m_attenuation.G1 ); }

        // filtering operator override:
        inline __type operator()( __type *input ) override { return __filt__< __type >( input , m_cfmatrix.cfnum , m_cfmatrix.cfden , m_cfmatrix.gains  , m_cfmatrix.N ,  m_buff_sx , m_buff_sy ); }
    };

    // Elliptic filter 32-bit realization:
    template<> class elliptic< __fx32 > final : public classic_iir_abstract< __fx32 >
    {
            typedef __fx32 __type;
        public:
        // constructors:
         elliptic< __type >() : classic_iir_abstract< __type >(){}
         elliptic< __type >( __fx64 Fs , __ix32 order , filter_type type , bandwidth bandwidth , attenuation attenuation )
             : classic_iir_abstract< __type >( Fs , order , type , bandwidth , attenuation ) {}
        ~elliptic< __type >(){};

        // base class virtual functions overriding:
        cf< __type > compute_lowpass () override { return __cheb2_ellip_digital_lp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 1 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_highpass() override { return __cheb2_ellip_digital_hp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 1 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_bandpass() override { return __cheb2_ellip_digital_bp__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 1 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_bandstop() override { return __cheb2_ellip_digital_bs__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 1 , m_attenuation.G2 , m_attenuation.G1 ); }

        // filtering operator override:
        inline __type operator()( __type *input ) override { return __filt__< __type >( input , m_cfmatrix.cfnum , m_cfmatrix.cfden , m_cfmatrix.gains  , m_cfmatrix.N ,  m_buff_sx , m_buff_sy ); }
    };

    // Elliptic filter 64-bit realization:
    template<> class elliptic< __fx64 > final : public classic_iir_abstract< __fx64 >
    {
            typedef __fx64 __type;
        public:
        // constructors:
         elliptic< __type >() : classic_iir_abstract< __type >(){}
         elliptic< __type >( __fx64 Fs , __ix32 order , filter_type type , bandwidth bandwidth , attenuation attenuation )
             : classic_iir_abstract< __type >( Fs , order , type , bandwidth , attenuation ) {}
        ~elliptic< __type >(){};

        // base class virtual functions overriding:
        cf< __type > compute_lowpass () override { return __cheb2_ellip_digital_lp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 1 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_highpass() override { return __cheb2_ellip_digital_hp__< __type >( m_Fs , m_bandwidth.Fc , m_order , 1 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_bandpass() override { return __cheb2_ellip_digital_bp__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 1 , m_attenuation.G2 , m_attenuation.G1 ); }
        cf< __type > compute_bandstop() override { return __cheb2_ellip_digital_bs__< __type >( m_Fs , m_bandwidth.Fc , m_bandwidth.BW , m_order , 1 , m_attenuation.G2 , m_attenuation.G1 ); }

        // filtering operator override:
        inline __type operator()( __type *input ) override { return __filt__< __type >( input , m_cfmatrix.cfnum , m_cfmatrix.cfden , m_cfmatrix.gains  , m_cfmatrix.N ,  m_buff_sx , m_buff_sy ); }
    };

    // FIR filter 32-bit realization:
    template<> class fir< __fx32 > final : public classic_fir_abstract< __fx32 >
    {
        typedef __fx32 __type;
    public:
        fir() : classic_fir_abstract< __type >(){}
        fir( __fx64 Fs , __ix32 order , filter_type type , bandwidth bandwidth , bool scale = true )
            : classic_fir_abstract< __type >( Fs , order , type , bandwidth , scale ){}
       ~fir() {}
        inline __type operator()( __type  *input ) override { return filt< __type , __type >( input , m_coeffs , m_buff_sx , m_order ); }
        inline __type operator()( __fx64  *input ) { return filt< __fx64  , __type >( input , m_coeffs , m_buff_sx , m_order ); }
        inline __type operator()( __fxx64 *input ) { return filt< __fxx64 , __type >( input , m_coeffs , m_buff_sx , m_order ); }

        inline __type operator()() override { return filt < __type > ( m_coeffs , m_buff_sx , m_order ); }
        inline void operator << ( __type  *input ) override { m_buff_sx( input ); }
        inline void operator << ( __fx64  *input ) { m_buff_sx( input ); }
        inline void operator << ( __fxx64 *input ) { m_buff_sx( input ); }
    };

    // FIR filter 64-bit realization:
    template<> class fir< __fx64 > final : public classic_fir_abstract< __fx64 >
    {
        typedef __fx64 __type;
    public:
        fir() : classic_fir_abstract< __type >(){}
        fir( __fx64 Fs , __ix32 order , filter_type type , bandwidth bandwidth , bool scale = true )
            : classic_fir_abstract< __type >( Fs , order , type , bandwidth , scale ){}
       ~fir() {}
        inline __type operator()( __type  *input ) override { return filt< __type , __type >( input , m_coeffs , m_buff_sx , m_order ); }
        inline __type operator()( __fxx64 *input ) { return filt< __fxx64 , __type >( input , m_coeffs , m_buff_sx , m_order ); }

        inline __type operator()() override { return filt < __type > ( m_coeffs , m_buff_sx , m_order ); }
        inline void operator << ( __type  *input ) override { m_buff_sx( input ); }
        inline void operator << ( __fxx64 *input ) { m_buff_sx( input ); }
    };

};

#endif // FILTERS_FACTORY_H
