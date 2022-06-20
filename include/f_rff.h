#ifndef F_RFF_H
#define F_RFF_H

// custom includes
#include "dsp.h"
using namespace DSP_KERNEL;

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

/*!
 *  \brief defines recursive Fourier filter 32-bit realization
 *  \details Defines recursive FIR filter having the following transfer function:
 *   \f[
 *      W(z) = \frac{1}{N} * \frac{ 1 - z^{-N} }{ 1 - z^{-1} * e^{-2*\pi * T_s} }
 *  \f]
*/
template< typename __type >
class recursive_fourier_abstract : public filter_abstract
{
protected:

    // system fields
    __fx64           m_Gain;
    __fx64           m_hnum;
    __fx64           m_Fn;
    fcomplex<__fx64> m_rot;
    fcomplex<__fx64> m_out;
    delay<__type>    m_buffer_sx;

    // memory allocation function
   __ix32  allocate() override
   {
       return ( m_buffer_sx.allocate( m_order + 1 ) );
   }

   // memory deallocation function
   __ix32 deallocate() override
   {
       m_buffer_sx.deallocate();
       return 0;
   }

    // filtering funtion
    template< typename T > inline fcomplex<__type>
    filt ( T *_input )
    {
        m_buffer_sx(_input );
        return (m_out = m_out * m_rot + ( ( __fx64 )*_input - ( __fx64 )m_buffer_sx[ m_order ] ) * m_Gain);
    }

   public:

    // default constructor
    recursive_fourier_abstract() : filter_abstract() {}

    // initializing constructor
    recursive_fourier_abstract(__fx64 _Fs, __fx64 _Fn, __ix32 _hnum )
    {
        init(_Fs, _Fn, _hnum);
    }

    // destructor
    virtual ~recursive_fourier_abstract(){}

    // initialization function
     void init( __fx64 _Fs, __fx64 _Fn, __ix32 _hnum )
     {
         m_Fn      = _Fn;
         m_Fs      = _Fs;
         m_Ts      = 1 / m_Fs;
         m_order   = ceil( _Fs / _Fn );
         m_Gain    = ( _hnum == 0 ) ? ( 1.0 / (__fx64)m_order ) : ( 2.0 / (__fx64)m_order / sqrt(2) );
         m_hnum    = _hnum;
         m_rot(cos( PI2 * (__fx64)m_hnum / (__fx64)m_order ), sin( PI2 * (__fx64)m_hnum / (__fx64)m_order ) );
     }

    // frequency computation function
    fcomplex<__fx64> frequency_response( __fx64 F ) override
    {
        fcomplex<__fx64> num = fcomplex<__fx64>(1,0) - fcomplex<__fx64>( cos( -PI2 * F * m_order * m_Ts ) , sin( -PI2 * F * m_order * m_Ts ) );
        fcomplex<__fx64> den = fcomplex<__fx64>(1,0) - fcomplex<__fx64>( cos( -PI2 * F * m_Ts ) , sin( -PI2 * F * m_Ts ) ) * m_rot;
        fcomplex<__fx64> Wz = num / den / (__fx64)m_order;
        return Wz;
    }

    // operators
    inline virtual fcomplex<__type> operator ()(__type  *input ) = 0;
};

template<typename __type > class recursive_fourier;

// recursive Fourirer filter 32-bit realization
template<> class recursive_fourier<__fx32> final : public recursive_fourier_abstract<__fx32>
{
    typedef __fx32 __type;
public:

    // initialization functiom
    void init( __fx64 _Fs, __fx64 _Fn, __ix32 _hnum )
    {
       recursive_fourier_abstract<__fx32> :: init(_Fs, _Fn, _hnum );
       allocate();
    }

    // constructors
    recursive_fourier() : recursive_fourier_abstract(){}
    recursive_fourier(__fx64 _Fs, __fx64 _Fn, __ix32 _hnum ) : recursive_fourier_abstract(_Fs, _Fn, _hnum)
    {
        init(_Fs, _Fn, _hnum );
    }

    // destructor
    ~recursive_fourier()
    {
        deallocate();
    }

    // operators
    inline fcomplex<__type> operator ()(__type  *input ) override { return filt<__type> ( input ); }
    inline fcomplex<__type> operator ()(__fx64  *input ){ return filt<__fx64> ( input ); }
    inline fcomplex<__type> operator ()(__fxx64 *input ){ return filt<__fxx64>( input ); }
};

// recursive Fourirer filter 64-bit realization
template<> class recursive_fourier<__fx64> final : public recursive_fourier_abstract<__fx64>
{
    typedef __fx64 __type;
public:

    // initialization functiom
    void init( __fx64 _Fs, __fx64 _Fn, __ix32 _hnum )
    {
       recursive_fourier_abstract<__fx64> :: init(_Fs, _Fn, _hnum );
       allocate();
    }

    // constructors
    recursive_fourier() : recursive_fourier_abstract(){}
    recursive_fourier(__fx64 _Fs, __fx64 _Fn, __ix32 _hnum ) : recursive_fourier_abstract(_Fs, _Fn, _hnum)
    {
        init( _Fs, _Fn, _hnum );
    }

    // desturctor
    ~recursive_fourier()
    {
        deallocate();
    }

    // operators
    inline fcomplex<__type> operator ()(__type  *input ) override { return filt<__type> ( input ); }
    inline fcomplex<__type> operator ()(__fxx64 *input ){ return filt<__fxx64>( input ); }
};


#undef __fx32
#undef __fx64
#undef __fxx64
#undef __ix32
#undef PI0
#undef PI2

#endif // F_RFF_H
