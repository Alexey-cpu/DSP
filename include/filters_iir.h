#ifndef FILTERS_IIR_H
#define FILTERS_IIR_H

// custom includes
#include "buffer.h"
#include "fcomplex.h"
#include "kernel_iir.h"
using namespace IIR_KERNEL;

#ifndef __ALG_PLATFORM
#define IIR_FILTERS_DEBUG // debugging is not available if the algorithm is running on a device !!!
#endif

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

template<typename __type> class iir_abstract : public filter_abstract
{
protected:

    // system variables
    delay<__type>      *m_buff_sx;
    delay<__type>      *m_buff_sy;
    __ix32              m_SectionsNumber;
    filter_data<__type> m_FilterData;
    filter_type         m_FilterType;
    bandwidth           m_Bandwidth;
    attenuation         m_Attenuation;

    // coefficients computation functions
    virtual filter_data<__type> compute_lowpass()  = 0;
    virtual filter_data<__type> compute_highpass() = 0;
    virtual filter_data<__type> compute_bandpass() = 0;
    virtual filter_data<__type> compute_bandstop() = 0;

    // buffers array deallocation function
    delay<__type>* buffers_array_free(delay<__type> *buffers, __ix32 buffersNumber)
    {
        if( buffers != nullptr )
        {
            for( __ix32 i = 0 ; i < buffersNumber ; i++ ) buffers[i].deallocate();

            delete [] buffers;
            buffers = nullptr;
        }

        return buffers;
    }

    // buffers array allocatinon function
    delay<__type>* buffers_array_alloc(__ix32 buffersNumber, __ix32 singleBufferSize)
    {
        delay<__type>* buffers = new delay<__type>[buffersNumber];

        for( __ix32 i = 0 ; i < buffersNumber ; i++)
        {
            buffers[i].allocate(singleBufferSize);
        }

        return buffers;
    }

    template<typename T> T filt(T *_input)
    {
        return __filt__(_input, m_FilterData.cfnum, m_FilterData.cfden, m_FilterData.gains, m_FilterData.Nx, m_FilterData.Ny, m_FilterData.N, m_buff_sx, m_buff_sy );
    }

public:

    // memory allocation
    __ix32 allocate() override
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("iir_abstract","allocate()", "memory allocation started");
        #endif

        switch (m_FilterType)
        {
            case filter_type::lowpass :
                m_FilterData = compute_lowpass();
            break;

            case filter_type::highpass :
                m_FilterData = compute_highpass();
            break;

            case filter_type::bandpass :
                m_FilterData = compute_bandpass();
            break;

            case filter_type::bandstop :
                m_FilterData = compute_bandstop();
            break;

            case filter_type::other :
                m_FilterData = compute_lowpass();
            break;
        }

        m_SectionsNumber = m_FilterData.N;

        if( m_FilterData.cfden && m_FilterData.cfnum  && m_FilterData.gains )
        {
            m_buff_sx = buffers_array_alloc(m_FilterData.N, m_FilterData.Nx+1);
            m_buff_sy = buffers_array_alloc(m_FilterData.N, m_FilterData.Ny+1);
        }

        if( !m_buff_sx || !m_buff_sy )
        {
            #ifdef IIR_FILTERS_DEBUG
            Debugger::Log("iir_abstract","allocate()", "memory allocation failed");
            #endif
        }

        return ( m_buff_sx && m_buff_sy );

    }

    // memory deallocation
    __ix32 deallocate() override
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("iir_abstract","deallocate()", "memory deallocation");
        #endif

        m_buff_sx = buffers_array_free(m_buff_sx, m_SectionsNumber);
        m_buff_sy = buffers_array_free(m_buff_sy, m_SectionsNumber);
        __dsp_clear_filter__(m_FilterData);
        return 1;
    }

    void init( __fx64 Fs , __ix32 Order , filter_type FilterType , bandwidth Bandwidth, attenuation Attenuation )
    {
        m_Fs          = Fs;
        m_Bandwidth   = Bandwidth;
        m_Attenuation = Attenuation;
        m_FilterType  = FilterType;
        m_order       = Order;
    }

    // default constructor
    iir_abstract() : filter_abstract()
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("iir_abstract","iir_abstract()", "constructor call");
        #endif

        m_buff_sx     = nullptr;
        m_buff_sy     = nullptr;
        m_FilterType  = filter_type::lowpass;
        m_Bandwidth   = { 100 , 500 };
        m_Attenuation = { 80  , 1   };
    }

    // destructor
    virtual ~iir_abstract()
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("iir_abstract","iir_abstract()", "destructor call");
        #endif
    }

    // frequency response computation function
    fcomplex<__fx64> frequency_response( __fx64 F ) override
    {
        return __freq_resp__(m_FilterData.cfnum, m_FilterData.cfden, m_FilterData.gains, m_FilterData.Nx, m_FilterData.Ny, m_FilterData.N, m_Fs, F );
    }

    // filter show function
    void show()
    {
        __show__<__type>(m_FilterData, m_FilterData.type);
    }

    virtual inline __type operator()( __type* _input ) = 0;
};

