#ifndef FILTERS_FIR_H
#define FILTERS_FIR_H

#include "kernel_fir.h"
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

template< typename __type > class classic_fir_abstract : public  filter_abstract
{
protected:

    filter_type   m_filter_type;
    bandwidth     m_bandwidth;
    bool          m_scale;
    delay<__type> m_buff_sx;
    filter_data<__type>    m_cfmatrix;
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


#undef __fx32
#undef __fx64
#undef __fxx64
#undef __ix32
#undef PI0
#undef PI2

#endif // FILTERS_FIR_H
