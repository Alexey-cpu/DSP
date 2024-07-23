#ifndef COMPLEX_H
#define COMPLEX_H

#ifndef __ALG_PLATFORM
#include "math.h"
#include "stdlib.h"
#include "string"
#include <iomanip>
#endif

//------------------------------------------------------------------------------------------------------------------------------------------------
// define custom types
//------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef __TO_RADIANS_CONVERSION_MULTIPLYER__
#define __TO_RADIANS_CONVERSION_MULTIPLYER__ 0.01745329251994329576923690768489
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------

/*! \defgroup <COMPLEX_ARITHMETICS> ( Complex numbers )
 *  \brief the module describes complex numbers functions
    @{
*/

/*! \defgroup <COMPLEX_NUMBER_REPRESENTATION> ( Complex numbers representation )
 *  \ingroup COMPLEX_ARITHMETICS
 *  \brief the module constins the complex number class
    @{
*/

/*!
* \class fcomplex
* \brief Represents complex number
*/
template<typename __type = float> class Complex
{
public:

    // service types
    /*!
    * \enum complex_plain
    * \brief defines the complex plain
    */
    struct complex_plain
    {
        __type REAL = __type(); ///< complex number real part
        __type IMAG = __type();  ///< complex number imaginary part
    };

    // service methods

    // complex numbers addtion function
    static complex_plain __cadd__(const complex_plain _c1, const  complex_plain _c2)
    {
        return
        {
            _c1.REAL + _c2.REAL,
            _c1.IMAG + _c2.IMAG
        };
    }

    // complex numbers substraction function
    static complex_plain __csub__(const complex_plain _c1, const  complex_plain _c2)
    {
        return
        {
            _c1.REAL - _c2.REAL,
            _c1.IMAG - _c2.IMAG
        };
    }

    // complex numbers multiplication function
    static complex_plain __cmul__(const complex_plain _c1, const  complex_plain _c2)
    {
        return
        {
            _c1.REAL * _c2.REAL - _c1.IMAG * _c2.IMAG,
            _c1.REAL * _c2.IMAG + _c1.IMAG * _c2.REAL
        };
    }

    // complex numbers division function
    static complex_plain __cdiv__(const complex_plain _c1, const  complex_plain _c2)
    {
        __type scal =  _c2.REAL * _c2.REAL + _c2.IMAG * _c2.IMAG;

        return
        {
            ( +_c1.REAL * _c2.REAL + _c1.IMAG * _c2.IMAG ) / scal,
            ( -_c1.REAL * _c2.IMAG + _c1.IMAG * _c2.REAL ) / scal
        };
    }

    // complex numbers modulus computation function
    static __type __cabs__(const complex_plain _c1 )
    {
        return _c1.REAL == 0.0 && _c1.IMAG == 0.0 ? 0.0 : sqrt( _c1.REAL * _c1.REAL + _c1.IMAG * _c1.IMAG );
    }

    // complex numbers angle computation function
    static __type __carg__(const complex_plain _c1 )
    {
        return atan2( _c1.IMAG, _c1.REAL );
    }

    // complex numbers greater function
    static bool __cgt__(const complex_plain _c1, const  complex_plain _c2 )
    {
        return __cabs__(_c1) > __cabs__(_c2);
    }

    // complex numbers lower function
    static bool __clt__(const complex_plain _c1, const  complex_plain _c2 )
    {
        return __cabs__(_c1) < __cabs__(_c2);
    }

    // complex numbers equality function
    static bool __ceq__(const complex_plain _c1, const  complex_plain _c2 )
    {
        return __cabs__(_c1) == __cabs__(_c2);
    }

    // complex numbers greater/equal function
    static bool __cge__(const complex_plain _c1, const  complex_plain _c2 )
    {
        return __cabs__(_c1) >= __cabs__(_c2);
    }

    // complex numbers lower/equal function
    static bool __cle__(const complex_plain _c1, const  complex_plain _c2 )
    {
        return __cabs__(_c1) <= __cabs__(_c2);
    }

    /*! \brief complex number real and imaginary part buffer */
    mutable complex_plain m_data;

    /*! \brief default constructor */
    Complex()
    {
        m_data.REAL = 0;
        m_data.IMAG = 0;
    }

    /*!
     *  \brief initializing constructor
     *  \param[_n] input real number
    */
    Complex( __type _n )
    {
        m_data.REAL = _n;
        m_data.IMAG = 0;
    }

    /*!
     *  \brief initializing constructor
     *  \param[_re] input real part of a complex number
     *  \param[_im] input imaginary of a complex number
    */
    Complex( __type _re , __type _im )
    {
        m_data.REAL = _re;
        m_data.IMAG = _im;
    }

    /*!
     *  \brief initializing constructor
     *  \param[_c] input 2D array with a real and imaginary part of a complex number
    */
    Complex( __type _c[2] )
    {
        m_data.REAL = _c[0];
        m_data.IMAG = _c[1];
    }

    /*! \brief operator to convert fcomplex<__type>() -> fcomplex<float>() */
    inline operator Complex<float>() const
    {
        return Complex<float>( m_data.REAL, m_data.IMAG );
    }

    /*! \brief operator to convert fcomplex<__type>() -> fcomplex<double>() */
    inline operator Complex<double>() const
    {
        return Complex<double>( m_data.REAL, m_data.IMAG );
    }

    /*! \brief operator to convert fcomplex<__type>() -> fcomplex<long double>() */
    inline operator Complex<long double>() const
    {
        return Complex<long double>( m_data.REAL, m_data.IMAG );
    }

    /*! \brief operator to convert fcomplex<__type>() -> float it must be used explicitly !!! */
    inline explicit operator float() const
    {
        return m_data.REAL;
    }

    /*! \brief operator to convert fcomplex<__type>() -> float it must be used explicitly !!! */
    inline explicit operator double() const
    {
        return m_data.REAL;
    }

    /*! \brief operator to convert fcomplex<__type>() -> float it must be used explicitly !!! */
    inline explicit operator long double() const
    {
        return m_data.REAL;
    }

    /*!
     *  \brief initializing operator
     *  \param[_re] input real part of a complex number
     *  \param[_im] input imaginary of a complex number
    */
    inline void operator()( __type _re, __type _im )
    {
        m_data.REAL = _re;
        m_data.IMAG = _im;
    }

    /*!
     *  \brief initializing operator
     *  \param[_c] input complex number
    */
    inline void operator()( Complex<__type> _c )
    {
        m_data.REAL = _c.m_data.REAL;
        m_data.IMAG = _c.m_data.IMAG;
    }

    /*!
     *  \brief add operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator + (Complex<__type> _c2 ) const
    {
        complex_plain _c3 = __cadd__( this->m_data , _c2.m_data );
        return Complex<__type>( _c3.REAL, _c3.IMAG );
    }

    /*!
     *  \brief add operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator += (Complex<__type> _c2 )
    {
        complex_plain _c3 = __cadd__( this->m_data, _c2.m_data );
        this->m_data.REAL = _c3.REAL;
        this->m_data.IMAG = _c3.IMAG;
        return *this;
    }

    /*!
     *  \brief substraction operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator - (Complex<__type> _c2 ) const
    {
        complex_plain _c3 = __csub__( this->m_data , _c2.m_data ) ;
        return Complex<__type>( _c3.REAL, _c3.IMAG );
    }

    /*!
     *  \brief substraction operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator -= ( Complex<__type> _c2 )
    {
        complex_plain _c3 = __csub__( this->m_data , _c2.m_data );
        this->m_data.REAL = _c3.REAL;
        this->m_data.IMAG = _c3.IMAG;
        return *this;
    }

    /*!
     *  \brief multiply operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator * (Complex<__type> _c2 ) const
    {
        complex_plain _c3 = __cmul__( this->m_data , _c2.m_data );
        return Complex<__type>( _c3.REAL, _c3.IMAG );
    }

    /*!
     *  \brief multiply operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator * ( __type _c2 ) const
    {
        return Complex<__type>( this->m_data.REAL * _c2 , this->m_data.IMAG * _c2 );
    }

    /*!
     *  \brief multiply operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator *= (Complex<__type> _c2 )
    {
        complex_plain _c3 = __cmul__( this->m_data , _c2.m_data );
        this->m_data.REAL = _c3.REAL;
        this->m_data.IMAG = _c3.IMAG;
        return *this;
    }

    /*!
     *  \brief multiply operator
     *  \param[_c2] input real number
    */
    inline Complex<__type> operator *= (__type _c2 ) const
    {
        this->m_data.REAL *=_c2;
        this->m_data.IMAG *=_c2;
        return *this;
    }

    /*!
     *  \brief divide operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator / (Complex<__type> _c2 ) const
    {
        complex_plain _c3 = __cdiv__( this->m_data , _c2.m_data );
        return Complex<__type>( _c3.REAL, _c3.IMAG );
    }

    /*!
     *  \brief divide operator
     *  \param[_c2] input real number
    */
    inline Complex<__type> operator / ( __type _c2 ) const
    {
        return Complex<__type>( this->m_data.REAL / _c2 ,this->m_data.IMAG / _c2 );
    }

    /*!
     *  \brief divide operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator /= (Complex<__type> _c2 ) const
    {
        complex_plain _c3 = __cdiv__( this->m_data , _c2.m_data );
        this->m_data.REAL = _c3.REAL;
        this->m_data.IMAG = _c3.IMAG;
        return *this;
    }

    /*!
     *  \brief divide operator
     *  \param[_c2] input real number
    */
    inline Complex<__type> operator /= (__type _c2 ) const
    {
        this->m_data.REAL /= _c2;
        this->m_data.IMAG /= _c2;
        return *this;
    }

    /*!
     *  \brief greater operator
     *  \param[_c2] input complex number
    */
    inline bool operator > (Complex<__type> _c2 ) const
    {
        return __cgt__( this->m_data , _c2.m_data );
    }

    /*!
     *  \brief greater or equal operator
     *  \param[_c2] input complex number
    */
    inline bool operator >= (Complex<__type> _c2 ) const
    {
        return __cge__( this->m_data , _c2.m_data );
    }

    /*!
     *  \brief lower operator
     *  \param[_c2] input complex number
    */
    inline bool operator < (Complex<__type> _c2 ) const
    {
        return __clt__( this->m_data , _c2.m_data );
    }

    /*!
     *  \brief lower or equal operator
     *  \param[_c2] input complex number
    */
    inline bool operator <= (Complex<__type> _c2 ) const
    {
        return __cle__( this->m_data , _c2.m_data );
    }

    /*!
     *  \brief equal operator
     *  \param[_c2] input complex number
    */
    inline bool operator == (Complex<__type> _c2 ) const
    {
        return __ceq__( this->m_data , _c2.m_data );
    }

    /*!
     *  \brief not equal operator
     *  \param[_c2] input complex number
    */
    inline bool operator != (Complex<__type> _c2 ) const
    {
        return !__ceq__( this->m_data , _c2.m_data );
    }

    static Complex<__type> zero()
    {
        return Complex<__type>(0.0, 0.0);
    }

    static Complex<__type> one()
    {
        return Complex<__type>(1.0, 0.0);
    }

    #if defined(_STRINGFWD_H) || defined(_LIBCPP_IOSFWD)

        std::string to_string(int _Precision = 3) const
        {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(_Precision) << m_data.REAL
                   << ( m_data.IMAG > 0 ? "+j" : "-j" ) << std::fixed << std::setprecision(_Precision) << ( m_data.IMAG < 0 ? -m_data.IMAG : m_data.IMAG );

            return stream.str();
        }

        explicit operator std::string() const
        {
            return this->to_string();
        }

        friend std::ostream& operator << ( std::ostream& _Stream, Complex<__type> const& _Input)
        {
               return _Stream << _Input.to_string();
        }

    #endif

    /*!
     *  \brief friend real part extraction function
     *  \param[_complex] input complex number
    */
    template<typename T> friend T __realf__(Complex<T> _complex );

    /*!
     *  \brief friend imaginary part extraction function
     *  \param[_complex] input complex number
    */
    template<typename T> friend T __imagf__(Complex<T> _complex );

    /*!
     *  \brief friend complex number modulus computation function
     *  \param[_complex] input complex number
    */
    template<typename T> friend T __cabsf__ (Complex<T> _complex );

    /*!
     *  \brief friend complex number angle computation function
     *  \param[_complex] input complex number
    */
    template<typename T> friend T __cargf__ (Complex<T> _complex );
};

