#ifndef TRANSFER_FUNCTIONS_H
#define TRANSFER_FUNCTIONS_H

#ifndef __ALG_PLATFORM
#include <iostream>
#include <fstream>
#include "math.h"
#endif

#include "buffer.h"

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

/*! \brief differentiator transfer function */
template< typename T > class differentiator;
/*! \brief aperiodic transfer function */
template< typename T > class aperiodic;
/*! \brief leadlag transfer function */
template< typename T > class leadlag;
/*! \brief integrator transfer function */
template< typename T > class integrator;

/*! \brief second order lowpass filter transfer function */
template< typename T > class lowpass2_filter;
/*! \brief second order highpass filter transfer function */
template< typename T > class highpass2_filter;
/*! \brief second order bandpass filter transfer function */
template< typename T > class bandpass2_filter;
/*! \brief second order bandstop filter of type 1 transfer function */
template< typename T > class bandstop2_filter_type1;
/*! \brief second order bandstop filter of type 2 transfer function */
template< typename T > class bandstop2_filter_type2;

/*!
 *  \brief transfer function filtering function
 *  \param[input] - pointer to the input samples buffer
 *  \param[cfnum] - pointer to the transfer function numerator   coefficients
 *  \param[cfden] - pointer to the transfer function denominator coefficients
 *  \param[Nx   ] - number of the numerator   coefficients
 *  \param[Ny   ] - number of the denominator coefficients
 *  \param[bx   ] - input  buffer
 *  \param[by   ] - output buffer
*/
template< typename T > extern inline T __tf_filt__( T *input , T *cfnum , T *cfden , T gain , __ix32 Nx , __ix32 Ny , mirror_ring_buffer<T> &bx , mirror_ring_buffer<T> &by )
{
    T sum_num = 0 , sum_den = 0 , out = 0;
    bx( input );
    for ( __ix32 m = 0 ; m < Nx ; m++)
    {
        sum_num += gain * bx[m] * cfnum[m];
        if ( m < Ny ) sum_den += by[m] * cfden[m + 1];
    }
    by( &( out = sum_num - sum_den ) );
    return out;
}

/*! \brief 32-bit realization of differentiator transfer function */
template<> class differentiator< __fx32 >
{
    typedef __fx32 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief differentiator time constant , s */
    __fx64 m_T;
    /*! \brief transfer function gain */
    __fx64 m_Gain;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 2 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 2 , sizeof ( __type ) );
        m_bx.allocate(3);
        m_by.allocate(3);

        // Gain:
        m_Gain     = 2 / ( m_Ts * ( 1 + 2 * m_T / m_Ts ) );

        // Numerator:
        m_cfnum[0] = 1;
        m_cfnum[1] = -1;

        // Denominator:
        m_cfden[0] = 1;
        m_cfden[1] = (1 - 2 * m_T / m_Ts) / (1 + 2 * m_T / m_Ts);
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    differentiator()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_T    = 0.02;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
    }

    /*! \brief default destructor */
    ~differentiator(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn , __fx64 Td )
    {
        m_Fs        = Fs;
        m_Ts        = 1 / Fs;
        m_Fn        = Fn;
        m_T         = Td;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 2 , 1 , m_bx , m_by ) ); }
};

/*! \brief 64-bit realization of differentiator transfer function */
template<> class differentiator< __fx64 >
{
    typedef __fx64 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief differentiator time constant , s */
    __fx64 m_T;
    /*! \brief transfer function gain */
    __fx64 m_Gain;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 2 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 2 , sizeof ( __type ) );
        m_bx.allocate(3);
        m_by.allocate(3);

        // Gain:
        m_Gain     = 2 / ( m_Ts * ( 1 + 2 * m_T / m_Ts ) );

        // Numerator:
        m_cfnum[0] = 1;
        m_cfnum[1] = -1;

        // Denominator:
        m_cfden[0] = 1;
        m_cfden[1] = (1 - 2 * m_T / m_Ts) / (1 + 2 * m_T / m_Ts);
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    differentiator()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_T    = 0.02;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
    }

    /*! \brief default destructor */
    ~differentiator(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn , __fx64 Td )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_T  = Td;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 2 , 1 , m_bx , m_by ) ); }
};

