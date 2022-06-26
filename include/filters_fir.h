#ifndef FILTERS_FIR_H
#define FILTERS_FIR_H

#include "kernel_fir.h"
#include "special_functions.h"
using namespace FIR_KERNEL;

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

/*! \class window_function */
class window_function
{
    __ix32  m_order;
    __fx64 *m_window;
public:

    __fx64 *WindowFunction()
    {
        return m_window;
    }

    /*!
     * \brief Barlett window function computation
     * \param[order] window function size
    */
    void Bartlett( __ix32 order )
    {
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
        m_order  = order;
        m_window = __mfree__(m_window);
        m_window = __Tukey__(R, order);
    }

    /*! \brief default constructor */
    window_function(){}

    /*! \brief default destructor */
    ~window_function()
    {
        m_window = __mfree__(m_window);
    }
};

template< typename __type >
class classic_fir_abstract : public  model_base
{
protected:

    filter_type         m_FilterType;
    filter_data<__type> m_FilterData;
    window_function     m_WindowFunction;
    bandwidth           m_Bandwidth;
    __ix32              m_Scale;
    delay<__type>       m_buff_sx;
    __type             *m_Coeffs;

    __ix32 allocate() override
    {
        switch ( m_FilterType )
        {
            case filter_type::lowpass:
            m_FilterData = __fir_wind_digital_lp__<__type>( m_Fs , m_Bandwidth.Fc , m_order , m_Scale , m_WindowFunction.WindowFunction() );
            break;

            case filter_type::highpass:
            m_FilterData = __fir_wind_digital_hp__<__type>( m_Fs , m_Bandwidth.Fc , m_order , m_Scale , m_WindowFunction.WindowFunction() );
            break;

            case filter_type::bandpass:
            m_FilterData = __fir_wind_digital_bp__<__type>( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , m_Scale , m_WindowFunction.WindowFunction() );
            break;

            case filter_type::bandstop:
            m_FilterData = __fir_wind_digital_bs__<__type>( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , m_Scale , m_WindowFunction.WindowFunction() );
            break;

            default:
            m_FilterData = __fir_wind_digital_lp__<__type>( m_Fs , m_Bandwidth.Fc , m_order , m_Scale , m_WindowFunction.WindowFunction() );
            break;
        }

        if( m_FilterData.cfnum != nullptr )
        {
            m_order  = m_FilterData.N;
            m_Coeffs = m_FilterData.cfnum;
            return m_buff_sx.allocate( m_FilterData.N + 1 );
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
        m_FilterData = __cf_free__( m_FilterData );

        #ifdef DEBUG
            std::cout << "memory deallocation invocation" << "\n";
            std::cout << "m_cfmatrix.cfnum cleand = " << !m_FilterData.cfnum << "\n";
            std::cout << "m_cfmatrix.cfden cleand = " << !m_FilterData.cfden << "\n";
            std::cout << "m_cfmatrix.gains cleand = " << !m_FilterData.gains << "\n";
            std::cout << "\n";
        #endif

        return ( !m_FilterData.cfnum && !m_FilterData.cfden );
    }

    fcomplex<__fx64> frequency_response( __fx64 F ) override
    {
        return __freq_resp__<__type> ( m_FilterData.cfnum , m_order, m_Fs , F  );
    }

    void init( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth , __fx64* _window , bool _scale )
    {
        m_Fs          = _Fs;
        m_FilterType  = _type;
        m_order       = _order;
        m_Bandwidth   = _bandwidth;
        m_Scale       = _scale;

        // memory allocation function call:
        allocate(_window);
    }

    classic_fir_abstract() : model_base()
    {
        m_FilterType = filter_type::lowpass;
        m_Bandwidth   = { 100 , 500 };
    }

    classic_fir_abstract( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth , __fx64* _window , bool _scale )
    {
        init(_Fs, _order,  _type , _bandwidth , _window , _scale );
    }

    #ifndef __ALG_PLATFORM
    void show()
    {
        __show__( m_Coeffs, m_order, m_FilterType );
    }
    #endif
};


#undef __fx32
#undef __fx64
#undef __fxx64
#undef __ix32
#undef PI0
#undef PI2

#endif // FILTERS_FIR_H