/*! @} */

/*! \defgroup <COMPLEX_NUMBER_FUNCTIONS> ( Complex numbers utility functions )
 *  \ingroup COMPLEX_ARITHMETICS
 *  \brief Rhe module contains functions to operate with the complex numbers
    @{
*/

template<typename __type> inline std::string __to_string__( Complex<__type> _Input )
{
    return _Input.to_string();
}

/*!
 *  \brief complex number angle computation function
 *  \param[_complex] input complex number
 *  \return The function returns the input complex number real part
*/
template<typename __type> __type
__realf__(Complex<__type> _complex )
{
    return _complex.m_data.REAL;
}

template<typename __type> __type
__realf__(__type _complex )
{
    return _complex;
}

/*!
 *  \brief complex number angle computation function
 *  \param[_complex] input complex number
 *  \return The function returns the input complex number imaginary part
*/
template<typename __type> __type
__imagf__(Complex<__type> _complex )
{
    return _complex.m_data.IMAG;
}

template<typename __type> __type
__imagf__(__type _complex )
{
    (void)_complex;
    return 0.0;
}

/*!
* \brief complex number modulus computation function
* \param[_complex] input complex number
* \return the function returns input 32-bit complex number modulus:
* \f[
*       abs(z) = \sqrt{ real\left( z \right)^2 + imag\left( z \right)^2 }
* \f]
*/
template<typename __type>
__type __cabsf__(Complex<__type> _complex )
{
    return Complex<__type>::__cabs__(_complex.m_data );
}