/*! \brief 32-bit realization of delay transfer function */
template<> class aperiodic< __fx32 >
{
    typedef __fx32 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief differentiator time constant , s */
    __fx64 m_T;
    /*! \brief transfer function gain */
    __fx64 m_Gain;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 2 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 2 , sizeof ( __type ) );
        m_bx.allocate(3);
        m_by.allocate(3);

        // Gain:
        m_Gain     = 1 / (1 + 2 * m_T / m_Ts);

        // numerator:
        m_cfnum[0] = 1;
        m_cfnum[1] = 1;

        // denominator:
        m_cfden[0] = 1;
        m_cfden[1] = (1 - 2 * m_T / m_Ts) / (1 + 2 * m_T / m_Ts);
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    aperiodic()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_T    = 0.02;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
    }

    /*! \brief default destructor */
    ~aperiodic(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn , __fx64 Td )
    {
        m_Fs        = Fs;
        m_Ts        = 1 / Fs;
        m_Fn        = Fn;
        m_T         = Td;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 2 , 1 , m_bx , m_by ) ); }
};

/*! \brief 64-bit realization of delay transfer function */
template<> class aperiodic< __fx64 >
{
    typedef __fx64 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief differentiator time constant , s */
    __fx64 m_T;
    /*! \brief transfer function gain */
    __fx64 m_Gain;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 2 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 2 , sizeof ( __type ) );
        m_bx.allocate(3);
        m_by.allocate(3);

        // Gain:
        m_Gain     = 1 / (1 + 2 * m_T / m_Ts);

        // numerator:
        m_cfnum[0] = 1;
        m_cfnum[1] = 1;

        // denominator:
        m_cfden[0] = 1;
        m_cfden[1] = (1 - 2 * m_T / m_Ts) / (1 + 2 * m_T / m_Ts);
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    aperiodic()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_T    = 0.02;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
    }

    /*! \brief default destructor */
    ~aperiodic(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn , __fx64 Td )
    {
        m_Fs        = Fs;
        m_Ts        = 1 / Fs;
        m_Fn        = Fn;
        m_T         = Td;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 2 , 1 , m_bx , m_by ) ); }
};

/*! \brief 32-bit realization of phase shifting transfer function */
template<> class leadlag< __fx32 >
{
    typedef __fx32 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief time constant T1 , s */
    __fx64 m_T1;
    /*! \brief time constant T2 , s */
    __fx64 m_T2;
    /*! \brief transfer function gain */
    __fx64 m_Gain;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 2 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 2 , sizeof ( __type ) );
        m_bx.allocate(3);
        m_by.allocate(3);

        // Gain:
        m_Gain     = (1 + 2 * m_T1 / m_Ts) / (1 + 2 * m_T2 / m_Ts);

        // numerator:
        m_cfnum[0] = 1;
        m_cfnum[1] = (1 - 2 * m_T1 / m_Ts) / (1 + 2 * m_T1 / m_Ts);;

        // denominator:
        m_cfden[0] = 1;
        m_cfden[1] = (1 - 2 * m_T2 / m_Ts) / (1 + 2 * m_T2 / m_Ts);
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    leadlag()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_T1   = 0.02;
        m_T1   = 0.01;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
    }

    /*! \brief default destructor */
    ~leadlag(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn , __fx64 T1 , __fx64 T2 )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_T1 = T1;
        m_T2 = T2;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 2 , 1 , m_bx , m_by ) ); }
};

