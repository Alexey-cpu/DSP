#ifndef FILTERS_RFF_H
#define FILTERS_RFF_H

// custom includes
#include "kernel_dsp.h"
using namespace DSP_KERNEL;

#ifndef __ALG_PLATFORM
#define RFF_DEBUG // debugging is not available if the algorithm is running on a device !!!
#endif

#ifndef PI0
#define PI0 3.1415926535897932384626433832795
#endif

#ifndef PI2
#define PI2 6.283185307179586476925286766559
#endif

/*! \defgroup <RECURSIVE_FOURIER_FILTER> ( Recursive fourier filter )
 *  \ingroup SPECIAL_FILTERS
 *  \brief the module contains model implementation of recursive Fourier filter
    @{
*/

/*!
 *  \class recursive_fourier
 *  \brief defines recursive Fourier filter
 *  \details Defines recursive FIR filter having the following transfer function:
 *   \f[
 *      W(z) = \frac{1}{N} * \frac{ 1 - z^{-N} }{ 1 - z^{-1} * e^{-2*\pi * T_s} }
 *  \f]
*/

template< typename __type >
class recursive_fourier : public transfer_function_model
{
protected:

    // system fields
    double          m_Gain;      ///< recursive Fourier filter gain
    double          m_hnum;      ///< recursive Fourier harmonic number
    double          m_Fn;        ///< recursive Fourier reference signal frequency, Hz
    Complex<double> m_rot;       ///< recursive Fourier rotation coefficient
    Complex<double> m_out;       ///< recursive Fourier rotation output
    delay<__type>   m_buffer_sx; ///< recursive Fourier rotation delay buffer

    //Complex<double> m_rot;

    /*!
     *  \brief Filtering function
     *  \param[input] input signal sample pointer
    */
    template< typename T > inline Complex<__type>
    filt ( T *_input )
    {
        m_buffer_sx( _input );
        return (m_out = m_out * m_rot + ( ( double )*_input - ( double )m_buffer_sx[ m_Order ] ) * m_Gain);
    }

   public:

    /*!  \brief default constructor */
    recursive_fourier() : transfer_function_model()
    {
        #ifdef RFF_DEBUG
        Debugger::Log("recursive_fourier_abstract","recursive_fourier_abstract()","Filter constructor call");
        #endif
    }

    /*!  \brief destructor */
    virtual ~recursive_fourier()
    {
        #ifdef RFF_DEBUG
        Debugger::Log("recursive_fourier_abstract","~recursive_fourier_abstract()","Filter destructor call");
        #endif
    }

    /*!  \brief returns the filter output */
    Complex<__type> vector()
    {
        return m_out;
    }

    /*!
     *  \brief initializes the harmonic filter
     *  \param[Fs] filter sampling frequency, Hz
     *  \param[Fn] filter reference signal frequency, Hz
     *  \param[hnum] harmonic number
     *  \details The function is supposed to be called explicitly by the user
     *           before filter resources are allocated
    */
     void init( double _Fs, double _Fn, int64_t _HarmonicNumber )
     {
         m_Fn      = _Fn;
         m_Fs      = _Fs;
         m_Ts      = (double)1 / m_Fs;
         m_Order   = ceil( _Fs / _Fn );
         m_Gain    = ( _HarmonicNumber == 0 ) ? ( 1.0 / (double)m_Order ) : ( 2.0 / (double)m_Order );
         m_hnum    = _HarmonicNumber;
         m_rot(cos( PI2 * (double)m_hnum / (double)m_Order ), sin( PI2 * (double)m_hnum / (double)m_Order ) );

        #ifdef RFF_DEBUG

        Debugger::Log("recursive_fourier_abstract","init()","Filter initialization");
        Debugger::Log("Fn    = " + to_string(m_Fn) );
        Debugger::Log("Fs    = " + to_string(m_Fs) );
        Debugger::Log("Ts    = " + to_string(m_Ts) );
        Debugger::Log("order = " + to_string(m_Order));
        Debugger::Log("Gain  = " + to_string(m_Gain));
        Debugger::Log("hnum  = " + to_string(m_hnum) + "\n");
        #endif

        // memory allocation
        #ifdef RFF_DEBUG
        Debugger::Log("recursive_fourier_abstract","allocate()","Filter memory allocation");
        #endif

        m_buffer_sx.allocate( m_Order + 2 );
     }

     /*!
      *  \brief computes filter frequecny reponse for the given frequency
      *  \param[F] input frequency, Hz
      *  \returns The function returns the fiter transfer function complex value for the given frequency
     */
    Complex<double> frequency_response( double F ) override
    {
        Complex<double> num = Complex<double>(1,0) - Complex<double>( cos( -PI2 * F * m_Order * m_Ts ) , sin( -PI2 * F * m_Order * m_Ts ) );
        Complex<double> den = Complex<double>(1,0) - Complex<double>( cos( -PI2 * F * m_Ts ) , sin( -PI2 * F * m_Ts ) ) * m_rot;
        Complex<double> Wz = num / den / (double)m_Order;
        return Wz;
    }

    /*!
     *  \brief filtering operator
     *  \details The operator supposes to invoke filtering function
    */
    inline Complex<__type> operator ()(__type  *input )
    {
        return filt<__type> ( input );
    }
};

/*!
 *  \example example_custom_filters_rff.h
*/

/*! @} */

/*! @} */

// macro undefenition to avoid aliases
#undef PI0
#undef PI2

#endif // FILTERS_RFF_H
