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
class shared_recursive_fourier : public transfer_function_model
{
protected:

    // system fields
    double          m_Gain;                                       ///< recursive Fourier filter gain
    double          m_HarmonicNumber   = 1;                       ///< recursive Fourier harmonic number
    double          m_Fn               = 50;                      ///< recursive Fourier reference signal frequency, Hz
    Complex<double> m_RotationFactor   = Complex<double>::zero(); ///< recursive Fourier rotation coefficient
    Complex<double> m_PureVector       = Complex<double>::zero(); ///< recursive Fourier output vector
    Complex<double> m_UnitVector       = Complex<double>::zero(); ///< recursive Fourier base unit vector used to generate reference frame
    Complex<double> m_ReferenceFrame   = Complex<double>::zero(); ///< recursive Fourier reference frame
    Complex<double> m_ConvolutedVector = Complex<double>::zero(); ///< recursive Fourier output vector with phase relative to the reference frame

   public:

    /*!  \brief default constructor */
    shared_recursive_fourier() : transfer_function_model()
    {
        #ifdef RFF_DEBUG
        Debugger::Log("shared_recursive_fourier","recursive_fourier_abstract()","Filter constructor call");
        #endif
    }

    /*!  \brief destructor */
    virtual ~shared_recursive_fourier()
    {
        #ifdef RFF_DEBUG
        Debugger::Log("shared_recursive_fourier","~recursive_fourier_abstract()","Filter destructor call");
        #endif
    }

    /*!  \brief returns the filter output */
    Complex<__type> vector( bool _ConvolutedOutput = false )
    {
        return _ConvolutedOutput ? m_ConvolutedVector : m_PureVector;
    }

    /*!
     *  \brief initializes the revursive Fourier filter
     *  \param[Fs] filter sampling frequency, Hz
     *  \param[Fn] filter reference signal frequency, Hz
     *  \param[hnum] harmonic number
     *  \details The function is supposed to be called explicitly by the user
    */
     void init( double _Fs, double _Fn, int64_t _HarmonicNumber )
     {
        m_Fn             = _Fn;
        m_Fs             = _Fs;
        m_Ts             = (double)1.0 / m_Fs;
        m_Order          = ceil( _Fs / _Fn );
        m_Gain           = ( _HarmonicNumber == 0 ) ? ( 1.0 / (double)m_Order ) : ( 2.0 / (double)m_Order );
        m_HarmonicNumber = _HarmonicNumber;
        m_RotationFactor = Complex<double>(cos( PI2 * (double)m_HarmonicNumber / (double)m_Order ), sin( PI2 * (double)m_HarmonicNumber / (double)m_Order ) );

        m_UnitVector =
                Complex<double>
                (
                    cos( PI2 * m_HarmonicNumber * m_Fn / m_Fs ),
                    sin( PI2 * m_HarmonicNumber * m_Fn / m_Fs )
                );

        m_ReferenceFrame = Complex<double>( 1.0, 0.0 );

        #ifdef RFF_DEBUG
        Debugger::Log("shared_recursive_fourier","init()","Filter initialization");
        Debugger::Log("Fn    = " + to_string(m_Fn) );
        Debugger::Log("Fs    = " + to_string(m_Fs) );
        Debugger::Log("Ts    = " + to_string(m_Ts) );
        Debugger::Log("order = " + to_string(m_Order));
        Debugger::Log("Gain  = " + to_string(m_Gain));
        Debugger::Log("hnum  = " + to_string(m_HarmonicNumber) + "\n");
        #endif

        // memory allocation
        #ifdef RFF_DEBUG
        Debugger::Log("shared_recursive_fourier","allocate()","Filter memory allocation");
        #endif
     }

     /*!
      *  \brief computes filter frequecny reponse for the given frequency
      *  \param[F] input frequency, Hz
      *  \returns The function returns the fiter transfer function complex value for the given frequency
     */
    Complex<double> frequency_response( double F ) override
    {
        Complex<double> num = Complex<double>(1,0) - Complex<double>( cos( -PI2 * F * m_Order * m_Ts ) , sin( -PI2 * F * m_Order * m_Ts ) );
        Complex<double> den = Complex<double>(1,0) - Complex<double>( cos( -PI2 * F * m_Ts ) , sin( -PI2 * F * m_Ts ) ) * m_RotationFactor;
        Complex<double> Wz = num / den / (double)m_Order;
        return Wz;
    }

    /*!
     *  \brief Filtering function
     *  \param[input] input signal sample pointer
    */
    template< typename T > inline void
    filt ( T* _Input, delay<__type>& _Buffer )
    {
        // fill buffer
        _Buffer( _Input );

        // compute output
        m_PureVector = m_PureVector * m_RotationFactor + ( ( double )*_Input - ( double )_Buffer[ m_Order ] ) * m_Gain;

        // convolve output vector with reference frame
        m_ConvolutedVector = m_PureVector * __conjf__( m_ReferenceFrame );

        // update reference frame
        m_ReferenceFrame *= m_UnitVector;
    }
};

template< typename __type >
class standalone_recursive_fourier : public transfer_function_model
{
protected:

