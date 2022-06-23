#ifndef FILTERS_IIR_H
#define FILTERS_IIR_H

// custom includes
#include "buffer.h"
#include "fcomplex.h"
#include "kernel_iir.h"
using namespace IIR_KERNEL;

#ifndef __ALG_PLATFORM
//#define IIR_DEBUG // debugging is not available if the algorithm is running on a device !!!
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
    delay<__type>       *m_buff_sx;
    delay<__type>       *m_buff_sy;
    filter_data<__type> m_FilterData;
    filter_type         m_FilterType;
    bandwidth           m_Bandwidth;
    attenuation         m_Attenuation;

    // coefficients computation functions
    virtual filter_data<__type> compute_lowpass()  = 0;
    virtual filter_data<__type> compute_highpass() = 0;
    virtual filter_data<__type> compute_bandpass() = 0;
    virtual filter_data<__type> compute_bandstop() = 0;

    // memory allocation
    __ix32 allocate() override
    {
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

        if( m_FilterData.cfden != nullptr && m_FilterData.cfnum != nullptr  && m_FilterData.gains != nullptr )
        {

            m_buff_sx = new delay< __type >[m_FilterData.N];
            m_buff_sy = new delay< __type >[m_FilterData.N];

            for( __ix32 i = 0 ; i < m_FilterData.N ; i++ )
            {
                m_buff_sx[i].allocate(m_FilterData.Nx+1);
                m_buff_sy[i].allocate(m_FilterData.Ny);
            }
        }

        return ( m_buff_sx != nullptr && m_buff_sy != nullptr );

    }

    // memory deallocation
    __ix32 deallocate() override
    {
        if( m_buff_sx != nullptr )
        {
            for( __ix32 i = 0 ; i < m_order ; i++ )
            {
                m_buff_sx[i].deallocate();
            }

            delete [] m_buff_sx;
            m_buff_sx = nullptr;
        }

        if( m_buff_sy != nullptr )
        {
            for( __ix32 i = 0 ; i < m_order ; i++ )
            {
                m_buff_sy[i].deallocate();
            }

            delete [] m_buff_sy;
            m_buff_sy = nullptr;
        }

        // clear filter data
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

    // frequency response computation function
    fcomplex<__fx64> frequency_response( __fx64 F ) override
    {
        return __freq_resp__(m_FilterData.cfnum, m_FilterData.cfden, m_FilterData.gains, m_FilterData.Nx, m_FilterData.Ny, m_FilterData.N, m_Fs, F );
    }

    template<typename T> T filt(T *_input)
    {
        return __filt__(_input, m_FilterData.cfnum, m_FilterData.cfden, m_FilterData.gains, m_FilterData.Nx, m_FilterData.Ny, m_FilterData.N, m_buff_sx, m_buff_sy );
    }

public:

    // constructors
    iir_abstract()
    {
        m_buff_sx     = nullptr;
        m_buff_sy     = nullptr;
        m_FilterType  = filter_type::lowpass;
        m_Bandwidth   = { 100 , 500 };
        m_Attenuation = { 80  , 1   };
    }

    iir_abstract(__fx64 Fs, __ix32 Order, filter_type FilterType, bandwidth Bandwidth, attenuation Attenuation)
    {
        init(Fs, Order, FilterType, Bandwidth, Attenuation);
    }

    // destructor
    virtual ~iir_abstract(){}

    virtual inline __type operator()( __type* _input ) = 0;
};

// butterworth filter realization
namespace
{
    template< typename __type > class butterworth ;

    template<> class butterworth< __fx32 > final : public iir_abstract< __fx32 >
    {
        typedef __fx32 __type;

        filter_data< __type > compute_lowpass () override { return __butt_cheb1_digital_lp__< __type >( m_Fs , m_Bandwidth.Fc , m_order , 0 , m_Attenuation.G1 ); }
        filter_data< __type > compute_highpass() override { return __butt_cheb1_digital_hp__< __type >( m_Fs , m_Bandwidth.Fc , m_order , 0 , m_Attenuation.G1 ); }
        filter_data< __type > compute_bandpass() override { return __butt_cheb1_digital_bp__< __type >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , 0 , m_Attenuation.G1 ); }
        filter_data< __type > compute_bandstop() override { return __butt_cheb1_digital_bs__< __type >( m_Fs , m_Bandwidth.Fc , m_Bandwidth.BW , m_order , 0 , m_Attenuation.G1 ); }

        public:

        // initialization function
        void init( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth )
        {
            iir_abstract< __type >::init(_Fs, _order,  _type, _bandwidth, {1, -1} );
            allocate();
        }

         // constructors
         butterworth< __type >() : iir_abstract< __type >(){}

         butterworth< __type >( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth )
         {
             init(_Fs, _order,  _type, _bandwidth );
         }