// butterworth filter realization
template< typename __type > class butterworth;

template<> class butterworth< __fx32 > final : public iir_abstract< __fx32 >
{
    typedef __fx32 __type;

    filter_data< __type > compute_lowpass () override { return __butt_cheb1_digital_lp__< __type >( m_Fs, m_Bandwidth.Fc, m_order, 0 , m_Attenuation.G1 ); }
    filter_data< __type > compute_highpass() override { return __butt_cheb1_digital_hp__< __type >( m_Fs, m_Bandwidth.Fc, m_order, 0 , m_Attenuation.G1 ); }
    filter_data< __type > compute_bandpass() override { return __butt_cheb1_digital_bp__< __type >( m_Fs, m_Bandwidth.Fc, m_Bandwidth.BW , m_order , 0 , m_Attenuation.G1 ); }
    filter_data< __type > compute_bandstop() override { return __butt_cheb1_digital_bs__< __type >( m_Fs, m_Bandwidth.Fc, m_Bandwidth.BW , m_order , 0 , m_Attenuation.G1 ); }

    public:

    // initialization function
    void init( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth )
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("butterworth","init()", "filter initialization");
        Debugger::Log("Fs           = " + to_string(_Fs));
        Debugger::Log("order        = " + to_string(_order));
        Debugger::Log("type         = " + to_string(_type));
        Debugger::Log("bandwidth.Fc = " + to_string(_bandwidth.Fc));
        Debugger::Log("bandwidth.BW = " + to_string(_bandwidth.BW));
        #endif

        iir_abstract< __type >::init(_Fs, _order,  _type, _bandwidth, {1, -1} );
    }

     // constructors
     butterworth< __type >() : iir_abstract< __type >()
     {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("butterworth","butterworth()", "constructor call");
        #endif
     }

     butterworth< __type >( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth )
     {
         init(_Fs, _order,  _type, _bandwidth );
     }

     // destructor
     ~butterworth< __type >()
     {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("butterworth","~butterworth()", "destructor call");
        #endif
     }

    inline __type operator()( __type* _input ) override { return filt<__type>(_input); }
};

template<> class butterworth< __fx64 > final : public iir_abstract< __fx64 >
{
    typedef __fx64 __type;

    filter_data< __type > compute_lowpass () override { return __butt_cheb1_digital_lp__< __type >( m_Fs , m_Bandwidth.Fc , m_order , 0 , m_Attenuation.G1 ); }
    filter_data< __type > compute_highpass() override { return __butt_cheb1_digital_hp__< __type >( m_Fs , m_Bandwidth.Fc , m_order , 0 , m_Attenuation.G1 ); }
    filter_data< __type > compute_bandpass() override { return __butt_cheb1_digital_bp__< __type >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , 0 , m_Attenuation.G1 ); }
    filter_data< __type > compute_bandstop() override { return __butt_cheb1_digital_bs__< __type >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , 0 , m_Attenuation.G1 ); }

    public:

    // initialization function
    void init( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth )
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("butterworth","init()", "filter initialization");
        Debugger::Log("Fs           = " + to_string(_Fs));
        Debugger::Log("order        = " + to_string(_order));
        Debugger::Log("type         = " + to_string(_type));
        Debugger::Log("bandwidth.Fc = " + to_string(_bandwidth.Fc));
        Debugger::Log("bandwidth.BW = " + to_string(_bandwidth.BW));
        #endif

        iir_abstract< __type >::init(_Fs, _order,  _type, _bandwidth, {1, -1} );
    }

     // constructors
     butterworth< __type >() : iir_abstract< __type >()
     {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("butterworth","butterworth()", "constructor call");
        #endif
     }

     butterworth< __type >( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth )
     {
         init(_Fs, _order,  _type, _bandwidth );
     }

     // destructor
     ~butterworth< __type >()
     {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("butterworth","~butterworth()", "destructor call");
        #endif
     }

    inline __type operator()( __type* _input ) override { return filt<__type>(_input); }
};

// checbyshev 1 filter realization
template< typename __type > class chebyshev_1;

