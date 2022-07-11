#ifndef FILTERS_FIR_H
#define FILTERS_FIR_H

#include "kernel_fir.h"
#include "special_functions.h"
using namespace FIR_KERNEL;

#ifndef __ALG_PLATFORM
//#define FIR_FILTERS_DEBUG // debugging is not available if the algorithm is running on a device !!!
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

#ifndef PI0
#define PI0 3.1415926535897932384626433832795
#endif

#ifndef PI2
#define PI2 6.283185307179586476925286766559
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
class window_function
{
    __ix32  m_order  = 80;
    __fx64 *m_window = nullptr;
public:

    /*! \brief returns window function coefficients vector */
    __fx64 *WindowFunction()
    {
        return m_window;
    }

     /*! \brief returns window order */
    __ix32 Order()
    {
        return m_order;
    }

    /*!
     * \brief Barlett window function computation
     * \param[order] window function size
    */
    void Bartlett( __ix32 order )
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Bartlett()", "Bartlett window function generation");
        #endif

        m_order  = order;
        m_window = __mfree__(m_window);
        m_window = __Bartlett__(order);
    }

    /*!
     * \brief Barlett Hanning window function computation
     * \param[order] window function size
    */
    void BartlettHanning( __ix32 order )
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","BartlettHanning()", "Bartlett-Hanning window function generation");
        #endif

        m_order  = order;
        m_window = __mfree__(m_window);
        m_window = __BartlettHanning__(order);
    }

    /*!
    * \brief Balckman window function computation
    * \param[order] window function size
    */
    void Blackman(__ix32 order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Blackman()", "Blackman window function generation");
        #endif

        m_order  = order;
        m_window = __mfree__(m_window);
        m_window = __Blackman__(order);
    }

    /*!
     * \brief Balckman-Harris window function computation
     * \param[order] window function size
    */
    void BlackmanHarris(__ix32 order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","BlackmanHarris()", "Blackman-Harris window function generation");
        #endif

        m_order  = order;
        m_window = __mfree__(m_window);
        m_window = __BlackmanHarris__(order);
    }

    /*!
    * \brief Bohman window function computation
    * \param[order] window function size
    */
    void Bohman(__ix32 order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Bohman()", "Bohman window function generation");
        #endif

        m_order  = order;
        m_window = __mfree__(m_window);
        m_window = __Bohman__(order);
    }

    /*!
      * \brief Flat-Top window computation function
      * \param[order] window function size
    */
    void FlatTop(__ix32 order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","FlatTop()", "FlatTop window function generation");
        #endif

        m_order  = order;
        m_window = __mfree__(m_window);
        m_window = __FlatTop__(order);
    }

    /*!
      * \brief Hamming window computation function
      * \param[order] window function size
    */
    void Hamming(__ix32 order )
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Hamming()", "Hamming window function generation");
        #endif

        m_order  = order;
        m_window = __mfree__(m_window);
        m_window = __Hamming__(order);
    }

    /*!
     * \brief Hann window computation function
     * \param[order] window function size
    */
    void Hann(__ix32 order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Hann()", "Hann window function generation");
        #endif

        m_order  = order;
        m_window = __mfree__(m_window);
        m_window = __Hann__(order);
    }

    /*!
     * \brief Nutall window computation function
     * \param[order] window function size
    */
    void Nutall(__ix32 order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Nutall()", "Nutall window function generation");
        #endif

        m_order  = order;
        m_window = __mfree__(m_window);
        m_window = __Nutall__(order);
    }

    /*!
     * \brief Parzen window computation function
     * \param[order] window function size
    */
    void Parzen(__ix32 order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Parzen()", "Parzen window function generation");
        #endif

        m_order  = order;
        m_window = __mfree__(m_window);
        m_window = __Parzen__(order);
    }

    /*!
     * \brief Rectangular window computation function
     * \param[order] window function size
    */
    void Rectangular(__ix32 order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Rectangular()", "Rectangular window function generation");
        #endif

        m_order  = order;
        m_window = __mfree__(m_window);
        m_window = __Rectangular__(order);
    }

    /*!
    * \brief Triangular window computation function
    * \param[order] window function size
    */
    void Triangular(__ix32 order)
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Triangular()", "Triangular window function generation");
        #endif

        m_order  = order;
        m_window = __mfree__(m_window);
        m_window = __Triangular__(order);
    }

    /*!
    * \brief Chebyshev window computation function
    * \param[atten] sidelobe attenuation , Db
    * \param[order] window function size
    */
    void Chebyshev( __fx64 atten , __ix32 order )
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Chebyshev()", "Chebyshev window function generation");
        #endif

        m_order  = order;
        m_window = __mfree__(m_window);
        m_window = __Chebyshev__(atten, order);
    }

    /*!
     * \brief Gaussian window computation function
     * \param[alpha] Gaussian window parameter
     * \param[order] window function size
    */
    void Gaussian( __fx64 alpha, __ix32 order )
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Gaussian()", "Gaussian window function generation");
        #endif

        m_order  = order;
        m_window = __mfree__(m_window);
        m_window = __Gaussian__(alpha, order);
    }

    /*!
     * \brief Kaiser window computation function
     * \param[betta] Kaiser window function parameter
     * \param[order] window function size
    */
    void Kaiser(__fx64 betta, __ix32 order )
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Kaiser()", "Kaiser window function generation");
        #endif

        m_order  = order;
        m_window = __mfree__(m_window);
        m_window = __Kaiser__(betta, order);
    }

    /*!
     * \brief Tukey window computation function
     * \param[_R] Tukey window function parameter
     * \param[_order] window function size
    */
    void Tukey(__fx64 R, __ix32 order )
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("window_function","Tukey()", "Tukey window function generation");
        #endif

        m_order  = order;
        m_window = __mfree__(m_window);
        m_window = __Tukey__(R, order);
    }

    void copy(const window_function& window)
    {
        m_order  = window.m_order;
        m_window = __mfree__(m_window);
        m_window = __alloc__<__fx64>(m_order);
        for(__ix32 i = 0 ; i < m_order; i++)
        {
            m_window[i] = window.m_window[i];
        }
    }

    /*! \brief default constructor */
    window_function(){}

    /*!
     *  \brief copy constructor
     *  \param[window] input window function
    */
    window_function(const window_function& window)
    {
        copy(window);
    }

    /*! \brief default destructor */
    ~window_function()
    {
        m_window = __mfree__(m_window);
    }

    inline void operator = ( window_function& window)
    {
        copy(window);
    }
};