         // destructor
         ~butterworth< __type >()
         {
             deallocate();
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
            iir_abstract< __type >::init(_Fs, _order,  _type, _bandwidth, {1, -1} );
            allocate();
        }

         // constructors
         butterworth< __type >() : iir_abstract< __type >(){}
         butterworth< __type >( __fx64 _Fs , __ix32 _order , filter_type _type , bandwidth _bandwidth )
         {
             init(_Fs, _order,  _type, _bandwidth );
         }

         // destructor
         ~butterworth< __type >()
         {
             deallocate();
         };

        inline __type operator()( __type* _input ) override { return filt<__type>(_input); }
    };

}

// checbyshev 1 filter realization
namespace
{
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
            iir_abstract< __type > :: init(_Fs, _order,  _type, _bandwidth, { _Gs, -1 } );
            allocate();
        }

        // constructors
         chebyshev_1< __type >() : iir_abstract< __type >(){}
         chebyshev_1< __type >( __fx64 _Fs , __ix32 _order , filter_type _type, bandwidth _bandwidth , __fx64 _Gs ) : iir_abstract(_Fs, _order,  _type, _bandwidth, { _Gs, -1 } )
         {
             init(_Fs, _order,  _type, _bandwidth, _Gs );
         }

         // destructor
        ~chebyshev_1< __type >()
         {
             deallocate();
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

        // initializing function
        void init( __fx64 _Fs , __ix32 _order , filter_type _type, bandwidth _bandwidth , __fx64 _Gs )
        {
            iir_abstract< __type > :: init(_Fs, _order,  _type, _bandwidth, { _Gs, -1 } );
            allocate();
        }

        // constructor
         chebyshev_1< __type >() : iir_abstract< __type >(){}
         chebyshev_1< __type >( __fx64 _Fs , __ix32 _order , filter_type _type, bandwidth _bandwidth , __fx64 _Gs ) : iir_abstract(_Fs, _order,  _type, _bandwidth, { _Gs, -1 } )
         {
             init(_Fs, _order,  _type, _bandwidth, _Gs );
         }

         // destructor
        ~chebyshev_1< __type >()
         {
             deallocate();
         }

        inline __type operator()( __type* _input ) override { return filt<__type>(_input); }
    };
}

// chebyshev 2 filter realization
namespace
{
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
            iir_abstract< __type >::init(_Fs, _order,  _type, _bandwidth, { _Gp, -1 } );
            allocate();
        }

        // constructors
        chebyshev_2< __type >() : iir_abstract< __type >(){}
        chebyshev_2< __type >( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, __fx64 _Gp )
        {
            init(_Fs, _order,  _type, _bandwidth, _Gp );
        }

        // destructor
        ~chebyshev_2< __type >()
         {
             deallocate();
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

        // initialization function
        void init( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, __fx64 _Gp )
        {
            iir_abstract< __type >::init(_Fs, _order,  _type, _bandwidth, { _Gp, -1 } );
            allocate();
        }

        // constructors
        chebyshev_2< __type >() : iir_abstract< __type >(){}
        chebyshev_2< __type >( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, __fx64 _Gp )
        {
            init(_Fs, _order,  _type, _bandwidth, _Gp );
        }

         // desturctor
        ~chebyshev_2< __type >()
         {
             deallocate();
         }

         // operators
        inline __type operator()( __type* _input ) override { return filt<__type>(_input); }
    };


}

// elliptic filter realization
namespace
{
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
            iir_abstract< __type >::init(_Fs, _order,  _type, _bandwidth, { _Gs, _Gp } );
            allocate();
        }

        // constructors
        elliptic< __type >() : iir_abstract< __type >(){}
        elliptic< __type >( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, __fx64 _Gs, __fx64 _Gp )
        {
            init(_Fs, _order,  _type, _bandwidth, _Gs, _Gp );
        }


        // destructor
        ~elliptic< __type >()
        {
            deallocate();
        };

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

        // initializaton function
        void init( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, __fx64 _Gs, __fx64 _Gp )
        {
            iir_abstract< __type >::init(_Fs, _order,  _type, _bandwidth, { _Gs, _Gp } );
            allocate();
        }

        // constructors
        elliptic< __type >() : iir_abstract< __type >(){}
        elliptic< __type >( __fx64 _Fs, __ix32 _order, filter_type _type, bandwidth _bandwidth, __fx64 _Gs, __fx64 _Gp )
        {
            init(_Fs, _order,  _type, _bandwidth, _Gs, _Gp );
        }

        // destructor
        ~elliptic< __type >()
        {
            deallocate();
        }

        // operators
        inline __type operator()( __type* _input ) override { return filt<__type>(_input); }
    };

}

#undef __fx32
#undef __fx64
#undef __fxx64
#undef __ix32
#undef PI0
#undef PI2

#endif // FILTERS_IIR_H