/*! \brief 64-bit realization of phase shifting transfer function */
template<> class leadlag< __fx64 >
{
    typedef __fx64 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief time constant T1 , s */
    __fx64 m_T1;
    /*! \brief time constant T2 , s */
    __fx64 m_T2;
    /*! \brief transfer function gain */
    __fx64 m_Gain;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 2 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 2 , sizeof ( __type ) );
        m_bx.allocate(3);
        m_by.allocate(3);

        // Gain:
        m_Gain     = (1 + 2 * m_T1 / m_Ts) / (1 + 2 * m_T2 / m_Ts);

        // numerator:
        m_cfnum[0] = 1;
        m_cfnum[1] = (1 - 2 * m_T1 / m_Ts) / (1 + 2 * m_T1 / m_Ts);;

        // denominator:
        m_cfden[0] = 1;
        m_cfden[1] = (1 - 2 * m_T2 / m_Ts) / (1 + 2 * m_T2 / m_Ts);
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    leadlag()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_T1   = 0.02;
        m_T1   = 0.01;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
    }

    /*! \brief default destructor */
    ~leadlag(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn , __fx64 T1 , __fx64 T2 )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_T1 = T1;
        m_T2 = T2;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 2 , 1 , m_bx , m_by ) ); }
};

/*! \brief 32-bit realization of integrator transfer function */
template<> class integrator< __fx32 >
{
    typedef __fx32 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief transfer function gain */
    __fx64 m_Gain;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 2 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 2 , sizeof ( __type ) );
        m_bx.allocate(3);
        m_by.allocate(3);

        // Gain:
        m_Gain = 0.5 * m_Ts;

        // Numerator:
        m_cfnum[0] = +1;
        m_cfnum[1] = +1;

        // Denominator:
        m_cfden[0] = +1;
        m_cfden[1] = -1;
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    integrator()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
    }

    /*! \brief default destructor */
    ~integrator(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn )
    {
        m_Fs        = Fs;
        m_Ts        = 1 / Fs;
        m_Fn        = Fn;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 2 , 1 , m_bx , m_by ) ); }
};

/*! \brief 64-bit realization of integrator transfer function */
template<> class integrator< __fx64 >
{
    typedef __fx64 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief transfer function gain */
    __fx64 m_Gain;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 2 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 2 , sizeof ( __type ) );
        m_bx.allocate(3);
        m_by.allocate(3);

        // Gain:
        m_Gain = 0.5 * m_Ts;

        // Numerator:
        m_cfnum[0] = +1;
        m_cfnum[1] = +1;

        // Denominator:
        m_cfden[0] = +1;
        m_cfden[1] = -1;
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    integrator()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
    }

    /*! \brief default destructor */
    ~integrator(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn )
    {
        m_Fs        = Fs;
        m_Ts        = 1 / Fs;
        m_Fn        = Fn;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 2 , 1 , m_bx , m_by ) ); }
};

/*! \brief 32-bit realization of second order lowpass filter transfer function */
template<> class lowpass2_filter< __fx32 >
{
    typedef __fx32 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief transfer function gain */
    __fx64 m_Gain;
     /*! \brief damping ratio */
    __fx64 m_Kd;
     /*! \brief cut-off frequency , Hz */
    __fx64 m_Fc;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_bx.allocate(4);
        m_by.allocate(4);

        // auxiliary variables:
        __fx64 omega = tan( ( PI2 * m_Fc ) * m_Ts / 2);
        __fx64 a     = 1;
        __fx64 b     = omega / m_Kd;
        __fx64 c     = omega * omega;
        __fx64 k1    = a + b + c;
        __fx64 k2    = 2 * c - 2 * a;
        __fx64 k3    = a - b + c;

        // Gain:
        m_Gain = omega * omega / k1;

        // Numerator:
        m_cfnum[0] = 1;
        m_cfnum[1] = 2;
        m_cfnum[2] = 1;

        // Denominator:
        m_cfden[0] = 1;
        m_cfden[1] = k2 / k1;
        m_cfden[2] = k3 / k1;
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    lowpass2_filter()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
        m_Kd   = 0.707;
        m_Fc   = 100;
    }

    /*! \brief default destructor */
    ~lowpass2_filter(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn , __fx64 Kd , __fx64 Fc )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_Kd = Kd;
        m_Fc = Fc;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 3 , 2 , m_bx , m_by ) ); }
};

