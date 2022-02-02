/*!
 * \file
 * \brief   Quadrature demodulator
 * \authors A.Tykvinskiy
 * \date    21.12.2021
 * \version 1.0
 *
 * The header declares quadrature demodulator template class for real time and cycling computations
*/

#include "fir.h"
#include "complex.h"
#include "buffer.h"
#include "recursive_mean.h"

#ifndef QUAD_MLTPX_H
#define QUAD_MLTPX_H

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

/*! \brief defines pi / 2 */
#ifndef PI_2
#define PI_2 1.5707963267948966192313216916398
#endif

/*! \brief defines pi / 4 */
#ifndef PI_4
#define PI_4 0.78539816339744830961566084581988
#endif

/*! \brief Quadrature demodulator template class for the cycling computations */
template< typename T > class quad_mltpx;

/*! \brief Quadrature demodulator template class for the real time computations */
template< typename T > class quad_mltpx_rt;

/*! \brief 32-bit real-time quadrature demodulator realization */
template<> class quad_mltpx<__fx32>
{
    typedef __fx32 __type ;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
     /*! \brief input signal sampling period   , s */
    __fx64 m_Ts;
     /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input buffer size */
    __ix32 m_HBuffSize;
    /*! \brief input signal frames per period of nominal frequency */
    __ix32 m_SamplesPerPeriod;

     /*! \brief auxiliary variable to compute input signal real component */
    __type m_a;
    /*! \brief auxiliary variable to compute input signal imaginary component */
    __type m_b;
    /*! \brief auxiliary variable to compute input signal slip frequency */
    __type m_K;
    /*! \brief auxiliary variable to compensate filter frequency amplitude response slope */
    __type m_Km;

    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_A1;
    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_B1;
    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_C1;

    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_A2;
    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_B2;
    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_C2;

    /*! \brief reference frame counter */
    __ix32 m_cnt;
    /*! \brief reference frame samples per period */
    __ix32 m_ElemNum;

    /*! \brief reference cosine frame */
    __type *m_ref_cos;
    /*! \brief reference sine frame */
    __type *m_ref_sin;

    /*! \brief input signal real component buffer */
    mirror_ring_buffer<__type> m_buff_re;
    /*! \brief input signal imaginary component buffer */
    mirror_ring_buffer<__type> m_buff_im;

    /*! \brief comb filter */
    fcomb<__type> m_apfilt;
    /*! \brief fir filter to extract input signal real component */
    fir<__type> m_fir_re;
    /*! \brief fir filter to extract input signal imaginary component */
    fir<__type> m_fir_im;
    /*! \brief fir filter to smooth the input signal frequency slip */
    fir<__type> m_fir_df;

    /*! \brief auxiliary vector */
    complex< __type > m_vector1;
    /*! \brief auxiliary vector */
    complex< __type > m_vector2;
    /*! \brief auxiliary vector */
    complex< __type > m_vector3;

public:

    /*! \brief input signal real component */
    __type m_Re;
    /*! \brief input signal imaginary component */
    __type m_Im;
    /*! \brief input signal frequency , Hz */
    __type m_F;
    /*! \brief input signal frequency slip , Hz */
    __type m_dF;

    /*!
     *  \brief filter initialization function
     *  \param[Fs        ] - input signal sampling frequency , Hz
     *  \param[Fn        ] - input signal nominal frequency  , Hz
     *  \param[CycleWidth] - IDE cycle duration , ms
    */
    void init( __fx64 Fs, __fx64 Fn, __fx64 SideLobeAtten , __ix32 FiltWindowWidth , __ix32 CycleWidth )
    {
        // variables:
        m_Fs               = Fs;
        m_Ts               = 1 / m_Fs;
        m_Fn               = Fn;
        m_F                = m_Fn;
        m_HBuffSize        = m_Fs / 1000 * CycleWidth;
        m_SamplesPerPeriod = m_Fs / m_Fn;
        m_K                = m_Fn * (__fx64)m_SamplesPerPeriod / (__fx64)m_HBuffSize / PI2;

        // reference frame constants:
        m_cnt     = 0;
        m_ElemNum = m_SamplesPerPeriod - 1;

        // aperiodic component filter initialization:
        m_apfilt.init( m_Fs , m_Fn );

        // ortohonal components filters initialization:
        m_fir_re.lp_init (m_Fs , m_Fn , 0.1 , FiltWindowWidth , true);
        m_fir_im.lp_init (m_Fs , m_Fn , 0.1 , FiltWindowWidth , true);
        m_fir_re.m_wind.Chebyshev(SideLobeAtten);
        m_fir_im.m_wind.Chebyshev(SideLobeAtten);

        // frequency slip filter initialization:
        m_fir_df.lp_init ( m_Fs , m_Fn , 5 , 10 * (m_Fs / m_HBuffSize / 250) , true );
        m_fir_df.m_wind.Chebyshev(60);

        // output:
        m_Re = 0;
        m_Im = 0;
        m_F  = m_Fn;
        m_dF = 0;

        // intial values of filter frequency amplitude response slope compensation function factors:
        m_A1 = 0;
        m_B1 = 0;
        m_C1 = 1;
        m_A2 = 0;
        m_B2 = 0;
        m_C2 = 1;
        m_Km = 1;

        // reference frame:
        m_ref_cos = nullptr;
        m_ref_sin = nullptr;
    }

    /*! \brief default constructor */
    quad_mltpx()
    {
        // variables:
        m_Fs               = 4000;
        m_Ts               = 1 / m_Fs;
        m_Fn               = 50;
        m_F                = m_Fn;
        m_HBuffSize        = m_Fs / 1000 * 5;
        m_SamplesPerPeriod = m_Fs / m_Fn;
        m_K                = m_Fn * m_SamplesPerPeriod / m_HBuffSize / PI2;

        // reference frame constants:
        m_cnt     = 0;
        m_ElemNum = m_SamplesPerPeriod - 1;

        // aperiodic component filter initialization:
        m_apfilt.init( m_Fs , m_Fn );

        // ortohonal components filters initialization:
        m_fir_re.lp_init (m_Fs , m_Fn , 0.1 , 95 , true);
        m_fir_im.lp_init (m_Fs , m_Fn , 0.1 , 95 , true);
        m_fir_re.m_wind.Chebyshev(60);
        m_fir_im.m_wind.Chebyshev(60);

        // frequency slip filter initialization:
        m_fir_df.lp_init ( m_Fs , m_Fn , 5 , 10 * (m_Fs / m_HBuffSize / 250) , true );
        m_fir_df.m_wind.Chebyshev(60);

        // output:
        m_Re = 0;
        m_Im = 0;
        m_F  = m_Fn;
        m_dF = 0;

        // intial values of filter frequency amplitude response slope compensation function factors:
        m_A1 = 0;
        m_B1 = 0;
        m_C1 = 1;
        m_A2 = 0;
        m_B2 = 0;
        m_C2 = 1;
        m_Km = 1;

        // reference frame:
        m_ref_cos = nullptr;
        m_ref_sin = nullptr;
    }

    /*! \brief memory allocation function */
    __ix32 allocate()
    {
        // filters and ring buffers memory allocation:
        m_apfilt .allocate( );
        m_fir_re .allocate( );
        m_fir_im .allocate( );
        m_fir_df .allocate( );
        m_buff_re.allocate(2);
        m_buff_im.allocate(2);

        // reference frame memory allocation:
        m_ref_cos = ( __type* )calloc( m_SamplesPerPeriod , sizeof ( __type ) );
        m_ref_sin = ( __type* )calloc( m_SamplesPerPeriod , sizeof ( __type ) );

        // reference frame initialization:
        for( __ix32 n = 0; n < m_SamplesPerPeriod ; n++)
        {
            m_ref_sin[n] = sin( PI2 * m_Fn * (__fx64)n / m_Fs ) / sqrt(2);
            m_ref_cos[n] = cos( PI2 * m_Fn * (__fx64)n / m_Fs ) / sqrt(2);
        }

        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response slope compensation function coefficients computation:
        //----------------------------------------------------------------------------------------------------------------------
        // initials:
        //----------------------------------------------------------------------------------------------------------------------
        __fx64 dF1 = 5 ; // slip 1
        __fx64 dF2 = 10; // slip 2
        __fx64 Km1 = 0 ; // frequency response slope compensation function coefficient for slip 1
        __fx64 Km2 = 0 ; // frequency response slope compensation function coefficient for slip 2

        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response slope compensation function coefficients of filter 1:
        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response for dF1 , dF2:
        Km1 =  m_fir_re.freq_resp( dF1 ).Km;
        Km2 =  m_fir_re.freq_resp( dF2 ).Km;

        // amplitude frequency response slope compensation function coefficients:
        m_A1 = ( Km1 * ( dF1 - dF2 ) - (Km1 - Km2) * dF1 - (dF1 - dF2) ) / ( dF1*dF1*(dF1 - dF2) - dF1*(dF1*dF1 - dF2*dF2) );
        m_B1 = ( (Km1 - Km2) - m_A1 * ( dF1 * dF1 - dF2 * dF2 ) ) / ( dF1 - dF2 );
        m_C1 = 1;
        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response slope compensation function coefficients of filter 2:
        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response for dF1:
        Km1 = m_apfilt.freq_resp( m_Fn + dF1 ).Km;
        Km2 = m_apfilt.freq_resp( m_Fn + dF1 ).Km;

        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response slope compensation function coefficients of total computational chain:
        //----------------------------------------------------------------------------------------------------------------------
        // расчет коэффициентов компенсации амплитудного искажения:
        m_A2 = ( Km1 * ( dF1 - dF2 ) - (Km1 - Km2) * dF1 - (dF1 - dF2) ) / ( dF1*dF1*(dF1 - dF2) - dF1*(dF1*dF1 - dF2*dF2) );
        m_B2 = ( (Km1 - Km2) - m_A2 * ( dF1 * dF1 - dF2 * dF2 ) ) / ( dF1 - dF2 );
        m_C2 = 1;
        //----------------------------------------------------------------------------------------------------------------------

        return 0;
    }

    /*! \brief memory deallocation function */
    __ix32 deallocate()
    {
        // filters and buffers memory deallocation:
        m_apfilt .deallocate();
        m_fir_re .deallocate();
        m_fir_re .deallocate();
        m_fir_df .deallocate();
        m_buff_re.deallocate();
        m_buff_re.deallocate();

        // reference frame and output memory deallocation:
        if( m_ref_cos != nullptr ) { free( m_ref_cos ); m_ref_cos = nullptr; }
        if( m_ref_sin != nullptr ) { free( m_ref_sin ); m_ref_sin = nullptr; }

        return 0;
    }

    /*! \brief default destructor */
    ~quad_mltpx() { deallocate(); }

    /*!
     *  \brief template filtering function
     *  \param[input        ] - pointer to the input signal frames
     *  \param[intChanDelay ] - inter channel delay , s
     *  \param[FreqCorr     ] - amplitude frequency response slope compensation flag
    */
    template< typename T > inline void filt( T *input , __fx64 intChanDelay )
    {
        for ( __ix32 n = 0 ; n < m_HBuffSize; n++)
        {
            // aperiodic component filtering:
            m_apfilt( &input[n] );

            // quadrature multiplication and FIR filters buffers filling:
            m_fir_re.fill_fir_buff( &( m_a = m_ref_sin[m_cnt] * m_apfilt.m_out ) );
            m_fir_im.fill_fir_buff( &( m_b = m_ref_cos[m_cnt] * m_apfilt.m_out ) );

            // reference frame counter update:
            if (m_cnt < m_ElemNum) m_cnt++;
            else m_cnt = 0;
        }

        // filtering:
        m_fir_re.filt();
        m_fir_im.filt();

        // vector computation:
        m_vector1( m_fir_re.m_out , m_fir_im.m_out );
        m_vector2( 1 , -PI2 * m_F * intChanDelay );
        m_vector3 = m_vector1 * m_vector2;
        m_Re      = m_vector3.m_re;
        m_Im      = m_vector3.m_im;

        // frequency computation:
        m_buff_re.fill_buff( &m_vector3.m_re  );
        m_buff_im.fill_buff( &m_vector3.m_im  );
        m_vector1( +m_buff_re[1] , -m_buff_im[1] );
        m_vector2( +m_buff_re[0] , +m_buff_im[0] );
        m_vector3 = m_vector1 * m_vector2;
        m_dF = atan2( m_vector3.m_im , m_vector3.m_re ) * m_K;
        m_dF =  m_fir_df( &m_dF );
        m_F  = m_Fn + m_dF;

        // filter amplitude frequence response slope compensation:
        if( fabs(m_dF) < 5.5 )
        {
            m_Km = (2 - ( m_A1 * m_dF * m_dF + m_B1 * fabs(m_dF) + m_C1 ) ) * (2 - ( m_A2 * m_dF * m_dF + m_B2 * fabs(m_dF) + m_C2 ) );
        }
        else
        {
            m_Km = (2 - ( m_A1 * 25 + m_B1 * 5 + m_C1 ) ) * (2 - ( m_A2 * 25 + m_B2 * 5 + m_C2 ) );
        }

        m_Re *= m_Km;
        m_Im *= m_Km;
    }

    /*!
     *  \brief 32-bit filtering function
     *  \param[input        ] - pointer to the input signal frames
     *  \param[intChanDelay ] - inter channel delay , s
     *  \param[FreqCorr     ] - amplitude frequency response slope compensation flag
    */
    void process( __type *input , __fx64 intChanDelay ) { filt< __type >( input , intChanDelay ); }

    /*!
     *  \brief 64-bit filtering function
     *  \param[input        ] - pointer to the input signal frames
     *  \param[intChanDelay ] - inter channel delay , s
     *  \param[FreqCorr     ] - amplitude frequency response slope compensation flag
    */
    void process( __fx64 *input , __fx64 intChanDelay ) { filt< __fx64 >( input , intChanDelay ); }

    /*!
     *  \brief filtering function operator
     *  \param[input        ] - pointer to the input signal frames
     *  \param[intChanDelay ] - inter channel delay , s
     *  \param[FreqCorr     ] - amplitude frequency response slope compensation flag
    */
    inline void operator() ( __type *input , __fx64 intChanDelay ){ process( input , intChanDelay ); }

    /*!
     *  \brief filtering function operator
     *  \param[input        ] - pointer to the input signal frames
     *  \param[intChanDelay ] - inter channel delay , s
     *  \param[FreqCorr     ] - amplitude frequency response slope compensation flag
    */
    inline void operator() ( __fx64 *input , __fx64 intChanDelay ){ process( input , intChanDelay ); }
};

