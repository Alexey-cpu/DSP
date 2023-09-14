#ifndef KERNEL_IIR_H
#define KERNEL_IIR_H

#include "special_functions.h"
#include "kernel_dsp.h"
using namespace DSP_KERNEL;

#ifndef __ALG_PLATFORM
//#define IIR_KERNEL_DEBUG // debugging is not available if the algorithm is running on a device !!!
#endif

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

/*!  \namespace IIR_KERNEL */
namespace IIR_KERNEL
{

    /*! \defgroup <DSP_CLASSIC_IIR_FILTERS_KERNEL> ( Classic IIR low level kernel )
     *  \ingroup KERNELS
     *  \brief   The module contains classic IIR filters low level kernel
     *  \details The module contains the Butterworth, Chebyshev type I, Chebyshev type II and Elliptic
     *           filters low level computational utilities. All the mentioned above filters are modelled
     *           by quadratic sections model ty avoid the rounding errors. All the functions compute
     *          floating point type coefficients.
        @{
    */

    using namespace DSP_KERNEL;

    /*!
     * \brief IIR filter filtering function
     * \param[_input] pointer to the input signal samples buffer
     * \param[_cfnum] IIR filter numerator quadratic sections coefficients matrix
     * \param[_cfden] IIR filter denominator quadratic sections coefficients matrix
     * \param[_gains] IIR filter quadratic sections gains vector
     * \param[_Nx] IIR filter single section numerator order
     * \param[_Ny] IIR filter single section denominator order
     * \param[_N] IIR filter sections number
     * \param[_buff_sx] IIR filter quadratic sections input  buffers vector
     * \param[_buff_sy] IIR filter quadratic sections output buffers vector
     * \returns  The function implements input signal filtering using
     *           IIR filter coefficients matrix, gains and I/O buffers vectors.
     *           The input signal is going trough the group of the second order filters cascade:
     *           \f[
     *              y_{i}(k) = gain_{i} * \sum_{j=0}^{N_x} a_{ji} * x(k-j) - \sum_{j=0}^{N_y} b_{ji} * y(k-j)
     *           \f]
    */
    template< typename __type >
    inline __type __filt__( __type *_input, __type *_cfnum, __type *_cfden, __type *_gains, __ix32 _Nx, __ix32 _Ny, __ix32 _N, delay< __type > *_buff_sx, delay< __type > *_buff_sy )
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
            sum_num = sum_den = 0;
            for (__ix32 j = 0 ; j < _Nx   ; j++) sum_num += _buff_sx[ i ][ j ] * _cfnum[_Nx * i + j ];
            for (__ix32 j = 0 ; j < _Ny-1 ; j++) sum_den += _buff_sy[ i ][ j ] * _cfden[_Ny * i + j + 1 ];
            _buff_sy[ i ]( &( out = sum_num * _gains[i] - sum_den ) );
            if( i < _N - 1 ) _buff_sx[ i + 1 ]( &out );
        }
        return out;
    }


    /*!
     * \brief IIR filter frequency response computation function
     * \param[_cfnum] IIR filter numerator quadratic sections coefficients matrix
     * \param[_cfden] IIR filter denominator quadratic sections coefficients matrix
     * \param[_gains] IIR filter quadratic sections gains vector
     * \param[_Nx] IIR filter single section numerator order
     * \param[_Ny] IIR filter single section denominator order
     * \param[_N] IIR filter sections number
     * \param[_Fs] sampling frequency, Hz
     * \param[_F] input signal frequency, Hz
     * \return The function computes IIR filter transfer function frequency response:
     * \f[
     *      W(j*2*\pi*f) = \prod_{i=0}^N \frac{ \sum_{k=0}^N_{x} a_{ki} * e^{-j*2*\pi*f*Ts*k} }
     *                                        { \sum_{m=0}^N_{y} b_{mi} * e^{-j*2*\pi*f*Ts*m} }
     * \f]
    */
    template<typename __type> Complex<__fx64> __freq_resp__ (__type *_cfnum, __type *_cfden,  __type *_gains, __ix32 _Nx, __ix32 _Ny, __ix32 _N,  __fx64 _Fs,  __fx64 _F )
    {
        // check the input:
        if( !_cfnum || !_cfden || !_gains )
        {

            #ifdef IIR_KERNEL_DEBUG
            Debugger::Log("kernel_iir.h", "__freq_resp__", "filter coefficients { cfnum, cfden, gains } are null !!!");
            #endif

            return { -1 , -1 };
        }

        // sampling period:
        __fx64 Ts = 1 / _Fs;

        // transfer function initialization:
        Complex< __fx64 > Wz(1,0), num(0,0), den(0,0);

        for(__ix32 i = 0; i < _N; i++ )
        {
            // zerowing numerator and denominator sections sums:
            num(0,0);
            den(0,0);

            // numerator and denominator sums computation:
            for(__ix32 j = 0; j < _Nx; j++) num += Complex<__fx64>( cos( -PI2 * _F * Ts * j ) , sin( -PI2 * _F * Ts * j ) ) * (__fx64)_cfnum[_Nx * i + j];
            for(__ix32 j = 0; j < _Ny; j++) den += Complex<__fx64>( cos( -PI2 * _F * Ts * j ) , sin( -PI2 * _F * Ts * j ) ) * (__fx64)_cfden[_Ny * i + j];
            Wz *= num / den * (__fx64)_gains[i];
        }

        // multiply by an output gain:
        if( _N > 1) Wz *= (__fx64)_gains[_N];
        return Wz;
    }

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
    */
    template<typename __type>
    filter_data<__type> __butt_zeros_poles_plain__( __ix32 _order, __fx64 _g_stop ) // refactored !!!
    {
        // number of zeros, poles, coeffs:
        __ix32 L = trunc( _order / 2 );
        __ix32 R = _order - 2 * L;
        __ix32 N = L+R;

        // stopband attenuation:
        __fx64 epsilon_stop = sqrt(pow(10, _g_stop / 10) - 1);

        // memory allocation for the lowpass analogue prototype poles, zeros and gains
        Complex<__fx64> *plp = __alloc__< Complex<__fx64> >(N+0);
        Complex<__fx64> *zlp = __alloc__< Complex<__fx64> >(N+0);
        Complex<__fx64> *glp = __alloc__< Complex<__fx64> >(N+1);

        if( !plp || !zlp || !glp )
        {
            #ifdef IIR_KERNEL_DEBUG
            Debugger::Log("kernel_iir.h", "__butt_zeros_poles_plain__", "memory allocation has failed !!!");
            #endif

            plp = __mfree__(plp);
            zlp = __mfree__(zlp);
            glp = __mfree__(glp);
            return __dsp_gen_empty_filter__<__type>();
        }

        // fcomplex-conjugate pairs:
        __fx64 alpha = 0;
        __fx64 beta  = 1 / sqrt( pow( epsilon_stop , 1 / _order ) );

        for( __ix32 i = 0 ; i < L ; i++ )
        {
            alpha = (2 * (i + 1) - 1) * PI0 / (2 * _order);
            plp[i] = Complex<__fx64>( ( -1 / beta * sin(alpha) ) , ( +1 / beta * cos(alpha) ) );
            glp[i] = plp[i] * __conjf__( plp[i] ) ;
        }

        // real odd pole:
        if( R == 1 )
        {
            plp[ N - 1 ] = Complex< __fx64 >( ( -1 / beta ) , 0 );
            glp[ N - 1 ] = -__realf__<__fx64>( plp[ N - 1 ] );
        }

        // setting the output gain:
        glp[ N ] = 1;

        // generate output
        filter_data<__type> data;
        data.poles = plp;
        data.zeros = zlp;
        data.ratio = glp;
        data.L     = L;
        data.R     = R;
        data.N     = N;

        return data;
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
    */
    template<typename __type>
    filter_data<__type> __cheb1_zeros_poles_plain__ ( __ix32 _order,  __fx64 _g_stop ) // refactored
    {
        // number of zeros, poles, coeffs:
        __ix32 L = trunc( _order / 2 );
        __ix32 R = _order - 2 * L;
        __ix32 N = L + R;

        // stopband attenuation:
        __fx64 epsilon_stop = sqrt(pow(10, _g_stop / 10) - 1);

        // memory allocation for the lowpass analogue prototype poles, zeros and gains:
        Complex< __fx64 >  *plp = __alloc__< Complex<__fx64> >(N+0);
        Complex< __fx64 >  *zlp = __alloc__< Complex<__fx64> >(N+0);
        Complex< __fx64 >  *glp = __alloc__< Complex<__fx64> >(N+1);

        if( !plp || !zlp || !glp )
        {
            #ifdef IIR_KERNEL_DEBUG
            Debugger::Log("kernel_iir.h", "__cheb1_zeros_poles_plain__", "memory allocation has failed !!!");
            #endif

            plp = __mfree__(plp);
            zlp = __mfree__(zlp);
            glp = __mfree__(glp);
            return __dsp_gen_empty_filter__<__type>();
        }

        // fcomplex conjugate pairs:
        __fx64 alpha = 0;
        __fx64 beta  = asinh( 1 / epsilon_stop ) / _order;
        for ( __ix32 i = 0; i < L; i++)
        {
            alpha = (2 * (i + 1) - 1) * PI0 / (2 * _order);
            plp[i] = Complex< __fx64 >( -sin( alpha ) * sinh( beta ) , +cos( alpha ) * cosh( beta ) );
            glp[i] = plp[i] * __conjf__( plp[i] );
        }

        // real odd pole:
        if ( R == 1 )
        {
            plp[ N - 1 ] = Complex< __fx64 >( -sinh( beta ) , 0 );
            glp[ N - 1 ] = -__realf__<__fx64>( plp[ N - 1 ] );
        }

        // setting the output gain:
        glp[N] = ( R < 1 ) ? ( sqrt(1 / (1 + epsilon_stop * epsilon_stop) ) ) : 1;

        // generate output
        filter_data<__type> data;
        data.poles = plp;
        data.zeros = zlp;
        data.ratio = glp;
        data.L     = L;
        data.R     = R;
        data.N     = N;

        // end the computation and return the filter data
        return data;
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
     *           All the data is stored within zp data structure and returned.
    */
    template<typename __type>
    filter_data<__type> __cheb2_zeros_poles_plain__( __ix32 _order, __fx64 _g_stop ) // refactored
    {
        // stopband attenuation:
        __fx64 epsilon_stop = sqrt(pow(10, _g_stop / 10) - 1);

        // identify the number of zeros and poles:
        __ix32 L = trunc( _order / 2 );
        __ix32 R = _order - 2 * L;
        __ix32 N = L + R;

        // allocate zeros and poles arrays:
        Complex< __fx64 > *plp = __alloc__< Complex< __fx64 > >(N+0);
        Complex< __fx64 > *zlp = __alloc__< Complex< __fx64 > >(N+0);
        Complex< __fx64 > *glp = __alloc__< Complex< __fx64 > >(N+1);

        if( !plp || !zlp || !glp )
        {
            #ifdef IIR_KERNEL_DEBUG
            Debugger::Log("kernel_iir.h", "__cheb2_zeros_poles_plain__", "memory allocation has failed !!!");
            #endif

            plp = __mfree__(plp);
            plp = __mfree__(zlp);
            plp = __mfree__(glp);
            return __dsp_gen_empty_filter__<__type>();
        }

        // ZEROS AND POLES COMPUTATION:

        // fcomplex conjugate pairs:

        // auxiliary variables:
        __fx64 alpha = 0;
        __fx64 beta  = asinh( epsilon_stop ) / _order;
        __fx64 re    = 0;
        __fx64 im    = 0;

        // zeros , poles, gains computation:
        for ( __ix32 i = 0 ; i < L ; i++ )
        {
            alpha = (2 * ( i + 1 ) - 1) * PI0 / ( 2 * _order );

            // zeros:
            zlp[i] = Complex< __fx64 >( 0 , 1 / cos( alpha ) );

            // poles:
            re = -( sin( alpha ) * sinh( beta ) ) / ( cos( alpha ) * cos( alpha ) * cosh( beta ) * cosh( beta ) + sin( alpha ) * sin( alpha ) * sinh( beta ) * sinh( beta ) );
            im = +( cos( alpha ) * cosh( beta ) ) / ( cos( alpha ) * cos( alpha ) * cosh( beta ) * cosh( beta ) + sin( alpha ) * sin( alpha ) * sinh( beta ) * sinh( beta ) );
            plp[i] = Complex< __fx64 >( re , im );

            // gains:
            glp[i] = ( zlp[i] * __conjf__( zlp[i] ) ) / ( plp[i] * __conjf__( plp[i] ) );
            glp[i] = Complex< __fx64 >( 1 , 0 ) / glp[i];
        }

        // real odd pole:
        if( R >= 1 )
        {
            plp[ N - 1 ] = Complex< __fx64 >( -1 / sinh( beta ) , 0 );
            glp[ N - 1 ] = -__realf__<__fx64>( plp[ N - 1 ] );
        }

        // set the output gain:
        glp[N] = 1;

        // generate output
        filter_data<__type> data;
        data.poles = plp;
        data.zeros = zlp;
        data.ratio = glp;
        data.L     = L;
        data.R     = R;
        data.N     = N;
        return data;
    }


    /*!
     * \brief Elliptic lowpass analogue prototype zeros/poles plain computation function
     * \param[_g_stop] stopband attenuation , Db
     * \param[_g_pass] passband attenuation , Db
     * \param[_order]  filter order
     * \return   The function computes zeros/poles pairs of Elliptic lowpass
     *           analogue prototype represented as:
     *
     *           \f[
     *              W(s) = \frac{1}{ ( s - p_r ) } * \sum_{i=0}^L \frac{ ( s - z_i ) * ( s - conj(z_i) ) }{ ( s - p_i ) * ( s - conj(p_i) ) }
     *           \f]
     *
     *            All the data is stored within zp data structure and returned.
    */
    template<typename __type>
    filter_data<__type> __ellip_zeros_poles_plain__( __ix32 _order, __fx64 _g_pass, __fx64 _g_stop ) // refactored
    {
        // INITIALIZATION:

        // stopband and passband attenuation:
        __fx64 epsilon_stop = sqrt(pow(10, _g_stop / 10) - 1);
        __fx64 epsilon_pass = sqrt(pow(10, _g_pass / 10) - 1);

        // identify the number of zeros and poles:
        __ix32 L = trunc( _order / 2 );
        __ix32 R = _order - 2 * L;
        __ix32 N = L + R;

        // allocate zeros and poles arrays:
        Complex< __fx64 > *plp = __alloc__< Complex< __fx64 > >(N+0);
        Complex< __fx64 > *zlp = __alloc__< Complex< __fx64 > >(N+0);
        Complex< __fx64 > *glp = __alloc__< Complex< __fx64 > >(N+1);

        if( !plp || !zlp || !glp )
        {
            #ifdef IIR_KERNEL_DEBUG
            Debugger::Log("kernel_iir.h", "__ellip_zeros_poles_plain__", "memory allocation has failed !!!");
            #endif

            plp = __mfree__(plp);
            plp = __mfree__(zlp);
            plp = __mfree__(glp);
            return __dsp_gen_empty_filter__<__type>();
        }

        // TRANSIENT SUPPRESSION FACTORS COMPUTATION:

        // auxiliary variables:
        __fx64 SN    = 0;
        __fx64 KE    = 0;
        __fx64 Ke    = epsilon_pass / epsilon_stop;
        __fx64 Kp    = 1;
        __fx64 m     = sqrt( 1 - Ke * Ke );
        __fx64 alpha = 0;
        __fx64 Kw    = 0;

        // transient suppression factors computation:
        for ( __ix32 i = 0; i < L; i++ )
        {
            alpha = (2 * ( i + 1 ) - 1) / ( ( __fx64 )_order);
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
                alpha  = (2 * i + 1) / ( ( __fx64 )_order) * KE;
                zlp[i] = Complex< __fx64 >( 0 , 1 /( Kw * __sn__( alpha , Kw ) ) );
            }
        }
        else if( _order % 2 != 0 ) // odd order filter
        {
            for ( __ix32 i = 0; i < L; i++)
            {
                alpha  = (2 * i + 2) / ( ( __fx64 )_order) * KE;
                zlp[i] = Complex< __fx64 >( 0 , 1 /( Kw * __sn__( alpha , Kw ) ) );
            }
        }

        // POLES COMPUTATION:
        __fx64 V0 = 0;
        __fx64 A  = 0;
        __fx64 B  = 0;
        __fx64 C  = 0;
        __fx64 D  = 0;
        __fx64 E  = 0;
        __fx64 F  = 0;
        V0 = -__ellip_k__( Kw ) * __isc__(1 / epsilon_pass, sqrt(1 - Ke * Ke)) / ( __ellip_k__( Ke ) * ( ( __fx64 )_order ) );
        KE =  __ellip_k__( Kw );

        // fcomplex conjugate pairs:
        for ( __ix32 i = 0; i < L; i++)
        {
            alpha = ( _order % 2 == 0 ) ? (2 * i + 1) / ( ( __fx64 )_order ) * KE
                                        : (2 * i + 2) / ( ( __fx64 )_order ) * KE;

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
            plp[ N - 1 ] = Complex< __fx64 >( A * B / ( 1 - C )  , 0 );
        }

        // GAINS COMPUTATION:
        if( R >= 1 )
        {
            // fcomplex conjugate pairs:
            for ( __ix32 i = 0; i < L; i++)
            {
                glp[i] = ( zlp[i] * __conjf__( zlp[i] ) ) / ( plp[i] * __conjf__( plp[i] ) );
                glp[i] = Complex< __fx64 >( 1 , 0 ) / glp[i];
            }

            // real odd pole:
            glp[ N - 1 ] = -__realf__<__fx64>( plp[ N - 1 ] );
        }
        else
        {
            for ( __ix32 i = 0; i < L; i++) glp[i] = pow ( sqrt( 1 / (1 + epsilon_stop * epsilon_stop) ) , 1 / ( ( __fx64 ) L ) );
        }

        // output gain:
        glp[N] = 1;

        // generate output
        filter_data<__type> data;
        data.poles = plp;
        data.zeros = zlp;
        data.ratio = glp;
        data.L   = L;
        data.R   = R;
        data.N   = N;

        return data;
    }

    /*!
     * \brief Butterworth or Chebyshev I digital lowpass filter coefficients computation function
     * \param[_Fs] sampling frequency , Hz
     * \param[_Fc] cut-off frequency , Hz
     * \param[_order] filter order
     * \param[_type] filter type ( 0 - Butterworth , 1 - Chebyshev_I )
     * \param[_g_stop] stopband attenuation , Db
     * \return   The function computes Butterworth or Chebyshev I digital lowpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */
    template<typename __type>
    filter_data<__type> __butt_cheb1_digital_lp__( __fx64 _Fs, __fx64 _Fc, __ix32 _order, __ix32 _type   = 0, __fx64 _g_stop = 1 ) // refactored
    {
        // COMPUTE LOWPASS ANALOGUE PROTOTYPE ZEROS, POLES AND GAINS:
        filter_data<__type> data = ( !_type ) ? __butt_zeros_poles_plain__<__type>( _order , _g_stop ) : __cheb1_zeros_poles_plain__<__type>( _order , _g_stop );

        // allocate zeros and poles arrays:
        Complex< __fx64 > *plp = data.poles;
        Complex< __fx64 > *zlp = data.zeros;
        Complex< __fx64 > *glp = data.ratio;
        __ix32 L = data.L;
        __ix32 R = data.R;
        __ix32 N = data.N;

        // frequency deformation coefficient:
        __fx64 K = tan( PI2 * _Fc / 2 / _Fs );

        // coefficients matrix computation:
        __type *cfnum = __alloc__<__type>(3*N);
        __type *cfden = __alloc__<__type>(3*N);
        __type *gains = __alloc__<__type>(N+1);

        if( !cfnum || !cfden || !gains || !plp || !zlp || !glp )
        {
            #ifdef IIR_KERNEL_DEBUG
            Debugger::Log("kernel_iir.h", "__butt_cheb1_digital_lp__", "memory allocation has failed !!!");
            #endif

            cfnum = __mfree__(cfnum);
            cfden = __mfree__(cfden);
            gains = __mfree__(gains);
            plp   = __mfree__(plp);
            zlp   = __mfree__(zlp);
            glp   = __mfree__(glp);
            return __dsp_gen_empty_filter__<__type>();
        }

        // BILLINEAR LP-LP TRANSFORM:

        // fcomplex-conjugate pairs:
        for( __ix32 i = 0 ; i < L ; i++ )
        {
            // quadratic section gain:
            Complex< __fx64 > gain0 = glp[i];
            Complex< __fx64 > gain1 = Complex< __fx64 >( K * K , 0 );
            Complex< __fx64 > gain2 = ( Complex< __fx64 >( 1 , 0 ) - plp[i] * K ) * ( Complex< __fx64 >( 1 , 0 ) - __conjf__( plp[i] ) * K );
            gains[i] = __realf__( gain0 * gain1 / gain2 );

            // zeros and poles transformation:
            zlp[i] = Complex< __fx64 >( -1 , 0 );
            plp[i] = ( Complex< __fx64 >( 1 , 0 ) + plp[i] * K ) / ( Complex< __fx64 >( 1 , 0 ) - plp[i] * K );

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
            Complex< __fx64 > gain0 = glp[N-1];
            Complex< __fx64 > gain1 = Complex< __fx64 >( K , 0 );
            Complex< __fx64 > gain2 = ( Complex< __fx64 >( 1 , 0 ) - plp[N-1] * K );
            gains[N-1] = __realf__( gain0 * gain1 / gain2 );

            // zeros and poles transformation:
            zlp[ N - 1 ] = Complex< __fx64 >( -1 , 0 );
            plp[ N - 1 ] = ( Complex< __fx64 >( 1 , 0 ) + plp[N-1] * K ) / ( Complex< __fx64 >( 1 , 0 ) - plp[N-1] * K );

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

        // generate output
        data.poles = plp;
        data.zeros = zlp;
        data.ratio = glp;
        data.cfnum = cfnum;
        data.cfden = cfden;
        data.gains = gains;
        data.L     = L;
        data.R     = R;
        data.N     = N;
        data.Nx    = 3;
        data.Ny    = 3;

        return data;
    }

    /*!
     * \brief Butterworth or Chebyshev I digital highpass filter coefficients computation function
     * \param[_Fs] sampling frequency , Hz
     * \param[_Fp] pass frequency , Hz
     * \param[_order] filter order
     * \param[_type] filter type ( 0 - Butterworth , 1 - Chebyshev_I )
     * \param[_g_stop] stopband attenuation , Db
     * \return   The function computes Butterworth or Chebyshev I digital highpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */
    template<typename __type>
    filter_data<__type> __butt_cheb1_digital_hp__( __fx64 _Fs, __fx64 _Fp, __ix32 _order, __ix32 _type   = 0, __fx64 _g_stop = 1 ) // refactored
    {
        // INITIALIZATION:
        filter_data<__type> data = ( !_type ) ? __butt_zeros_poles_plain__<__type>( _order , _g_stop ) : __cheb1_zeros_poles_plain__<__type>( _order , _g_stop );

        Complex< __fx64 > *plp = data.poles;
        Complex< __fx64 > *zlp = data.zeros;
        Complex< __fx64 > *glp = data.ratio;
        __ix32 L = data.L;
        __ix32 R = data.R;
        __ix32 N = L + R;

        // frequency deformation coefficient:
        __fx64 w = tan( PI2 * _Fp / 2 / _Fs );

        // coefficients matrix computation:
        __type *cfnum = __alloc__<__type>(3*N);
        __type *cfden = __alloc__<__type>(3*N);
        __type *gains = __alloc__<__type>(N+1);

        if( !cfnum || !cfden || !gains || !plp || !zlp || !glp )
        {
            #ifdef IIR_KERNEL_DEBUG
            Debugger::Log("kernel_iir.h", "__butt_cheb1_digital_hp__", "memory allocation has failed !!!");
            #endif

            cfnum = __mfree__(cfnum);
            cfden = __mfree__(cfden);
            gains = __mfree__(gains);
            plp   = __mfree__(plp);
            zlp   = __mfree__(zlp);
            glp   = __mfree__(glp);
            return __dsp_gen_empty_filter__<__type>();
        }

        // BILLINEAR LP-HP TRANSFORM:

        // fcomplex conjugate pairs:
        for ( __ix32 i = 0; i < L; i++)
        {
            // gains compputation:
            Complex< __fx64 > gain0 = glp[i];
            Complex< __fx64 > gain1 = ( Complex< __fx64 >( 1 , 0 ) - plp[i] / (__fx64)w ) * ( Complex< __fx64 >( 1 , 0 ) - __conjf__( plp[i] ) / (__fx64)w );
            gains[i] = __realf__( gain0 / gain1 / (__fx64)w / (__fx64)w );

            // zeros and poles transformation:
            zlp[i] = Complex< __fx64 >( 1 , 0 );
            plp[i] = ( Complex< __fx64 >( 1 , 0 ) + plp[i] / (__fx64)w ) / ( Complex< __fx64 >( 1 , 0 ) - plp[i] / (__fx64)w ) * (__fx64)(-1);

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
            Complex< __fx64 > gain0 = glp[N-1];
            Complex< __fx64 > gain1 = ( Complex< __fx64 >( 1 , 0 ) - plp[N-1] / (__fx64)w );
            gains[N-1] = __realf__( gain0 / gain1 / (__fx64)w );

            // zeros and poles transformation:
            zlp[ N - 1 ] = Complex< __fx64 >( 1 , 0 );
            plp[ N - 1 ] = ( Complex< __fx64 >( 1 , 0 ) + plp[ N - 1 ] / (__fx64)w ) / ( Complex< __fx64 >( 1 , 0 ) - plp[ N - 1 ] / (__fx64)w ) * (__fx64)(-1);

            // digital highpass coefficients computation:

            // numerator:
            cfnum[3*(N-1)+0] = +1;
            cfnum[3*(N-1)+1] = -1;
            cfnum[3*(N-1)+2] = +0;

            // denominator:
            cfden[3*(N-1)+0] = 1;
            cfden[3*(N-1)+1] = -__realf__(plp[ N - 1 ]);
            cfden[3*(N-1)+2] = 0;
        }

        // setting filter output gain:
        gains[N] = __realf__(glp[N]);

        // generate output
        data.poles = plp;
        data.zeros = zlp;
        data.ratio = glp;
        data.cfnum = cfnum;
        data.cfden = cfden;
        data.gains = gains;
        data.L     = L;
        data.R     = R;
        data.N     = N;
        data.Nx    = 3;
        data.Ny    = 3;

        return data;
    }

    /*!
     * \brief Butterworth or Chebyshev I digital bandpass filter coefficients computation function
     * \param[_Fs] sampling frequency , Hz
     * \param[_Fp] pass frequency , Hz
     * \param[_BandWidth] passband width , Hz
     * \param[_order] filter order
     * \param[_type] filter type ( 0 - Butterworth , 1 - Chebyshev_I )
     * \param[_g_stop] stopband attenuation , Db
     * \return   The function computes Butterworth or Chebyshev I digital bandpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */
    template<typename __type>
    filter_data<__type> __butt_cheb1_digital_bp__ ( __fx64 _Fs, __fx64 _Fp, __fx64 _BandWidth, __ix32 _order, __ix32 _type   = 0, __fx64 _g_stop = 1 ) // refactored
    {
        // INITIALIZATION
        if( _order % 2 > 0 ) _order++;
        _order /= 2;

        // frequency deformation coefficient:
        __fx64 w1 = tan( PI2 * _Fp / 2 / _Fs );
        __fx64 w2 = tan( PI2 * ( _Fp + _BandWidth ) / 2 / _Fs );

        // allocate zeros and poles arrays:

        // lowpass analogue prototype poles, zeros and gains:
        filter_data<__type> data = ( !_type ) ? __butt_zeros_poles_plain__<__type>( _order, _g_stop ) : __cheb1_zeros_poles_plain__<__type>( _order, _g_stop );
        Complex< __fx64 > *plp = data.poles;
        Complex< __fx64 > *glp = data.ratio;
        Complex< __fx64 > *zlp = data.zeros;
        __ix32 L = data.L;
        __ix32 R = data.R;
        __ix32 N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        Complex< __fx64 > *pbp = __alloc__< Complex< __fx64 > >(2*N);
        Complex< __fx64 > *zbp = __alloc__< Complex< __fx64 > >(2*N);
        Complex< __fx64 > *gbp = __alloc__< Complex< __fx64 > >(2*N);

        // coefficients matrix computation:
        __type *cfnum = __alloc__< __type >(3*(2*L+R));
        __type *cfden = __alloc__< __type >(3*(2*L+R));
        __type *gains = __alloc__< __type >((2*L+R+1));

        if( !cfnum || !cfden || !gains || !plp || !zlp || !glp || !pbp || !zbp || !gbp )
        {
            #ifdef IIR_KERNEL_DEBUG
            Debugger::Log("kernel_iir.h", "__butt_cheb1_digital_bp__", "memory allocation has failed !!!");
            #endif

            cfnum = __mfree__(cfnum);
            cfden = __mfree__(cfden);
            gains = __mfree__(gains);
            plp   = __mfree__(plp);
            zlp   = __mfree__(zlp);
            glp   = __mfree__(glp);
            pbp   = __mfree__(pbp);
            zbp   = __mfree__(zbp);
            gbp   = __mfree__(gbp);

            return __dsp_gen_empty_filter__<__type>();
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
            Complex< __fx64 > a( 1 / w0 , 0 );
            Complex< __fx64 > b( -__realf__(plp[i]) * dW / w0 , -__imagf__(plp[i]) * dW / w0 );
            Complex< __fx64 > c( 1 , 0 );
            Complex< __fx64 > D = b * b - a * c * (__fx64)4;
            Complex< __fx64 > p1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            Complex< __fx64 > p2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // zeros and poles bilinear transform:
            zbp[ j + 0 ] = Complex< __fx64 >( +1 , 0 );
            zbp[ j + 1 ] = Complex< __fx64 >( -1 , 0 );
            pbp[ j + 0 ] = ( Complex< __fx64 >( 1 , 0 ) + p1 ) / ( Complex< __fx64 >( 1 , 0 ) - p1 );
            pbp[ j + 1 ] = ( Complex< __fx64 >( 1 , 0 ) + p2 ) / ( Complex< __fx64 >( 1 , 0 ) - p2 );

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
            Complex< __fx64 > gain0 = glp[i];
            Complex< __fx64 > gain1 = p1 * __conjf__( p1 );
            Complex< __fx64 > gain2 = p2 * __conjf__( p2 );
            Complex< __fx64 > gain3 = (Complex< __fx64 >(1,0) - p1)*(Complex< __fx64 >(1,0) - __conjf__(p1));
            Complex< __fx64 > gain4 = (Complex< __fx64 >(1,0) - p2)*(Complex< __fx64 >(1,0) - __conjf__(p2));
            Complex< __fx64 > gain5 = gain0 * gain1 * gain2 / gain3 / gain4 * dW * dW / w0 / w0;
            gain5 = __csqrtf__( gain5 );
            gains[j+0] = __realf__(gain5);
            gains[j+1] = __realf__(gain5);
        }

        // real odd pole:
        if( R == 1 )
        {
            // pole transformation by means of square equation solve:
            Complex< __fx64 > a( 1 / w0 , 0 );
            Complex< __fx64 > b( -__realf__(plp[ N - 1 ]) * dW / w0 , -__realf__(plp[ N - 1 ]) * dW / w0 );
            Complex< __fx64 > c( 1 , 0 );
            Complex< __fx64 > D = b * b - a * c * (__fx64)4;
            Complex< __fx64 > p1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            Complex< __fx64 > p2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // zeros and poles bilinear transform:
            zbp[ j + 0 ] = Complex< __fx64 >( +1 , 0 );
            zbp[ j + 1 ] = Complex< __fx64 >( -1 , 0 );
            pbp[ j + 0 ] = ( Complex< __fx64 >( 1 , 0 ) + p1 ) / ( Complex< __fx64 >( 1 , 0 ) - p1 );
            pbp[ j + 1 ] = ( Complex< __fx64 >( 1 , 0 ) + p2 ) / ( Complex< __fx64 >( 1 , 0 ) - p2 );

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
            Complex< __fx64 > gain0 = glp[N-1];
            Complex< __fx64 > gain1 = p1 * p2;
            Complex< __fx64 > gain2 = (Complex< __fx64 >(1,0) - p1)*(Complex< __fx64 >(1,0) - p2);
            Complex< __fx64 > gain3 = gain0 * gain1 / gain2 * dW / w0;
            gains[j+0] = __realf__(gain3);
        }

        // setting filter output gain:
        gains[ 2*L+R ] = __realf__(glp[N]);

        // memory free
        plp = __mfree__( plp );
        zlp = __mfree__( zlp );
        glp = __mfree__( glp );

        // generate output
        data.poles = pbp;
        data.zeros = zbp;
        data.ratio = gbp;
        data.cfnum = cfnum;
        data.cfden = cfden;
        data.gains = gains;
        data.L     = 2*L;
        data.R     = R;
        data.N     = 2*L+R;
        data.Nx    = 3;
        data.Ny    = 3;

        return data;
    }

    /*!
     * \brief Butterworth or Chebyshev I digital bandstop filter coefficients computation function
     * \param[_Fs] sampling frequency , Hz
     * \param[_Fc] cut-off frequency  , Hz
     * \param[_BandWidth] cut-off bandwidth  , Hz
     * \param[_order] filter order
     * \param[_type] filter type ( 0 - Butterworth , 1 - Chebyshev_I )
     * \param[_g_stop] stopband attenuation , Db
     * \return   The function computes Butterworth or Chebyshev I digital bandstop filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */
    template<typename __type>
    filter_data<__type> __butt_cheb1_digital_bs__( __fx64 _Fs, __fx64 _Fc, __fx64 _BandWidth, __ix32 _order, __fx32 _type   = 0, __fx64 _g_stop = 1 ) // refactored
    {
       // INITIALIZATION:
       if( _order % 2 > 0 ) _order++;
       _order /= 2;

       // frequency deformation coefficient:
       __fx64 w1 = tan( PI2 * _Fc / 2 / _Fs );
       __fx64 w2 = tan( PI2 * ( _Fc + _BandWidth ) / 2 / _Fs );

       // allocate zeros and poles arrays:

       // lowpass analogue prototype poles, zeros and gains:
       filter_data<__type> data = ( !_type ) ? __butt_zeros_poles_plain__<__type>( _order, _g_stop ) : __cheb1_zeros_poles_plain__<__type>( _order, _g_stop );
       Complex< __fx64 > *plp = data.poles;
       Complex< __fx64 > *glp = data.ratio;
       Complex< __fx64 > *zlp = data.zeros;
       __ix32 L = data.L;
       __ix32 R = data.R;
       __ix32 N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        Complex< __fx64 > *pbs = __alloc__<Complex< __fx64 >>(2*N);
        Complex< __fx64 > *zbs = __alloc__<Complex< __fx64 >>(2*N);
        Complex< __fx64 > *gbs = __alloc__<Complex< __fx64 >>(2*N);

        // coefficients matrix computation:
        __type *cfnum = __alloc__< __type >(3*(2*L+R));
        __type *cfden = __alloc__< __type >(3*(2*L+R));
        __type *gains = __alloc__< __type >((2*L+R+1));

        if( !cfnum || !cfden || !gains || !plp || !zlp || !glp || !pbs || !zbs || !gbs)
        {
            #ifdef IIR_KERNEL_DEBUG
            Debugger::Log("kernel_iir.h", "__butt_cheb1_digital_bs__", "memory allocation has failed !!!");
            #endif

            cfnum = __mfree__(cfnum);
            cfden = __mfree__(cfden);
            gains = __mfree__(gains);
            plp   = __mfree__(plp);
            zlp   = __mfree__(zlp);
            glp   = __mfree__(glp);
            pbs   = __mfree__(pbs);
            pbs   = __mfree__(zbs);
            gbs   = __mfree__(gbs);

            return __dsp_gen_empty_filter__<__type>();
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
            Complex< __fx64 > a = Complex< __fx64 >( -1 / w0 , 0 );
            Complex< __fx64 > b = Complex< __fx64 >( dW , 0 ) / plp[i] / w0;
            Complex< __fx64 > c = Complex< __fx64 >( -1 , 0 );
            Complex< __fx64 > D = b * b - a * c * (__fx64)4;
            Complex< __fx64 > p1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            Complex< __fx64 > p2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // zeros and poles bilinear transform:
            zbs[ j + 0 ] = Complex< __fx64 >( +1 , 0 );
            zbs[ j + 1 ] = Complex< __fx64 >( -1 , 0 );
            pbs[ j + 0 ] = ( Complex< __fx64 >( 1 , 0 ) + p1 ) / ( Complex< __fx64 >( 1 , 0 ) - p1 );
            pbs[ j + 1 ] = ( Complex< __fx64 >( 1 , 0 ) + p2 ) / ( Complex< __fx64 >( 1 , 0 ) - p2 );

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
            Complex< __fx64 > gain0 = Complex< __fx64 >(1,0);
            Complex< __fx64 > gain1 = p1 * __conjf__( p1 );
            Complex< __fx64 > gain2 = p2 * __conjf__( p2 );
            Complex< __fx64 > gain3 = (Complex< __fx64 >(1,0) - p1)*(Complex< __fx64 >(1,0) - __conjf__(p1));
            Complex< __fx64 > gain4 = (Complex< __fx64 >(1,0) - p2)*(Complex< __fx64 >(1,0) - __conjf__(p2));
            Complex< __fx64 > gain5 = gain0 * gain1 * gain2 / gain3 / gain4 / w0 / w0 * ( 1 + w0 ) * ( 1 + w0 );
            gain5 = __csqrtf__( gain5 );
            gains[j+0] = __realf__(gain5);
            gains[j+1] = gains[j+0];
        }

        // real odd pole:
        if( R == 1 )
        {
            // poles transformation by means of square equation solve:
            Complex< __fx64 > a = Complex< __fx64 >( -1 / w0 , 0 );
            Complex< __fx64 > b = Complex< __fx64 >( dW , 0 ) / plp[N-1] / w0;
            Complex< __fx64 > c = Complex< __fx64 >( -1 , 0 );
            Complex< __fx64 > D = b * b - a * c * (__fx64)4;
            Complex< __fx64 > p1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            Complex< __fx64 > p2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // zeros and poles bilinear transform:
            zbs[ j + 0 ] = Complex< __fx64 >( +1 , 0 );
            zbs[ j + 1 ] = Complex< __fx64 >( -1 , 0 );
            pbs[ j + 0 ] = ( Complex< __fx64 >( 1 , 0 ) + p1 ) / ( Complex< __fx64 >( 1 , 0 ) - p1 );
            pbs[ j + 1 ] = ( Complex< __fx64 >( 1 , 0 ) + p2 ) / ( Complex< __fx64 >( 1 , 0 ) - p2 );

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
            Complex< __fx64 > gain0 = glp[N-1];
            Complex< __fx64 > gain1 = p1 * p2;
            Complex< __fx64 > gain2 = Complex< __fx64 >(1,0) / plp[N-1] / (__fx64)(-1);
            Complex< __fx64 > gain3 = (Complex< __fx64 >(1,0) - p1)*(Complex< __fx64 >(1,0) - p2);
            Complex< __fx64 > gain4 = gain0 * gain1 * gain2 / gain3 / w0 * ( 1 + w0 );
            gains[j+0] = __realf__(gain4);
        }

        // setting filter output gain:
        gains[ 2 * L + R ] = __realf__(glp[ N ]);

        // memory free
        plp   = __mfree__(plp);
        zlp   = __mfree__(zlp);
        glp   = __mfree__(glp);

        // generate output
        data.poles = pbs;
        data.zeros = zbs;
        data.ratio = gbs;
        data.cfnum = cfnum;
        data.cfden = cfden;
        data.gains = gains;
        data.L     = 2*L;
        data.R     = R;
        data.N     = 2*L+R;
        data.Nx    = 3;
        data.Ny    = 3;

        return data;
    }


    /*!
     * \brief Chebyshev II or Elliptic digital lowpass filter coefficients computation function
     * \param[_Fs] sampling frequency , Hz
     * \param[_Fc] cut-off frequency , Hz
     * \param[_order] filter order
     * \param[_type] filter type ( 0 - Chebyshev II , 1 - Elliptic )
     * \param[_g_stop] stopband attenuation , Db
     * \param[_g_pass] passband attenuation , Db
     * \return   The function computes Chebyshev II or Elliptic digital lowpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */
    template<typename __type>
    filter_data<__type> __cheb2_ellip_digital_lp__( __fx64 _Fs,  __fx64 _Fc, __ix32 _order, __ix32 _type   = 0, __fx64 _g_pass = 1, __fx64 _g_stop = 80 ) // refactored
    {
        __fx64 w = tan( PI2 * _Fc / 2 / _Fs );

        // digital lowpass coefficients computation:
        filter_data<__type> data = ( !_type ) ? __cheb2_zeros_poles_plain__<__type>( _order, _g_stop ) : __ellip_zeros_poles_plain__<__type>( _order , _g_pass , _g_stop );

        // zeros / poles plain initialization:
        Complex< __fx64 > *plp = data.poles;
        Complex< __fx64 > *zlp = data.zeros;
        Complex< __fx64 > *glp = data.ratio;

        // zeros/poles and coefficients number:
        __ix32 L = data.L;
        __ix32 R = data.R;
        __ix32 N = data.N;

        // coefficients matrix computation:
         __type *cfnum = __alloc__<__type>(3*N);
         __type *cfden = __alloc__<__type>(3*N);
         __type *gains = __alloc__<__type>(N+1);

        if( !cfnum || !cfden || !gains || !plp || !zlp || !glp )
        {
            #ifdef IIR_KERNEL_DEBUG
            Debugger::Log("kernel_iir.h", "__cheb2_ellip_digital_lp__", "memory allocation has failed !!!");
            #endif

            cfnum = __mfree__(cfnum);
            cfden = __mfree__(cfden);
            gains = __mfree__(gains);
            plp   = __mfree__(plp);
            zlp   = __mfree__(zlp);
            glp   = __mfree__(glp);

            return __dsp_gen_empty_filter__<__type>();
        }

        // fcomplex conjugate pairs:
        for ( __ix32 i = 0 ; i < L ; i++)
        {
            // quadratic sections gains computation:
            Complex< __fx64 > gain0 = glp[i];
            Complex< __fx64 > gain1 = ( Complex< __fx64 >( 1 , 0 ) - zlp[i] * w ) * ( Complex< __fx64 >( 1 , 0 ) - __conjf__( zlp[i] ) * w );
            Complex< __fx64 > gain2 = ( Complex< __fx64 >( 1 , 0 ) - plp[i] * w ) * ( Complex< __fx64 >( 1 , 0 ) - __conjf__( plp[i] ) * w );
            gains[ i ] = __realf__( gain0 * gain1 / gain2 );

            // zeros and poles transformation:
            zlp[i] = ( Complex< __fx64 >( 1 , 0 ) + zlp[i] * w ) / ( Complex< __fx64 >( 1 , 0 ) - zlp[i] * w );
            plp[i] = ( Complex< __fx64 >( 1 , 0 ) + plp[i] * w ) / ( Complex< __fx64 >( 1 , 0 ) - plp[i] * w );

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
            Complex< __fx64 > gain0 = glp[ N - 1 ];
            Complex< __fx64 > gain1 = Complex< __fx64 >( w , 0 );
            Complex< __fx64 > gain2 = ( Complex< __fx64 >( 1 , 0 ) - plp[ N - 1 ] * w );
            gains[ N - 1 ] = __realf__( gain0 * gain1 / gain2 );

            // zeros and poles transformation:
            zlp[ N - 1 ] = Complex< __fx64 >( -1 , 0 );
            plp[ N - 1 ] = ( Complex< __fx64 >( 1 , 0 ) + plp[ N - 1 ] * w ) / ( Complex< __fx64 >( 1 , 0 ) - plp[ N - 1 ] * w );

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

        // generate output
        data.cfnum = cfnum;
        data.cfden = cfden;
        data.gains = gains;
        data.L     = L;
        data.R     = R;
        data.N     = N;
        data.Nx    = 3;
        data.Ny    = 3;

        return data;
    }

    /*!
     * \brief Chebyshev II or Elliptic digital highpass filter coefficients computation function
     * \param[_Fs] sampling frequency , Hz
     * \param[_Fc] cut-off frequency , Hz
     * \param[_order] filter order
     * \param[_type] filter type ( 0 - Chebyshev II , 1 - Elliptic )
     * \param[_g_stop] stopband attenuation , Db
     * \param[_g_pass] passband attenuation , Db
     * \return   The function computes Chebyshev II or Elliptic digital lowpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */
    template<typename __type>
    filter_data<__type> __cheb2_ellip_digital_hp__( __fx64 _Fs, __fx64 _Fc, __ix32 _order, __ix32 _type   = 0, __fx64 _g_pass = 1, __fx64 _g_stop = 80 ) // refactored
    {
         __fx64  w = tan( PI2 * _Fc / 2 / _Fs );

        // digital lowpass coefficients computation:
        filter_data<__type> data = ( !_type ) ?__cheb2_zeros_poles_plain__<__type>(_order, _g_stop ) : __ellip_zeros_poles_plain__<__type>(_order, _g_pass, _g_stop );

        // zeros/poles and coefficients number:
        __ix32 L = data.L;
        __ix32 R = data.R;
        __ix32 N = data.N;

        // zeros / poles plain initialization:
        Complex< __fx64 > *plp = data.poles;
        Complex< __fx64 > *zlp = data.zeros;
        Complex< __fx64 > *glp = data.ratio;

        // coefficients matrix memory allocation:
         __type *cfnum = __alloc__<__type>(3*N);
         __type *cfden = __alloc__<__type>(3*N);
         __type *gains = __alloc__<__type>(N+1);

         if( !cfnum || !cfden || !gains || !plp || !zlp || !glp )
         {
            #ifdef IIR_KERNEL_DEBUG
            Debugger::Log("kernel_iir.h", "__cheb2_ellip_digital_hp__", "memory allocation has failed !!!");
            #endif

            cfnum = __mfree__(cfnum);
            cfden = __mfree__(cfden);
            gains = __mfree__(gains);
            plp   = __mfree__(plp);
            zlp   = __mfree__(zlp);
            glp   = __mfree__(glp);

             return __dsp_gen_empty_filter__<__type>();
         }

        // coefficients matrix computation:

        // fcomplex conjugate pairs:
        for ( __ix32 i = 0 ; i < L ; i++)
        {
            // quadratic sections gains computation:
            Complex< __fx64 > gain0 = glp[i];
            Complex< __fx64 > gain1 = ( Complex< __fx64 >( 1 , 0 ) - zlp[i] / w ) * ( Complex< __fx64 >( 1 , 0 ) - __conjf__( zlp[i] ) / w );
            Complex< __fx64 > gain2 = ( Complex< __fx64 >( 1 , 0 ) - plp[i] / w ) * ( Complex< __fx64 >( 1 , 0 ) - __conjf__( plp[i] ) / w );
            gains[ i ] = __realf__( gain0 * gain1 / gain2 );

            // zeros and poles transformation:
            zlp[i] = ( Complex< __fx64 >( 1 , 0 ) + zlp[i] / w ) / ( Complex< __fx64 >( 1 , 0 ) - zlp[i] / w );
            plp[i] = ( Complex< __fx64 >( 1 , 0 ) + plp[i] / w ) / ( Complex< __fx64 >( 1 , 0 ) - plp[i] / w );

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
            Complex< __fx64 > gain0 = glp[ N - 1 ];
            Complex< __fx64 > gain1 = Complex< __fx64 >( 1 / w , 0 );
            Complex< __fx64 > gain2 = ( Complex< __fx64 >( 1 , 0 ) - plp[ N - 1 ] / w );
            gains[ N - 1 ] = __realf__( gain0 * gain1 / gain2 );

            // zeros and poles transformation:
            zlp[ N - 1 ] = Complex< __fx64 >( -1 , 0 );
            plp[ N - 1 ] = ( Complex< __fx64 >( 1 , 0 ) + plp[ N - 1 ] / w ) / ( Complex< __fx64 >( 1 , 0 ) - plp[ N - 1 ] / w );

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

        // generate output
        data.cfnum = cfnum;
        data.cfden = cfden;
        data.gains = gains;
        data.L     = L;
        data.R     = R;
        data.N     = N;
        data.Nx    = 3;
        data.Ny    = 3;

        return data;
    }

    /*!
     * \brief Chebyshev II or Elliptic digital bandpass filter coefficients computation function
     * \param[_Fs] sampling frequency , Hz
     * \param[_Fp] pass frequency     , Hz
     * \param[_BandWidth] passband width     , Hz
     * \param[_order] filter order
     * \param[_type] filter type ( 0 - Chebyshev II , 1 - Elliptic )
     * \param[_g_stop] stopband attenuation , Db
     * \param[_g_pass] passband attenuation , Db
     * \return   The function computes Chebyshev II or Elliptic digital bandpass filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */
    template<typename __type>
    filter_data<__type> __cheb2_ellip_digital_bp__ ( __fx64 _Fs, __fx64 _Fp, __fx64 _BandWidth, __ix32 _order, __ix32 _type   = 0, __fx64 _g_pass = 1, __fx64 _g_stop = 80 ) // refactored
    {
        // INITIALIZATION
        if( _order % 2 > 0 ) _order++;
        _order /= 2;

        // frequency deformation coefficient:
        __fx64 w1 = tan( PI2 * _Fp / 2 / _Fs ) , w2 = tan( PI2 * ( _Fp + _BandWidth ) / 2 / _Fs );

        // allocate zeros and poles arrays:

        // lowpass analogue prototype poles, zeros and gains:
        filter_data<__type> data = ( !_type ) ?__cheb2_zeros_poles_plain__<__type>(_order, _g_stop ) :__ellip_zeros_poles_plain__<__type>(_order, _g_pass, _g_stop );
        Complex< __fx64 > *plp = data.poles;
        Complex< __fx64 > *glp = data.ratio;
        Complex< __fx64 > *zlp = data.zeros;
        __ix32 L = data.L;
        __ix32 R = data.R;
        __ix32 N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        Complex< __fx64 > *pbp = __alloc__< Complex< __fx64 > >(2*N);
        Complex< __fx64 > *zbp = __alloc__< Complex< __fx64 > >(2*N);
        Complex< __fx64 > *gbp = __alloc__< Complex< __fx64 > >(2*N);

        // coefficients matrix computation:
        __type *cfnum = __alloc__<__type>( 3*(2*L+R) );
        __type *cfden = __alloc__<__type>( 3*(2*L+R) );
        __type *gains = __alloc__<__type>( (2*L+R+1) );

        if( !cfnum || !cfden || !gains || !plp || !zlp || !glp || !pbp || !zbp || !gbp)
        {
            #ifdef IIR_KERNEL_DEBUG
            Debugger::Log("kernel_iir.h", "__cheb2_ellip_digital_bp__", "memory allocation has failed !!!");
            #endif

            cfnum = __mfree__(cfnum);
            cfden = __mfree__(cfden);
            gains = __mfree__(gains);
            plp   = __mfree__(plp);
            zlp   = __mfree__(zlp);
            glp   = __mfree__(glp);
            pbp   = __mfree__(pbp);
            zbp   = __mfree__(zbp);
            gbp   = __mfree__(gbp);

            return __dsp_gen_empty_filter__<__type>();
        }

        // LP-BP BILLINEAR TRANSFORM:

        // poles transformation:
        __fx64 w0 = w1 * w2;
        __fx64 dW = w2 - w1;

        // fcomplex conjugate pairs:
        __ix32 j = 0;

        // auxiliary variables:
        Complex< __fx64 > a , b , c , D , z1 , z2 , p1 , p2;

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
            zbp[ j + 0 ] = ( Complex< __fx64 >( 1 , 0 ) + z1 ) / ( Complex< __fx64 >( 1 , 0 ) - z1 );
            zbp[ j + 1 ] = ( Complex< __fx64 >( 1 , 0 ) + z2 ) / ( Complex< __fx64 >( 1 , 0 ) - z2 );
            pbp[ j + 0 ] = ( Complex< __fx64 >( 1 , 0 ) + p1 ) / ( Complex< __fx64 >( 1 , 0 ) - p1 );
            pbp[ j + 1 ] = ( Complex< __fx64 >( 1 , 0 ) + p2 ) / ( Complex< __fx64 >( 1 , 0 ) - p2 );

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
            Complex< __fx64 > gain0 = glp[i];
            Complex< __fx64 > gain1 = p1 * __conjf__( p1 );
            Complex< __fx64 > gain2 = p2 * __conjf__( p2 );
            Complex< __fx64 > gain3 = z1 * __conjf__( z1 );
            Complex< __fx64 > gain4 = z2 * __conjf__( z2 );
            Complex< __fx64 > gain5 = ( Complex< __fx64 >(1,0) - z1)*(Complex< __fx64 >(1,0) - __conjf__(z1) );
            Complex< __fx64 > gain6 = ( Complex< __fx64 >(1,0) - z2)*(Complex< __fx64 >(1,0) - __conjf__(z2) );
            Complex< __fx64 > gain7 = ( Complex< __fx64 >(1,0) - p1)*(Complex< __fx64 >(1,0) - __conjf__(p1) );
            Complex< __fx64 > gain8 = ( Complex< __fx64 >(1,0) - p2)*(Complex< __fx64 >(1,0) - __conjf__(p2) );

            Complex< __fx64 > gain9 = gain0 * gain1 * gain2 / ( gain3 * gain4 ) * ( gain5 * gain6 ) / ( gain7 * gain8 );
            gain9 = __csqrtf__( gain9 );
            gains[j+0] = __realf__(gain9);
            gains[j+1] = gains[j+0];
        }

        // real odd pole:
        if( R == 1 )
        {
            // pole transformation by means of square equation solve:
            Complex< __fx64 > a( 1 / w0 , 0 );
            Complex< __fx64 > b( -__realf__(plp[ N - 1 ]) * dW / w0 , -__realf__(plp[ N - 1 ]) * dW / w0 );
            Complex< __fx64 > c( 1 , 0 );
            Complex< __fx64 > D = b * b - a * c * (__fx64)4;
            Complex< __fx64 > p1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            Complex< __fx64 > p2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // zeros and poles bilinear transform:
            zbp[ j + 0 ] = Complex< __fx64 >( +1 , 0 );
            zbp[ j + 1 ] = Complex< __fx64 >( -1 , 0 );
            pbp[ j + 0 ] = ( Complex< __fx64 >( 1 , 0 ) + p1 ) / ( Complex< __fx64 >( 1 , 0 ) - p1 );
            pbp[ j + 1 ] = ( Complex< __fx64 >( 1 , 0 ) + p2 ) / ( Complex< __fx64 >( 1 , 0 ) - p2 );

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
            Complex< __fx64 > gain0 = glp[N-1];
            Complex< __fx64 > gain1 = p1 * p2;
            Complex< __fx64 > gain2 = (Complex< __fx64 >(1,0) - p1)*(Complex< __fx64 >(1,0) - p2);
            Complex< __fx64 > gain3 = gain0 * gain1 / gain2 * dW / w0;
            gains[j+0] = __realf__(gain3);
        }

        // setting filter output gain:
        gains[ 2*L+R ] = __realf__(glp[N]);

        // memory free:
        plp = __mfree__(plp);
        zlp = __mfree__(zlp);
        glp = __mfree__(glp);

        // generate output
        data.poles   = pbp;
        data.zeros   = zbp;
        data.ratio   = gbp;
        data.cfnum = cfnum;
        data.cfden = cfden;
        data.gains = gains;
        data.L     = 2*L;
        data.R     = R;
        data.N     = 2*L+R;
        data.Nx    = 3;
        data.Ny    = 3;

        return data;
    }

    /*!
     * \brief Chebyshev II or Elliptic digital bandstop filter coefficients computation function
     * \param[_Fs] sampling frequency , Hz
     * \param[_Fc] cut-off frequency  , Hz
     * \param[_BandWidth] stopband width     , Hz
     * \param[_order] filter order
     * \param[_type] filter type ( 0 - Chebyshev II , 1 - Elliptic )
     * \param[_g_stop] stopband attenuation , Db
     * \param[_g_pass] passband attenuation , Db
     * \return   The function computes Chebyshev II or Elliptic digital bandstop filter coefficients that are represented
     *           in the way of second order sections and their gains. All the data is stored within cf data structure
     *           and returned.
    */
    template<typename __type>
    filter_data<__type> __cheb2_ellip_digital_bs__( __fx64 _Fs, __fx64 _Fc, __fx64 _BandWidth, __ix32 _order, __ix32 _type   = 0, __fx64 _g_pass = 1, __fx64 _g_stop = 80 ) // refactored
    {
        // INITIALIZATION
        if( _order % 2 > 0 ) _order++;
        _order /= 2;

        // frequency deformation coefficient:
        __fx64 w1 = tan( PI2 * _Fc / 2 / _Fs ) , w2 = tan( PI2 * (_Fc + _BandWidth ) / 2 / _Fs );

        // allocate zeros and poles arrays:

        // lowpass analogue prototype poles, zeros and gains:
        filter_data<__type> data = ( !_type ) ?__cheb2_zeros_poles_plain__<__type>(_order, _g_stop ) :__ellip_zeros_poles_plain__<__type>(_order, _g_pass, _g_stop );

        Complex< __fx64 > *plp = data.poles;
        Complex< __fx64 > *glp = data.ratio;
        Complex< __fx64 > *zlp = data.zeros;
        __ix32 L = data.L , R = data.R , N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        Complex< __fx64 > *pbs = __alloc__< Complex< __fx64 > >(2*N);
        Complex< __fx64 > *zbs = __alloc__< Complex< __fx64 > >(2*N);
        Complex< __fx64 > *gbs = __alloc__< Complex< __fx64 > >(2*N);

        // coefficients matrix computation:
         __type *cfnum = __alloc__< __type >(3*(2*L+R));
         __type *cfden = __alloc__< __type >(3*(2*L+R));
         __type *gains = __alloc__< __type >((2*L+R+1));

         if( !cfnum || !cfden || !gains || !plp || !zlp || !glp || !pbs || !zbs || !gbs)
         {
            #ifdef IIR_KERNEL_DEBUG
            Debugger::Log("kernel_iir.h", "__cheb2_ellip_digital_bs__", "memory allocation has failed !!!");
            #endif

             cfnum = __mfree__(cfnum);
             cfden = __mfree__(cfden);
             gains = __mfree__(gains);
             plp   = __mfree__(plp);
             zlp   = __mfree__(zlp);
             glp   = __mfree__(glp);
             pbs   = __mfree__(pbs);
             zbs   = __mfree__(zbs);
             gbs   = __mfree__(gbs);

             return __dsp_gen_empty_filter__<__type>();
         }

        // LP-BP BILLINEAR TRANSFORM:

        // poles transformation:
        __fx64  w0 = w1 * w2;
        __fx64  dW = w2 - w1;

        // fcomplex conjugate pairs:
        __ix32 j = 0;

        // auxiliary variables:
        Complex< __fx64 > a , b , c , D , z1 , z2 , p1 , p2;

        for( __ix32 i = 0 ; i < L ; i++ , j+=2 )
        {
            // zeros frequency transformation by means of square equation solve:
            a = -1 / w0;
            b = Complex< __fx64 >( 1 , 0 ) / zlp[i] * dW / w0;
            c = -1;
            D = b * b - a * c * (__fx64)4;
            z1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            z2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // poles frequency transformation by means of square equation solve:
            a = -1 / w0;
            b = Complex< __fx64 >( 1 , 0 ) / plp[i] * dW / w0;
            c = -1;
            D = b * b - a * c * (__fx64)4;
            p1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            p2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // zeros and poles bilinear transform:
            zbs[ j + 0 ] = ( Complex< __fx64 >( 1 , 0 ) + z1 ) / ( Complex< __fx64 >( 1 , 0 ) - z1 );
            zbs[ j + 1 ] = ( Complex< __fx64 >( 1 , 0 ) + z2 ) / ( Complex< __fx64 >( 1 , 0 ) - z2 );
            pbs[ j + 0 ] = ( Complex< __fx64 >( 1 , 0 ) + p1 ) / ( Complex< __fx64 >( 1 , 0 ) - p1 );
            pbs[ j + 1 ] = ( Complex< __fx64 >( 1 , 0 ) + p2 ) / ( Complex< __fx64 >( 1 , 0 ) - p2 );

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
            Complex< __fx64 > gain0 = p1 * __conjf__( p1 );
            Complex< __fx64 > gain1 = p2 * __conjf__( p2 );
            Complex< __fx64 > gain2 = z1 * __conjf__( z1 );
            Complex< __fx64 > gain3 = z2 * __conjf__( z2 );
            Complex< __fx64 > gain4 = ( Complex< __fx64 >(1,0) - z1)*(Complex< __fx64 >(1,0) - __conjf__(z1) );
            Complex< __fx64 > gain5 = ( Complex< __fx64 >(1,0) - z2)*(Complex< __fx64 >(1,0) - __conjf__(z2) );
            Complex< __fx64 > gain6 = ( Complex< __fx64 >(1,0) - p1)*(Complex< __fx64 >(1,0) - __conjf__(p1) );
            Complex< __fx64 > gain7 = ( Complex< __fx64 >(1,0) - p2)*(Complex< __fx64 >(1,0) - __conjf__(p2) );
            Complex< __fx64 > gain8 = ( gain0 * gain1 ) / ( gain2 * gain3 ) * ( gain4 * gain5 ) / ( gain6 * gain7 );
            gain8 = __csqrtf__( gain8 );
            gains[j+0] = __realf__(gain8);
            gains[j+1] = gains[j+0];
        }

        // real odd pole:
        if( R == 1 )
        {
            // poles transformation by means of square equation solve:
            Complex< __fx64 > a = Complex< __fx64 >( -1 / w0 , 0 );
            Complex< __fx64 > b = Complex< __fx64 >( dW , 0 ) / plp[N-1] / w0;
            Complex< __fx64 > c = Complex< __fx64 >( -1 , 0 );
            Complex< __fx64 > D = b * b - a * c * (__fx64)4;
            Complex< __fx64 > p1 = ( b*(__fx64)(-1) - __csqrtf__( D ) ) / (__fx64)2 / a;
            Complex< __fx64 > p2 = ( b*(__fx64)(-1) + __csqrtf__( D ) ) / (__fx64)2 / a;

            // zeros and poles bilinear transform:
            zbs[ j + 0 ] = Complex< __fx64 >( +1 , 0 );
            zbs[ j + 1 ] = Complex< __fx64 >( -1 , 0 );
            pbs[ j + 0 ] = ( Complex< __fx64 >( 1 , 0 ) + p1 ) / ( Complex< __fx64 >( 1 , 0 ) - p1 );
            pbs[ j + 1 ] = ( Complex< __fx64 >( 1 , 0 ) + p2 ) / ( Complex< __fx64 >( 1 , 0 ) - p2 );

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
            Complex< __fx64 > gain0 = glp[N-1];
            Complex< __fx64 > gain1 = p1 * p2;
            Complex< __fx64 > gain2 = Complex< __fx64 >(1,0) / plp[N-1] / (__fx64)(-1);
            Complex< __fx64 > gain3 = (Complex< __fx64 >(1,0) - p1)*(Complex< __fx64 >(1,0) - p2);
            Complex< __fx64 > gain4 = gain0 * gain1 * gain2 / gain3 / w0 * ( 1 + w0 );
            gains[j+0] = __realf__(gain4);
        }

        // setting filter output gain:
        gains[ 2*L+R ] = __realf__(glp[N]);

        // memory free:
        plp = __mfree__(plp);
        zlp = __mfree__(zlp);
        glp = __mfree__(glp);

        // generate output
        data.poles = pbs;
        data.zeros = zbs;
        data.ratio = gbs;
        data.cfnum = cfnum;
        data.cfden = cfden;
        data.gains = gains;
        data.L     = 2*L;
        data.R     = R;
        data.N     = 2*L+R;
        data.Nx    = 3;
        data.Ny    = 3;

        return data;
    }


    #ifndef __ALG_PLATFORM

    template<typename __type>
    void __show__( filter_data<__type> _data )
    {
        if( _data.cfnum && _data.cfden && _data.gains )
        {
            switch ( _data.type )
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

            __ix32  N     = _data.N;
            __type *cfnum = _data.cfnum;
            __type *cfden = _data.cfden;
            __type *gains = _data.gains;
            __type  fgain = 1;

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
            Debugger::Log("The filter has been destryed or it's coeffcients have not been computed yet...\n");
        }
    }

    #endif

    /*! @} */
}

#undef __fx32
#undef __fx64
#undef __fxx64
#undef __ix32

#endif // KERNEL_IIR_H