template<typename __type>
inline __attribute__((always_inline)) __type __abs__(Complex<__type> _complex )
{
    return __cabsf__(_complex);
}

/*!
* \brief complex number angle computation function
* \param[_complex] input complex number
* \return the function returns input 32-bit complex number angle:
* \f[
*       arg(z) = atan2\left( real\left( z \right) , imag\left( z \right) \right)
* \f]
*/
template<typename __type>
__type __cargf__( Complex<__type> _complex )
{
    return Complex<__type>::__carg__(_complex.m_data );
}

/*!
* \brief complex number square root computation function
* \param[_complex] input complex number
* \return the function returns input 32-bit complex number square root:
* \f[
*       \sqrt{z} = \sqrt{ abs(z) } * cos \left( \frac{ arg(z) }{ 2 } \right) + j*\sqrt{ abs(z) } * sin \left( \frac{ arg(z) }{ 2 } \right)
* \f]
*/
template<typename __type> Complex<__type>
__csqrtf__( Complex<__type> _complex )
{
    __type abs = __cabsf__(_complex );
    __type arg = __cargf__(_complex );
    return Complex<__type>( cos( arg * 0.5 ), sin( arg * 0.5 ) ) * sqrt( abs );
}

template<typename __type> Complex<__type>
__cpowf__( Complex<__type> _complex, __type _power )
{
    __type abs = __cabsf__(_complex );
    __type arg = __cargf__(_complex );
    return Complex<__type>( cos( arg * _power ), sin( arg * _power ) ) * pow( abs, _power );
}