/*! \brief 32-bit real-time quadrature demodulator realization */
template<> class quad_mltpx_rt<__fx32>
{
    typedef __fx32 __type ;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
     /*! \brief input signal sampling period   , s */
    __fx64 m_Ts;
     /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input buffer size */
    __ix32 m_HBuffSize;
    /*! \brief input signal frames per period of nominal frequency */
    __ix32 m_SamplesPerPeriod;

     /*! \brief auxiliary variable to compute input signal real component */
    __type m_a;
    /*! \brief auxiliary variable to compute input signal imaginary component */
    __type m_b;
    /*! \brief auxiliary variable to compute input signal slip frequency */
    __type m_K;
    /*! \brief auxiliary variable to compensate filter frequency amplitude response slope */
    __type m_Km;

    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_A1;
    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_B1;
    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_C1;

    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_A2;
    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_B2;
    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_C2;

    /*! \brief reference frame counter */
    __ix32 m_cnt;
    /*! \brief reference frame samples per period */
    __ix32 m_ElemNum;

    /*! \brief reference cosine frame */
    __type *m_ref_cos;
    /*! \brief reference sine frame */
    __type *m_ref_sin;

    /*! \brief input signal real component buffer */
    mirror_ring_buffer<__type> m_buff_re;
    /*! \brief input signal imaginary component buffer */
    mirror_ring_buffer<__type> m_buff_im;

    /*! \brief recursuve mean filter to extract input signal real component */
    recursive_mean<__type> m_rmean_re1;
    /*! \brief recursuve mean filter to extract input signal imaginary component */
    recursive_mean<__type> m_rmean_im1;
    /*! \brief recursuve mean filter to smooth input signal real component */
    recursive_mean<__type> m_rmean_re2;
    /*! \brief recursuve mean filter to smooth input signal imaginary component */
    recursive_mean<__type> m_rmean_im2;
    /*! \brief fir filter to smooth input signal frequency slip */
    fir<__type> m_fir_df;

    /*! \brief auxiliary vector */
    complex< __type > m_vector1;
    /*! \brief auxiliary vector */
    complex< __type > m_vector2;
    /*! \brief auxiliary vector */
    complex< __type > m_vector3;

public:

    /*! \brief input signal real component */
    __type *m_Re;
    /*! \brief input signal imaginary component */
    __type *m_Im;
    /*! \brief input signal frequency , Hz */
    __type m_F;
    /*! \brief input signal frequency slip , Hz */
    __type m_dF;

    /*!
     *  \brief filter initialization function
     *  \param[Fs        ] - input signal sampling frequency , Hz
     *  \param[Fn        ] - input signal nominal frequency  , Hz
     *  \param[CycleWidth] - IDE cycle duration , ms
    */
    void init( __fx64 Fs, __fx64 Fn, __ix32 CycleWidth )
    {
        // variables:
        m_Fs               = Fs;
        m_Ts               = 1 / m_Fs;
        m_Fn               = Fn;
        m_F                = m_Fn;
        m_HBuffSize        = m_Fs / 1000 * CycleWidth;
        m_SamplesPerPeriod = m_Fs / m_Fn;
        m_K                = m_Fn * (double)m_SamplesPerPeriod / (double)m_HBuffSize / PI2;

        // reference frame constants:
        m_cnt     = 0;
        m_ElemNum = m_SamplesPerPeriod - 1;

        // ortohonal components filters initialization:
        m_rmean_re1.init (m_Fs , m_Fn , (m_Fs / m_Fn)       );
        m_rmean_im1.init (m_Fs , m_Fn , (m_Fs / m_Fn)       );
        m_rmean_re2.init (m_Fs , m_Fn , (m_Fs / m_Fn) * 0.5 );
        m_rmean_im2.init (m_Fs , m_Fn , (m_Fs / m_Fn) * 0.5 );

        // frequency slip filter initialization:
        m_fir_df.lp_init ( m_Fs , m_Fn , 5 , 10 * (m_Fs / m_HBuffSize / 250) , true );
        m_fir_df.m_wind.Chebyshev(60);

        // output:
        m_Re = nullptr;
        m_Im = nullptr;
        m_F  = m_Fn;
        m_dF = 0;

        // intial values of filter frequency amplitude response slope compensation function factors:
        m_A1 = 0;
        m_B1 = 0;
        m_C1 = 1;
        m_A2 = 0;
        m_B2 = 0;
        m_C2 = 1;
        m_Km = 1;

        // reference frame:
        m_ref_cos = nullptr;
        m_ref_sin = nullptr;
    }

    /*! \brief default constructor */
    quad_mltpx_rt()
    {
        // variables:
        m_Fs               = 4000;
        m_Ts               = 1 / m_Fs;
        m_Fn               = 50;
        m_F                = m_Fn;
        m_HBuffSize        = m_Fs / 1000 * 5;
        m_SamplesPerPeriod = m_Fs / m_Fn;
        m_K                = m_Fn * m_SamplesPerPeriod / m_HBuffSize / PI2;

        // reference frame constants:
        m_cnt     = 0;
        m_ElemNum = m_SamplesPerPeriod - 1;

        // ortohonal components filters initialization:
        m_rmean_re1.init (m_Fs , m_Fn , (m_Fs / m_Fn)       );
        m_rmean_im1.init (m_Fs , m_Fn , (m_Fs / m_Fn)       );
        m_rmean_re2.init (m_Fs , m_Fn , (m_Fs / m_Fn) * 0.5 );
        m_rmean_im2.init (m_Fs , m_Fn , (m_Fs / m_Fn) * 0.5 );

        // frequency slip filter initialization:
        m_fir_df.lp_init (m_Fs , m_Fn , 5 , 10 * (m_Fs / m_HBuffSize / 250) , true);
        m_fir_df.m_wind.Chebyshev(60);

        // output:
        m_Re = nullptr;
        m_Im = nullptr;
        m_F  = m_Fn;
        m_dF = 0;

        // intial values of filter frequency amplitude response slope compensation function factors:
        m_A1 = 0;
        m_B1 = 0;
        m_C1 = 1;
        m_A2 = 0;
        m_B2 = 0;
        m_C2 = 1;
        m_Km = 1;

        // reference frame:
        m_ref_cos = nullptr;
        m_ref_sin = nullptr;
    }

    /*! \brief memory allocation function */
    __ix32 allocate()
    {
        // filters and ring buffers memory allocation:
        m_rmean_re1.allocate( );
        m_rmean_im1.allocate( );
        m_rmean_re2.allocate( );
        m_rmean_im2.allocate( );
        m_fir_df   .allocate( );
        m_buff_re  .allocate(2);
        m_buff_im  .allocate(2);

        // reference frame memory allocation:
        m_ref_cos = ( __type* )calloc( m_SamplesPerPeriod , sizeof ( __type ) );
        m_ref_sin = ( __type* )calloc( m_SamplesPerPeriod , sizeof ( __type ) );

        // output memory allocation:
        m_Re  = ( __type* )calloc( m_HBuffSize , sizeof ( __type ) );
        m_Im  = ( __type* )calloc( m_HBuffSize , sizeof ( __type ) );

        // reference frame initialization:
        for( __ix32 n = 0; n < m_SamplesPerPeriod ; n++)
        {
            m_ref_sin[n] = sin( PI2 * m_Fn * (__fx64)n / m_Fs) * 2 / sqrt(2);
            m_ref_cos[n] = cos( PI2 * m_Fn * (__fx64)n / m_Fs) * 2 / sqrt(2);
        }

        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response slope compensation function coefficients computation:
        //----------------------------------------------------------------------------------------------------------------------
        // initials:
        //----------------------------------------------------------------------------------------------------------------------
        __fx64 dF1 = 5 ; // slip 1
        __fx64 dF2 = 10; // slip 2
        __fx64 Km1 = 0 ; // frequency response slope compensation function coefficient for slip 1
        __fx64 Km2 = 0 ; // frequency response slope compensation function coefficient for slip 2

        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response slope compensation function coefficients of filter 1:
        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response for dF1, dF2:
        Km1 = m_rmean_re1.freq_resp(dF1).Km;
        Km2 = m_rmean_re1.freq_resp(dF2).Km;

        // amplitude frequency response slope compensation function coefficients:
        m_A1 = ( Km1 * ( dF1 - dF2 ) - (Km1 - Km2) * dF1 - (dF1 - dF2) ) / ( dF1*dF1*(dF1 - dF2) - dF1*(dF1*dF1 - dF2*dF2) );
        m_B1 = ( (Km1 - Km2) - m_A1 * ( dF1 * dF1 - dF2 * dF2 ) ) / ( dF1 - dF2 );
        m_C1 = 1;
        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response slope compensation function coefficients of filter 2:
        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response for dF1, dF2:
        Km1 = m_rmean_re2.freq_resp(dF1).Km;
        Km2 = m_rmean_re2.freq_resp(dF2).Km;

        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response slope compensation function coefficients of total computational chain:
        //----------------------------------------------------------------------------------------------------------------------
        // расчет коэффициентов компенсации амплитудного искажения:
        m_A2 = ( Km1 * ( dF1 - dF2 ) - (Km1 - Km2) * dF1 - (dF1 - dF2) ) / ( dF1*dF1*(dF1 - dF2) - dF1*(dF1*dF1 - dF2*dF2) );
        m_B2 = ( (Km1 - Km2) - m_A2 * ( dF1 * dF1 - dF2 * dF2 ) ) / ( dF1 - dF2 );
        m_C2 = 1;
        //----------------------------------------------------------------------------------------------------------------------

        return 0;
    }

    /*! \brief memory deallocation function */
    __ix32 deallocate()
    {
        // filters and buffers memory deallocation:
        m_rmean_re1.deallocate();
        m_rmean_im1.deallocate();
        m_rmean_re2.deallocate();
        m_rmean_im2.deallocate();
        m_fir_df   .deallocate();
        m_buff_re  .deallocate();
        m_buff_re  .deallocate();

        // reference frame and output memory deallocation:
        if( m_ref_cos != nullptr ) { free( m_ref_cos ); m_ref_cos = nullptr; }
        if( m_ref_sin != nullptr ) { free( m_ref_sin ); m_ref_sin = nullptr; }
        if( m_Re      != nullptr ) { free( m_Re      ); m_Re      = nullptr; }
        if( m_Im      != nullptr ) { free( m_Im      ); m_Im      = nullptr; }

        return 0;
    }

    /*! \brief default destructor */
    ~quad_mltpx_rt() { deallocate(); }


    /*!
     *  \brief template filtering function
     *  \param[input        ] - pointer to the input signal frames
     *  \param[intChanDelay ] - inter channel delay , s
     *  \param[FreqCorr     ] - amplitude frequency response slope compensation flag
    */
    template< typename T > inline void filt( T *input , __fx64 intChanDelay , bool FreqCorr = 0 )
    {
        if( FreqCorr == true )
        {
            for ( __ix32 n = 0 ; n < m_HBuffSize; n++)
            {
                // quadrature multiplication and quadrature components filtering:
                m_rmean_re1( &( m_a = m_ref_sin[m_cnt] * input[n] ) );
                m_rmean_im1( &( m_b = m_ref_cos[m_cnt] * input[n] ) );
                m_rmean_re2( &m_rmean_re1.m_out );
                m_rmean_im2( &m_rmean_im1.m_out );

                // vector computation:
                m_vector1( m_rmean_re2.m_out , m_rmean_im2.m_out );
                m_vector2( 1 , -PI2 * m_F * intChanDelay );
                m_vector3 = m_vector1 * m_vector2;
                m_Re[n]   = m_vector3.m_re;
                m_Im[n]   = m_vector3.m_im;

                // amplitude frequency response slope compensation:
                if( fabs(m_dF) < 5.5 )
                {
                    m_Km = (2 - ( m_A1 * m_dF * m_dF + m_B1 * fabs(m_dF) + m_C1 ) ) * (2 - ( m_A2 * m_dF * m_dF + m_B2 * fabs(m_dF) + m_C2 ) );
                }
                else
                {
                    m_Km = (2 - ( m_A1 * 25 + m_B1 * 5 + m_C1 ) ) * (2 - ( m_A2 * 25 + m_B2 * 5 + m_C2 ) );
                }

                m_Re[n] = m_Re[n] * m_Km;
                m_Im[n] = m_Im[n] * m_Km;

                // reference frame counter zerowing:
                if ( m_cnt < m_ElemNum ) m_cnt++;
                else m_cnt = 0;
            }

            // frequency computation:
            m_buff_re.fill_buff( &m_vector3.m_re  );
            m_buff_im.fill_buff( &m_vector3.m_im  );
            m_vector1( +m_buff_re[1] , -m_buff_im[1] );
            m_vector2( +m_buff_re[0] , +m_buff_im[0] );
            m_vector3 = m_vector1 * m_vector2;
            m_dF = atan2( m_vector3.m_im , m_vector3.m_re ) * m_K;
            m_dF =  m_fir_df( &m_dF );
            m_F = m_Fn + m_dF;
        }
        else  // simplified vector computation
        {
            for ( __ix32 n = 0 ; n < m_HBuffSize; n++)
            {
                // quadrature multiplication:
                m_a = m_ref_sin[m_cnt] * input[n];
                m_b = m_ref_cos[m_cnt] * input[n];

                // reference frame counter zerowing:
                if ( m_cnt < m_ElemNum ) m_cnt++;
                else m_cnt = 0;

                // quadrature components filtering:
                m_rmean_re1( &m_a );
                m_rmean_im1( &m_b );
                m_rmean_re2( &m_rmean_re1.m_out );
                m_rmean_im2( &m_rmean_im1.m_out );

                // vector computation:
                m_vector1( m_rmean_re2.m_out , m_rmean_im2.m_out );
                m_vector2( 1 , -PI2 * m_F * intChanDelay );
                m_vector3 = m_vector1 * m_vector2;
                m_Re[n]   = m_vector3.m_re;
                m_Im[n]   = m_vector3.m_im;
            }
        }
    }

    /*!
     *  \brief 32-bit filtering function
     *  \param[input        ] - pointer to the input signal frames
     *  \param[intChanDelay ] - inter channel delay , s
     *  \param[FreqCorr     ] - amplitude frequency response slope compensation flag
    */
    void process( __type *input , __fx64 intChanDelay , bool FreqCorr = 0 ){ filt< __type >( input , intChanDelay , FreqCorr ); }

    /*!
     *  \brief 64-bit filtering function
     *  \param[input        ] - pointer to the input signal frames
     *  \param[intChanDelay ] - inter channel delay , s
     *  \param[FreqCorr     ] - amplitude frequency response slope compensation flag
    */
    void process( __fx64 *input , __fx64 intChanDelay , bool FreqCorr ){ filt< __fx64 >( input , intChanDelay , FreqCorr ); }

    /*!
     *  \brief filtering function operator
     *  \param[input        ] - pointer to the input signal frames
     *  \param[intChanDelay ] - inter channel delay , s
     *  \param[FreqCorr     ] - amplitude frequency response slope compensation flag
    */
    inline void operator() ( __type *input , __fx64 intChanDelay , bool FreqCorr ){ process( input , intChanDelay , FreqCorr ); }

    /*!
     *  \brief filtering function operator
     *  \param[input        ] - pointer to the input signal frames
     *  \param[intChanDelay ] - inter channel delay , s
     *  \param[FreqCorr     ] - amplitude frequency response slope compensation flag
    */
    inline void operator() ( __fx64 *input , __fx64 intChanDelay , bool FreqCorr ){ process( input , intChanDelay , FreqCorr ); }
};

