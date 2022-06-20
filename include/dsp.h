#ifndef DSP_H
#define DSP_H

#ifndef __ALG_PLATFORM
#include "cmath"
#include "math.h"
#include <iostream>
#include <cstring>
#endif

#include "utils.h"
#include "buffer.h"
#include "fcomplex.h"
#include "special_functions.h"

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

namespace DSP_KERNEL
{
    /*!
     *  \struct bandwidth
     *  \brief  filter passband/stopband attenuation data structure
    */
    struct bandwidth
    {
        __fx64 Fc; ///< filter passband/stopband frequency, Hz
        __fx64 BW; ///< filter passband/stopband width, Hz
    };

    /*!
     *  \struct attenuation
     *  \brief filter passband/stopband attenuation data structure
    */
    struct attenuation
    {
        __fx64 G1; ///< filter passband attenuation, Db
        __fx64 G2; ///< filter stopband attenuation, Db
    };

    /*!
     *  \enum filter_type
     *  \brief filter type enumeration
    */
    enum filter_type
    {
        lowpass,  ///< lowpass  type filter
        highpass, ///< highpass type filter
        bandpass, ///< bandpass type filter
        bandstop, ///< bandstop type filter
        other     ///< other    type filter
    };

    /*!
     *  \brief base filter class
     *  \details defines the building base for the filtres creation
    */
    class filter_abstract
    {
    protected:

        // system variables
        __ix32 m_order;
        __fx64 m_Fs;
        __fx64 m_Ts;

        // default constructor
        filter_abstract(){}

        // virtual destructor
        virtual ~filter_abstract(){};

        // memory manipulation functions
        // do not call these function in the constructor
        // or destructor of the class that not marked as final !!!
        virtual __ix32 allocate()   = 0;
        virtual __ix32 deallocate() = 0;

        // frequency response  computation function
        virtual fcomplex<__fx64> frequency_response( __fx64 F ) = 0;
    };

    /*!
     *  \brief filter data structure
     *  \details contains all the filter information
     *           used for the analysis and modelling
    */
    template<typename __type>
    struct filter_data
    {
        filter_type        type = lowpass; ///< filter type
        fcomplex< __fx64 > *plp = nullptr; ///< lowpass normalized analogue prototype complex conjugate poles pairs
        fcomplex< __fx64 > *zlp = nullptr; ///< lowpass normalized analogue prototype complex conjugate zeros pairs
        fcomplex< __fx64 > *glp = nullptr; ///< lowpass normalized analogue prototype gains vector
        __type *cfnum           = nullptr; ///< IIR filter numerator quadratic sections coefficients matrix
        __type *cfden           = nullptr; ///< IIR filter denominator quadratic sections coefficients matrix
        __type *gains           = nullptr; ///< IIR filter quadratic sections gains vector
        __ix32 L                = -1;      ///< IIR filter complex conjugate poles/zeros pairs number
        __ix32 R                = -1;      ///< IIR filter real odd pole existance flag
        __ix32 N                = -1;      ///< IIR filter order
        __ix32 Nx               = -1;      ///< IIR single section numerator order
        __ix32 Ny               = -1;      ///< IIR single section denominator order
    };

    /*!
     *  \brief empty filter generation function
     *  \details generates empty filter data structure
    */
    template<typename __type>
    filter_data<__type> __dsp_gen_empty_filter__()
    {
        filter_data<__type> output;
        output.type  = lowpass;
        output.plp   = nullptr;
        output.zlp   = nullptr;
        output.glp   = nullptr;
        output.cfnum = nullptr;
        output.cfden = nullptr;
        output.gains = nullptr;
        output.L     = -1;
        output.R     = -1;
        output.N     = -1;
        output.Nx    = -1;
        output.Ny    = -1;
        return output;
    }

    /*!
     *  \brief filter clear function
     *  \param[data] input filter data structure
     *  \details clears filter data structure
    */
    template<typename __type>
    filter_data<__type> __dsp_clear_filter__(filter_data<__type> data)
    {
        data.type  = lowpass;
        data.plp   = __mfree__(data.plp);
        data.zlp   = __mfree__(data.zlp);
        data.glp   = __mfree__(data.glp);
        data.cfnum = __mfree__(data.cfnum);
        data.cfden = __mfree__(data.cfden);
        data.gains = __mfree__(data.gains);
        data.L     = -1;
        data.R     = -1;
        data.N     = -1;
        data.Nx    = -1;
        data.Ny    = -1;

        return data;
    }

}