/*!
* \brief complex number conjugation functon
* \param[_complex] input complex number
* \return the function returns input 32-bit complex conjugated number:
* \f[
*       conj(z) = real(z) + j * imag(z)
* \f]
*/
template<typename __type> Complex<__type>
__conjf__( Complex<__type> _complex )
{
    _complex( __realf__(_complex) , -__imagf__(_complex) );
    return _complex;
}

/*!
* \brief complex number normalization function
* \param[_complex] input complex number
* \return the function returns 32-bit normalized complex number:
* \f[
*       norm(z) = \frac{ real(z) + j * imag(z) }{ abs( z ) }
* \f]
*/
template<typename __type> Complex<__type>
__cnormf__( Complex<__type> _complex )
{
    __type abs = __cabsf__( _complex );

    return Complex<__type>( _complex / ( abs < 1e-12 ? 1.0 : abs ) );
}

/*!
* \brief rotation vector form function
* \param[_arg] input unit vector angle
* \param[_mode] input unit vector angle format ( degrees / radians )
* \return the function returns 32-bit unit vector represented by a complex number with argument arg:
* \f[
*       rot( arg ) =
*       \begin{cases}
*       cos( arg ) + j*sin( arg ) \quad , \quad mode = 0
*       \\
*       cos( arg * \pi / 180 ) + j*sin( arg * \pi / 180 ) \quad , \quad mode = 1
*       \end{cases}
* \f]
*/
template<typename __type> Complex<__type>
__crotf__(double _arg, bool _mode )
{
    if(_mode ) { _arg *= __TO_RADIANS_CONVERSION_MULTIPLYER__; }
    return Complex<__type>( cos(_arg) , sin(_arg) );
}

