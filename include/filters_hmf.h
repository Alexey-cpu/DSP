#ifndef FILTERS_HMF_H
#define FILTERS_HMF_H

// include recursive Fourier filter
#include "filters_rff.h"

#ifndef __ALG_PLATFORM
#define HMF_DEBUG // debugging is not available if the algorithm is running on a device !!!
#endif

/*! \defgroup <HARMONIC_FILTER> ( harmonic filter )
 *  \ingroup SPECIAL_FILTERS
 *  \brief The module contains implementation of the harmonic filter
    @{
*/

/*!
 *  \class harmonic_filter
 *  \brief defines harmonic filter based on recursive Fourier filter
 *  \details The filter computes a given harmonic root-mean-square (RMS) value. Recursive fourier algorithm is used as the filter kernel.
 *           The filter is supposed to be used within the relay protection IDE's. The filter operates according the following algorithm:
 *           On every IDE operation step the input ADC buffer is transferred to the recusive Fourier filter.
 *           Recursive Fourier filter computes the input signal vector without convolution with any reference frame.
 *           The real and imaginary part of the mentioned uncovoluted vector are then transferred to the
 *           RMS filter. The RMS filter outputs the average of RMS values of real and imaginary parts RMS values.
*/

template<typename __type>
class harmonic_filter final
{

private:

    /*! \brief filte epsilon value ( the filte lowest operation value ) */
    __type m_epsilon   = 1e-4;
    __type m_max_value = +sqrt( 3.4028235E+38 / 2 );
    __type m_min_value = -sqrt( 3.4028235E+38 / 2 );

    // filters
    recursive_fourier<__type> *m_rdft;     ///< recursive Fourier filters ( each for it's own frequency )
    recursive_fourier<__type>  m_rmean_re; ///< zero-frequency recursive Fourier filter
    recursive_fourier<__type>  m_rmean_im; ///< zero-frequency recursive Fourier filter

    // system variables
    double  m_Fs;               ///< recurisve Fourier sampling frequency, Hz
    double  m_Fn;               ///< recurisve Fourier reference signal frequency, Hz
    int64_t m_HBuffSize;        ///< IDE ADC buffer size
    int64_t m_CycleWidth;       ///< IDE ADC cycle width
    int64_t m_SpectrumWidth;    ///< harmonic filter spectrum width
    int64_t m_SamplesPerPeriod; ///< sample per period of the reference signal
    __type  m_Module;           ///< harmonic RMS value

    // debugging output
    __type *m_Re; ///< recursive Fourier output vector real part
    __type *m_Im; ///< recursive Fourier output vector imaginary part


    /*! \brief allocates filter resources */
    int64_t allocate()
    {
        #ifdef HMF_DEBUG
        Debugger::Log("harmonic_filter","allocate()","Memory allocation");
        #endif

        // allocate debugging output
        m_Re = __alloc__<__type>(m_HBuffSize);
        m_Im = __alloc__<__type>(m_HBuffSize);

        // initialize and allocate recursive Fourier filter
        m_rdft = new recursive_fourier<__type>[m_SpectrumWidth];
        for( int64_t i = 0 ; i < m_SpectrumWidth ; i++ )
        {
            m_rdft[i].init( m_Fs, m_Fn, i );
        }

        // initialize and allocate recursive mean filters
        m_rmean_re.init( m_Fs, m_Fn, 0);
        m_rmean_im.init( m_Fs, m_Fn, 0);

        return 1;
    }

    /*! \brief allocates filter resources */
    int64_t deallocate()
    {

        #ifdef HMF_DEBUG
        Debugger::Log("harmonic_filter","deallocate()","Memory free");
        #endif

        // free debugging outputs
        m_Re = __mfree__(m_Re);
        m_Im = __mfree__(m_Im);

        // clear recursive Fourier filter
        if( m_rdft != nullptr )
        {
            delete [] m_rdft; // delete operator calls object destructor which frees the object resources
            m_rdft = nullptr;
        }

        return 1;
    }

public:

    /*!
     *  \brief default constructor
    */
    harmonic_filter()
    {
        #ifdef HMF_DEBUG
        Debugger::Log("harmonic_filter","harmonic_filter()","Filter constructor call");
        #endif

        m_Fs               = 4000;
        m_Fn               = 50;
        m_HBuffSize        = 20;
        m_SpectrumWidth    = 41;
        m_SamplesPerPeriod = m_Fs / m_Fn;
        m_CycleWidth	   = m_HBuffSize / m_Fs * 1000;
        m_rdft             = nullptr;
        m_Re               = nullptr;
        m_Im               = nullptr;
    }