/*! \brief 64-bit realization of second order lowpass filter transfer function */
template<> class lowpass2_filter< __fx64 >
{
    typedef __fx64 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief transfer function gain */
    __fx64 m_Gain;
     /*! \brief damping ratio */
    __fx64 m_Kd;
     /*! \brief cut-off frequency , Hz */
    __fx64 m_Fc;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_bx.allocate(4);
        m_by.allocate(4);

        // auxiliary variables:
        __fx64 omega = tan( ( PI2 * m_Fc ) * m_Ts / 2);
        __fx64 a     = 1;
        __fx64 b     = omega / m_Kd;
        __fx64 c     = omega * omega;
        __fx64 k1    = a + b + c;
        __fx64 k2    = 2 * c - 2 * a;
        __fx64 k3    = a - b + c;

        // Gain:
        m_Gain = omega * omega / k1;

        // Numerator:
        m_cfnum[0] = 1;
        m_cfnum[1] = 2;
        m_cfnum[2] = 1;

        // Denominator:
        m_cfden[0] = 1;
        m_cfden[1] = k2 / k1;
        m_cfden[2] = k3 / k1;
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    lowpass2_filter()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
        m_Kd   = 0.707;
        m_Fc   = 100;
    }

    /*! \brief default destructor */
    ~lowpass2_filter(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn , __fx64 Kd , __fx64 Fc )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_Kd = Kd;
        m_Fc = Fc;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 3 , 2 , m_bx , m_by ) ); }
};

/*! \brief 32-bit realization of second order highpass filter transfer function */
template<> class highpass2_filter< __fx32 >
{
    typedef __fx32 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief transfer function gain */
    __fx64 m_Gain;
     /*! \brief damping ratio */
    __fx64 m_Kd;
     /*! \brief cut-off frequency , Hz */
    __fx64 m_Fc;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_bx.allocate(4);
        m_by.allocate(4);

        // auxiliary variables:
        __fx64 omega = tan( ( PI2 * m_Fc ) * m_Ts / 2);
        __fx64 a = 1;
        __fx64 b = omega / m_Kd;
        __fx64 c = omega * omega;
        __fx64 k1 = a + b + c;
        __fx64 k2 = 2 * c - 2 * a;
        __fx64 k3 = a - b + c;

        // Gain:
        m_Gain = 1 / k1;

        // Numerator:
        m_cfnum[0] = +1;
        m_cfnum[1] = -2;
        m_cfnum[2] = +1;

        // Denominator:
        m_cfden[0] = 1;
        m_cfden[1] = k2 / k1;
        m_cfden[2] = k3 / k1;
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    highpass2_filter()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
        m_Kd   = 0.707;
        m_Fc   = 100;
    }

    /*! \brief default destructor */
    ~highpass2_filter(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn , __fx64 Kd , __fx64 Fc )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_Kd = Kd;
        m_Fc = Fc;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 3 , 2 , m_bx , m_by ) ); }
};