/*!
* \brief complex hyperbollic sinus function
* \param[_complex] input complex number
* \return the function returns 32-bit complex number hyperbollic sine:
* \f[
*       snih( z ) = \frac{ e^{z} - e^{-z} }{ 2 }
* \f]
*/
template<typename __type> Complex<__type>
__sinhf__( Complex<__type> _complex )
{
    __type re = ( exp( __realf__(_complex) ) * cos( __imagf__(_complex) ) - exp( -__realf__(_complex) ) * cos( -__imagf__(_complex) ) ) * (__type)0.5;
    __type im = ( exp( __realf__(_complex) ) * sin( __imagf__(_complex) ) - exp( -__realf__(_complex) ) * sin( -__imagf__(_complex) ) ) * (__type)0.5;
    return Complex<__type>( re , im );
}

/*!
* \brief complex hyperbollic cos function
* \param[_complex] input complex number
* \return the function returns 32-bit complex number hyperbollic cos:
* \f[
*       cosh( z ) = \frac{ e^{z} + e^{-z} }{ 2 }
* \f]
*/
template<typename __type> Complex<__type>
__coshf__( Complex<__type> _complex )
{
    float re = ( exp( __realf__(_complex) ) * cos( __imagf__(_complex) ) + exp( -__realf__(_complex) ) * cos( -__imagf__(_complex) ) ) * (__type)0.5;
    float im = ( exp( __realf__(_complex) ) * sin( __imagf__(_complex) ) + exp( -__realf__(_complex) ) * sin( -__imagf__(_complex) ) ) * (__type)0.5;
    return Complex<__type>( re , im );
}

/*!
* \brief complex hyperbollic tan function
* \param[_complex] input complex number
* \return the function returns 32-bit complex number hyperbollic tan:
* \f[
*       tanh( z ) = \frac{ snih(z) }{ cosh(z) }
* \f]
*/
template<typename __type> Complex<__type>
__tanhf__( Complex<__type> _complex ) { return __sinhf__(_complex) / __coshf__(_complex); }

/*!
* \brief complex hyperbollic catan function
* \param[_complex] input complex number
* \return the function returns 32-bit complex number hyperbollic catan:
* \f[
*       catanh( z ) = \frac{ cosh(z) }{ snih(z) }
* \f]
*/
template<typename __type> Complex<__type>
__ctnhf__( Complex<__type> _complex ) { return __coshf__(_complex) / __sinhf__(_complex); }

/*! @} */

/*! @} */

//------------------------------------------------------------------------------------------------------------------------------------------------
// forget macro to avoid aliases
//------------------------------------------------------------------------------------------------------------------------------------------------
#undef __TO_RADIANS_CONVERSION_MULTIPLYER__
//------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPLEX_H