/*! \brief 32-bit real-time quadrature demodulator realization */
template<> class quad_mltpx<__fx64>
{
    typedef __fx64 __type ;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
     /*! \brief input signal sampling period   , s */
    __fx64 m_Ts;
     /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input buffer size */
    __ix32 m_HBuffSize;
    /*! \brief input signal frames per period of nominal frequency */
    __ix32 m_SamplesPerPeriod;

     /*! \brief auxiliary variable to compute input signal real component */
    __type m_a;
    /*! \brief auxiliary variable to compute input signal imaginary component */
    __type m_b;
    /*! \brief auxiliary variable to compute input signal slip frequency */
    __type m_K;
    /*! \brief auxiliary variable to compensate filter frequency amplitude response slope */
    __type m_Km;

    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_A1;
    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_B1;
    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_C1;

    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_A2;
    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_B2;
    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_C2;

    /*! \brief reference frame counter */
    __ix32 m_cnt;
    /*! \brief reference frame samples per period */
    __ix32 m_ElemNum;

    /*! \brief reference cosine frame */
    __type *m_ref_cos;
    /*! \brief reference sine frame */
    __type *m_ref_sin;

    /*! \brief input signal real component buffer */
    mirror_ring_buffer<__type> m_buff_re;
    /*! \brief input signal imaginary component buffer */
    mirror_ring_buffer<__type> m_buff_im;

    /*! \brief comb filter */
    fcomb<__type> m_apfilt;
    /*! \brief fir filter to extract input signal real component */
    fir<__type> m_fir_re;
    /*! \brief fir filter to extract input signal imaginary component */
    fir<__type> m_fir_im;
    /*! \brief fir filter to smooth the input signal frequency slip */
    fir<__type> m_fir_df;

    /*! \brief auxiliary vector */
    complex< __type > m_vector1;
    /*! \brief auxiliary vector */
    complex< __type > m_vector2;
    /*! \brief auxiliary vector */
    complex< __type > m_vector3;

public:

    /*! \brief input signal real component */
    __type m_Re;
    /*! \brief input signal imaginary component */
    __type m_Im;
    /*! \brief input signal frequency , Hz */
    __type m_F;
    /*! \brief input signal frequency slip , Hz */
    __type m_dF;

    /*!
     *  \brief filter initialization function
     *  \param[Fs        ] - input signal sampling frequency , Hz
     *  \param[Fn        ] - input signal nominal frequency  , Hz
     *  \param[CycleWidth] - IDE cycle duration , ms
    */
    void init( __fx64 Fs, __fx64 Fn, __fx64 SideLobeAtten , __ix32 FiltWindowWidth , __ix32 CycleWidth )
    {
        // variables:
        m_Fs               = Fs;
        m_Ts               = 1 / m_Fs;
        m_Fn               = Fn;
        m_F                = m_Fn;
        m_HBuffSize        = m_Fs / 1000 * CycleWidth;
        m_SamplesPerPeriod = m_Fs / m_Fn;
        m_K                = m_Fn * (__fx64)m_SamplesPerPeriod / (__fx64)m_HBuffSize / PI2;

        // reference frame constants:
        m_cnt     = 0;
        m_ElemNum = m_SamplesPerPeriod - 1;

        // aperiodic component filter initialization:
        m_apfilt.init( m_Fs , m_Fn );

        // ortohonal components filters initialization:
        m_fir_re.lp_init (m_Fs , m_Fn , 0.1 , FiltWindowWidth , true);
        m_fir_im.lp_init (m_Fs , m_Fn , 0.1 , FiltWindowWidth , true);
        m_fir_re.m_wind.Chebyshev(SideLobeAtten);
        m_fir_im.m_wind.Chebyshev(SideLobeAtten);

        // frequency slip filter initialization:
        m_fir_df.lp_init ( m_Fs , m_Fn , 5 , 10 * (m_Fs / m_HBuffSize / 250) , true );
        m_fir_df.m_wind.Chebyshev(60);

        // output:
        m_Re = 0;
        m_Im = 0;
        m_F  = m_Fn;
        m_dF = 0;

        // intial values of filter frequency amplitude response slope compensation function factors:
        m_A1 = 0;
        m_B1 = 0;
        m_C1 = 1;
        m_A2 = 0;
        m_B2 = 0;
        m_C2 = 1;
        m_Km = 1;

        // reference frame:
        m_ref_cos = nullptr;
        m_ref_sin = nullptr;
    }

    /*! \brief default constructor */
    quad_mltpx()
    {
        // variables:
        m_Fs               = 4000;
        m_Ts               = 1 / m_Fs;
        m_Fn               = 50;
        m_F                = m_Fn;
        m_HBuffSize        = m_Fs / 1000 * 5;
        m_SamplesPerPeriod = m_Fs / m_Fn;
        m_K                = m_Fn * m_SamplesPerPeriod / m_HBuffSize / PI2;

        // reference frame constants:
        m_cnt     = 0;
        m_ElemNum = m_SamplesPerPeriod - 1;

        // aperiodic component filter initialization:
        m_apfilt.init( m_Fs , m_Fn );

        // ortohonal components filters initialization:
        m_fir_re.lp_init (m_Fs , m_Fn , 0.1 , 95 , true);
        m_fir_im.lp_init (m_Fs , m_Fn , 0.1 , 95 , true);
        m_fir_re.m_wind.Chebyshev(60);
        m_fir_im.m_wind.Chebyshev(60);

        // frequency slip filter initialization:
        m_fir_df.lp_init ( m_Fs , m_Fn , 5 , 10 * (m_Fs / m_HBuffSize / 250) , true );
        m_fir_df.m_wind.Chebyshev(60);

        // output:
        m_Re = 0;
        m_Im = 0;
        m_F  = m_Fn;
        m_dF = 0;

        // intial values of filter frequency amplitude response slope compensation function factors:
        m_A1 = 0;
        m_B1 = 0;
        m_C1 = 1;
        m_A2 = 0;
        m_B2 = 0;
        m_C2 = 1;
        m_Km = 1;

        // reference frame:
        m_ref_cos = nullptr;
        m_ref_sin = nullptr;
    }

    /*! \brief memory allocation function */
    __ix32 allocate()
    {
        // filters and ring buffers memory allocation:
        m_apfilt .allocate( );
        m_fir_re .allocate( );
        m_fir_im .allocate( );
        m_fir_df .allocate( );
        m_buff_re.allocate(2);
        m_buff_im.allocate(2);

        // reference frame memory allocation:
        m_ref_cos = ( __type* )calloc( m_SamplesPerPeriod , sizeof ( __type ) );
        m_ref_sin = ( __type* )calloc( m_SamplesPerPeriod , sizeof ( __type ) );

        // reference frame initialization:
        for( __ix32 n = 0; n < m_SamplesPerPeriod ; n++)
        {
            m_ref_sin[n] = sin( PI2 * m_Fn * (__fx64)n / m_Fs ) / sqrt(2);
            m_ref_cos[n] = cos( PI2 * m_Fn * (__fx64)n / m_Fs ) / sqrt(2);
        }

        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response slope compensation function coefficients computation:
        //----------------------------------------------------------------------------------------------------------------------
        // initials:
        //----------------------------------------------------------------------------------------------------------------------
        __fx64 dF1 = 5 ; // slip 1
        __fx64 dF2 = 10; // slip 2
        __fx64 Km1 = 0 ; // frequency response slope compensation function coefficient for slip 1
        __fx64 Km2 = 0 ; // frequency response slope compensation function coefficient for slip 2

        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response slope compensation function coefficients of filter 1:
        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response for dF1 , dF2:
        Km1 =  m_fir_re.freq_resp( dF1 ).Km;
        Km2 =  m_fir_re.freq_resp( dF2 ).Km;

        // amplitude frequency response slope compensation function coefficients:
        m_A1 = ( Km1 * ( dF1 - dF2 ) - (Km1 - Km2) * dF1 - (dF1 - dF2) ) / ( dF1*dF1*(dF1 - dF2) - dF1*(dF1*dF1 - dF2*dF2) );
        m_B1 = ( (Km1 - Km2) - m_A1 * ( dF1 * dF1 - dF2 * dF2 ) ) / ( dF1 - dF2 );
        m_C1 = 1;
        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response slope compensation function coefficients of filter 2:
        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response for dF1:
        Km1 = m_apfilt.freq_resp( m_Fn + dF1 ).Km;
        Km2 = m_apfilt.freq_resp( m_Fn + dF1 ).Km;

        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response slope compensation function coefficients of total computational chain:
        //----------------------------------------------------------------------------------------------------------------------
        // расчет коэффициентов компенсации амплитудного искажения:
        m_A2 = ( Km1 * ( dF1 - dF2 ) - (Km1 - Km2) * dF1 - (dF1 - dF2) ) / ( dF1*dF1*(dF1 - dF2) - dF1*(dF1*dF1 - dF2*dF2) );
        m_B2 = ( (Km1 - Km2) - m_A2 * ( dF1 * dF1 - dF2 * dF2 ) ) / ( dF1 - dF2 );
        m_C2 = 1;
        //----------------------------------------------------------------------------------------------------------------------

        return 0;
    }

    /*! \brief memory deallocation function */
    __ix32 deallocate()
    {
        // filters and buffers memory deallocation:
        m_apfilt .deallocate();
        m_fir_re .deallocate();
        m_fir_re .deallocate();
        m_fir_df .deallocate();
        m_buff_re.deallocate();
        m_buff_re.deallocate();

        // reference frame and output memory deallocation:
        if( m_ref_cos != nullptr ) { free( m_ref_cos ); m_ref_cos = nullptr; }
        if( m_ref_sin != nullptr ) { free( m_ref_sin ); m_ref_sin = nullptr; }

        return 0;
    }

    /*! \brief default destructor */
    ~quad_mltpx() { deallocate(); }

    /*!
     *  \brief template filtering function
     *  \param[input        ] - pointer to the input signal frames
     *  \param[intChanDelay ] - inter channel delay , s
     *  \param[FreqCorr     ] - amplitude frequency response slope compensation flag
    */
    template< typename T > inline void filt( T *input , __fx64 intChanDelay )
    {
        for ( __ix32 n = 0 ; n < m_HBuffSize; n++)
        {
            // aperiodic component filtering:
            m_apfilt( &input[n] );

            // quadrature multiplication and FIR filters buffers filling:
            m_fir_re.fill_fir_buff( &( m_a = m_ref_sin[m_cnt] * m_apfilt.m_out ) );
            m_fir_im.fill_fir_buff( &( m_b = m_ref_cos[m_cnt] * m_apfilt.m_out ) );

            // reference frame counter update:
            if (m_cnt < m_ElemNum) m_cnt++;
            else m_cnt = 0;
        }

        // filtering:
        m_fir_re.filt();
        m_fir_im.filt();

        // vector computation:
        m_vector1( m_fir_re.m_out , m_fir_im.m_out );
        m_vector2( 1 , -PI2 * m_F * intChanDelay );
        m_vector3 = m_vector1 * m_vector2;
        m_Re      = m_vector3.m_re;
        m_Im      = m_vector3.m_im;

        // frequency computation:
        m_buff_re.fill_buff( &m_vector3.m_re  );
        m_buff_im.fill_buff( &m_vector3.m_im  );
        m_vector1( +m_buff_re[1] , -m_buff_im[1] );
        m_vector2( +m_buff_re[0] , +m_buff_im[0] );
        m_vector3 = m_vector1 * m_vector2;
        m_dF = atan2( m_vector3.m_im , m_vector3.m_re ) * m_K;
        m_dF =  m_fir_df( &m_dF );
        m_F  = m_Fn + m_dF;

        // filter amplitude frequence response slope compensation:
        if( fabs(m_dF) < 5.5 )
        {
            m_Km = (2 - ( m_A1 * m_dF * m_dF + m_B1 * fabs(m_dF) + m_C1 ) ) * (2 - ( m_A2 * m_dF * m_dF + m_B2 * fabs(m_dF) + m_C2 ) );
        }
        else
        {
            m_Km = (2 - ( m_A1 * 25 + m_B1 * 5 + m_C1 ) ) * (2 - ( m_A2 * 25 + m_B2 * 5 + m_C2 ) );
        }

        m_Re *= m_Km;
        m_Im *= m_Km;
    }

    /*!
     *  \brief 32-bit filtering function
     *  \param[input        ] - pointer to the input signal frames
     *  \param[intChanDelay ] - inter channel delay , s
     *  \param[FreqCorr     ] - amplitude frequency response slope compensation flag
    */
    void process( __type *input , __fx64 intChanDelay ) { filt< __type >( input , intChanDelay ); }

    /*!
     *  \brief filtering function operator
     *  \param[input        ] - pointer to the input signal frames
     *  \param[intChanDelay ] - inter channel delay , s
     *  \param[FreqCorr     ] - amplitude frequency response slope compensation flag
    */
    inline void operator() ( __type *input , __fx64 intChanDelay ){ process( input , intChanDelay ); }
};