/*! \brief 32-bit realization of second order highpass filter transfer function */
template<> class highpass2_filter< __fx64 >
{
    typedef __fx32 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief transfer function gain */
    __fx64 m_Gain;
     /*! \brief damping ratio */
    __fx64 m_Kd;
     /*! \brief cut-off frequency , Hz */
    __fx64 m_Fc;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_bx.allocate(4);
        m_by.allocate(4);

        // auxiliary variables:
        __fx64 omega = tan( ( PI2 * m_Fc ) * m_Ts / 2);
        __fx64 a = 1;
        __fx64 b = omega / m_Kd;
        __fx64 c = omega * omega;
        __fx64 k1 = a + b + c;
        __fx64 k2 = 2 * c - 2 * a;
        __fx64 k3 = a - b + c;

        // Gain:
        m_Gain = 1 / k1;

        // Numerator:
        m_cfnum[0] = +1;
        m_cfnum[1] = -2;
        m_cfnum[2] = +1;

        // Denominator:
        m_cfden[0] = 1;
        m_cfden[1] = k2 / k1;
        m_cfden[2] = k3 / k1;
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    highpass2_filter()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
        m_Kd   = 0.707;
        m_Fc   = 100;
    }

    /*! \brief default destructor */
    ~highpass2_filter(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn , __fx64 Kd , __fx64 Fc )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_Kd = Kd;
        m_Fc = Fc;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 3 , 2 , m_bx , m_by ) ); }
};

/*! \brief 32-bit realization of second order highpass filter transfer function */
template<> class bandpass2_filter< __fx32 >
{
    typedef __fx32 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief transfer function gain */
    __fx64 m_Gain;
     /*! \brief damping ratio */
    __fx64 m_Kd;
     /*! \brief cut-off frequency , Hz */
    __fx64 m_Fc;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_bx.allocate(4);
        m_by.allocate(4);

        // auxiliary variables:
        __fx64 omega = tan( ( PI2 * m_Fc ) * m_Ts / 2);
        __fx64 a     = 1;
        __fx64 b     = omega / m_Kd;
        __fx64 c     = omega * omega;
        __fx64 k1    = a + b + c;
        __fx64 k2    = 2 * c - 2 * a;
        __fx64 k3    = a - b + c;

        // Gain:
        m_Gain = omega / m_Kd / k1;;

        // Numerator:
        m_cfnum[0] = +1;
        m_cfnum[1] = -0;
        m_cfnum[2] = -1;

        // Denominator:
        m_cfden[0] = 1;
        m_cfden[1] = k2 / k1;
        m_cfden[2] = k3 / k1;
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    bandpass2_filter()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
        m_Kd   = 0.707;
        m_Fc   = 100;
    }

    /*! \brief default destructor */
    ~bandpass2_filter(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn , __fx64 Kd , __fx64 Fc )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_Kd = Kd;
        m_Fc = Fc;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 3 , 2 , m_bx , m_by ) ); }
};

/*! \brief 64-bit realization of second order highpass filter transfer function */
template<> class bandpass2_filter< __fx64 >
{
    typedef __fx32 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief transfer function gain */
    __fx64 m_Gain;
     /*! \brief damping ratio */
    __fx64 m_Kd;
     /*! \brief cut-off frequency , Hz */
    __fx64 m_Fc;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_bx.allocate(4);
        m_by.allocate(4);

        // auxiliary variables:
        __fx64 omega = tan( ( PI2 * m_Fc ) * m_Ts / 2);
        __fx64 a     = 1;
        __fx64 b     = omega / m_Kd;
        __fx64 c     = omega * omega;
        __fx64 k1    = a + b + c;
        __fx64 k2    = 2 * c - 2 * a;
        __fx64 k3    = a - b + c;

        // Gain:
        m_Gain = omega / m_Kd / k1;;

        // Numerator:
        m_cfnum[0] = +1;
        m_cfnum[1] = -0;
        m_cfnum[2] = -1;

        // Denominator:
        m_cfden[0] = 1;
        m_cfden[1] = k2 / k1;
        m_cfden[2] = k3 / k1;
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    bandpass2_filter()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
        m_Kd   = 0.707;
        m_Fc   = 100;
    }

    /*! \brief default destructor */
    ~bandpass2_filter(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn , __fx64 Kd , __fx64 Fc )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_Kd = Kd;
        m_Fc = Fc;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 3 , 2 , m_bx , m_by ) ); }
};

