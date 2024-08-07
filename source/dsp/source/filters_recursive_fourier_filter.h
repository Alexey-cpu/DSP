#ifndef FILTERS_RECURSIVE_FOURIER_FILTER_H
#define FILTERS_RECURSIVE_FOURIER_FILTER_H

// custom includes
#include "kernel_dsp.h"
using namespace DSP_KERNEL;

/*! \defgroup <RECURSIVE_FOURIER_FILTER> ( recursive fourier filter )
 *  \ingroup SPECIAL_FILTERS
 *  \brief the module contains implementation of recursive Fourier filter
    @{
*/

/*!
 *  \class shared_recursive_fourier
 *  \brief defines recursive Fourier filter
 *  \details Defines recursive FIR filter having the following transfer function:
 *   \f[
 *      W(z) = \frac{1}{N} * \frac{ 1 - z^{-N} }{ 1 - z^{-1} * e^{-2*\pi * T_s} }
 *  \f]
 *
 *  The filter implements Fourier transform using delay buffer provided by the user.
 *  The delay buffer size may be retrived using get_buffer_size() function after
 *  filter initialization.
*/
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

    virtual void allocate(){}

public:

    /*!  \brief default constructor */
    shared_recursive_fourier() : transfer_function_model(){}

    /*!  \brief destructor */
    virtual ~shared_recursive_fourier(){}

    /*!
     *   \brief returns the filter output
     *   \param[_ConvolutedOutput] if the value is true filter returns vector
     *   which phase is computed relatively to the reference frame of the nominal frequency if the value is false
     *   filter returns phase computed without convolution
    */
    Complex<double> get_vector( bool _ConvolutedOutput = false )
    {
        return _ConvolutedOutput ? m_ConvolutedVector : m_PureVector;
    }

    /*!
     *   \brief returns the minimum buffer size needed by the filter
     *   which phase is computed relatively to the reference frame of the nominal frequency if the value is false
     *   filter returns phase computed without convolution
    */
    int get_buffer_size()
    {
        return this->m_Order + 2;
    }

    /*!
     *  \brief initializes the revursive Fourier filter
     *  \param[_Fs] filter sampling frequency, Hz
     *  \param[_Fn] filter reference signal frequency, Hz
     *  \param[_HarmonicNumber] harmonic number
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
                    +cos( PI2 * m_HarmonicNumber * m_Fn / m_Fs ),
                    +sin( PI2 * m_HarmonicNumber * m_Fn / m_Fs )
                );

        m_ReferenceFrame = Complex<double>( 0.0, -1.0 );

        allocate();
    }

    /*!
     *  \brief Filtering function
     *  \param[_Input] input signal sample pointer
     *  \param[_Buffer] the delay buffer used for computations
    */
    template< typename T1, typename T2 > inline void
    filt ( T1* _Input, delay<T2>* _Buffer )
    {
        // check
        if( _Input == nullptr || _Buffer == nullptr || _Buffer->get_buff_size() < this->get_buffer_size() )
            return;

        // compute output
        m_PureVector = m_PureVector * m_RotationFactor + ( ( double )*_Input - ( double )_Buffer->get_data(m_Order) ) * m_Gain;

        // convolve output vector with reference frame
        m_ConvolutedVector = m_PureVector * __conjf__( m_ReferenceFrame );

        // update reference frame
        m_ReferenceFrame *= m_UnitVector;
    }

    /*!
      *  \brief computes filter frequecny reponse for the given frequency
      *  \param[F] input frequency, Hz
      *  \returns The function returns the fiter transfer function complex value for the given frequency
    */
    Complex<double> frequency_response( double _F ) override
    {
        Complex<double> num = Complex<double>(1,0) - Complex<double>( cos( -PI2 * _F * m_Order * m_Ts ) , sin( -PI2 * _F * m_Order * m_Ts ) );
        Complex<double> den = Complex<double>(1,0) - Complex<double>( cos( -PI2 * _F * m_Ts ) , sin( -PI2 * _F * m_Ts ) ) * m_RotationFactor;
        Complex<double> Wz = num / den / (double)m_Order;
        return Wz;
    }
};

/*! @} */

/*!
 *  \class shared_recursive_fourier
 *  \brief defines recursive Fourier filter
 *  \details The filter implements Fourier transform using it's own delay buffer.
*/
template< typename __type >
class standalone_recursive_fourier : public shared_recursive_fourier
{
protected:

    // system fields
    delay<__type> m_InputBuffer;

public:

    standalone_recursive_fourier() : shared_recursive_fourier(){}

    virtual ~standalone_recursive_fourier(){}

    virtual void allocate() override
    {
        m_InputBuffer.allocate( m_Order + 2 );
    }

    inline Complex<__type> filt( __type* _Input, bool _ConvolutedOutput = false )
    {
        // fill buffer
        m_InputBuffer.fill_buff(_Input);

        // filt
        shared_recursive_fourier::filt<__type, __type> ( _Input, &m_InputBuffer );

        // output
        return get_vector( _ConvolutedOutput );
    }

    inline Complex<__type> operator ()(__type* _Input, bool _ConvolutedOutput = false )
    {
        return this->filt( _Input, _ConvolutedOutput );
    }
};

/*! @} */

/*! @} */

#endif // FILTERS_RECURSIVE_FOURIER_FILTER_H
