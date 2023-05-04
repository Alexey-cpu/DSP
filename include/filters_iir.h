#ifndef FILTERS_IIR_H
#define FILTERS_IIR_H

// custom includes
#include "Complex.h"
#include "kernel_iir.h"
using namespace IIR_KERNEL;

#ifndef __ALG_PLATFORM
#define IIR_FILTERS_DEBUG // debugging is not available if the algorithm is running on a device !!!
#endif

/*! \defgroup <CLASSIC_IIR_FILTERS> ( classic IIR filters)
 *  \ingroup FILTERS
 *  \brief The module contains abstract model and implementation of the classic IIR filters
    @{
*/

/*! @} */

template< typename __type >
class IIRCreator
{
public:

    // constructor
    IIRCreator(){}

    // virtual destructor
    virtual ~IIRCreator(){}

    // creator function
    virtual filter_data< __type >
    Create(
            double _Fs,
            int64_t _order,
            filter_type _type,
            bandwidth _bandwidth,
            attenuation _attenuation = {1, -1} ) = 0;

};

template< typename __type >
class ButterworthCreator : public IIRCreator<__type>, public classic_filter_interface
{
protected:

    int64_t     m_Order;
    double      m_Fs;
    filter_type m_FilterType;
    bandwidth   m_Bandwidth;
    attenuation m_Attenuation;

public:

    // constructor
    ButterworthCreator(){}

    // virtual destructor
    virtual ~ButterworthCreator(){}

    // filter interface implementation
    filter_data< double > compute_lowpass () override
    {
        return __butt_cheb1_digital_lp__< double >( m_Fs, m_Bandwidth.Fc, m_Order, 0 , m_Attenuation.G1 );
    }

    filter_data< double > compute_highpass() override
    {
        return __butt_cheb1_digital_hp__< double >( m_Fs, m_Bandwidth.Fc, m_Order, 0 , m_Attenuation.G1 );
    }

    filter_data< double > compute_bandpass() override
    {
        return __butt_cheb1_digital_bp__< double >( m_Fs, m_Bandwidth.Fc, m_Bandwidth.BW , m_Order , 0 , m_Attenuation.G1 );
    }

    filter_data< double > compute_bandstop() override
    {
        return __butt_cheb1_digital_bs__< double >( m_Fs, m_Bandwidth.Fc, m_Bandwidth.BW , m_Order , 0 , m_Attenuation.G1 );
    }

    // creator function
    filter_data< __type > Create(
            double _Fs,
            int64_t _order,
            filter_type _type,
            bandwidth _bandwidth,
            attenuation _attenuation ) override
    {
        m_Fs          = _Fs;
        m_Order       = _order;
        m_FilterType  = _type;
        m_Bandwidth   = _bandwidth;
        m_Attenuation = _attenuation;

        return round_coefficients<__type>(m_FilterType);
    }
};

template< typename __type >
class Chebyshev1Creator : public IIRCreator<__type>, public classic_filter_interface
{
protected:

    int64_t     m_Order;
    double      m_Fs;
    filter_type m_FilterType;
    bandwidth   m_Bandwidth;
    attenuation m_Attenuation;

public:

    // constructor
    Chebyshev1Creator(){}

    // virtual destructor
    virtual ~Chebyshev1Creator(){}

    // filter interface implementation
    filter_data< double > compute_lowpass () override
    {
        return __butt_cheb1_digital_lp__< double >( m_Fs , m_Bandwidth.Fc , m_Order , 1 , m_Attenuation.G1 );
    }

    filter_data< double > compute_highpass() override
    {
        return __butt_cheb1_digital_hp__< double >( m_Fs , m_Bandwidth.Fc , m_Order , 1 , m_Attenuation.G1 );
    }

    filter_data< double > compute_bandpass() override
    {
        return __butt_cheb1_digital_bp__< double >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_Order , 1 , m_Attenuation.G1 );
    }

    filter_data< double > compute_bandstop() override
    {
        return __butt_cheb1_digital_bs__< double >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_Order , 1 , m_Attenuation.G1 );
    }


    // creator function
    filter_data< __type > Create(
            double _Fs,
            int64_t _order,
            filter_type _type,
            bandwidth _bandwidth,
            attenuation _attenuation ) override
    {
        m_Fs          = _Fs;
        m_Order       = _order;
        m_FilterType  = _type;
        m_Bandwidth   = _bandwidth;
        m_Attenuation = _attenuation;

        return round_coefficients<__type>(m_FilterType);
    }
};

template< typename __type >
class Chebyshev2Creator : public IIRCreator<__type>, public classic_filter_interface
{
protected:

    int64_t     m_Order;
    double      m_Fs;
    filter_type m_FilterType;
    bandwidth   m_Bandwidth;
    attenuation m_Attenuation;

public:

    // constructor
    Chebyshev2Creator(){}

    // virtual destructor
    virtual ~Chebyshev2Creator(){}

    // filter interface implementation
    filter_data< double > compute_lowpass () override
    {
        return __cheb2_ellip_digital_lp__< double >( m_Fs , m_Bandwidth.Fc , m_Order , 0 , m_Attenuation.G2 , m_Attenuation.G1 );
    }

    filter_data< double > compute_highpass() override
    {
        return __cheb2_ellip_digital_hp__< double >( m_Fs , m_Bandwidth.Fc , m_Order , 0 , m_Attenuation.G2 , m_Attenuation.G1 );
    }

    filter_data< double > compute_bandpass() override
    {
        return __cheb2_ellip_digital_bp__< double >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_Order , 0 , m_Attenuation.G2 , m_Attenuation.G1 );
    }

    filter_data< double > compute_bandstop() override
    {
        return __cheb2_ellip_digital_bs__< double >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_Order , 0 , m_Attenuation.G2 , m_Attenuation.G1 );
    }

    // creator function
    filter_data< __type > Create(
            double _Fs,
            int64_t _order,
            filter_type _type,
            bandwidth _bandwidth,
            attenuation _attenuation ) override
    {
        m_Fs          = _Fs;
        m_Order       = _order;
        m_FilterType  = _type;
        m_Bandwidth   = _bandwidth;
        m_Attenuation = _attenuation;

        return round_coefficients<__type>(m_FilterType);
    }
};

template< typename __type >
class EllipticCreator : public IIRCreator<__type>, public classic_filter_interface
{
protected:

    int64_t     m_Order;
    double      m_Fs;
    filter_type m_FilterType;
    bandwidth   m_Bandwidth;
    attenuation m_Attenuation;

public:

    // constructor
    EllipticCreator(){}

    // virtual destructor
    virtual ~EllipticCreator(){}

    // filter interface implementation
    filter_data< double > compute_lowpass () override
    {
        return __cheb2_ellip_digital_lp__< double >( m_Fs, m_Bandwidth.Fc, m_Order, 1.0, m_Attenuation.G2, m_Attenuation.G1 );
    }

    filter_data< double > compute_highpass() override
    {
        return __cheb2_ellip_digital_hp__< double >( m_Fs, m_Bandwidth.Fc, m_Order, 1.0, m_Attenuation.G2, m_Attenuation.G1 );
    }

    filter_data< double > compute_bandpass() override
    {
        return __cheb2_ellip_digital_bp__< double >( m_Fs, m_Bandwidth.Fc, m_Bandwidth.BW, m_Order, 1.0, m_Attenuation.G2, m_Attenuation.G1 );
    }

    filter_data< double > compute_bandstop() override
    {
        return __cheb2_ellip_digital_bs__< double >( m_Fs, m_Bandwidth.Fc, m_Bandwidth.BW, m_Order, 1.0, m_Attenuation.G2, m_Attenuation.G1 );
    }

    // creator function
    filter_data< __type > Create(
            double _Fs,
            int64_t _order,
            filter_type _type,
            bandwidth _bandwidth,
            attenuation _attenuation ) override
    {
        m_Fs          = _Fs;
        m_Order       = _order;
        m_FilterType  = _type;
        m_Bandwidth   = _bandwidth;
        m_Attenuation = _attenuation;

        return round_coefficients< __type >(m_FilterType);
    }
};

template<typename __type>
class IIRFactory : public transfer_function_model
{
private:

    // buffers array deallocation function
    delay<__type>* buffers_array_free(delay<__type> *buffers)
    {
        if( buffers != nullptr )
        {
            delete [] buffers;  // delete operator calls object destructor which frees the object resources
            buffers = nullptr;
        }

        return buffers;
    }

    // buffers array allocatinon function
    delay<__type>* buffers_array_alloc(int64_t buffersNumber, int64_t singleBufferSize)
    {
        delay<__type>* buffers = new delay<__type>[buffersNumber];

        for( int64_t i = 0 ; i < buffersNumber ; i++)
        {
            buffers[i].allocate(singleBufferSize);
        }

        return buffers;
    }

protected:

    IIRCreator<__type>* m_IIRCreator = nullptr;
    filter_data<__type> m_FilterData;
    delay<__type>      *m_buff_sx;
    delay<__type>      *m_buff_sy;