    // system fields
    double          m_Gain;                                       ///< recursive Fourier filter gain
    double          m_HarmonicNumber   = 1;                       ///< recursive Fourier harmonic number
    double          m_Fn               = 50;                      ///< recursive Fourier reference signal frequency, Hz
    Complex<double> m_RotationFactor   = Complex<double>::zero(); ///< recursive Fourier rotation coefficient
    Complex<double> m_PureVector       = Complex<double>::zero(); ///< recursive Fourier output vector
    Complex<double> m_UnitVector       = Complex<double>::zero(); ///< recursive Fourier base unit vector used to generate reference frame
    Complex<double> m_ReferenceFrame   = Complex<double>::zero(); ///< recursive Fourier reference frame
    Complex<double> m_ConvolutedVector = Complex<double>::zero(); ///< recursive Fourier output vector with phase relative to the reference frame
    delay<__type>   m_InputBuffer;                                ///< recursive Fourier rotation delay buffer

    /*!
     *  \brief Filtering function
     *  \param[input] input signal sample pointer
    */
    template< typename T > inline void
    filt ( T* _Input )
    {
        // fill buffer
        m_InputBuffer( _Input );

        // compute output
        m_PureVector = m_PureVector * m_RotationFactor + ( ( double )*_Input - ( double )m_InputBuffer[ m_Order ] ) * m_Gain;

        // convolve output vector with reference frame
        m_ConvolutedVector = m_PureVector * __conjf__( m_ReferenceFrame );

        // update reference frame
        m_ReferenceFrame *= m_UnitVector;
    }

   public:

    /*!  \brief default constructor */
    standalone_recursive_fourier() : transfer_function_model()
    {
        #ifdef RFF_DEBUG
        Debugger::Log("recursive_fourier_abstract","recursive_fourier_abstract()","Filter constructor call");
        #endif
    }

    /*!  \brief destructor */
    virtual ~standalone_recursive_fourier()
    {
        #ifdef RFF_DEBUG
        Debugger::Log("recursive_fourier_abstract","~recursive_fourier_abstract()","Filter destructor call");
        #endif
    }

    /*!  \brief returns the filter output */
    Complex<__type> vector( bool _ConvolutedOutput = false )
    {
        return _ConvolutedOutput ? m_ConvolutedVector : m_PureVector;
    }

    /*!
     *  \brief initializes the revursive Fourier filter
     *  \param[Fs] filter sampling frequency, Hz
     *  \param[Fn] filter reference signal frequency, Hz
     *  \param[hnum] harmonic number
     *  \details The function is supposed to be called explicitly by the user
    */
     void init( double _Fs, double _Fn, int64_t _HarmonicNumber )
     {
        m_Fn             = _Fn;
        m_Fs             = _Fs;
        m_Ts             = (double)1.0 / m_Fs;
        m_Order          = ceil( _Fs / _Fn );
        m_Gain           = ( _HarmonicNumber == 0 ) ? ( 1.0 / (double)m_Order ) : ( 2.0 / (double)m_Order );
        m_HarmonicNumber = _HarmonicNumber;
        m_RotationFactor = Complex<double>(cos( PI2 * (double)m_HarmonicNumber / (double)m_Order ), sin( PI2 * (double)m_HarmonicNumber / (double)m_Order ) );

        m_UnitVector =
                Complex<double>
                (
                    cos( PI2 * m_HarmonicNumber * m_Fn / m_Fs ),
                    sin( PI2 * m_HarmonicNumber * m_Fn / m_Fs )
                );

        m_ReferenceFrame = Complex<double>( 1.0, 0.0 );

        #ifdef RFF_DEBUG
        Debugger::Log("recursive_fourier_abstract","init()","Filter initialization");
        Debugger::Log("Fn    = " + to_string(m_Fn) );
        Debugger::Log("Fs    = " + to_string(m_Fs) );
        Debugger::Log("Ts    = " + to_string(m_Ts) );
        Debugger::Log("order = " + to_string(m_Order));
        Debugger::Log("Gain  = " + to_string(m_Gain));
        Debugger::Log("hnum  = " + to_string(m_HarmonicNumber) + "\n");
        #endif

        // memory allocation
        #ifdef RFF_DEBUG
        Debugger::Log("recursive_fourier_abstract","allocate()","Filter memory allocation");
        #endif

        m_InputBuffer.allocate( m_Order + 2 );
     }

     /*!
      *  \brief computes filter frequecny reponse for the given frequency
      *  \param[F] input frequency, Hz
      *  \returns The function returns the fiter transfer function complex value for the given frequency
     */
    Complex<double> frequency_response( double F ) override
    {
        Complex<double> num = Complex<double>(1,0) - Complex<double>( cos( -PI2 * F * m_Order * m_Ts ) , sin( -PI2 * F * m_Order * m_Ts ) );
        Complex<double> den = Complex<double>(1,0) - Complex<double>( cos( -PI2 * F * m_Ts ) , sin( -PI2 * F * m_Ts ) ) * m_RotationFactor;
        Complex<double> Wz = num / den / (double)m_Order;
        return Wz;
    }

    /*!
     *  \brief filtering operator
     *  \details The operator supposes to invoke filtering function
    */
    inline Complex<__type> operator ()(__type* _Input, bool _ConvolutedOutput = false )
    {
        filt<__type> ( _Input );

        return vector( _ConvolutedOutput );
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