    /*!
     *  \brief default destructor
    */
    virtual ~harmonic_filter()
    {
        #ifdef HMF_DEBUG
        Debugger::Log("harmonic_filter","~harmonic_filter()","Filter destructor call");
        #endif

        deallocate();
    }

    /*! \brief outputs harmonic RMS value */
    __type module()
    {
        return m_Module;
    }

    /*!
     *  \brief outputs harmonic recursive Fourier vector real part
     *  \param[n] recursive Fourier vector real part sample number
    */
    __type real(int64_t n)
    {
        return m_Re[n];
    }

    /*!
     *  \brief outputs harmonic recursive Fourier vector imaginary part
     *  \param[n] recursive Fourier vector imaginary part sample number
    */
    __type imag(int64_t n)
    {
        return m_Im[n];
    }

    /*!
     *  \brief initializes the harmonic filter
     *  \param[Fs] filter sampling frequency, Hz
     *  \param[Fn] filter reference signal frequency, Hz
     *  \param[HBuffSize] IDE's ADC buffer size
     *  \param[SpectrumWidth] filter spectrum width
     *  \details The function is supposed to be called explicitly by the user
     *           before filter resources are allocated
    */
    void init(double Fs, double Fn, int64_t HBuffSize, int64_t SpectrumWidth)
    {
        m_Fs               = Fs;
        m_Fn               = Fn;
        m_HBuffSize        = HBuffSize;
        m_SpectrumWidth    = SpectrumWidth;
        m_SamplesPerPeriod = m_Fs / m_Fn;
        m_CycleWidth	   = m_HBuffSize / m_Fs * 1000;
        m_rdft             = nullptr;
        m_Re               = nullptr;
        m_Im               = nullptr;

        #ifdef HMF_DEBUG
        Debugger::Log("harmonic_filter","init()","Filter initialization");
        Debugger::Log("Fs               = " + to_string(m_Fs));
        Debugger::Log("Fn               = " + to_string(m_Fn));
        Debugger::Log("HBuffSize        = " + to_string(m_HBuffSize));
        Debugger::Log("SpectrumWidth    = " + to_string(m_SpectrumWidth));
        Debugger::Log("SamplesPerPeriod = " + to_string(m_SamplesPerPeriod));
        Debugger::Log("CycleWidth       = " + to_string(m_CycleWidth));
        #endif

        //
        allocate();

    }

    /*!
     *  \brief initializes the harmonic filter
     *  \param[input] input signal
     *  \param[harmonic_num] harmonic number
     *  \details The function is supposed to be called
     *           every IDE's operation step
    */
    void filt(int64_t harmonic_num, double *input)
    {
        // check input
        if( input == nullptr )
        {
            return;
        }

        // check input
        if( harmonic_num >= m_SpectrumWidth ) // may be add error code here
        {
            harmonic_num = m_SpectrumWidth - 1;
        }

        // filter input
        for( int64_t i = 0 ; i < m_HBuffSize ; i++ )
        {
            // saturation ( keeps the signal value between the given Upper and Lower limits )
            __type signal = __saturation__(input[i], m_max_value, m_min_value);

            // filtering
            Complex<__type> vector = m_rdft[harmonic_num](&signal);
            double a = __realf__(vector) * __realf__(vector);
            double b = __imagf__(vector) * __imagf__(vector);
            m_rmean_re(&a);
            m_rmean_im(&b);

            // generate dubugging output
            m_Re[i] = __realf__(vector);
            m_Im[i] = __imagf__(vector);
        }

        // generating output:
        if( harmonic_num == 0 )
        {
            double a = __abs__( __realf__( m_rmean_re.vector() ) );
            m_Module = ( a < m_epsilon ) ? (__type)0 : sqrt( a );
        }
        else
        {
            double a = __abs__( __realf__( m_rmean_re.vector() ) );
            double b = __abs__( __realf__( m_rmean_im.vector() ) );
            m_Module = ( ( a < m_epsilon ) && ( b < m_epsilon ) ) ? (__type)0 : sqrt( ( a + b ) * (__type)0.5 );
        }
    }
};

/*! @} */

#endif // FILTERS_HMF_H