    template<typename T> T filt(T *_input)
    {
        return __filt__(
                    _input,
                    m_FilterData.cfnum,
                    m_FilterData.cfden,
                    m_FilterData.gains,
                    m_FilterData.Nx,
                    m_FilterData.Ny,
                    m_FilterData.N,
                    m_buff_sx,
                    m_buff_sy );
    }

public:

    /*!
     *  \brief memory free function
     *  \param[Fs] filter sampling function
     *  \param[Order] filter order
     *  \param[FilterType] filter type
     *  \param[Bandwidth] filter frequency bandwidth
     *  \param[Attenuation] filter pass and stop band attenuation
     *  \details The function initializes the filter and is supposed to be called explicitly by the user
     *           before the filter resources are allocated.
    */
    void init( double Fs , int Order , filter_type FilterType , bandwidth Bandwidth, attenuation Attenuation )
    {
        transfer_function_model::init(Order, Fs);

        // allocation
        if( m_IIRCreator == nullptr )
            return;

        m_FilterData = m_IIRCreator->Create(Fs, Order, FilterType, Bandwidth, Attenuation);

        if( m_FilterData.cfden != nullptr && m_FilterData.cfnum != nullptr && m_FilterData.gains != nullptr )
        {
            m_buff_sx = buffers_array_alloc(m_FilterData.N, m_FilterData.Nx+1);
            m_buff_sy = buffers_array_alloc(m_FilterData.N, m_FilterData.Ny+1);
        }

        if( m_buff_sx == nullptr || m_buff_sy == nullptr )
        {
            #ifdef IIR_FILTERS_DEBUG
            Debugger::Log("iir_abstract","allocate()", "memory allocation failed");
            #endif
        }
    }

    /*! \brief default constructor */
    IIRFactory( IIRCreator<__type>* _IIRCreator ) : transfer_function_model(), m_IIRCreator( _IIRCreator )
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("iir_base","iir_abstract()", "constructor call");
        #endif

        m_buff_sx = nullptr;
        m_buff_sy = nullptr;
    }

    /*! \brief virtual destructor */
    virtual ~IIRFactory()
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("iir_base","~iir_abstract()", "destructor call");
        #endif

        // memory deallocation
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("iir_abstract","deallocate()", "memory deallocation");
        #endif

        // delete creator
        delete m_IIRCreator;
        m_IIRCreator = nullptr;

        // clear other resources
        m_buff_sx = buffers_array_free(m_buff_sx);
        m_buff_sy = buffers_array_free(m_buff_sy);
        __dsp_clear_filter__(m_FilterData);
    }

    /*!
     *  \brief frequency responce computation function
     *  \param[F] input frequency, Hz
     *  \details The function computes the filter complex transfer function value for the given frequency
    */
    Complex<double> frequency_response( double F ) override
    {
        return __freq_resp__(
                    m_FilterData.cfnum,
                    m_FilterData.cfden,
                    m_FilterData.gains,
                    m_FilterData.Nx,
                    m_FilterData.Ny,
                    m_FilterData.N, m_Fs, F );
    }


    #ifndef __ALG_PLATFORM

        void show()
        {
            __show__<__type>(m_FilterData);
        }

    #endif

    /*!
     *  \brief filtering operator
     *  \param[_input] input pointer
    */
    inline __type operator()( __type* _input )
    {
        return filt<__type>(_input);
    }

};

/*! \defgroup <BUTTERWORTH_FILTER_IMPLEMENTATION> ( Butterworth filter )
 *  \ingroup CLASSIC_IIR_FILTERS
 *  \brief The module contains implementation of the Butterworth filter
    @{
*/

// butterworth filter realization
template< typename __type > class butterworth final : public IIRFactory< __type >
{
    public:

    // initialization function
    void init( double _Fs, int _order, filter_type _type, bandwidth _bandwidth )
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("butterworth","init()", "filter initialization");
        Debugger::Log("Fs           = " + to_string(_Fs));
        Debugger::Log("order        = " + to_string(_order));
        Debugger::Log("type         = " + to_string(_type));
        Debugger::Log("bandwidth.Fc = " + to_string(_bandwidth.Fc));
        Debugger::Log("bandwidth.BW = " + to_string(_bandwidth.BW));
        #endif

        IIRFactory< __type >::init(_Fs, _order,  _type, _bandwidth, {1, -1} );
    }

     // constructors
    butterworth() : IIRFactory< __type >( new ButterworthCreator<__type>() )
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("butterworth","butterworth()", "constructor call");
        #endif
    }

     // destructor
    virtual ~butterworth()
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("butterworth","~butterworth()", "destructor call");
        #endif
    }
};

/*!
 *  \example example_classic_iir_butt.h
*/

/*! @} */