/*! \brief 32-bit real-time quadrature demodulator realization */
template<> class quad_mltpx_rt<__fx64>
{
    typedef __fx64 __type ;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
     /*! \brief input signal sampling period   , s */
    __fx64 m_Ts;
     /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input buffer size */
    __ix32 m_HBuffSize;
    /*! \brief input signal frames per period of nominal frequency */
    __ix32 m_SamplesPerPeriod;

     /*! \brief auxiliary variable to compute input signal real component */
    __type m_a;
    /*! \brief auxiliary variable to compute input signal imaginary component */
    __type m_b;
    /*! \brief auxiliary variable to compute input signal slip frequency */
    __type m_K;
    /*! \brief auxiliary variable to compensate filter frequency amplitude response slope */
    __type m_Km;

    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_A1;
    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_B1;
    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_C1;

    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_A2;
    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_B2;
    /*! \brief filter frequency amplitude response slope compensation function coefficient */
    __type m_C2;

    /*! \brief reference frame counter */
    __ix32 m_cnt;
    /*! \brief reference frame samples per period */
    __ix32 m_ElemNum;

    /*! \brief reference cosine frame */
    __type *m_ref_cos;
    /*! \brief reference sine frame */
    __type *m_ref_sin;

    /*! \brief input signal real component buffer */
    mirror_ring_buffer<__type> m_buff_re;
    /*! \brief input signal imaginary component buffer */
    mirror_ring_buffer<__type> m_buff_im;

    /*! \brief recursuve mean filter to extract input signal real component */
    recursive_mean<__type> m_rmean_re1;
    /*! \brief recursuve mean filter to extract input signal imaginary component */
    recursive_mean<__type> m_rmean_im1;
    /*! \brief recursuve mean filter to smooth input signal real component */
    recursive_mean<__type> m_rmean_re2;
    /*! \brief recursuve mean filter to smooth input signal imaginary component */
    recursive_mean<__type> m_rmean_im2;
    /*! \brief fir filter to smooth input signal frequency slip */
    fir<__type> m_fir_df;

    /*! \brief auxiliary vector */
    complex< __type > m_vector1;
    /*! \brief auxiliary vector */
    complex< __type > m_vector2;
    /*! \brief auxiliary vector */
    complex< __type > m_vector3;

public:

    /*! \brief input signal real component */
    __type *m_Re;
    /*! \brief input signal imaginary component */
    __type *m_Im;
    /*! \brief input signal frequency , Hz */
    __type m_F;
    /*! \brief input signal frequency slip , Hz */
    __type m_dF;

    /*!
     *  \brief filter initialization function
     *  \param[Fs        ] - input signal sampling frequency , Hz
     *  \param[Fn        ] - input signal nominal frequency  , Hz
     *  \param[CycleWidth] - IDE cycle duration , ms
    */
    void init( __fx64 Fs, __fx64 Fn, __ix32 CycleWidth )
    {
        // variables:
        m_Fs               = Fs;
        m_Ts               = 1 / m_Fs;
        m_Fn               = Fn;
        m_F                = m_Fn;
        m_HBuffSize        = m_Fs / 1000 * CycleWidth;
        m_SamplesPerPeriod = m_Fs / m_Fn;
        m_K                = m_Fn * (double)m_SamplesPerPeriod / (double)m_HBuffSize / PI2;

        // reference frame constants:
        m_cnt     = 0;
        m_ElemNum = m_SamplesPerPeriod - 1;

        // ortohonal components filters initialization:
        m_rmean_re1.init (m_Fs , m_Fn , (m_Fs / m_Fn)       );
        m_rmean_im1.init (m_Fs , m_Fn , (m_Fs / m_Fn)       );
        m_rmean_re2.init (m_Fs , m_Fn , (m_Fs / m_Fn) * 0.5 );
        m_rmean_im2.init (m_Fs , m_Fn , (m_Fs / m_Fn) * 0.5 );

        // frequency slip filter initialization:
        m_fir_df.lp_init ( m_Fs , m_Fn , 5 , 10 * (m_Fs / m_HBuffSize / 250) , true );
        m_fir_df.m_wind.Chebyshev(60);

        // output:
        m_Re = nullptr;
        m_Im = nullptr;
        m_F  = m_Fn;
        m_dF = 0;

        // intial values of filter frequency amplitude response slope compensation function factors:
        m_A1 = 0;
        m_B1 = 0;
        m_C1 = 1;
        m_A2 = 0;
        m_B2 = 0;
        m_C2 = 1;
        m_Km = 1;

        // reference frame:
        m_ref_cos = nullptr;
        m_ref_sin = nullptr;
    }

    /*! \brief default constructor */
    quad_mltpx_rt()
    {
        // variables:
        m_Fs               = 4000;
        m_Ts               = 1 / m_Fs;
        m_Fn               = 50;
        m_F                = m_Fn;
        m_HBuffSize        = m_Fs / 1000 * 5;
        m_SamplesPerPeriod = m_Fs / m_Fn;
        m_K                = m_Fn * m_SamplesPerPeriod / m_HBuffSize / PI2;

        // reference frame constants:
        m_cnt     = 0;
        m_ElemNum = m_SamplesPerPeriod - 1;

        // ortohonal components filters initialization:
        m_rmean_re1.init (m_Fs , m_Fn , (m_Fs / m_Fn)       );
        m_rmean_im1.init (m_Fs , m_Fn , (m_Fs / m_Fn)       );
        m_rmean_re2.init (m_Fs , m_Fn , (m_Fs / m_Fn) * 0.5 );
        m_rmean_im2.init (m_Fs , m_Fn , (m_Fs / m_Fn) * 0.5 );

        // frequency slip filter initialization:
        m_fir_df.lp_init (m_Fs , m_Fn , 5 , 10 * (m_Fs / m_HBuffSize / 250) , true);
        m_fir_df.m_wind.Chebyshev(60);

        // output:
        m_Re = nullptr;
        m_Im = nullptr;
        m_F  = m_Fn;
        m_dF = 0;

        // intial values of filter frequency amplitude response slope compensation function factors:
        m_A1 = 0;
        m_B1 = 0;
        m_C1 = 1;
        m_A2 = 0;
        m_B2 = 0;
        m_C2 = 1;
        m_Km = 1;

        // reference frame:
        m_ref_cos = nullptr;
        m_ref_sin = nullptr;
    }

    /*! \brief memory allocation function */
    __ix32 allocate()
    {
        // filters and ring buffers memory allocation:
        m_rmean_re1.allocate( );
        m_rmean_im1.allocate( );
        m_rmean_re2.allocate( );
        m_rmean_im2.allocate( );
        m_fir_df   .allocate( );
        m_buff_re  .allocate(2);
        m_buff_im  .allocate(2);

        // reference frame memory allocation:
        m_ref_cos = ( __type* )calloc( m_SamplesPerPeriod , sizeof ( __type ) );
        m_ref_sin = ( __type* )calloc( m_SamplesPerPeriod , sizeof ( __type ) );

        // output memory allocation:
        m_Re  = ( __type* )calloc( m_HBuffSize , sizeof ( __type ) );
        m_Im  = ( __type* )calloc( m_HBuffSize , sizeof ( __type ) );

        // reference frame initialization:
        for( __ix32 n = 0; n < m_SamplesPerPeriod ; n++)
        {
            m_ref_sin[n] = sin( PI2 * m_Fn * (__fx64)n / m_Fs) * 2 / sqrt(2);
            m_ref_cos[n] = cos( PI2 * m_Fn * (__fx64)n / m_Fs) * 2 / sqrt(2);
        }

        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response slope compensation function coefficients computation:
        //----------------------------------------------------------------------------------------------------------------------
        // initials:
        //----------------------------------------------------------------------------------------------------------------------
        __fx64 dF1 = 5 ; // slip 1
        __fx64 dF2 = 10; // slip 2
        __fx64 Km1 = 0 ; // frequency response slope compensation function coefficient for slip 1
        __fx64 Km2 = 0 ; // frequency response slope compensation function coefficient for slip 2

        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response slope compensation function coefficients of filter 1:
        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response for dF1, dF2:
        Km1 = m_rmean_re1.freq_resp(dF1).Km;
        Km2 = m_rmean_re1.freq_resp(dF2).Km;

        // amplitude frequency response slope compensation function coefficients:
        m_A1 = ( Km1 * ( dF1 - dF2 ) - (Km1 - Km2) * dF1 - (dF1 - dF2) ) / ( dF1*dF1*(dF1 - dF2) - dF1*(dF1*dF1 - dF2*dF2) );
        m_B1 = ( (Km1 - Km2) - m_A1 * ( dF1 * dF1 - dF2 * dF2 ) ) / ( dF1 - dF2 );
        m_C1 = 1;
        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response slope compensation function coefficients of filter 2:
        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response for dF1, dF2:
        Km1 = m_rmean_re2.freq_resp(dF1).Km;
        Km2 = m_rmean_re2.freq_resp(dF2).Km;

        //----------------------------------------------------------------------------------------------------------------------
        // amplitude frequency response slope compensation function coefficients of total computational chain:
        //----------------------------------------------------------------------------------------------------------------------
        // расчет коэффициентов компенсации амплитудного искажения:
        m_A2 = ( Km1 * ( dF1 - dF2 ) - (Km1 - Km2) * dF1 - (dF1 - dF2) ) / ( dF1*dF1*(dF1 - dF2) - dF1*(dF1*dF1 - dF2*dF2) );
        m_B2 = ( (Km1 - Km2) - m_A2 * ( dF1 * dF1 - dF2 * dF2 ) ) / ( dF1 - dF2 );
        m_C2 = 1;
        //----------------------------------------------------------------------------------------------------------------------

        return 0;
    }

    /*! \brief memory deallocation function */
    __ix32 deallocate()
    {
        // filters and buffers memory deallocation:
        m_rmean_re1.deallocate();
        m_rmean_im1.deallocate();
        m_rmean_re2.deallocate();
        m_rmean_im2.deallocate();
        m_fir_df   .deallocate();
        m_buff_re  .deallocate();
        m_buff_re  .deallocate();

        // reference frame and output memory deallocation:
        if( m_ref_cos != nullptr ) { free( m_ref_cos ); m_ref_cos = nullptr; }
        if( m_ref_sin != nullptr ) { free( m_ref_sin ); m_ref_sin = nullptr; }
        if( m_Re      != nullptr ) { free( m_Re      ); m_Re      = nullptr; }
        if( m_Im      != nullptr ) { free( m_Im      ); m_Im      = nullptr; }

        return 0;
    }

    /*! \brief default destructor */
    ~quad_mltpx_rt() { deallocate(); }

    /*!
     *  \brief template filtering function
     *  \param[input        ] - pointer to the input signal frames
     *  \param[intChanDelay ] - inter channel delay , s
     *  \param[FreqCorr     ] - amplitude frequency response slope compensation flag
    */
    template< typename T > inline void filt( T *input , __fx64 intChanDelay , bool FreqCorr = 0 )
    {
        if( FreqCorr == true )
        {
            for ( __ix32 n = 0 ; n < m_HBuffSize; n++)
            {
                // quadrature multiplication and quadrature components filtering:
                m_rmean_re1( &( m_a = m_ref_sin[m_cnt] * input[n] ) );
                m_rmean_im1( &( m_b = m_ref_cos[m_cnt] * input[n] ) );
                m_rmean_re2( &m_rmean_re1.m_out );
                m_rmean_im2( &m_rmean_im1.m_out );

                // vector computation:
                m_vector1( m_rmean_re2.m_out , m_rmean_im2.m_out );
                m_vector2( 1 , -PI2 * m_F * intChanDelay );
                m_vector3 = m_vector1 * m_vector2;
                m_Re[n]   = m_vector3.m_re;
                m_Im[n]   = m_vector3.m_im;

                // amplitude frequency response slope compensation:
                if( fabs(m_dF) < 5.5 )
                {
                    m_Km = (2 - ( m_A1 * m_dF * m_dF + m_B1 * fabs(m_dF) + m_C1 ) ) * (2 - ( m_A2 * m_dF * m_dF + m_B2 * fabs(m_dF) + m_C2 ) );
                }
                else
                {
                    m_Km = (2 - ( m_A1 * 25 + m_B1 * 5 + m_C1 ) ) * (2 - ( m_A2 * 25 + m_B2 * 5 + m_C2 ) );
                }

                m_Re[n] = m_Re[n] * m_Km;
                m_Im[n] = m_Im[n] * m_Km;

                // reference frame counter zerowing:
                if ( m_cnt < m_ElemNum ) m_cnt++;
                else m_cnt = 0;
            }

            // frequency computation:
            m_buff_re.fill_buff( &m_vector3.m_re  );
            m_buff_im.fill_buff( &m_vector3.m_im  );
            m_vector1( +m_buff_re[1] , -m_buff_im[1] );
            m_vector2( +m_buff_re[0] , +m_buff_im[0] );
            m_vector3 = m_vector1 * m_vector2;
            m_dF = atan2( m_vector3.m_im , m_vector3.m_re ) * m_K;
            m_dF =  m_fir_df( &m_dF );
            m_F = m_Fn + m_dF;
        }
        else  // simplified vector computation
        {
            for ( __ix32 n = 0 ; n < m_HBuffSize; n++)
            {
                // quadrature multiplication:
                m_a = m_ref_sin[m_cnt] * input[n];
                m_b = m_ref_cos[m_cnt] * input[n];

                // reference frame counter zerowing:
                if ( m_cnt < m_ElemNum ) m_cnt++;
                else m_cnt = 0;

                // quadrature components filtering:
                m_rmean_re1( &m_a );
                m_rmean_im1( &m_b );
                m_rmean_re2( &m_rmean_re1.m_out );
                m_rmean_im2( &m_rmean_im1.m_out );

                // vector computation:
                m_vector1( m_rmean_re2.m_out , m_rmean_im2.m_out );
                m_vector2( 1 , -PI2 * m_F * intChanDelay );
                m_vector3 = m_vector1 * m_vector2;
                m_Re[n]   = m_vector3.m_re;
                m_Im[n]   = m_vector3.m_im;
            }
        }
    }

    /*!
     *  \brief 32-bit filtering function
     *  \param[input        ] - pointer to the input signal frames
     *  \param[intChanDelay ] - inter channel delay , s
     *  \param[FreqCorr     ] - amplitude frequency response slope compensation flag
    */
    void process( __type *input , __fx64 intChanDelay , bool FreqCorr = 0 ){ filt< __type >( input , intChanDelay , FreqCorr ); }

    /*!
     *  \brief filtering function operator
     *  \param[input        ] - pointer to the input signal frames
     *  \param[intChanDelay ] - inter channel delay , s
     *  \param[FreqCorr     ] - amplitude frequency response slope compensation flag
    */
    inline void operator() ( __type *input , __fx64 intChanDelay , bool FreqCorr ){ process( input , intChanDelay , FreqCorr ); }
};

#endif // QUAD_MLTPX_H
