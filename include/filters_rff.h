#ifndef FILTERS_RFF_H
#define FILTERS_RFF_H

// custom includes
#include "kernel_dsp.h"
using namespace DSP_KERNEL;

#ifndef __ALG_PLATFORM
#define RFF_DEBUG // debugging is not available if the algorithm is running on a device !!!
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

/*! \defgroup <RECURSIVE_FOURIER_FILTER> ( Recursive fourier filter )
 *  \ingroup FILTERS
 *  \brief the module contains abstract model and implementation of recursive Fourier filter
    @{
*/

/*! \defgroup <RECURSIVE_FOURIER_FILTER_ABSTRACT_MODEL> ( Recursive fourier filter abstract model )
 *  \ingroup RECURSIVE_FOURIER_FILTER
 *  \brief the module contains abstract model of recursive Fourier filter
    @{
*/

/*!
 *  \class recursive_fourier_abstract
 *  \brief defines recursive Fourier filter
 *  \details Defines recursive FIR filter having the following transfer function:
 *   \f[
 *      W(z) = \frac{1}{N} * \frac{ 1 - z^{-N} }{ 1 - z^{-1} * e^{-2*\pi * T_s} }
 *  \f]
*/
template< typename __type >
class recursive_fourier_abstract : public model_base
{
private:

    // memory allocation function
   __ix32  allocate()
   {
       #ifdef RFF_DEBUG
       Debugger::Log("recursive_fourier_abstract::allocate() call \n");
       #endif

       return ( m_buffer_sx.allocate( m_order + 2 ) );
   }

   // memory free function
   __ix32 deallocate()
   {
       #ifdef RFF_DEBUG
       Debugger::Log("recursive_fourier_abstract::deallocate() call \n");
       #endif

       m_buffer_sx.deallocate();
       return 0;
   }

protected:

    // system fields
    __fx64           m_Gain;      ///< recursive Fourier filter gain
    __fx64           m_hnum;      ///< recursive Fourier harmonic number
    __fx64           m_Fn;        ///< recursive Fourier reference signal frequency, Hz
    fcomplex<__fx64> m_rot;       ///< recursive Fourier rotation coefficient
    fcomplex<__fx64> m_out;       ///< recursive Fourier rotation output
    delay<__type>    m_buffer_sx; ///< recursive Fourier rotation delay buffer

    /*!
     *  \brief Filtering function
     *  \param[input] input signal sample pointer
    */
    template< typename T > inline fcomplex<__type>
    filt ( T *_input )
    {
        m_buffer_sx( _input );
        return (m_out = m_out * m_rot + ( ( __fx64 )*_input - ( __fx64 )m_buffer_sx[ m_order ] ) * m_Gain);
    }

   public:

    /*!  \brief returns the filter output */
    fcomplex<__type> vector()
    {
        return m_out;
    }

    /*!  \brief default constructor */
    recursive_fourier_abstract() : model_base()
    {
        #ifdef RFF_DEBUG
        Debugger::Log("recursive_fourier_abstract() call \n");
        #endif
    }

    /*!  \brief destructor */
    virtual ~recursive_fourier_abstract()
    {
        #ifdef RFF_DEBUG
        Debugger::Log("~recursive_fourier_abstract() call \n");
        #endif

        deallocate();
    }

    /*!
     *  \brief initializes the harmonic filter
     *  \param[Fs] filter sampling frequency, Hz
     *  \param[Fn] filter reference signal frequency, Hz
     *  \param[hnum] harmonic number
     *  \details The function is supposed to be called explicitly by the user
     *           before filter resources are allocated
    */
     void init( __fx64 _Fs, __fx64 _Fn, __ix32 _hnum )
     {
         m_Fn      = _Fn;
         m_Fs      = _Fs;
         m_Ts      = (__fx64)1 / m_Fs;
         m_order   = ceil( _Fs / _Fn );
         m_Gain    = ( _hnum == 0 ) ? ( 1.0 / (__fx64)m_order ) : ( 2.0 / (__fx64)m_order / sqrt(2) );
         m_hnum    = _hnum;
         m_rot(cos( PI2 * (__fx64)m_hnum / (__fx64)m_order ), sin( PI2 * (__fx64)m_hnum / (__fx64)m_order ) );

        #ifdef RFF_DEBUG
        Debugger::Log("recursive_fourier_abstract() init call: ");
        Debugger::Log("Fn    = " + to_string(m_Fn) );
        Debugger::Log("Fs    = " + to_string(m_Fs) );
        Debugger::Log("Ts    = " + to_string(m_Ts) );
        Debugger::Log("order = " + to_string(m_order));
        Debugger::Log("Gain  = " + to_string(m_Gain));
        Debugger::Log("hnum  = " + to_string(m_hnum) + "\n");
        #endif

        //
        allocate();

     }