/*! \brief 32-bit realization of second order bandstop type 1 filter transfer function */
template<> class bandstop2_filter_type1< __fx32 >
{
    typedef __fx32 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief transfer function gain */
    __fx64 m_Gain;
     /*! \brief damping ratio */
    __fx64 m_Kd;
     /*! \brief cut-off frequency , Hz */
    __fx64 m_Fc;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_bx.allocate(4);
        m_by.allocate(4);

        // auxiliary variables:
        __fx64 omega = tan( ( PI2 * m_Fc ) * m_Ts / 2);
        __fx64 a     = 1;
        __fx64 b     = omega / m_Kd;
        __fx64 c     = omega * omega;
        __fx64 k1    = a + b + c;
        __fx64 k2    = 2 * c - 2 * a;
        __fx64 k3    = a - b + c;

        // Gain:
        m_Gain = 1 / k1;

        // Numerator:
        m_cfnum[0] = 1 + omega * omega;
        m_cfnum[1] = 2 * omega*omega - 2;
        m_cfnum[2] = 1 + omega * omega;

        // Denominator:
        m_cfden[0] = 1;
        m_cfden[1] = k2 / k1;
        m_cfden[2] = k3 / k1;
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    bandstop2_filter_type1()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
        m_Kd   = 0.707;
        m_Fc   = 100;
    }

    /*! \brief default destructor */
    ~bandstop2_filter_type1(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn , __fx64 Kd , __fx64 Fc )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_Kd = Kd;
        m_Fc = Fc;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 3 , 2 , m_bx , m_by ) ); }
};

/*! \brief 32-bit realization of second order bandstop type 1 filter transfer function */
template<> class bandstop2_filter_type1< __fx64 >
{
    typedef __fx64 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief transfer function gain */
    __fx64 m_Gain;
     /*! \brief damping ratio */
    __fx64 m_Kd;
     /*! \brief cut-off frequency , Hz */
    __fx64 m_Fc;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_bx.allocate(4);
        m_by.allocate(4);

        // auxiliary variables:
        __fx64 omega = tan( ( PI2 * m_Fc ) * m_Ts / 2);
        __fx64 a     = 1;
        __fx64 b     = omega / m_Kd;
        __fx64 c     = omega * omega;
        __fx64 k1    = a + b + c;
        __fx64 k2    = 2 * c - 2 * a;
        __fx64 k3    = a - b + c;

        // Gain:
        m_Gain = 1 / k1;

        // Numerator:
        m_cfnum[0] = 1 + omega * omega;
        m_cfnum[1] = 2 * omega*omega - 2;
        m_cfnum[2] = 1 + omega * omega;

        // Denominator:
        m_cfden[0] = 1;
        m_cfden[1] = k2 / k1;
        m_cfden[2] = k3 / k1;
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    bandstop2_filter_type1()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
        m_Kd   = 0.707;
        m_Fc   = 100;
    }

    /*! \brief default destructor */
    ~bandstop2_filter_type1(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn , __fx64 Kd , __fx64 Fc )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_Kd = Kd;
        m_Fc = Fc;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 3 , 2 , m_bx , m_by ) ); }
};