namespace IIR_KERNEL
{
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
    template<typename __type> fcomplex<__fx64> __freq_resp__ (__type *_cfnum, __type *_cfden,  __type *_gains, __ix32 _Nx, __ix32 _Ny, __ix32 _N,  __fx64 _Fs,  __fx64 _F )
    {
        // check the input:
        if( !_cfnum || !_cfden || !_gains ) return { -1 , -1 };

        // sampling period:
        __fx64 Ts = 1 / _Fs;

        // transfer function initialization:
        fcomplex< __fx64 > Wz(1,0), num(0,0), den(0,0);

        for(__ix32 i = 0; i < _N; i++ )
        {
            // zerowing numerator and denominator sections sums:
            num(0,0);
            den(0,0);

            // numerator and denominator sums computation:
            for(__ix32 j = 0; j < _Nx; j++) num += fcomplex<__fx64>( cos( -PI2 * _F * Ts * j ) , sin( -PI2 * _F * Ts * j ) ) * (__fx64)_cfnum[_Nx * i + j];
            for(__ix32 j = 0; j < _Ny; j++) den += fcomplex<__fx64>( cos( -PI2 * _F * Ts * j ) , sin( -PI2 * _F * Ts * j ) ) * (__fx64)_cfden[_Ny * i + j];
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
    filter_data<__type> __butt_zeros_poles_plain__( __ix32 _order, __fx64 _g_stop )
    {
        // number of zeros, poles, coeffs:
        __ix32 L = trunc( _order / 2 ) , R = _order - 2 * L , N = L + R;

        // stopband attenuation:
        __fx64 epsilon_stop = sqrt(pow(10, _g_stop / 10) - 1);

        // memory allocation for the lowpass analogue prototype poles, zeros and gains:
        fcomplex<__fx64> *plp = ( fcomplex<__fx64>* ) calloc( N+0 , sizeof ( fcomplex<__fx64> ) );
        fcomplex<__fx64> *zlp = ( fcomplex<__fx64>* ) calloc( N+0 , sizeof ( fcomplex<__fx64> ) );
        fcomplex<__fx64> *glp = ( fcomplex<__fx64>* ) calloc( N+1 , sizeof ( fcomplex<__fx64> ) );

        if( !plp || !zlp || !glp )
        {
            free( plp );
            free( zlp );
            free( glp );
            return __dsp_gen_empty_filter__<__type>();
        }

        // fcomplex-conjugate pairs:
        __fx64 alpha = 0 , beta = 1 / sqrt( pow( epsilon_stop , 1 / _order ) );

        for( __ix32 i = 0 ; i < L ; i++ )
        {
            alpha = (2 * (i + 1) - 1) * PI0 / (2 * _order);
            plp[i] = fcomplex<__fx64>( ( -1 / beta * sin(alpha) ) , ( +1 / beta * cos(alpha) ) );
            glp[i] = plp[i] * __conjf__( plp[i] ) ;
        }

        // real odd pole:
        if( R == 1 )
        {
            plp[ N - 1 ] = fcomplex< __fx64 >( ( -1 / beta ) , 0 );
            glp[ N - 1 ] = -__realf__<__fx64>( plp[ N - 1 ] );
        }

        // setting the output gain:
        glp[ N ] = 1;

        // generate output
        filter_data<__type> data;
        data.plp = plp;
        data.zlp = zlp;
        data.glp = glp;
        data.L   = L;
        data.R   = R;
        data.N   = N;

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
    filter_data<__type> __cheb1_zeros_poles_plain__ ( __ix32 _order,  __fx64 _g_stop )
    {
        // number of zeros, poles, coeffs:
        __ix32 L = trunc( _order / 2 ) , R = _order - 2 * L , N = L + R;

        // stopband attenuation:
        __fx64 epsilon_stop = sqrt(pow(10, _g_stop / 10) - 1);

        // memory allocation for the lowpass analogue prototype poles, zeros and gains:
        fcomplex< __fx64 >  *plp = ( fcomplex<__fx64>* ) calloc( N   , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 >  *zlp = ( fcomplex<__fx64>* ) calloc( N   , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 >  *glp = ( fcomplex<__fx64>* ) calloc( N+1 , sizeof ( fcomplex< __fx64 > ) );

        if( plp == 0 || zlp == 0 || glp == 0 )
        {
            free( plp );
            free( zlp );
            free( glp );
            return __dsp_gen_empty_filter__<__type>();
        }

        // fcomplex conjugate pairs:
        __fx64 alpha = 0 , betta = asinh( 1 / epsilon_stop ) / _order;
        for ( __ix32 i = 0; i < L; i++)
        {
            alpha = (2 * (i + 1) - 1) * PI0 / (2 * _order);
            plp[i] = fcomplex< __fx64 >( -sin( alpha ) * sinh( betta ) , +cos( alpha ) * cosh( betta ) );
            glp[i] = plp[i] * __conjf__( plp[i] );
        }

        // real odd pole:
        if ( R == 1 )
        {
            plp[ N - 1 ] = fcomplex< __fx64 >( -sinh( betta ) , 0 );
            glp[ N - 1 ] = -__realf__<__fx64>( plp[ N - 1 ] );
        }

        // setting the output gain:
        glp[N] = ( R < 1 ) ? ( sqrt(1 / (1 + epsilon_stop * epsilon_stop) ) ) : 1;

        // generate output
        filter_data<__type> data;
        data.plp = plp;
        data.zlp = zlp;
        data.glp = glp;
        data.L   = L;
        data.R   = R;
        data.N   = N;

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
    filter_data<__type> __cheb2_zeros_poles_plain__( __ix32 _order, __fx64 _g_stop )
    {
        // stopband attenuation:
        __fx64 epsilon_stop = sqrt(pow(10, _g_stop / 10) - 1);

        // identify the number of zeros and poles:
        __ix32 L = trunc( _order / 2 ) , R = _order - 2 * L , N = L + R;

        // allocate zeros and poles arrays:
        fcomplex< __fx64 > *plp = ( fcomplex< __fx64 >* ) calloc( N+0 , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *zlp = ( fcomplex< __fx64 >* ) calloc( N+0 , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *glp = ( fcomplex< __fx64 >* ) calloc( N+1 , sizeof ( fcomplex< __fx64 > ) );

        if( !plp || !zlp || !glp )
        {
            free( plp );
            free( zlp );
            free( glp );
            return __dsp_gen_empty_filter__<__type>();
        }

        // ZEROS AND POLES COMPUTATION:

        // fcomplex conjugate pairs:

        // auxiliary variables:
        __fx64 alpha , betta = asinh( epsilon_stop ) / _order , re , im;

        // zeros , poles, gains computation:
        for ( __ix32 i = 0 ; i < L ; i++ )
        {
            alpha = (2 * ( i + 1 ) - 1) * PI0 / ( 2 * _order );

            // zeros:
            zlp[i] = fcomplex< __fx64 >( 0 , 1 / cos( alpha ) );

            // poles:
            re = -( sin( alpha ) * sinh( betta ) ) / ( cos( alpha ) * cos( alpha ) * cosh( betta ) * cosh( betta ) + sin( alpha ) * sin( alpha ) * sinh( betta ) * sinh( betta ) );
            im = +( cos( alpha ) * cosh( betta ) ) / ( cos( alpha ) * cos( alpha ) * cosh( betta ) * cosh( betta ) + sin( alpha ) * sin( alpha ) * sinh( betta ) * sinh( betta ) );
            plp[i] = fcomplex< __fx64 >( re , im );

            // gains:
            glp[i] = ( zlp[i] * __conjf__( zlp[i] ) ) / ( plp[i] * __conjf__( plp[i] ) );
            glp[i] = fcomplex< __fx64 >( 1 , 0 ) / glp[i];
        }

        // real odd pole:
        if( R >= 1 )
        {
            plp[ N - 1 ] = fcomplex< __fx64 >( -1 / sinh( betta ) , 0 );
            glp[ N - 1 ] = -__realf__<__fx64>( plp[ N - 1 ] );
        }

        // set the output gain:
        glp[N] = 1;

        // generate output
        filter_data<__type> data;
        data.plp = plp;
        data.zlp = zlp;
        data.glp = glp;
        data.L   = L;
        data.R   = R;
        data.N   = N;

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
    filter_data<__type> __ellip_zeros_poles_plain__( __ix32 _order, __fx64 _g_pass, __fx64 _g_stop )
    {
        // INITIALIZATION:

        // stopband and passband attenuation:
        __fx64 epsilon_stop = sqrt(pow(10, _g_stop / 10) - 1);
        __fx64 epsilon_pass = sqrt(pow(10, _g_pass / 10) - 1);

        // identify the number of zeros and poles:
        __ix32 L = trunc( _order / 2 ) , R = _order - 2 * L , N = L + R;

        // allocate zeros and poles arrays:
        fcomplex< __fx64 > *plp = ( fcomplex< __fx64 >* ) calloc( N   , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *zlp = ( fcomplex< __fx64 >* ) calloc( N   , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *glp = ( fcomplex< __fx64 >* ) calloc( N+1 , sizeof ( fcomplex< __fx64 > ) );

        if( !plp || !zlp || !glp )
        {
            free( plp );
            free( zlp );
            free( glp );
            return __dsp_gen_empty_filter__<__type>();
        }

        // TRANSIENT SUPPRESSION FACTORS COMPUTATION:

        // auxiliary variables:
        __fx64 SN , KE;
        __fx64 Ke = epsilon_pass / epsilon_stop , Kp = 1;
        __fx64 m = sqrt( 1 - Ke * Ke ) , alpha , Kw;

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
                zlp[i] = fcomplex< __fx64 >( 0 , 1 /( Kw * __sn__( alpha , Kw ) ) );
            }
        }
        else if( _order % 2 != 0 ) // odd order filter
        {
            for ( __ix32 i = 0; i < L; i++)
            {
                alpha  = (2 * i + 2) / ( ( __fx64 )_order) * KE;
                zlp[i] = fcomplex< __fx64 >( 0 , 1 /( Kw * __sn__( alpha , Kw ) ) );
            }
        }

        // POLES COMPUTATION:
        __fx64 V0 = 0 , A = 0 , B = 0 , C = 0 , D = 0 , E = 0 , F = 0;
        V0        = -__ellip_k__( Kw ) * __isc__(1 / epsilon_pass, sqrt(1 - Ke * Ke)) / ( __ellip_k__( Ke ) * ( ( __fx64 )_order ) );
        KE        =  __ellip_k__( Kw );

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
            plp[ N - 1 ] = fcomplex< __fx64 >( A * B / ( 1 - C )  , 0 );
        }

        // GAINS COMPUTATION:
        if( R >= 1 )
        {
            // fcomplex conjugate pairs:
            for ( __ix32 i = 0; i < L; i++)
            {
                glp[i] = ( zlp[i] * __conjf__( zlp[i] ) ) / ( plp[i] * __conjf__( plp[i] ) );
                glp[i] = fcomplex< __fx64 >( 1 , 0 ) / glp[i];
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
        data.plp = plp;
        data.zlp = zlp;
        data.glp = glp;
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
    filter_data<__type> __butt_cheb1_digital_lp__ ( __fx64 _Fs, __fx64 _Fc, __ix32 _order, __ix32 _type   = 0, __fx64 _g_stop = 1 )
    {
        // COMPUTE LOWPASS ANALOGUE PROTOTYPE ZEROS, POLES AND GAINS:
        filter_data<__type> data = ( !_type ) ? __butt_zeros_poles_plain__<__type>( _order , _g_stop ) : __cheb1_zeros_poles_plain__<__type>( _order , _g_stop );

        // allocate zeros and poles arrays:
        fcomplex< __fx64 > *plp = data.plp;
        fcomplex< __fx64 > *zlp = data.zlp;
        fcomplex< __fx64 > *glp = data.glp;
        __ix32 L = data.L , R = data.R , N = L + R;

        // frequency deformation coefficient:
        __fx64 K = tan( PI2 * _Fc / 2 / _Fs );

        // coefficients matrix computation:
        __type *cfnum = ( __type* )calloc( 3 * N , sizeof ( __type ) );
        __type *cfden = ( __type* )calloc( 3 * N , sizeof ( __type ) );
        __type *gains = ( __type* )calloc( N + 1 , sizeof ( __type ) );

        if( cfnum == 0 || cfden == 0 || gains == 0 || plp == 0 || zlp == 0 || glp == 0 )
        {
            free( cfnum );
            free( cfden );
            free( gains );
            free( plp );
            free( zlp );
            free( glp );
            return __dsp_gen_empty_filter__<__type>();
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

        // generate output
        data.plp   = plp;
        data.zlp   = zlp;
        data.glp   = glp;
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
    filter_data<__type> __butt_cheb1_digital_hp__ ( __fx64 _Fs, __fx64 _Fp, __ix32 _order, __ix32 _type   = 0, __fx64 _g_stop = 1 )
    {
        // INITIALIZATION:
        filter_data<__type> data = ( !_type ) ? __butt_zeros_poles_plain__<__type>( _order , _g_stop ) : __cheb1_zeros_poles_plain__<__type>( _order , _g_stop );

        fcomplex< __fx64 > *plp = data.plp;
        fcomplex< __fx64 > *zlp = data.zlp;
        fcomplex< __fx64 > *glp = data.glp;
        __ix32 L = data.L , R = data.R , N = L + R;

        // frequency deformation coefficient:
        __fx64 w = tan( PI2 * _Fp / 2 / _Fs );

        // coefficients matrix computation:
        __type *cfnum = ( __type* )calloc( 3 * N , sizeof ( __type ) );
        __type *cfden = ( __type* )calloc( 3 * N , sizeof ( __type ) );
        __type *gains = ( __type* )calloc( N + 1 , sizeof ( __type ) );

        if( cfnum == 0 || cfden == 0 || gains == 0 || plp == 0 || zlp == 0 || glp == 0 )
        {
            free( cfnum );
            free( cfden );
            free( gains );
            free( plp )  ;
            free( zlp )  ;
            free( glp );
            return __dsp_gen_empty_filter__<__type>();
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

        // generate output
        data.plp   = plp;
        data.zlp   = zlp;
        data.glp   = glp;
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
    filter_data<__type> __butt_cheb1_digital_bp__ ( __fx64 _Fs, __fx64 _Fp, __fx64 _BandWidth, __ix32 _order, __ix32 _type   = 0, __fx64 _g_stop = 1 )
    {
        _order /= 2;

        // frequency deformation coefficient:
        __fx64 w1 = tan( PI2 * _Fp / 2 / _Fs ) , w2 = tan( PI2 * ( _Fp + _BandWidth ) / 2 / _Fs );

        // allocate zeros and poles arrays:

        // lowpass analogue prototype poles, zeros and gains:
        filter_data<__type> data = ( !_type ) ? __butt_zeros_poles_plain__<__type>( _order, _g_stop ) : __cheb1_zeros_poles_plain__<__type>( _order, _g_stop );

        fcomplex< __fx64 > *plp = data.plp;
        fcomplex< __fx64 > *glp = data.glp;
        fcomplex< __fx64 > *zlp = data.zlp;
        __ix32 L = data.L;
        __ix32 R = data.R;
        __ix32 N = L + R;

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
            free( cfnum );
            free( cfden );
            free( gains );
            free( plp )  ;
            free( zlp )  ;
            free( glp );
            free( pbp )  ;
            free( zbp )  ;
            free( gbp );
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

        // memory free
        free( plp );
        free( zlp );
        free( glp );

        // generate output
        data.plp   = pbp;
        data.zlp   = zbp;
        data.glp   = gbp;
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
    filter_data<__type> __butt_cheb1_digital_bs__( __fx64 _Fs, __fx64 _Fc, __fx64 _BandWidth, __ix32 _order, __fx32 _type   = 0, __fx64 _g_stop = 1 )
    {

       // INITIALIZATION:
       _order /= 2;

       // frequency deformation coefficient:
       __fx64 w1 = tan( PI2 * _Fc / 2 / _Fs ) , w2 = tan( PI2 * ( _Fc + _BandWidth ) / 2 / _Fs );

       // allocate zeros and poles arrays:

       // lowpass analogue prototype poles, zeros and gains:
       filter_data<__type> data = ( !_type ) ? __butt_zeros_poles_plain__<__type>( _order, _g_stop ) : __cheb1_zeros_poles_plain__<__type>( _order, _g_stop );

       fcomplex< __fx64 > *plp = data.plp;
       fcomplex< __fx64 > *glp = data.glp;
       fcomplex< __fx64 > *zlp = data.zlp;
       __ix32 L = data.L;
       __ix32 R = data.R;
       __ix32 N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        fcomplex< __fx64 > *pbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *zbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *gbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );

        // coefficients matrix computation:
        __type *cfnum = (__type* )calloc( 3 * (2*L+R) , sizeof ( __type ) );
        __type *cfden = (__type* )calloc( 3 * (2*L+R) , sizeof ( __type ) );
        __type *gains = (__type* )calloc( (2*L+R+1)   , sizeof ( __type ) );

        if( !cfnum || !cfden || !gains || !plp || !zlp || !glp || !pbs || !zbs || !gbs)
        {
            free( cfnum );
            free( cfden );
            free( gains );
            free( plp );
            free( zlp );
            free( glp );
            free( pbs );
            free( zbs );
            free( gbs );
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

        // memory free
        free( plp );
        free( zlp );
        free( glp );

        // generate output
        data.plp   = pbs;
        data.zlp   = zbs;
        data.glp   = gbs;
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
    filter_data<__type> __cheb2_ellip_digital_lp__ ( __fx64 _Fs,  __fx64 _Fc, __ix32 _order, __ix32 _type   = 0, __fx64 _g_pass = 1, __fx64 _g_stop = 80 )
    {
        __fx64 w = tan( PI2 * _Fc / 2 / _Fs );

        // digital lowpass coefficients computation:
        filter_data<__type> data = ( !_type ) ? __cheb2_zeros_poles_plain__<__type>( _order, _g_stop ) : __ellip_zeros_poles_plain__<__type>( _order , _g_pass , _g_stop );

        // zeros / poles plain initialization:
        fcomplex< __fx64 > *plp = data.plp;
        fcomplex< __fx64 > *zlp = data.zlp;
        fcomplex< __fx64 > *glp = data.glp;

        // zeros/poles and coefficients number:
        __ix32 L = data.L;
        __ix32 R = data.R;
        __ix32 N = data.N;

        // coefficients matrix computation:
         __type *cfnum = ( __type* )calloc( 3 * N , sizeof ( __type ) );
         __type *cfden = ( __type* )calloc( 3 * N , sizeof ( __type ) );
         __type *gains = ( __type* )calloc( N + 1 , sizeof ( __type ) );

        if( !cfnum || !cfden || !gains || !plp || !zlp || !glp )
        {
            free( cfnum );
            free( cfden );
            free( gains );
            free( plp );
            free( zlp );
            free( glp );
            return __dsp_gen_empty_filter__<__type>();
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
    filter_data<__type> __cheb2_ellip_digital_hp__ ( __fx64 _Fs, __fx64 _Fc, __ix32 _order, __ix32 _type   = 0, __fx64 _g_pass = 1, __fx64 _g_stop = 80 )
    {
         __fx64  w = tan( PI2 * _Fc / 2 / _Fs );

        // digital lowpass coefficients computation:
        filter_data<__type> data = ( !_type ) ?__cheb2_zeros_poles_plain__<__type>(_order, _g_stop ) :__ellip_zeros_poles_plain__<__type>(_order, _g_pass, _g_stop );

        // zeros/poles and coefficients number:
        __ix32 L = data.L;
        __ix32 R = data.R;
        __ix32 N = data.N;

        // zeros / poles plain initialization:
        fcomplex< __fx64 > *plp = data.plp;
        fcomplex< __fx64 > *zlp = data.zlp;
        fcomplex< __fx64 > *glp = data.glp;

        // coefficients matrix memory allocation:
         __type *cfnum = (__type* )calloc( 3 * N , sizeof ( __type ) );
         __type *cfden = (__type* )calloc( 3 * N , sizeof ( __type ) );
         __type *gains = (__type* )calloc( N + 1 , sizeof ( __type ) );

         if( !cfnum || !cfden || !gains || !plp || !zlp || !glp )
         {
             free( cfnum );
             free( cfden );
             free( gains );
             free( plp )  ;
             free( zlp )  ;
             free( glp );
             return __dsp_gen_empty_filter__<__type>();
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
    filter_data<__type> __cheb2_ellip_digital_bp__ ( __fx64 _Fs, __fx64 _Fp, __fx64 _BandWidth, __ix32 _order, __ix32 _type   = 0, __fx64 _g_pass = 1, __fx64 _g_stop = 80 )
    {
        _order /= 2;

        // frequency deformation coefficient:
        __fx64 w1 = tan( PI2 * _Fp / 2 / _Fs ) , w2 = tan( PI2 * ( _Fp + _BandWidth ) / 2 / _Fs );

        // allocate zeros and poles arrays:

        // lowpass analogue prototype poles, zeros and gains:
        filter_data<__type> data = ( !_type ) ?__cheb2_zeros_poles_plain__<__type>(_order, _g_stop ) :__ellip_zeros_poles_plain__<__type>(_order, _g_pass, _g_stop );

        fcomplex< __fx64 > *plp = data.plp;
        fcomplex< __fx64 > *glp = data.glp;
        fcomplex< __fx64 > *zlp = data.zlp;
        __ix32 L = data.L , R = data.R , N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        fcomplex< __fx64 > *pbp = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *zbp = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *gbp = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );

        // coefficients matrix computation:
         __type *cfnum = ( __type* )calloc( 3 * (2*L+R) , sizeof (__type ) );
         __type *cfden = ( __type* )calloc( 3 * (2*L+R) , sizeof (__type ) );
         __type *gains = ( __type* )calloc( (2*L+R + 1) , sizeof (__type ) );

        if( !cfnum || !cfden || !gains || !plp || !zlp || !glp || !pbp || !zbp || !gbp)
        {
            free( cfnum );
            free( cfden );
            free( gains );
            free( plp );
            free( zlp );
            free( glp );
            free( pbp );
            free( zbp );
            free( gbp );
            return __dsp_gen_empty_filter__<__type>();
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

        // generate output
        data.plp   = pbp;
        data.zlp   = pbp;
        data.glp   = gbp;
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
    filter_data<__type> __cheb2_ellip_digital_bs__ ( __fx64 _Fs, __fx64 _Fc, __fx64 _BandWidth, __ix32 _order, __ix32 _type   = 0, __fx64 _g_pass = 1, __fx64 _g_stop = 80 )
    {
        _order /= 2;

        // frequency deformation coefficient:
        __fx64 w1 = tan( PI2 * _Fc / 2 / _Fs ) , w2 = tan( PI2 * (_Fc + _BandWidth ) / 2 / _Fs );

        // allocate zeros and poles arrays:

        // lowpass analogue prototype poles, zeros and gains:
        filter_data<__type> data = ( !_type ) ?__cheb2_zeros_poles_plain__<__type>(_order, _g_stop ) :__ellip_zeros_poles_plain__<__type>(_order, _g_pass, _g_stop );

        fcomplex< __fx64 > *plp = data.plp;
        fcomplex< __fx64 > *glp = data.glp;
        fcomplex< __fx64 > *zlp = data.zlp;
        __ix32 L = data.L , R = data.R , N = L + R;

        // bandpass digital prototype poles, zeros and gains:
        fcomplex< __fx64 > *pbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *zbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );
        fcomplex< __fx64 > *gbs = ( fcomplex< __fx64 >* ) calloc( 2*N , sizeof ( fcomplex< __fx64 > ) );

        // coefficients matrix computation:
         __type *cfnum = ( __type* )calloc( 3 * (2*L+R) , sizeof ( __type ) );
         __type *cfden = ( __type* )calloc( 3 * (2*L+R) , sizeof ( __type ) );
         __type *gains = ( __type* )calloc( (2*L+R + 1) , sizeof ( __type ) );

         if( !cfnum || !cfden || !gains || !plp || !zlp || !glp || !pbs || !zbs || !gbs)
         {
             free( cfnum );
             free( cfden );
             free( gains );
             free( plp )  ;
             free( zlp )  ;
             free( glp );
             free( pbs )  ;
             free( zbs )  ;
             free( gbs );
             return __dsp_gen_empty_filter__<__type>();
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

        // generate output
        data.plp   = pbs;
        data.zlp   = pbs;
        data.glp   = gbs;
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
    void __show__( filter_data<__type> _data, filter_type _type )
    {
        if( _data.cfnum && _data.cfden && _data.gains )
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

            __ix32  L     = _data.L;
            __ix32  R     = _data.R;
            __ix32  N     = L + R;
            __fx64 *cfnum = _data.cfnum;
            __fx64 *cfden = _data.cfden;
            __fx64 *gains = _data.gains;
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

}

namespace FIR_KERNEL
{
    using namespace IIR_KERNEL;

    /*!
     * \brief FIR filter filtering function
     * \param[_input] pointer to the input signal samples buffer
     * \param[_coeffs] FIR filter coefficitns vector
     * \param[_buff_sx] FIR filter buffer
     * \param[_N] FIR filter filter order
     * \return   The function implements input signal filtering using
     *           FIR filter coefficients vector and buffer:
     *           \f[
     *              y(k) = \sum_{i=0}^N a_{i} * x_{k-i}
     *           \f]
    */
    template< typename __type0 , typename __type1 >
    inline  __type1 __filt__ ( __type0 *_input, __type1 *_coeffs, delay< __type1 > &_buff_sx, __ix32 _N )
    {
        _buff_sx( _input );
        __type1 out = 0;
        for ( __ix32 n = _N-1 ; n >= 0; n--) out += _buff_sx[ n ] * _coeffs[n];
        return out;
    }

    /*!
     * \brief FIR filter filtering function
     * \param[_coeffs] FIR filter coefficitns vector
     * \param[_buff_sx] FIR filter buffer
     * \param[_N] FIR filter filter order
     * \return   The function implements input signal filtering using
     *           FIR filter coefficients vector and ALREADY FILLED buffer
     *           \f[
     *              y(k) = \sum_{i=0}^N a_{i} * x_{k-i}
     *           \f]
    */
    template< typename __type >
    inline  __type __filt__
    ( __type *_coeffs,  delay< __type > &_buff_sx, __ix32 _N
    )
    {
        __type out = 0;
        for ( __ix32 n = _N-1 ; n >= 0; n--) out += _buff_sx[ n ] * _coeffs[n];
        return out;
    }

    /*!
     * \brief FIR filter frequency response computation function
     * \param[_cfbuff] FIR filter coefficients buffer
     * \param[_N] FIR filter order
     * \param[_Fs] sampling frequency
     * \param[_F] input signal frequency
     * \f[
     *      W(j*2*\pi*f) = \sum_{i=0}^N a_{i} * e^{-j*2*\pi*f*Ts }
     * \f]
    */
    template< typename __type > fcomplex<__fx64>  __freq_resp__ ( __type *_cfden, __ix32 _N, __fx64 _Fs, __fx64 _F )
    {
        __type Ts = 1 / _Fs;
        fcomplex<__fx64> Wz(0,0);
        for ( __ix32 i = 0; i < _N; i++) Wz += fcomplex<__fx64>(cos(-PI2 * i * _F * Ts), sin(-PI2 * i * _F * Ts))*_cfden[ i ];
        return Wz;
    }

    /*!
      * \brief FIR digital lowpass filter coefficients computation function
      * \param[_Fs] sampling frequency , Hz
      * \param[_Fc] cut-off frequency  , Hz
      * \param[_N] filter order
      * \param[_scale] scaling factor ( scale = 0 - not-scaled coefficients , scale = 1 - scaled coefficients )
      * \param[_wind] input window function object
      * \return The function returns digital lowpass FIR filter coefficients buffer.
    */
    template<typename __type>
    filter_data<__type> __fir_wind_digital_lp__ ( __fx64 _Fs, __fx64 _Fc, __ix32 _N, __ix32 _scale, __fx64 *_wind )
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
            __fx64 rem = ceil( (__fx64)_N / 2) - (__fx64)_N / 2;

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
            fcomplex<__fx64> fr = __freq_resp__( cfbuff, _N, _Fs, 0 );
            for ( __ix32 n = 0; n < _N; n++) cfbuff[n] /= __cabsf__(fr);
        }

        // returning the result
        filter_data<__type> data;
        data.plp   = nullptr;
        data.zlp   = nullptr;
        data.glp   = nullptr;
        data.cfnum = cfbuff;
        data.cfden = nullptr;
        data.gains = nullptr;
        data.L     = -1;
        data.R     = -1;
        data.N     = _N;
        data.Nx    = -1;
        data.Ny    = -1;

        return data;
    }

    /*!
      * \brief FIR digital lowpass filter coefficients computation function
      * \param[_Fs] sampling frequency , Hz
      * \param[_Fp] pass-off frequency , Hz
      * \param[_N] filter order
      * \param[_scale] scaling factor ( scale = 0 - not-scaled coefficients , scale = 1 - scaled coefficients )
      * \param[_wind] input window function object
      * \return The function returns digital lowpass FIR filter coefficients buffer.
    */
    template<typename __type>
    filter_data<__type> __fir_wind_digital_hp__( __fx64 _Fs, __fx64 _Fp, __ix32 _N, __ix32 _scale, __fx64 *_wind )
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
            __fx64 rem = ceil( (__fx64)_N / 2) - (__fx64)_N / 2;
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
            fcomplex<__fx64> fr = __freq_resp__( cfbuff, _N, _Fs, _Fs / 2 );
            for ( __ix32 n = 0; n < _N; n++) cfbuff[n] /= __cabsf__(fr);
        }

        // returning the result
        filter_data<__type> data;
        data.plp   = nullptr;
        data.zlp   = nullptr;
        data.glp   = nullptr;
        data.cfnum = cfbuff;
        data.cfden = nullptr;
        data.gains = nullptr;
        data.L     = -1;
        data.R     = -1;
        data.N     = _N;
        data.Nx    = -1;
        data.Ny    = -1;

        return data;
    }

    /*!
      * \brief FIR digital lowpass filter coefficients computation function
      * \param[_Fs] sampling frequency , Hz
      * \param[_Fp] pass frequency     , Hz
      * \param[_BW] passband width     , Hz
      * \param[_N] filter order
      * \param[_scale] scaling factor ( scale = 0 - not-scaled coefficients , scale = 1 - scaled coefficients )
      * \param[_wind] input window function object
      * \return The function returns digital lowpass FIR filter coefficients buffer.
    */
    template<typename __type>
    filter_data<__type> __fir_wind_digital_bp__( __fx64 _Fs, __fx64 _Fp,  __fx64 _BW, __ix32 _N, __ix32 _scale, __fx64 *_wind )
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
            __fx64 rem = ceil((__fx64)_N / 2) - (__fx64)_N / 2;

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
            fcomplex<__fx64> fr = __freq_resp__( cfbuff , _N , _Fs , ( ( Fp1 + 0.5 * ( Fp2 - Fp1 ) ) * _Fs ) );
            for ( __ix32 n = 0; n < _N; n++) cfbuff[n] /= __cabsf__(fr);
        }

        // returning the result
        filter_data<__type> data;
        data.plp   = nullptr;
        data.zlp   = nullptr;
        data.glp   = nullptr;
        data.cfnum = cfbuff;
        data.cfden = nullptr;
        data.gains = nullptr;
        data.L     = -1;
        data.R     = -1;
        data.N     = _N;
        data.Nx    = -1;
        data.Ny    = -1;

        return data;
    }

    /*!
      * \brief FIR digital bandpass filter coefficients computation function
      * \param[_Fs] sampling frequency , Hz
      * \param[_Fc] cut-off frequency  , Hz
      * \param[_BW] stopband width     , Hz
      * \param[_N] filter order
      * \param[_scale] scaling factor ( scale = 0 - not-scaled coefficients , scale = 1 - scaled coefficients )
      * \param[_wind] input window function object
      * \return The function returns digital lowpass FIR filter coefficients buffer.
    */
    template<typename __type>
    filter_data<__type> __fir_wind_digital_bs__( __fx64 _Fs, __fx64 _Fc, __fx64 _BW, __ix32 _N, __ix32 _scale, __fx64 *_wind )
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
            fcomplex<__fx64> fr = __freq_resp__(cfbuff, _N, _Fs , 0 );
            for ( __ix32 n = 0; n < _N; n++) cfbuff[n] /= __cabsf__(fr);
        }

        // returning the result
        filter_data<__type> data;
        data.plp   = nullptr;
        data.zlp   = nullptr;
        data.glp   = nullptr;
        data.cfnum = cfbuff;
        data.cfden = nullptr;
        data.gains = nullptr;
        data.L     = -1;
        data.R     = -1;
        data.N     = _N;
        data.Nx    = -1;
        data.Ny    = -1;

        return data;
    }

    #ifndef __ALG_PLATFORM

        template<typename __type>
        void __show__ ( filter_data<__type> _data )
        {
            if( _data.cfnum )
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
                for( __ix32 i = 0 ; i <= _data.N ; i++ )
                    printf("a[%d] = %.12f \n" , i , _data.cfnum[i]);
            }
            else
            {
                printf("The filter has been destryed or it's coeffcients have not been computed yet...\n");
            }
        }

    #endif

}

namespace DSP
{
    /*
    template< typename __type > class classic_iir_abstract : public  filter_abstract
    {
        protected:

        filter_type      m_filter_type;
        bandwidth        m_bandwidth;
        attenuation      m_attenuation;
        delay< __type > *m_buff_sx;
        delay< __type > *m_buff_sy;
        cf   < __type >  m_cfmatrix;

        virtual cf< __type > compute_lowpass () = 0;
        virtual cf< __type > compute_highpass() = 0;
        virtual cf< __type > compute_bandpass() = 0;
        virtual cf< __type > compute_bandstop() = 0;

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
                    m_buff_sx[i].allocate(m_cfmatrix.Nx+1);
                    m_buff_sy[i].allocate(m_cfmatrix.Ny);
                }
            }

            return ( m_buff_sx != nullptr && m_buff_sy != nullptr );
        }

        template<typename T> T filt(T *_input)
        {
            return __filt__(_input, m_cfmatrix.cfnum, m_cfmatrix.cfden, m_cfmatrix.gains, m_cfmatrix.Nx, m_cfmatrix.Ny, m_cfmatrix.N, m_buff_sx, m_buff_sy );
        }

    public:

        virtual inline __type operator()( __type* _input ) = 0;

        virtual ~classic_iir_abstract()
        {
            deallocate();
        }

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

        fcomplex<__fx64> frequency_response( __fx64 _F ) override
        {
            return __freq_resp__(m_cfmatrix.cfnum, m_cfmatrix.cfden, m_cfmatrix.gains, m_cfmatrix.Nx, m_cfmatrix.Ny, m_cfmatrix.N, m_Fs, _F );
        }

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
    */

/*
    template< typename __type > class classic_fir_abstract : public  filter_abstract
    {
    protected:

        filter_type   m_filter_type;
        bandwidth     m_bandwidth;
        bool          m_scale;
        delay<__type> m_buff_sx;
        cf<__type>    m_cfmatrix;
        __type       *m_coeffs;

        __ix32 allocate() override{ return 0; }

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

        virtual inline void operator <<  ( __type *input ) = 0;
        virtual inline __type operator() ( __type *input ) = 0;
        virtual inline __type operator() () = 0;

        virtual ~classic_fir_abstract()
        {
            deallocate();
        }

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

        fcomplex<__fx64> frequency_response( __fx64 F ) override
        {
            return __freq_resp__<__type> ( m_cfmatrix.cfnum , m_order, m_Fs , F  );
        }

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

        classic_fir_abstract() : filter_abstract()
        {
            m_filter_type = filter_type::lowpass;
            m_bandwidth   = { 100 , 500 };
        }

        classic_fir_abstract( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth , __fx64* _window , bool _scale )
        {
            init(_Fs, _order,  _type , _bandwidth , _window , _scale );
        }

        #ifndef __ALG_PLATFORM
        void show()
        {
            __show__( m_coeffs, m_order, m_filter_type );
        }
        #endif
    };

    template < typename T > class fcomb_abstract : public  filter_abstract
    {
        typedef T __type ;
    protected:
        __ix32 m_odd;
        __fx64 m_out;
         delay< __type > m_bx;

         __ix32 allocate() override
         {
             return m_bx.allocate( m_order + 1 );
         }

         template< typename F > inline __type filt( F *input)
         {
             m_bx( input );
             return ( m_out = (m_odd)? (__fx64)*input + (__fx64)m_bx[ m_order ] : (__fx64)*input - (__fx64)m_bx[ m_order ] );
         }

    public:

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

        __ix32 deallocate() override
        {
            m_bx.deallocate();
            return 0;
        }

        fcomb_abstract(){}

        fcomb_abstract( __fx64 _Fs , __fx64 _Fn, __ix32 _odd )
        {
            init( _Fs , _Fn, _odd );
        }

        virtual ~fcomb_abstract()
        {
            deallocate();
        }

        fcomplex<__fx64> frequency_response( __fx64 F ) override
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

            return Wz;
        }

        virtual inline __type operator ()( __type *input ) = 0;
    };

    template < typename __type > class fcombeq_abstract : public  filter_abstract
    {
    protected:

        __fx64 m_dF;
        __fx64 m_d_Amp;
        __fx64 m_K1;
        __fx64 m_K2;
        __ix32 m_ElemNum1;
        __ix32 m_ElemNum2;
        __ix32 m_odd;
         delay< __type > m_bx;

         template< typename F > inline __type filt( F *input )
         {
             m_bx(input);
             return ( m_out = ( !m_odd ) ? ( (__fx64)*input * m_K1 - (__fx64)m_bx[m_ElemNum1] - (__fx64)m_bx[m_ElemNum2] * m_K2 )
                                         : ( (__fx64)*input * m_K1 + (__fx64)m_bx[m_ElemNum1] - (__fx64)m_bx[m_ElemNum2] * m_K2 ) );
         }

         __ix32 allocate() override
         {
             // Complex Transfer functions:
             fcomplex<__fx64> WZ1(0.5 - 0.5 * cos(-PI2 * (m_Fn + m_dF) * (__fx64)m_order * m_Ts), -0.5 * sin(-PI2 * (m_Fn + m_dF) * (__fx64)m_order * m_Ts) );
             fcomplex<__fx64> WZ2(1.0 - cos(-PI2 * (m_Fn + m_dF) * (__fx64)m_order * 2.0 * m_Ts), -sin(-PI2 * (m_Fn + m_dF) * (__fx64)m_order * 2.0 * m_Ts) );

             // square equation coefficients:
             __fx64 a = 1;
             __fx64 b = __realf__( ( WZ1 * __conjf__(WZ2) + WZ2*__conjf__(WZ1) ) / WZ2 / __conjf__(WZ2) );
             __fx64 c = __realf__( ( WZ1 * __conjf__(WZ1) - fcomplex<__fx64>((1 + m_d_Amp / 100) * (1 + m_d_Amp / 100) , 0 ) ) / WZ2 / __conjf__(WZ2) );
             __fx64 D = b*b - 4*a*c;

             if( D > (__fx64)0 )
             {
                 __fx64 maxroot = fmax( ( -b - sqrt(D) ) * (__fx64)0.5, ( -b + sqrt(D) ) * (__fx64)0.5 ) * (__fx64)2;
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

         __fx64 m_out;

    public:

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

        fcombeq_abstract() {}

        fcombeq_abstract( __fx64 _Fs, __fx64 _Fn, __fx64 _dF, __fx64 _d_Amp, __ix32 _odd )
        {
            init( _Fs , _Fn , _dF , _d_Amp , _odd );
        }

        virtual ~fcombeq_abstract()
        {
            deallocate();
        }

        __ix32 deallocate() override
        {
            m_bx.deallocate();
            return 0;
        }

        fcomplex<__fx64> frequency_response( __fx64 F ) override
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
            return Wz;
        }

        virtual inline __type operator ()( __type *input ) = 0;
    };

    template< typename __type > class derivative_abstract : public filter_abstract
    {
    protected:
        __fx64  m_T1;
        __type *m_cfnum;
        __type *m_cfden;
        __type  m_Gain;
        delay<__type> m_bx;
        delay<__type> m_by;

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

        void init( __fx64 _Fs , __fx64 _Td )
        {
            m_Fs = _Fs;
            m_Ts = 1 / _Fs;
            m_T1 = _Td;

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

        fcomplex<__fx64> frequency_response(__fx64 _F ) override
        {
            return __freq_resp__ (m_cfnum, m_cfden, &m_Gain, 2, 2, 1, m_Fs, _F );
        }

        derivative_abstract() : filter_abstract(){}

        derivative_abstract( __fx64 _Fs , __fx64 _Td )
        {
            init( _Fs , _Td );
        };

        virtual ~derivative_abstract()
        {
            deallocate();
        };

        virtual inline __type operator ()( __type *_input ) = 0;
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

        void init(__fx64 _Fs, __fx64 _Ta )
        {
            m_Fs = _Fs;
            m_Ts = 1 / _Fs;
            m_T1 = _Ta;

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

        fcomplex<__fx64> frequency_response(__fx64 _F ) override
        {
            return __freq_resp__(m_cfnum, m_cfden, &m_Gain, 2, 2, 1, m_Fs, _F );
        }

        aperiodic_abstract() : filter_abstract(){}

        aperiodic_abstract(__fx64 _Fs, __fx64 _Ta )
        {
            init(_Fs, _Ta );
        }

        virtual ~aperiodic_abstract()
        {
            deallocate();
        };

        virtual inline __type operator ()( __type *_input ) = 0;
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

        void init( __fx64 _Fs )
        {
            m_Fs = _Fs;
            m_Ts = 1 / _Fs;

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

        fcomplex<__fx64> frequency_response(__fx64 _F ) override
        {
            return __freq_resp__ (m_cfnum, m_cfden, &m_Gain, 2, 2, 1, m_Fs, _F );
        }

        integrator_abstract() : filter_abstract(){}

        integrator_abstract(__fx64 _Fs )
        {
            init(_Fs);
            allocate();
        }

        virtual ~integrator_abstract()
        {
            deallocate();
        };

        virtual inline __type operator ()( __type *_input ) = 0;
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

        void init(__fx64 _Fs, __fx64 _T1, __fx64 _T2 )
        {
            m_Fs = _Fs;
            m_Ts = 1 / _Fs;
            m_T1 = _T1;
            m_T2 = _T2;

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

        fcomplex<__fx64> frequency_response(__fx64 _F ) override
        {
            return __freq_resp__ (m_cfnum, m_cfden, &m_Gain, 2, 2, 1, m_Fs, _F );
        }

        leadlag_abstract() : filter_abstract(){}

        leadlag_abstract(__fx64 _Fs, __fx64 _T1 , __fx64 _T2 )
        {
            init(_Fs, _T1, _T2 );
        }

        virtual ~leadlag_abstract()
        {
            deallocate();
        };

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
                    m_Gain     = omega / m_Kd / k1;
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

        void init( __fx64 _Fs, __fx64 _Fc, __fx64 _Kd, filter_type _type )
        {
            m_Fs   = _Fs;
            m_Ts   = 1 / _Fs;
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

        fcomplex<__fx64> frequency_response(__fx64 _F ) override
        {
            return __freq_resp__ (m_cfnum, m_cfden, &m_Gain, 3, 3, 1, m_Fs, _F );
        }

        // constructors and destructor:
        second_order_filter_abstract() : filter_abstract(){}

        second_order_filter_abstract(__fx64 _Fs, __fx64 _Fc, __fx64 _Kd, filter_type _type )
        {
            init( _Fs, _Kd, _Fc, _type );
        }

        virtual ~second_order_filter_abstract()
        {
            deallocate();
        }

        virtual inline __type operator ()( __type *_input ) = 0;
    };


    template< typename __type > class fir;
    template< typename __type > class recursive_fourier;
    template< typename __type > class fcomb;
    template< typename __type > class fcombeq;
    template< typename __type > class derivative;
    template< typename __type > class aperiodic;
    template< typename __type > class integrator;
    template< typename __type > class leadlag;
    template< typename __type > class second_order_filter;
*/



    /*! \example Butterworth_lowpass_example.cpp  */
    /*! \example Butterworth_highpass_example.cpp */
    /*! \example Butterworth_bandpass_example.cpp */
    /*! \example Butterworth_bandstop_example.cpp */


    /*! \example Butterworth_lowpass_example.cpp  */
    /*! \example Butterworth_highpass_example.cpp */
    /*! \example Butterworth_bandpass_example.cpp */
    /*! \example Butterworth_bandstop_example.cpp */

    /*!
     *  \brief Child Chebyshev I filter 32-bit realization
     *  \details Defines classic Chebyshev I filter represented in a biquadratic form:
     *  \f[
     *      W(z) = \prod_{i=0}^N gain_{i} * \frac{ a_{0i} + a_{1i} * z^{-1} + a_{2i} * z^{-2} }
     *                                           { b_{0i} + b_{1i} * z^{-1} + b_{2i} * z^{-2} }
     *  \f]
    */

    /*! \example Chebyshev_type_I_lowpass_example.cpp  */
    /*! \example Chebyshev_type_I_highpass_example.cpp */
    /*! \example Chebyshev_type_I_bandpass_example.cpp */
    /*! \example Chebyshev_type_I_bandstop_example.cpp */

    /*!
     *  \brief Child Chebyshev I filter 64-bit realization
     *  \details Defines classic Chebyshev I filter represented in a biquadratic form:
     *  \f[
     *      W(z) = \prod_{i=0}^N gain_{i} * \frac{ a_{0i} + a_{1i} * z^{-1} + a_{2i} * z^{-2} }
     *                                           { b_{0i} + b_{1i} * z^{-1} + b_{2i} * z^{-2} }
     *  \f]
    */

    /*! \example Chebyshev_type_I_lowpass_example.cpp  */
    /*! \example Chebyshev_type_I_highpass_example.cpp */
    /*! \example Chebyshev_type_I_bandpass_example.cpp */
    /*! \example Chebyshev_type_I_bandstop_example.cpp */

     /*!
      *  \brief Child Chebyshev II filter 32-bit realization
     *  \details Defines classic Chebyshev II filter represented in a biquadratic form:
     *  \f[
     *      W(z) = \prod_{i=0}^N gain_{i} * \frac{ a_{0i} + a_{1i} * z^{-1} + a_{2i} * z^{-2} }
     *                                           { b_{0i} + b_{1i} * z^{-1} + b_{2i} * z^{-2} }
     *  \f]
     */


    /*! \example Chebyshev_type_II_lowpass_example.cpp  */
    /*! \example Chebyshev_type_II_highpass_example.cpp */
    /*! \example Chebyshev_type_II_bandpass_example.cpp */
    /*! \example Chebyshev_type_II_bandstop_example.cpp */

    /*!
     *  \brief Child Chebyshev II filter 64-bit realization
     *  \details Defines classic Chebyshev II filter represented in a biquadratic form:
     *  \f[
     *      W(z) = \prod_{i=0}^N gain_{i} * \frac{ a_{0i} + a_{1i} * z^{-1} + a_{2i} * z^{-2} }
     *                                           { b_{0i} + b_{1i} * z^{-1} + b_{2i} * z^{-2} }
     *  \f]
    */

    /*! \example Chebyshev_type_II_lowpass_example.cpp  */
    /*! \example Chebyshev_type_II_highpass_example.cpp */
    /*! \example Chebyshev_type_II_bandpass_example.cpp */
    /*! \example Chebyshev_type_II_bandstop_example.cpp */

    /*!
     *  \brief Child Elliptic filter 32-bit realization
     *  \details Defines classic Elliptic filter represented in a biquadratic form:
     *  \f[
     *      W(z) = \prod_{i=0}^N gain_{i} * \frac{ a_{0i} + a_{1i} * z^{-1} + a_{2i} * z^{-2} }
     *                                           { b_{0i} + b_{1i} * z^{-1} + b_{2i} * z^{-2} }
     *  \f]
    */
    /*! \example Elliptic_lowpass_example.cpp  */
    /*! \example Elliptic_highpass_example.cpp */
    /*! \example Elliptic_bandpass_example.cpp */
    /*! \example Elliptic_bandstop_example.cpp */

     /*!
      *  \brief Child Elliptic filter 64-bit realization
     *  \details Defines classic Elliptic filter represented in a biquadratic form:
     *  \f[
     *      W(z) = \prod_{i=0}^N gain_{i} * \frac{ a_{0i} + a_{1i} * z^{-1} + a_{2i} * z^{-2} }
     *                                           { b_{0i} + b_{1i} * z^{-1} + b_{2i} * z^{-2} }
     *  \f]
     */

    /*! \example Elliptic_lowpass_example.cpp  */
    /*! \example Elliptic_highpass_example.cpp */
    /*! \example Elliptic_bandpass_example.cpp */
    /*! \example Elliptic_bandstop_example.cpp */

    /*! @} */

    /*! \defgroup <DSPB_ImplementationFIR> (FIR filters implementation )
     *  \ingroup DSPB
     *  \brief The module contains FIR filters fimplementation
        @{
    */


    /*
    template<> class fir< __fx32 > final : public classic_fir_abstract< __fx32 >
    {
        typedef __fx32 __type;
    public:

        fir() : classic_fir_abstract< __type >(){}

        fir( __fx64 _Fs , __ix32 _order, filter_type _type, bandwidth _bandwidth, __fx64* _window, __ix32 _scale )
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
    */

    /*
    template<> class fir< __fx64 > final : public classic_fir_abstract< __fx64 >
    {
        typedef __fx64 __type;
    public:

        fir() : classic_fir_abstract< __type >(){}

        fir( __fx64 _Fs , __ix32 _order, filter_type _type, bandwidth _bandwidth, __fx64* _window, __ix32 _scale )
            : classic_fir_abstract< __type >(_Fs, _order, _type, _bandwidth, _window, _scale ){}


        ~fir() {}

        inline __type operator()( __type*  _input ) override { return __filt__< __type , __type >( _input, m_coeffs, m_buff_sx, m_order ); }

        inline __type operator()( __fxx64* _input ) { return __filt__< __fxx64 , __type >( _input, m_coeffs, m_buff_sx, m_order ); }

        inline __type operator()() override { return __filt__ < __type > ( m_coeffs , m_buff_sx , m_order ); }

        inline void operator << ( __type*  _input ) override { m_buff_sx(_input ); }

        inline void operator << ( __fxx64* _input ) { m_buff_sx(_input ); }
    };
    */

    /*
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
    */


    /*
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
    */



    /*
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
    */


    /*
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
    */



    /*!
     *  \brief Child derivative transfer function 32-bit realization
     *  \details defines discrete derivative transfer function:
     *  \f[
     *      W_d(z) = \frac{ 2 }{ Ts * \left( 1 + 2 * \frac{Td}{Ts} \right) } *
     *      \frac{ z - 1 }{ z + \frac{ 1 - 2 * \frac{ Td }{ Ts } }{  1 + 2 * \frac{ Td }{ Ts } } }
     *  \f]
    */

    /*
    template<> class derivative<__fx32> final : public derivative_abstract<__fx32>
    {
        typedef __fx32 __type;
    public:

        derivative() : derivative_abstract(){}
        derivative(__fx64 _Fs, __fx64 _Td ) : derivative_abstract( _Fs, _Td ){}

        ~derivative(){};

        inline __type operator ()( __type  *_input ) override
        {
            return __filt__ ( _input, m_cfnum, m_cfden, &m_Gain, 2, 2, 1, &m_bx, &m_by );
        }
    };
    */

    /*!
     *  \brief Child derivative transfer function 64-bit realization
     *  \details defines discrete derivative transfer function:
     *  \f[
     *      W_d(z) = \frac{ 2 }{ Ts * \left( 1 + 2 * \frac{Td}{Ts} \right) } *
     *      \frac{ z - 1 }{ z + \frac{ 1 - 2 * \frac{ Td }{ Ts } }{  1 + 2 * \frac{ Td }{ Ts } } }
     *  \f]
    */

    /*
    template<> class derivative<__fx64> final : public derivative_abstract<__fx64>
    {
        typedef __fx64 __type;
    public:
        derivative() : derivative_abstract(){}
        derivative(__fx64 _Fs, __fx64 _Td ) : derivative_abstract( _Fs, _Td ){}
        ~derivative(){};
        inline __type operator ()( __type  *_input ) override
        {
            return __filt__ ( _input, m_cfnum, m_cfden, &m_Gain, 2, 2, 1, &m_bx, &m_by );
        }
    };
    */

    /*!
     *  \brief Child aperiodic transfer function 32-bit realization
     *  \details defines discrete aperiodic transfer function:
     *  \f[
     *      W_d(z) = \frac{ 1 }{ 1 + 2*\frac{Ta}{Ts} } *
     *      \frac{ z + 1 }{ z + \frac{ 1 - 2 * \frac{ Ta }{ Ts } }{  1 + 2 * \frac{ Ta }{ Ts } } }
     *  \f]
    */

    /*
    template<> class aperiodic<__fx64> final: public aperiodic_abstract<__fx64>
    {
        typedef __fx64 __type;
    public:
        aperiodic() : aperiodic_abstract(){}
        aperiodic(__fx64 _Fs, __fx64 _Ta ) : aperiodic_abstract( _Fs, _Ta ){}

        ~aperiodic(){};

        inline __type operator ()( __type *_input ) override
        {
            return __filt__ ( _input, m_cfnum, m_cfden, &m_Gain, 2, 2, 1, &m_bx, &m_by );
        }
    };
    */

    /*!
     *  \brief Child aperiodic transfer function 64-bit realization
     *  \details defines discrete aperiodic transfer function:
     *  \f[
     *      W_d(z) = \frac{ 1 }{ 1 + 2*\frac{Ta}{Ts} } *
     *      \frac{ z + 1 }{ z + \frac{ 1 - 2 * \frac{ Ta }{ Ts } }{  1 + 2 * \frac{ Ta }{ Ts } } }
     *  \f]
    */

    /*
    template<> class aperiodic<__fx32> final: public aperiodic_abstract<__fx32>
    {
        typedef __fx32 __type;
    public:
        aperiodic() : aperiodic_abstract(){}
        aperiodic(__fx64 _Fs, __fx64 _Ta ) : aperiodic_abstract( _Fs, _Ta ){}
        ~aperiodic(){};
        inline __type operator ()( __type *_input ) override
        {
            return __filt__ ( _input, m_cfnum, m_cfden, &m_Gain, 2, 2, 1, &m_bx, &m_by );
        }
    };
    */

     /*!
      *  \brief Child integrator transfer function 32-bit realization
     *  \details defines discrete integrator transfer function:
     *  \f[
     *      W_d(z) = 0.5 * Ts * \frac{z+1}{z-1}
     *  \f]
     */

    /*
    template<> class integrator<__fx32> final : public integrator_abstract<__fx32>
    {
        typedef __fx32 __type;
    public:

        integrator() : integrator_abstract(){}
        integrator(__fx64 _Fs ) : integrator_abstract(_Fs ){}

        ~integrator(){};
        inline __type operator ()( __type  *_input ) override
        {
            return __filt__( _input, m_cfnum, m_cfden, &m_Gain, 2, 2, 1, &m_bx, &m_by );
        }
    };
    */

    /*!
     *  \brief Child integrator transfer function 64-bit realization
     *  \details defines discrete integrator transfer function:
     *  \f[
     *      W_d(z) = 0.5 * Ts * \frac{z+1}{z-1}
     *  \f]
    */

    /*
    template<> class integrator<__fx64> final : public integrator_abstract<__fx64>
    {
        typedef __fx64 __type;
    public:
        integrator() : integrator_abstract(){}
        integrator(__fx64 _Fs ) : integrator_abstract(_Fs ){}
        ~integrator(){};
        inline __type operator ()( __type  *_input ) override
        {
            return __filt__( _input, m_cfnum, m_cfden, &m_Gain, 2, 2, 1, &m_bx, &m_by );
        }
    };
    */

    /*!
     *  \brief Child lead-lag transfer function 32-bit realization
     *  \details defines discrete lead-lag transfer function:
     *  \f[
     *      W_d(z) = \frac{ 1 + \frac{ 1 - 2 * \frac{ T1 }{ Ts } }{ 1 + \frac{ 1 - 2 * \frac{ T2 }{ Ts } } *
     *                    \frac
     *                    { 1 + \frac{ 1 - 2 * \frac{ T1 }{ Ts } }{ 1 + 2 * \frac{ T1 }{ Ts } } }
     *                    { 1 + \frac{ 1 - 2 * \frac{ T2 }{ Ts } }{ 1 + 2 * \frac{ T2 }{ Ts } } }
     *  \f]
    */

    /*
    template<> class leadlag<__fx32> final : public leadlag_abstract<__fx32>
    {
        typedef __fx32 __type;
    public:
        leadlag() : leadlag_abstract(){}
        leadlag(__fx64 _Fs, __fx64 _T1, __fx64 _T2 ) : leadlag_abstract(_Fs, _T1, _T2 ){}
        ~leadlag(){};

        inline __type operator ()( __type  *_input ) override
        {
            return __filt__ ( _input, m_cfnum, m_cfden, &m_Gain, 2, 2, 1, &m_bx, &m_by );
        }
    };

    template<> class leadlag<__fx64> final : public leadlag_abstract<__fx64>
    {
        typedef __fx64 __type;
    public:
        leadlag() : leadlag_abstract(){}
        leadlag(__fx64 _Fs, __fx64 _T1, __fx64 _T2 ) : leadlag_abstract(_Fs, _T1, _T2 ){}

        ~leadlag(){};
        inline __type operator ()( __type  *_input ) override
        {
            return __filt__ ( _input, m_cfnum, m_cfden, &m_Gain, 2, 2, 1, &m_bx, &m_by );
        }
    };
    */

    /*!
     *  \brief Child second order filter transfer function 32-bit realization
     *  \details The transfer functions are implemented for all the types
     *           of second order filters:
     *
     *           Lowpass filter transfer function:
     *  \f[
     *           W(z) = \frac{ \omega^2 }{ k1 } * \frac{ z^2 + 2*z + 1 }{ \frac{k3}{k1}*z^2 + \frac{k2}{k1}*z + 1 }
     *  \f]
     *
     *           Highpass filter transfer function:
     *  \f[
     *           W(z) = \frac{ 1 }{ k1 } * \frac{ z^2 - 2*z + 1 }{ \frac{k3}{k1}*z^2 + \frac{k2}{k1}*z + 1 }
     *  \f]
     *
     *           Bandpass filter transfer function:
     *  \f[
     *           W(z) =  \frac{ \omega }{ k1 * K_d } * \frac{ z^2 - 1 }{ \frac{k3}{k1}*z^2 + \frac{k2}{k1}*z + 1 }
     *  \f]
     *
     *           Bandstop filter transfer function:
     *  \f[
     *           W(z) =  \frac{}{} * \frac{ (1+\omega^2)*z^{-2} + (2*\omega^2-2)*z^{-1} + (1+\omega^2) }{ \frac{k3}{k1}*z^2 + \frac{k2}{k1}*z + 1 }
     *  \f]
     *
     *          Other filter transfer function - bandstop filter that has bandstop width instead of Kd as a parameter
     *  \f[
     *           W(z) = 0.5*(1+k_2)*\frac{ z^-2 + 2*k_1*z^-1 + 1 }{ k_2*z^-2 + k_1*(1+k_2)*z^-1 + 1 } \newline
     *           k_1  = -cos(2 * \pi * Fc * Ts)                                                       \newline
     *           k_2  = \frac{ 1 - tan(\pi * Kd * Ts) } { 1 + tan(\pi * Kd * Ts) }                    \newline
     *  \f]
    */

    /*
    template<> class second_order_filter<__fx32> final : public second_order_filter_abstract<__fx32>
    {
        typedef __fx32 __type;
    public:
        second_order_filter() : second_order_filter_abstract(){}

        second_order_filter(__fx64 _Fs, __fx64 _Fc , __fx64 _Kd , filter_type _type) : second_order_filter_abstract(_Fs, _Fc, _Kd, _type ){}

        ~second_order_filter(){};
        inline __type operator ()( __type  *input ) override
        {
            return __filt__ ( input, m_cfnum, m_cfden, &m_Gain, 3, 3, 1, &m_bx, &m_by );
        }
    };


    template<> class second_order_filter<__fx64> final : public second_order_filter_abstract<__fx64>
    {
        typedef __fx64 __type;
    public:
        second_order_filter() : second_order_filter_abstract(){}
        second_order_filter(__fx64 _Fs, __fx64 _Fc , __fx64 _Kd , filter_type _type) : second_order_filter_abstract(_Fs, _Fc, _Kd, _type ){}
        ~second_order_filter(){};
        inline __type operator ()( __type  *input ) override
        {
            return __filt__ ( input, m_cfnum, m_cfden, &m_Gain, 3, 3, 1, &m_bx, &m_by );
        }
    };
    */

};

/*! @} */

#undef __fx32
#undef __fx64
#undef __fxx64
#undef __ix32
#undef PI0
#undef PI2

#endif // DSP_H
