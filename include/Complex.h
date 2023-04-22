#ifndef COMPLEX_H
#define COMPLEX_H

#ifndef __ALG_PLATFORM

#include "math.h"
#include "stdlib.h"
#include <limits>
#include "string"
using namespace std;

#endif

//------------------------------------------------------------------------------------------------------------------------------------------------
// define custom types
//------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef __fx32
#define __fx32 float
#endif

#ifndef __fx64
#define __fx64 double
#endif

#ifndef __fxx64
#define __fxx64 long double
#endif

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
template<typename __type = __fx32> class Complex
{
private:

    // service types
    /*!
    * \enum complex_plain
    * \brief defines the complex plain
    */
    enum complex_plain
    {
        REAL, ///< complex number real part
        IMAG  ///< complex number imaginary part
    };

    // service methods

    // complex numbers addtion function
    static void __cadd__(const __type _c1[2], const  __type _c2[2], __type _c3[2] )
    {
        _c3[REAL] = _c1[REAL] + _c2[REAL];
        _c3[IMAG] = _c1[IMAG] + _c2[IMAG];
    }

    // complex numbers substraction function
    static void __csub__(const __type _c1[2], const  __type _c2[2], __type _c3[2] )
    {
        _c3[REAL] = _c1[REAL] - _c2[REAL];
        _c3[IMAG] = _c1[IMAG] - _c2[IMAG];
    }

    // complex numbers multiplication function
    static void __cmul__(const __type _c1[2], const  __type _c2[2], __type _c3[2] )
    {
        _c3[REAL] = _c1[REAL] * _c2[REAL] - _c1[IMAG] * _c2[IMAG];
        _c3[IMAG] = _c1[REAL] * _c2[IMAG] + _c1[IMAG] * _c2[REAL];
    }

    // complex numbers division function
    static void __cdiv__(const __type _c1[2], const  __type _c2[2], __type _c3[2] )
    {
        __type scal =  _c2[REAL] * _c2[REAL] + _c2[IMAG] * _c2[IMAG];
        _c3[REAL]   =  _c1[REAL] * _c2[REAL] + _c1[IMAG] * _c2[IMAG];
        _c3[IMAG]   = -_c1[REAL] * _c2[IMAG] + _c1[IMAG] * _c2[REAL];
        _c3[REAL]  /= scal;
        _c3[IMAG]  /= scal;
    }

    // complex numbers modulus computation function
    static __type __cabs__(const __type _c1[2] )
    {
        return _c1[REAL] == 0.0 && _c1[IMAG] == 0.0 ? 0.0 : sqrt( _c1[REAL] * _c1[REAL] + _c1[IMAG] * _c1[IMAG] );
    }

    // complex numbers angle computation function
    static __type __carg__(const __type _c1[2] )
    {
        return atan2( _c1[IMAG], _c1[REAL] );
    }

    // complex numbers greater function
    static bool __cgt__(const __type _c1[2], const  __type _c2[2] )
    {
        return __cabs__(_c1) > __cabs__(_c2);
    }

    // complex numbers lower function
    static bool __clt__(const __type _c1[2], const  __type _c2[2] )
    {
        return __cabs__(_c1) < __cabs__(_c2);
    }

    // complex numbers equality function
    static bool __ceq__(const __type _c1[2], const  __type _c2[2] )
    {
        return __cabs__(_c1) == __cabs__(_c2);
    }

    // complex numbers greater/equal function
    static bool __cge__(const __type _c1[2], const  __type _c2[2] )
    {
        return __cabs__(_c1) >= __cabs__(_c2);
    }

    // complex numbers lower/equal function
    static bool __cle__(const __type _c1[2], const  __type _c2[2] )
    {
        return __cabs__(_c1) <= __cabs__(_c2);
    }

    /*! \brief complex number real and imaginary part buffer */
    mutable __type m_data[2]{};
public:

    /*! \brief default constructor */
    Complex()
    {
        m_data[REAL] = 0;
        m_data[IMAG] = 0;
    }

    /*!
     *  \brief copy constructor
     *  \param[_c] input complex number
    */
    Complex(const Complex& _c)
    {
        m_data[REAL] = _c.m_data[REAL];
        m_data[IMAG] = _c.m_data[IMAG];
    }

    /*!
     *  \brief initializing constructor
     *  \param[_n] input real number
    */
    Complex( __type _n )
    {
        m_data[REAL] = _n;
        m_data[IMAG] = 0;
    }

    /*!
     *  \brief initializing constructor
     *  \param[_re] input real part of a complex number
     *  \param[_im] input imaginary of a complex number
    */
    Complex( __type _re , __type _im )
    {
        m_data[REAL] = _re;
        m_data[IMAG] = _im;
    }

    /*!
     *  \brief initializing constructor
     *  \param[_c] input 2D array with a real and imaginary part of a complex number
    */
    Complex( __type _c[2] )
    {
        m_data[REAL] = _c[REAL];
        m_data[IMAG] = _c[IMAG];
    }

    // properties
    __type get_real()
    {
        return m_data[REAL];
    }

    __type get_imag()
    {
        return m_data[IMAG];
    }

    void set_real( __type _Value )
    {
        m_data[REAL] = _Value;
    }

    void set_imag( __type _Value )
    {
        m_data[IMAG] = _Value;
    }

    /*! \brief operator to convert fcomplex<__type>() -> fcomplex<__fx32>() */
    operator Complex<__fx32>() const
    {
        return Complex<__fx32>( m_data[REAL], m_data[IMAG] );
    }

    /*! \brief operator to convert fcomplex<__type>() -> fcomplex<__fx64>() */
    operator Complex<__fx64>() const
    {
        return Complex<__fx64>( m_data[REAL], m_data[IMAG] );
    }

    /*! \brief operator to convert fcomplex<__type>() -> fcomplex<__fxx64>() */
    operator Complex<__fxx64>() const
    {
        return Complex<__fxx64>( m_data[REAL], m_data[IMAG] );
    }

    /*! \brief operator to convert fcomplex<__type>() -> __fx32 it must be used explicitly !!! */
    explicit operator __fx32() const
    {
        return m_data[REAL];
    }

    /*! \brief operator to convert fcomplex<__type>() -> __fx32 it must be used explicitly !!! */
    explicit operator __fx64() const
    {
        return m_data[REAL];
    }

    /*! \brief operator to convert fcomplex<__type>() -> __fx32 it must be used explicitly !!! */
    explicit operator __fxx64() const
    {
        return m_data[REAL];
    }

    /*!
     *  \brief initializing operator
     *  \param[_re] input real part of a complex number
     *  \param[_im] input imaginary of a complex number
    */
    inline void operator()( __type _re, __type _im )
    {
        m_data[REAL] = _re;
        m_data[IMAG] = _im;
    }

    /*!
     *  \brief initializing operator
     *  \param[_c] input complex number
    */
    inline void operator()( Complex<__type> _c )
    {
        m_data[REAL] = _c.m_data[REAL];
        m_data[IMAG] = _c.m_data[IMAG];
    }

    /*!
     *  \brief assigning operator
     *  \param[_c] input complex number
    */
    inline Complex<__type> operator = ( Complex<__type> _c )
    {
        m_data[REAL] = _c.m_data[REAL];
        m_data[IMAG] = _c.m_data[IMAG];
        return *this;
    }

    /*!
     *  \brief add operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator + (Complex<__type> _c2 ) const
    {
        Complex<__type> _c3;
        __cadd__( this->m_data , _c2.m_data , _c3.m_data );
        return _c3;
    }

    /*!
     *  \brief add operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator += (Complex<__type> _c2 )
    {
        Complex<__type> _c3;
        __cadd__( this->m_data, _c2.m_data , _c3.m_data );
        this->m_data[REAL] = _c3.m_data[REAL];
        this->m_data[IMAG] = _c3.m_data[IMAG];
        return *this;
    }

    /*!
     *  \brief substraction operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator - (Complex<__type> _c2 ) const
    {
        Complex<__type> _c3;
        __csub__( this->m_data , _c2.m_data , _c3.m_data ) ;
        return _c3;
    }

    /*!
     *  \brief substraction operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator -= ( Complex<__type> _c2 )
    {
        Complex<__type> _c3;
        __csub__( this->m_data , _c2.m_data , _c3.m_data );
        this->m_data[REAL] = _c3.m_data[REAL];
        this->m_data[IMAG] = _c3.m_data[IMAG];
        return *this;
    }

    /*!
     *  \brief multiply operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator * (Complex<__type> _c2 ) const
    {
        Complex<__type> _c3;
        __cmul__( this->m_data , _c2.m_data , _c3.m_data );
        return _c3;
    }

    /*!
     *  \brief multiply operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator * ( __type _c2 ) const
    {
        Complex<__type> _c3( this->m_data[REAL] * _c2 , this->m_data[IMAG] * _c2 );
        return _c3;
    }

    /*!
     *  \brief multiply operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator *= (Complex<__type> _c2 )
    {
        Complex<__type> _c3;
        __cmul__( this->m_data , _c2.m_data , _c3.m_data );
        this->m_data[REAL] = _c3.m_data[REAL];
        this->m_data[IMAG] = _c3.m_data[IMAG];
        return *this;
    }

    /*!
     *  \brief multiply operator
     *  \param[_c2] input real number
    */
    inline Complex<__type> operator *= (__type _c2 ) const
    {
        this->m_data[REAL] *=_c2;
        this->m_data[IMAG] *=_c2;
        return *this;
    }

    /*!
     *  \brief divide operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator / (Complex<__type> _c2 ) const
    {
        Complex<__type> _c3;
        __cdiv__( this->m_data , _c2.m_data , _c3.m_data );
        return _c3;
    }

    /*!
     *  \brief divide operator
     *  \param[_c2] input real number
    */
    inline Complex<__type> operator / ( __type _c2 ) const
    {
        Complex<__type> _c3( this->m_data[REAL] / _c2 ,this->m_data[IMAG] / _c2 );
        return _c3;
    }

    /*!
     *  \brief divide operator
     *  \param[_c2] input complex number
    */
    inline Complex<__type> operator /= (Complex<__type> _c2 ) const
    {
        Complex<__type> _c3;
        __cdiv__( this->m_data , _c2.m_data , _c3.m_data );
        this->m_data[REAL] = _c3.m_data[REAL];
        this->m_data[IMAG] = _c3.m_data[IMAG];
        return *this;
    }

    /*!
     *  \brief divide operator
     *  \param[_c2] input real number
    */
    inline Complex<__type> operator /= (__type _c2 ) const
    {
        this->m_data[REAL] /= _c2;
        this->m_data[IMAG] /= _c2;
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
        return Complex<__type>(0,0);
    }

    #ifdef _STRINGFWD_H

        string to_string() const
        {
            return std::to_string( m_data[REAL] ) + " " + std::to_string( m_data[IMAG] );
        }

        explicit operator string() const
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

/*!
 *  \brief complex number angle computation function
 *  \param[_complex] input complex number
 *  \return The function returns the input complex number real part
*/
template<typename __type> __type
__realf__(Complex<__type> _complex )
{
    return _complex.m_data[ Complex<__type>::complex_plain::REAL ];
}

/*!
 *  \brief complex number angle computation function
 *  \param[_complex] input complex number
 *  \return The function returns the input complex number imaginary part
*/
template<typename __type> __type
__imagf__(Complex<__type> _complex )
{
    return _complex.m_data[ Complex<__type>::complex_plain::IMAG ];
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
__type __abs__(Complex<__type> _complex )
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
__csqrtf__(Complex<__type> _complex )
{
    __type abs = __cabsf__(_complex );
    __type arg = __cargf__(_complex );
    return Complex<__type>( sqrt( abs ) * cos( arg / 2) , sqrt( abs ) * sin( arg / 2) );
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
    return Complex<__type>( _complex / __cabsf__( _complex ) );
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
__crotf__(__fx64 _arg, bool _mode )
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
    __fx32 re = ( exp( __realf__(_complex) ) * cos( __imagf__(_complex) ) + exp( -__realf__(_complex) ) * cos( -__imagf__(_complex) ) ) * (__type)0.5;
    __fx32 im = ( exp( __realf__(_complex) ) * sin( __imagf__(_complex) ) + exp( -__realf__(_complex) ) * sin( -__imagf__(_complex) ) ) * (__type)0.5;
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
#undef __fx32
#undef __fx64
#undef __fxx64
#undef __TO_RADIANS_CONVERSION_MULTIPLYER__
//------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPLEX_H