template<> class chebyshev_1< __fx32 > final : public iir_abstract< __fx32 >
{
    typedef __fx32 __type;

    filter_data< __type > compute_lowpass () override { return __butt_cheb1_digital_lp__< __type >( m_Fs , m_Bandwidth.Fc , m_order , 1 , m_Attenuation.G1 ); }
    filter_data< __type > compute_highpass() override { return __butt_cheb1_digital_hp__< __type >( m_Fs , m_Bandwidth.Fc , m_order , 1 , m_Attenuation.G1 ); }
    filter_data< __type > compute_bandpass() override { return __butt_cheb1_digital_bp__< __type >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , 1 , m_Attenuation.G1 ); }
    filter_data< __type > compute_bandstop() override { return __butt_cheb1_digital_bs__< __type >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , 1 , m_Attenuation.G1 ); }

    public:

    // initialization function
    void init( __fx64 _Fs , __ix32 _order , filter_type _type, bandwidth _bandwidth , __fx64 _Gs )
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

        iir_abstract< __type > :: init(_Fs, _order,  _type, _bandwidth, { _Gs, -1 } );
    }

    // constructors
     chebyshev_1< __type >() : iir_abstract< __type >()
     {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("chebyshev_1","chebyshev_1()", "constructor call");
        #endif
     }

     // destructor
    ~chebyshev_1< __type >()
     {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("chebyshev_1","~chebyshev_1()", "destructor call");
        #endif
     }

    inline __type operator()( __type* _input ) override { return filt<__type>(_input); }
};

template<> class chebyshev_1< __fx64 > final : public iir_abstract< __fx64 >
{
    typedef __fx64 __type;

    filter_data< __type > compute_lowpass () override { return __butt_cheb1_digital_lp__< __type >( m_Fs , m_Bandwidth.Fc , m_order , 1 , m_Attenuation.G1 ); }
    filter_data< __type > compute_highpass() override { return __butt_cheb1_digital_hp__< __type >( m_Fs , m_Bandwidth.Fc , m_order , 1 , m_Attenuation.G1 ); }
    filter_data< __type > compute_bandpass() override { return __butt_cheb1_digital_bp__< __type >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , 1 , m_Attenuation.G1 ); }
    filter_data< __type > compute_bandstop() override { return __butt_cheb1_digital_bs__< __type >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , 1 , m_Attenuation.G1 ); }

    public:

    // initialization function
    void init( __fx64 _Fs , __ix32 _order , filter_type _type, bandwidth _bandwidth , __fx64 _Gs )
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

        iir_abstract< __type > :: init(_Fs, _order,  _type, _bandwidth, { _Gs, -1 } );
    }

    // constructors
     chebyshev_1< __type >() : iir_abstract< __type >()
     {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("chebyshev_1","chebyshev_1()", "constructor call");
        #endif
     }

     // destructor
    ~chebyshev_1< __type >()
     {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("chebyshev_1","~chebyshev_1()", "destructor call");
        #endif
     }

    inline __type operator()( __type* _input ) override { return filt<__type>(_input); }
};

// chebyshev 2 filter realization
template< typename __type > class chebyshev_2;

template<> class chebyshev_2< __fx32 > final : public iir_abstract< __fx32 >
{
    typedef __fx32 __type;

    filter_data< __type > compute_lowpass () override { return __cheb2_ellip_digital_lp__< __type >( m_Fs , m_Bandwidth.Fc , m_order , 0 , m_Attenuation.G2 , m_Attenuation.G1 ); }
    filter_data< __type > compute_highpass() override { return __cheb2_ellip_digital_hp__< __type >( m_Fs , m_Bandwidth.Fc , m_order , 0 , m_Attenuation.G2 , m_Attenuation.G1 ); }
    filter_data< __type > compute_bandpass() override { return __cheb2_ellip_digital_bp__< __type >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , 0 , m_Attenuation.G2 , m_Attenuation.G1 ); }
    filter_data< __type > compute_bandstop() override { return __cheb2_ellip_digital_bs__< __type >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , 0 , m_Attenuation.G2 , m_Attenuation.G1 ); }

    public:

    // initializing function
    void init( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, __fx64 _Gp )
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

        iir_abstract< __type >::init(_Fs, _order,  _type, _bandwidth, { _Gp, -1 } );
    }

    // constructors
    chebyshev_2< __type >() : iir_abstract< __type >()
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("chebyshev_2","chebyshev_2()", "constructor call");
        #endif
    }

    // destructor
    ~chebyshev_2< __type >()
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("chebyshev_2","chebyshev_2()", "destructor call");
        #endif
    }

    // operayors
    inline __type operator()( __type* _input ) override { return filt<__type>(_input); }
};

