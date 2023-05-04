#ifndef FILTERS_FIR_H
#define FILTERS_FIR_H

#include "kernel_fir.h"
#include "special_functions.h"
using namespace FIR_KERNEL;

#ifndef __ALG_PLATFORM
#define FIR_FILTERS_DEBUG // debugging is not available if the algorithm is running on a device !!!
#endif

/*! \defgroup <CLASSIC_FIR_FILTERS> ( classic FIR filters)
 *  \ingroup FILTERS
 *  \brief The module contains abstract model and implementation of the classic FIR filter
    @{
*/

/*! @} */

/*! \defgroup <WINDOW_FUNCTIONS_HIGH_LEVEL_SHELL> ( Window functions high level shell )
 *  \ingroup CLASSIC_FIR_FILTERS
 *  \brief the module contains the high level shell for the sindow functions
    @{
*/

/*!
 *  \class window_function
 *  \brief Defines window functions high level shell
*/
class window_function final
{
    // info
    int64_t m_Order  = 80;
    double *m_Window = nullptr;

    // service methods
    void copy(const window_function& _Window)
    {
        m_Order  = _Window.m_Order;
        m_Window = __mfree__(m_Window);
        m_Window = __alloc__<double>(m_Order);

        for(int64_t i = 0 ; i < m_Order; i++)
        {
            m_Window[i] = _Window.m_Window[i];
        }
    }

public:

    /*! \brief default constructor */
    window_function(){}

    /*!
     *  \brief copy constructor
     *  \param[window] input window function
    */
    window_function(const window_function& _Window)
    {
        copy(_Window);
    }

    /*! \brief default destructor */
    virtual ~window_function()
    {
        m_Window = __mfree__(m_Window);
    }

    /*! \brief returns window function coefficients vector */
    double* get_window_function()
    {
        return m_Window;
    }

     /*! \brief returns window order */
    int64_t get_order()
    {
        return m_Order;
    }

    /*!
     * \brief Barlett window function computation
     * \param[_Order] window function size
    */
    void Bartlett( int64_t _Order )
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Bartlett()", "Bartlett window function generation");
        #endif