/*! @} */

/*! \defgroup <CLASSIC_FIR_FILTER_ABSTRACT_MODEL> ( Classic FIR filter abstract model )
 *  \ingroup CLASSIC_FIR_FILTERS
 *  \brief the module contains the abstract model of the classic FIR filter
    @{
*/

/*!  \brief defines the base of the FIR filters */
template< typename __type >
class fir_base : public  model_base, classic_filter_interface
{
private:
    /*!
     *   \brief allocates the filter resources
     *   \details The function is supposed to be called explicitly by the user.
    */
    __ix32 allocate()
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("classic_fir_base","allocate()", "fir filter memory allocation");
        #endif

        m_FilterData = round_coefficients<__type>(m_FilterType);

        if( m_FilterData.cfnum != nullptr )
        {
            return m_buff_sx.allocate( m_FilterData.N + 1 );
        }

        return 0;
    }

    /*!
     *   \brief frees the filter resources
     *   \details The function is supposed to be called explicitly by at the end of the filter's life time.
    */
    __ix32 deallocate()
    {
        #ifdef FIR_FILTERS_DEBUG
        Debugger::Log("classic_fir_base","deallocate()", "fir filter memory deallocation");
        #endif

        m_FilterData = __dsp_clear_filter__(m_FilterData);
        return ( !m_FilterData.cfnum && !m_FilterData.cfden );
    }

protected:

    filter_type         m_FilterType;
    filter_data<__type> m_FilterData;
    window_function     m_WindowFunction;
    bandwidth           m_Bandwidth;
    __ix32              m_Scale;
    delay<__type>       m_buff_sx;

public:

    /*!
     *   \brief initializes the classic FIR filter
     *   \param[Fs] filter sampling frequency, Hz
     *   \param[FilterType] filter type
     *   \param[Bandwidth] filter stopband/passband width
     *   \param[WindowFunction] filter window function
     *   \param[Scale] scale ot not to scale the filter coefficients
     *   \details The function is supposed to be called explicitly by the user before the filter resources are allocated
    */
    void init( __fx64 Fs, filter_type FilterType, bandwidth Bandwidth, window_function& WindowFunction, __ix32 Scale )
    {
        m_FilterType     = FilterType;
        m_Bandwidth      = Bandwidth;
        m_Scale          = Scale;
        m_WindowFunction = WindowFunction;
        model_base::init(WindowFunction.Order(), Fs);

        // allocation
        allocate();
    }

    /*! \brief default destructor */
    virtual ~fir_base()
    {
        deallocate();
    }

    /*! \brief default constructor */
    fir_base() : model_base()
    {
        m_FilterType   = filter_type::lowpass;
        m_Bandwidth    = { 100 , 500 };
    }

    /*!
     *  \brief frequency responce computation function
     *  \param[F] input frequency, Hz
     *  \details The function computes the filter complex transfer function value for the given frequency
    */
    fcomplex<__fx64> frequency_response( __fx64 F ) override
    {
        return __freq_resp__<__type> ( m_FilterData.cfnum , m_order, m_Fs , F  );
    }

    /*!
     *  \brief FIR filter buffer fill operator
     *  \param[input] input samples pointer
     *  \details The operator is supposed to be used to fill the FIR filter buffer
    */
    virtual inline void operator <<  ( __type *input ) = 0;

    /*!
     *  \brief FIR filter filtering operator
     *  \param[input] input samples pointer
     *  \details The operator is supposed to be used to filt the input signal
    */
    virtual inline __type operator() ( __type *input ) = 0;

    /*!
     *  \brief FIR filter filtering operator
     *  \details The operator is supposed to be used to filt the input signal right after the FIR filter buffer is filled.
    */
    virtual inline __type operator() () = 0;


    #ifndef __ALG_PLATFORM
        void show()
        {
            __show_fir__( m_FilterData );
        }
    #endif
};