template<> class chebyshev_2< __fx64 > final : public iir_abstract< __fx64 >
{
    typedef __fx64 __type;

    filter_data< __type > compute_lowpass () override { return __cheb2_ellip_digital_lp__< __type >( m_Fs , m_Bandwidth.Fc , m_order , 0 , m_Attenuation.G2 , m_Attenuation.G1 ); }
    filter_data< __type > compute_highpass() override { return __cheb2_ellip_digital_hp__< __type >( m_Fs , m_Bandwidth.Fc , m_order , 0 , m_Attenuation.G2 , m_Attenuation.G1 ); }
    filter_data< __type > compute_bandpass() override { return __cheb2_ellip_digital_bp__< __type >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , 0 , m_Attenuation.G2 , m_Attenuation.G1 ); }
    filter_data< __type > compute_bandstop() override { return __cheb2_ellip_digital_bs__< __type >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , 0 , m_Attenuation.G2 , m_Attenuation.G1 ); }

    public:

    // initializing function
    void init( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, __fx64 _Gp )
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

        iir_abstract< __type >::init(_Fs, _order,  _type, _bandwidth, { _Gp, -1 } );
    }

    // constructors
    chebyshev_2< __type >() : iir_abstract< __type >()
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("chebyshev_2","chebyshev_2()", "constructor call");
        #endif
    }

    // destructor
    ~chebyshev_2< __type >()
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("chebyshev_2","chebyshev_2()", "destructor call");
        #endif
    }

    // operayors
    inline __type operator()( __type* _input ) override { return filt<__type>(_input); }
};


// elliptic filter realization
template< typename __type > class elliptic;

template<> class elliptic< __fx32 > final : public iir_abstract< __fx32 >
{
typedef __fx32 __type;

    filter_data< __type > compute_lowpass () override { return __cheb2_ellip_digital_lp__< __type >( m_Fs , m_Bandwidth.Fc , m_order , 1 , m_Attenuation.G2 , m_Attenuation.G1 ); }
    filter_data< __type > compute_highpass() override { return __cheb2_ellip_digital_hp__< __type >( m_Fs , m_Bandwidth.Fc , m_order , 1 , m_Attenuation.G2 , m_Attenuation.G1 ); }
    filter_data< __type > compute_bandpass() override { return __cheb2_ellip_digital_bp__< __type >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , 1 , m_Attenuation.G2 , m_Attenuation.G1 ); }
    filter_data< __type > compute_bandstop() override { return __cheb2_ellip_digital_bs__< __type >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , 1 , m_Attenuation.G2 , m_Attenuation.G1 ); }

    public:

    // initialization
    void init( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, __fx64 _Gs, __fx64 _Gp )
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

        iir_abstract< __type >::init(_Fs, _order,  _type, _bandwidth, { _Gs, _Gp } );
    }

    // constructors
    elliptic< __type >() : iir_abstract< __type >()
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("elliptic","elliptic()", "constructor call");
        #endif
    }


    // destructor
    ~elliptic< __type >()
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("elliptic","~elliptic()", "destructor call");
        #endif
    }

    // opeartors
    inline __type operator()( __type* _input ) override { return filt<__type>(_input); }
};

template<> class elliptic< __fx64 > final : public iir_abstract< __fx64 >
{
typedef __fx64 __type;

    filter_data< __type > compute_lowpass () override { return __cheb2_ellip_digital_lp__< __type >( m_Fs , m_Bandwidth.Fc , m_order , 1 , m_Attenuation.G2 , m_Attenuation.G1 ); }
    filter_data< __type > compute_highpass() override { return __cheb2_ellip_digital_hp__< __type >( m_Fs , m_Bandwidth.Fc , m_order , 1 , m_Attenuation.G2 , m_Attenuation.G1 ); }
    filter_data< __type > compute_bandpass() override { return __cheb2_ellip_digital_bp__< __type >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , 1 , m_Attenuation.G2 , m_Attenuation.G1 ); }
    filter_data< __type > compute_bandstop() override { return __cheb2_ellip_digital_bs__< __type >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , 1 , m_Attenuation.G2 , m_Attenuation.G1 ); }

    public:

    // initialization
    void init( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, __fx64 _Gs, __fx64 _Gp )
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

        iir_abstract< __type >::init(_Fs, _order,  _type, _bandwidth, { _Gs, _Gp } );
    }

    // constructors
    elliptic< __type >() : iir_abstract< __type >()
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("elliptic","elliptic()", "constructor call");
        #endif
    }


    // destructor
    ~elliptic< __type >()
    {
        #ifdef IIR_FILTERS_DEBUG
        Debugger::Log("elliptic","~elliptic()", "destructor call");
        #endif
    }

    // opeartors
    inline __type operator()( __type* _input ) override { return filt<__type>(_input); }
};

#undef __fx32
#undef __fx64
#undef __fxx64
#undef __ix32
#undef PI0
#undef PI2

#endif // FILTERS_IIR_H