     /*!
      *  \brief computes filter frequecny reponse for the given frequency
      *  \param[F] input frequency, Hz
      *  \returns The function returns the fiter transfer function complex value for the given frequency
     */
    fcomplex<__fx64> frequency_response( __fx64 F ) override
    {
        fcomplex<__fx64> num = fcomplex<__fx64>(1,0) - fcomplex<__fx64>( cos( -PI2 * F * m_order * m_Ts ) , sin( -PI2 * F * m_order * m_Ts ) );
        fcomplex<__fx64> den = fcomplex<__fx64>(1,0) - fcomplex<__fx64>( cos( -PI2 * F * m_Ts ) , sin( -PI2 * F * m_Ts ) ) * m_rot;
        fcomplex<__fx64> Wz = num / den / (__fx64)m_order;
        return Wz;
    }

    /*!
     *  \brief filtering operator
     *  \details The operator supposes to invoke filtering function
    */
    inline virtual fcomplex<__type> operator ()(__type  *input ) = 0;
};

/*! @} */

/*! \defgroup <RECURSIVE_FOURIER_FILTER_IMPLEMENTATION> ( Recursive fourier filter abstract model implementation )
 *  \ingroup RECURSIVE_FOURIER_FILTER
 *  \brief the module contains abstract model of recursive Fourier filter
    @{
*/

template<typename __type > class recursive_fourier;

template<> class recursive_fourier<__fx32> final : public recursive_fourier_abstract<__fx32>
{
    typedef __fx32 __type;
public:

    // initialization functiom
    void init( __fx64 _Fs, __fx64 _Fn, __ix32 _hnum )
    {
       recursive_fourier_abstract<__fx32> :: init(_Fs, _Fn, _hnum );
    }

    // constructors
    recursive_fourier() : recursive_fourier_abstract()
    {
        #ifdef RFF_DEBUG
        Debugger::Log("recursive_fourier() call \n");
        #endif
    }

    // destructor
    ~recursive_fourier()
    {
        #ifdef RFF_DEBUG
        Debugger::Log("~recursive_fourier() call \n");
        #endif
    }

    // operators
    inline fcomplex<__type> operator ()(__type  *input ) override { return filt<__type> ( input ); }
    inline fcomplex<__type> operator ()(__fx64  *input ){ return filt<__fx64> ( input ); }
    inline fcomplex<__type> operator ()(__fxx64 *input ){ return filt<__fxx64>( input ); }
};

template<> class recursive_fourier<__fx64> final : public recursive_fourier_abstract<__fx64>
{
    typedef __fx64 __type;
public:

    // initialization functiom
    void init( __fx64 _Fs, __fx64 _Fn, __ix32 _hnum )
    {
       recursive_fourier_abstract<__fx64> :: init(_Fs, _Fn, _hnum );
    }

    // constructors
    recursive_fourier() : recursive_fourier_abstract()
    {
        #ifdef RFF_DEBUG
        Debugger::Log("recursive_fourier() call \n");
        #endif
    }

    // destructor
    ~recursive_fourier()
    {
        #ifdef RFF_DEBUG
        Debugger::Log("~recursive_fourier() call \n");
        #endif
    }

    // operators
    inline fcomplex<__type> operator ()(__type  *input ) override { return filt<__type> ( input ); }
    inline fcomplex<__type> operator ()(__fxx64 *input ){ return filt<__fxx64>( input ); }
};

/*! @} */

/*! @} */

// macro undefenition to avoid aliases
#undef __fx32
#undef __fx64
#undef __fxx64
#undef __ix32
#undef PI0
#undef PI2

#endif // FILTERS_RFF_H