/*! @} */

/*! \defgroup <CLASSIC_FIR_FILTER_IMPLEMENTATION> ( Classic FIR filter model implementation )
 *  \ingroup CLASSIC_FIR_FILTERS
 *  \brief the module contains the FIR filter model implementation
    @{
*/

template<typename __type> class fir;

template<> class fir< __fx32 > final : public fir_base< __fx32 >
{
    typedef __fx32 __type;

    filter_data< __fx64 > compute_lowpass () override { return __fir_wind_digital_lp__<__fx64>( m_Fs , m_Bandwidth.Fc , m_order , m_Scale , m_WindowFunction.WindowFunction() ); }
    filter_data< __fx64 > compute_highpass() override { return __fir_wind_digital_hp__<__fx64>( m_Fs , m_Bandwidth.Fc , m_order , m_Scale , m_WindowFunction.WindowFunction() ); }
    filter_data< __fx64 > compute_bandpass() override { return __fir_wind_digital_bp__<__fx64>( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , m_Scale , m_WindowFunction.WindowFunction() ); }
    filter_data< __fx64 > compute_bandstop() override { return __fir_wind_digital_bs__<__fx64>( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , m_Scale , m_WindowFunction.WindowFunction() ); }

public:

    // default constructor
    fir() : fir_base< __type >(){}

    // default destructor
    ~fir(){}

    // fir filtering operator
    inline __type operator()( __type*  _input ) override { return __filt__< __type , __type >( _input, m_FilterData.cfnum, m_buff_sx, m_FilterData.N ); }
    inline __type operator()( __fx64*  _input ) { return __filt__< __fx64  , __type >( _input, m_FilterData.cfnum, m_buff_sx, m_FilterData.N ); }
    inline __type operator()( __fxx64* _input ) { return __filt__< __fxx64 , __type >( _input, m_FilterData.cfnum, m_buff_sx, m_FilterData.N ); }
    inline __type operator()() override { return __filt__ < __type > ( m_FilterData.cfnum , m_buff_sx , m_FilterData.N ); }

    // fir buffer filling operator
    inline void operator << ( __type*  _input ) override { m_buff_sx(_input ); }
    inline void operator << ( __fx64*  _input ) { m_buff_sx(_input ); }
    inline void operator << ( __fxx64* _input ) { m_buff_sx(_input ); }
};

template<> class fir< __fx64 > final : public fir_base< __fx64 >
{
    typedef __fx64 __type;

    filter_data< __fx64 > compute_lowpass () override { return __fir_wind_digital_lp__<__fx64>( m_Fs , m_Bandwidth.Fc , m_order , m_Scale , m_WindowFunction.WindowFunction() ); }
    filter_data< __fx64 > compute_highpass() override { return __fir_wind_digital_hp__<__fx64>( m_Fs , m_Bandwidth.Fc , m_order , m_Scale , m_WindowFunction.WindowFunction() ); }
    filter_data< __fx64 > compute_bandpass() override { return __fir_wind_digital_bp__<__fx64>( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , m_Scale , m_WindowFunction.WindowFunction() ); }
    filter_data< __fx64 > compute_bandstop() override { return __fir_wind_digital_bs__<__fx64>( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , m_Scale , m_WindowFunction.WindowFunction() ); }

public:

    // default constructor
    fir() : fir_base< __type >(){}

    // default destructor
    ~fir(){}

    // fir filtering operator
    inline __type operator()( __type*  _input ) override { return __filt__< __type , __type >( _input, m_FilterData.cfnum, m_buff_sx, m_FilterData.N ); }
    inline __type operator()( __fxx64* _input ) { return __filt__< __fxx64 , __type >( _input, m_FilterData.cfnum, m_buff_sx, m_FilterData.N ); }
    inline __type operator()() override { return __filt__ < __type > ( m_FilterData.cfnum , m_buff_sx , m_FilterData.N ); }

    // fir buffer filling operator
    inline void operator << ( __type*  _input ) override { m_buff_sx(_input ); }
    inline void operator << ( __fxx64* _input ) { m_buff_sx(_input ); }
};

/*! @} */

#undef __fx32
#undef __fx64
#undef __fxx64
#undef __ix32
#undef PI0
#undef PI2

#endif // FILTERS_FIR_H