/*! \brief 32-bit realization of second order bandstop type 2 filter transfer function */
template<> class bandstop2_filter_type2< __fx32 >
{
    typedef __fx32 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief transfer function gain */
    __fx64 m_Gain;
     /*! \brief damping ratio */
    __fx64 m_Fb;
     /*! \brief cut-off frequency , Hz */
    __fx64 m_Fc;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_bx.allocate(4);
        m_by.allocate(4);

        // Расчет вспомогательных коэффициентов:
        __fx64 k1 = -cos(2 * PI0 * m_Fc * m_Ts);
        __fx64 k2 = (1 - tan(PI0 * m_Fb * m_Ts)) / (1 + tan(PI0 * m_Fb * m_Ts));

        // Gain:
        m_Gain = 0.5 * (1 + k2);

        // numerator:
        m_cfnum[0] = 1;
        m_cfnum[1] = 2 * k1;
        m_cfnum[2] = 1;

        // denominator:
        m_cfden[0] = 1;
        m_cfden[1] = k1 * (1 + k2);
        m_cfden[2] = k2;
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    bandstop2_filter_type2()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
        m_Fb   = 100;
        m_Fc   = 120;
    }

    /*! \brief default destructor */
    ~bandstop2_filter_type2(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn , __fx64 Fc , __fx64 Fb )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_Fb = Fb;
        m_Fc = Fc;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 3 , 2 , m_bx , m_by ) ); }
};

/*! \brief 32-bit realization of second order bandstop type 2 filter transfer function */
template<> class bandstop2_filter_type2< __fx64 >
{
    typedef __fx64 __type;
private:
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal nominal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief transfer function gain */
    __fx64 m_Gain;
     /*! \brief damping ratio */
    __fx64 m_Fb;
     /*! \brief cut-off frequency , Hz */
    __fx64 m_Fc;

    /*! \brief input buffer */
    mirror_ring_buffer< __type > m_bx;
    /*! \brief output buffer */
    mirror_ring_buffer< __type > m_by;
    /*! \brief transfer function numerator coefficients */
    __type *m_cfnum;
    /*! \brief transfer function denominator coefficients */
    __type *m_cfden;

public:
    /*! \brief filter output */
    __type m_out;

    /*! \brief memory allocation function */
    void allocate()
    {
        // transfer function input/output and coefficicents buffers memory allocation:
        m_cfnum = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_cfden = ( __type* )calloc( 3 , sizeof ( __type ) );
        m_bx.allocate(4);
        m_by.allocate(4);

        // Расчет вспомогательных коэффициентов:
        __fx64 k1 = -cos(2 * PI0 * m_Fc * m_Ts);
        __fx64 k2 = (1 - tan(PI0 * m_Fb * m_Ts)) / (1 + tan(PI0 * m_Fb * m_Ts));

        // Gain:
        m_Gain = 0.5 * (1 + k2);

        // numerator:
        m_cfnum[0] = 1;
        m_cfnum[1] = 2 * k1;
        m_cfnum[2] = 1;

        // denominator:
        m_cfden[0] = 1;
        m_cfden[1] = k1 * (1 + k2);
        m_cfden[2] = k2;
    }

    /*! \brief memory allocation function */
    void deallocate()
    {
        if( m_cfnum != nullptr ) { free( m_cfnum ); m_cfnum = nullptr; }
        if( m_cfden != nullptr ) { free( m_cfden ); m_cfden = nullptr; }
        m_bx.deallocate();
        m_by.deallocate();
    }

    /*! \brief default constructor */
    bandstop2_filter_type2()
    {
        m_Fs   = 4000;
        m_Fn   = 50;
        m_Ts   = 1 / m_Fs;
        m_Gain = 1;
        m_Fb   = 100;
        m_Fc   = 120;
    }

    /*! \brief default destructor */
    ~bandstop2_filter_type2(){ deallocate(); }

    /*! \brief initialization function */
    void init( __fx64 Fs , __fx64 Fn , __fx64 Fc , __fx64 Fb )
    {
        m_Fs = Fs;
        m_Ts = 1 / Fs;
        m_Fn = Fn;
        m_Fb = Fb;
        m_Fc = Fc;
    }

    /*!
     *  \brief filtering operator
     *  \param[input] - pointer to the input samples
    */
    inline __type operator() ( __type *input ) { return ( m_out = __tf_filt__< __type >( input , m_cfnum , m_cfden , m_Gain , 3 , 2 , m_bx , m_by ) ); }
};

#endif // TRANSFER_FUNCTIONS_H