/*!
 *  \defgroup <CHEBYSHEV_TYPE_I_FILTER_IMPLEMENTATION> ( Chebyshev type I filter )
 *  \ingroup CLASSIC_IIR_FILTERS
 *  \brief The module contains implementation of the Chebyshev type I filter
    @{
*/

// checbyshev 1 filter realization
template< typename __type > class chebyshev_1 final : public IIRFactory< __type >
{
    public:

    // initialization function
    void init( double _Fs , int64_t _order , filter_type _type, bandwidth _bandwidth , double _Gs )
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("chebyshev_1","init()", "filter initialization");
        Debugger::Log("Fs           = " + to_string(_Fs));
        Debugger::Log("order        = " + to_string(_order));
        Debugger::Log("type         = " + to_string(_type));
        Debugger::Log("bandwidth.Fc = " + to_string(_bandwidth.Fc));
        Debugger::Log("bandwidth.BW = " + to_string(_bandwidth.BW));
        Debugger::Log("Gs           = " + to_string(_Gs));
        #endif

        IIRFactory< __type > :: init(_Fs, _order,  _type, _bandwidth, { _Gs, -1 } );
    }

    // constructors
     chebyshev_1() : IIRFactory< __type >( new Chebyshev1Creator<__type>() )
     {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("chebyshev_1","chebyshev_1()", "constructor call");
        #endif
     }

     // destructor
    virtual ~chebyshev_1()
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("chebyshev_1","~chebyshev_1()", "destructor call");
        #endif
    }
};

/*!
 *  \example example_classic_iir_cheb1.h
*/

/*! @} */

/*!
 *  \defgroup <CHEBYSHEV_TYPE_II_FILTER_IMPLEMENTATION> ( Chebyshev type II filter )
 *  \ingroup CLASSIC_IIR_FILTERS
 *  \brief The module contains implementation of the Chebyshev type II filter
    @{
*/

// chebyshev 2 filter realization
template< typename __type > class chebyshev_2 final : public IIRFactory< __type >
{
public:

    // initializing function
    void init( double _Fs, int64_t _order, filter_type _type, bandwidth _bandwidth, double _Gp )
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("chebyshev_2","init()", "filter initialization");
        Debugger::Log("Fs           = " + to_string(_Fs));
        Debugger::Log("order        = " + to_string(_order));
        Debugger::Log("type         = " + to_string(_type));
        Debugger::Log("bandwidth.Fc = " + to_string(_bandwidth.Fc));
        Debugger::Log("bandwidth.BW = " + to_string(_bandwidth.BW));
        Debugger::Log("Gp           = " + to_string(_Gp));
        #endif

        IIRFactory< __type >::init(_Fs, _order,  _type, _bandwidth, { _Gp, -1 } );
    }

    // constructors
    chebyshev_2() : IIRFactory< __type >( new Chebyshev2Creator<__type>() )
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("chebyshev_2","chebyshev_2()", "constructor call");
        #endif
    }

    // destructor
    virtual ~chebyshev_2()
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("chebyshev_2","chebyshev_2()", "destructor call");
        #endif
    }
};

/*!
 *  \example example_classic_iir_cheb2.h
*/

/*! @} */

/*!
 *  \defgroup <ELLIPTIC_FILTER_IMPLEMENTATION> ( Elliptic filter )
 *  \ingroup CLASSIC_IIR_FILTERS
 *  \brief The module contains implementation of the Elliptic filter
    @{
*/

// elliptic filter realization
template< typename __type > class elliptic final : public IIRFactory< __type >
{
public:

    // initialization
    void init( double _Fs, int _order, filter_type _type, bandwidth _bandwidth, double _Gs, double _Gp )
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("elliptic","init()", "filter initialization");
        Debugger::Log("Fs           = " + to_string(_Fs));
        Debugger::Log("order        = " + to_string(_order));
        Debugger::Log("type         = " + to_string(_type));
        Debugger::Log("bandwidth.Fc = " + to_string(_bandwidth.Fc));
        Debugger::Log("bandwidth.BW = " + to_string(_bandwidth.BW));
        Debugger::Log("Gs           = " + to_string(_Gs));
        Debugger::Log("Gp           = " + to_string(_Gp));
        #endif

        IIRFactory< __type >::init(_Fs, _order,  _type, _bandwidth, { _Gs, _Gp } );
    }

    // constructors
    elliptic() : IIRFactory< __type >( new EllipticCreator<__type>() )
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("elliptic","elliptic()", "constructor call");
        #endif
    }


    // destructor
    virtual ~elliptic()
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("elliptic","~elliptic()", "destructor call");
        #endif
    }
};

/*!
 *  \example example_classic_iir_ellip.h
*/

/*! @} */

#endif // FILTERS_IIR_H
