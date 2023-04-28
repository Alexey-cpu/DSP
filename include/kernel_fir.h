#ifndef KERNEL_FIR_H
#define KERNEL_FIR_H

#include "kernel_dsp.h"
#include "special_functions.h"
using namespace DSP_KERNEL;

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

#ifndef PI0
#define PI0 3.1415926535897932384626433832795
#endif

#ifndef PI2
#define PI2 6.283185307179586476925286766559
#endif

/*!  \namespace FIR_KERNEL */
namespace FIR_KERNEL
{

    /*! \defgroup <DSP_CLASSIC_FIR_FILTERS_KERNEL> ( Classic FIR low level kernel )
     *  \ingroup KERNELS
     *  \brief   The module contains classic FIR filters low level kernel
     *  \details The module contains low level FIR filters computational utilities that compute
     *          floating point type coefficients.
        @{
    */

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
    ( __type *_coeffs,  delay< __type > &_buff_sx, __ix32 _N )
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
    template< typename __type > Complex<__fx64>  __freq_resp__ ( __type *_cfden, __ix32 _N, __fx64 _Fs, __fx64 _F )
    {
        __type Ts = 1 / _Fs;
        Complex<__fx64> Wz(0,0);
        for ( __ix32 i = 0; i < _N; i++) Wz += Complex<__fx64>(cos(-PI2 * i * _F * Ts), sin(-PI2 * i * _F * Ts))*_cfden[ i ];
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
            Complex<__fx64> fr = __freq_resp__( cfbuff, _N, _Fs, 0 );
            for ( __ix32 n = 0; n < _N; n++) cfbuff[n] /= __cabsf__(fr);
        }

        // returning the result
        filter_data<__type> data;
        data.poles   = nullptr;
        data.zeros   = nullptr;
        data.gains   = nullptr;
        data.cfnum = cfbuff;
        data.cfden = nullptr;
        data.gains = nullptr;
        data.L     = -1;
        data.R     = -1;
        data.N     = _N;
        data.Nx    = +1;
        data.Ny    = +1;

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
            Complex<__fx64> fr = __freq_resp__( cfbuff, _N, _Fs, _Fs / 2 );
            for ( __ix32 n = 0; n < _N; n++) cfbuff[n] /= __cabsf__(fr);
        }

        // returning the result
        filter_data<__type> data;
        data.poles = nullptr;
        data.zeros = nullptr;
        data.gains = nullptr;
        data.cfnum = cfbuff;
        data.cfden = nullptr;
        data.gains = nullptr;
        data.L     = -1;
        data.R     = -1;
        data.N     = _N;
        data.Nx    = +1;
        data.Ny    = +1;

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
            Complex<__fx64> fr = __freq_resp__( cfbuff , _N , _Fs , ( ( Fp1 + 0.5 * ( Fp2 - Fp1 ) ) * _Fs ) );
            for ( __ix32 n = 0; n < _N; n++) cfbuff[n] /= __cabsf__(fr);
        }

        // returning the result
        filter_data<__type> data;
        data.poles = nullptr;
        data.zeros = nullptr;
        data.gains = nullptr;
        data.cfnum = cfbuff;
        data.cfden = nullptr;
        data.gains = nullptr;
        data.L     = -1;
        data.R     = -1;
        data.N     = _N;
        data.Nx    = +1;
        data.Ny    = +1;

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
            Complex<__fx64> fr = __freq_resp__(cfbuff, _N, _Fs , 0 );
            for ( __ix32 n = 0; n < _N; n++) cfbuff[n] /= __cabsf__(fr);
        }

        // returning the result
        filter_data<__type> data;
        data.poles = nullptr;
        data.zeros = nullptr;
        data.gains = nullptr;
        data.cfnum = cfbuff;
        data.cfden = nullptr;
        data.gains = nullptr;
        data.L     = -1;
        data.R     = -1;
        data.N     = _N;
        data.Nx    = +1;
        data.Ny    = +1;

        return data;
    }

    #ifndef __ALG_PLATFORM

        template<typename __type>
        void __show_fir__ ( filter_data<__type> _data )
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

   /*! @} */

}

#undef __fx32
#undef __fx64
#undef __fxx64
#undef __ix32
#undef PI0
#undef PI2

#endif // KERNEL_FIR_H