        m_Order  = _Order;
        m_Window = __mfree__(m_Window);
        m_Window = __Bartlett__(_Order);
    }

    /*!
     * \brief Barlett Hanning window function computation
     * \param[_Order] window function size
    */
    void BartlettHanning( int64_t _Order )
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","BartlettHanning()", "Bartlett-Hanning window function generation");
        #endif

        m_Order  = _Order;
        m_Window = __mfree__(m_Window);
        m_Window = __BartlettHanning__(_Order);
    }

    /*!
    * \brief Balckman window function computation
    * \param[_Order] window function size
    */
    void Blackman( int64_t _Order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Blackman()", "Blackman window function generation");
        #endif

        m_Order  = _Order;
        m_Window = __mfree__(m_Window);
        m_Window = __Blackman__(_Order);
    }

    /*!
     * \brief Balckman-Harris window function computation
     * \param[_Order] window function size
    */
    void BlackmanHarris( int64_t _Order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","BlackmanHarris()", "Blackman-Harris window function generation");
        #endif

        m_Order  = _Order;
        m_Window = __mfree__(m_Window);
        m_Window = __BlackmanHarris__(_Order);
    }

    /*!
    * \brief Bohman window function computation
    * \param[_Order] window function size
    */
    void Bohman(int64_t _Order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Bohman()", "Bohman window function generation");
        #endif

        m_Order  = _Order;
        m_Window = __mfree__(m_Window);
        m_Window = __Bohman__(_Order);
    }

    /*!
      * \brief Flat-Top window computation function
      * \param[_Order] window function size
    */
    void FlatTop( int64_t _Order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","FlatTop()", "FlatTop window function generation");
        #endif

        m_Order  = _Order;
        m_Window = __mfree__(m_Window);
        m_Window = __FlatTop__(_Order);
    }

    /*!
      * \brief Hamming window computation function
      * \param[_Order] window function size
    */
    void Hamming( int64_t _Order )
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Hamming()", "Hamming window function generation");
        #endif

        m_Order  = _Order;
        m_Window = __mfree__(m_Window);
        m_Window = __Hamming__(_Order);
    }

    /*!
     * \brief Hann window computation function
     * \param[_Order] window function size
    */
    void Hann( int64_t _Order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Hann()", "Hann window function generation");
        #endif

        m_Order  = _Order;
        m_Window = __mfree__(m_Window);
        m_Window = __Hann__(_Order);
    }

    /*!
     * \brief Nutall window computation function
     * \param[_Order] window function size
    */
    void Nutall( int64_t _Order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Nutall()", "Nutall window function generation");
        #endif

        m_Order  = _Order;
        m_Window = __mfree__(m_Window);
        m_Window = __Nutall__(_Order);
    }

    /*!
     * \brief Parzen window computation function
     * \param[_Order] window function size
    */
    void Parzen( int64_t _Order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Parzen()", "Parzen window function generation");
        #endif

        m_Order  = _Order;
        m_Window = __mfree__(m_Window);
        m_Window = __Parzen__(_Order);
    }

    /*!
     * \brief Rectangular window computation function
     * \param[_Order] window function size
    */
    void Rectangular( int64_t _Order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Rectangular()", "Rectangular window function generation");
        #endif

        m_Order  = _Order;
        m_Window = __mfree__(m_Window);
        m_Window = __Rectangular__(_Order);
    }

    /*!
    * \brief Triangular window computation function
    * \param[_Order] window function size
    */
    void Triangular( int64_t _Order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Triangular()", "Triangular window function generation");
        #endif

        m_Order  = _Order;
        m_Window = __mfree__(m_Window);
        m_Window = __Triangular__(_Order);
    }

    /*!
    * \brief Chebyshev window computation function
    * \param[_Attenuation] sidelobe attenuation , Db
    * \param[_Order] window function size
    */
    void Chebyshev( double _Attenuation, int64_t _Order )
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Chebyshev()", "Chebyshev window function generation");
        #endif

        m_Order  = _Order;
        m_Window = __mfree__(m_Window);
        m_Window = __Chebyshev__(_Attenuation, _Order);
    }

    /*!
     * \brief Gaussian window computation function
     * \param[_Alpha] Gaussian window parameter
     * \param[_Order] window function size
    */
    void Gaussian( double _Alpha, int64_t _Order )
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Gaussian()", "Gaussian window function generation");
        #endif

        m_Order  = _Order;
        m_Window = __mfree__(m_Window);
        m_Window = __Gaussian__(_Alpha, _Order);
    }

    /*!
     * \brief Kaiser window computation function
     * \param[_Beta] Kaiser window function parameter
     * \param[_Order] window function size
    */
    void Kaiser( double _Beta, int64_t _Order )
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Kaiser()", "Kaiser window function generation");
        #endif

        m_Order  = _Order;
        m_Window = __mfree__(m_Window);
        m_Window = __Kaiser__(_Beta, _Order);
    }

    /*!
     * \brief Tukey window computation function
     * \param[_R] Tukey window function parameter
     * \param[_Order] window function size
    */
    void Tukey( double _R, int64_t _Order )
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Tukey()", "Tukey window function generation");
        #endif

        m_Order  = _Order;
        m_Window = __mfree__(m_Window);
        m_Window = __Tukey__(_R, _Order);
    }

    inline void operator = ( window_function& _Window)
    {
        copy(_Window);
    }
};

/*!
 *  \example example_classic_fir.h
*/

/*! @} */

/*! \defgroup <CLASSIC_FIR_FILTER_ABSTRACT_MODEL> ( Classic FIR filter abstract model )
 *  \ingroup CLASSIC_FIR_FILTERS
 *  \brief the module contains the abstract model of the classic FIR filter
    @{
*/

/*!  \brief defines the base of the FIR filters */
template< typename __type >
class fir final : public  transfer_function_model, classic_filter_interface
{
protected:

    filter_type         m_FilterType;
    filter_data<__type> m_FilterData;
    window_function      m_WindowFunction;
    bandwidth           m_Bandwidth;
    int64_t             m_Scale;
    delay<__type>       m_buff_sx;

public:

    /*! \brief default constructor */
    fir() : transfer_function_model()
    {
        m_FilterType   = filter_type::lowpass;
        m_Bandwidth    = { 100 , 500 };
    }

    /*! \brief default destructor */
    virtual ~fir()
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("classic_fir_base","deallocate()", "fir filter memory deallocation");
        #endif

        m_FilterData = __dsp_clear_filter__(m_FilterData);
    }

    /*!
     *   \brief initializes the classic FIR filter
     *   \param[Fs] filter sampling frequency, Hz
     *   \param[FilterType] filter type
     *   \param[Bandwidth] filter stopband/passband width
     *   \param[WindowFunction] filter window function
     *   \param[Scale] scale ot not to scale the filter coefficients
     *   \details The function is supposed to be called explicitly by the user before the filter resources are allocated
    */
    void init( double Fs, filter_type FilterType, bandwidth Bandwidth, window_function& WindowFunction, int64_t Scale )
    {
        m_FilterType     = FilterType;
        m_Bandwidth      = Bandwidth;
        m_Scale          = Scale;
        m_WindowFunction = WindowFunction;
        transfer_function_model::init(WindowFunction.get_order(), Fs);

        // allocation
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("classic_fir_base","allocate()", "fir filter memory allocation");
        #endif

        m_FilterData = round_coefficients<__type>(m_FilterType);
    }

    /*!
     *  \brief frequency responce computation function
     *  \param[F] input frequency, Hz
     *  \details The function computes the filter complex transfer function value for the given frequency
    */
    Complex<double> frequency_response( double F ) override
    {
        return __freq_resp__<__type> ( m_FilterData.cfnum , m_Order, m_Fs , F  );
    }

    /*!
     *  \brief FIR filter buffer fill operator
     *  \param[input] input samples pointer
     *  \details The operator is supposed to be used to fill the FIR filter buffer
    */
    virtual inline void operator <<  ( __type *_input )
    {
        m_buff_sx( _input );
    }

    /*!
     *  \brief FIR filter filtering operator
     *  \param[input] input samples pointer
     *  \details The operator is supposed to be used to filt the input signal
    */
    inline __type operator() ( __type* _input )
    {
        return __filt__< __type, __type >( _input, m_FilterData.cfnum, m_buff_sx, m_FilterData.N );
    }

    /*!
     *  \brief FIR filter filtering operator
     *  \details The operator is supposed to be used to filt the input signal right after the FIR filter buffer is filled.
    */
    inline __type operator() ()
    {
        return __filt__ < __type > ( m_FilterData.cfnum , m_buff_sx , m_FilterData.N );
    }

    virtual filter_data< double > compute_lowpass () override
    {
        return __fir_wind_digital_lp__<double>( m_Fs , m_Bandwidth.Fc , m_Order , m_Scale , m_WindowFunction.get_window_function() );
    }

    virtual filter_data< double > compute_highpass() override
    {
        return __fir_wind_digital_hp__<double>( m_Fs , m_Bandwidth.Fc , m_Order , m_Scale , m_WindowFunction.get_window_function() );
    }

    virtual filter_data< double > compute_bandpass() override
    {
        return __fir_wind_digital_bp__<double>( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_Order , m_Scale , m_WindowFunction.get_window_function() );
    }

    virtual filter_data< double > compute_bandstop() override
    {
        return __fir_wind_digital_bs__<double>( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_Order , m_Scale , m_WindowFunction.get_window_function() );
    }

    #ifndef __ALG_PLATFORM
        void show()
        {
            __show_fir__( m_FilterData );
        }
    #endif
};

/*!
 *  \example example_classic_fir.h
*/

/*! @} */

#endif // FILTERS_FIR_H
