/*!
 * \file
 * \brief   Complex numbers template class and functions
 * \authors A.Tykvinskiy
 * \date    03.22.2022
 * \version 1.0.1
 *
 * The header declares template complex number class and functions
*/

#ifndef FCOMPLEX_H
#define FCOMPLEX_H

/*! \defgroup <COMPLEX_ARITHMETICS> ( Complex numbers functions )
 *  \brief the module describes complex numbers functions
    @{
*/

#ifndef __ALG_PLATFORM
#include "math.h"
#include "stdlib.h"
#include <limits>
#endif

/*! \brief converts radians to degrees */
#ifndef __TO_DEGREES
#define __TO_DEGREES(x) ( (x) * 57.295779513082320876798154814105)
#endif

/*! \brief converts degrees to radians */
#ifndef __TO_RADIANS
#define __TO_RADIANS(x) ( (x) * 0.01745329251994329576923690768489)
#endif

/*! \brief defines 16-bit integer type */
#ifndef __ix16
#define __ix16 short
#endif

/*! \brief defines 32-bit integer type */
#ifndef __ix32
#define __ix32 int
#endif

/*! \brief defines 64-bit integer type */
#ifndef __ix64
#define __ix64 long long
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

/*! \brief defines upper limit of 32-bit floating point type */
#ifndef __max_fx32
#define __max_fx32 3.402823466e+38
#endif

/*! \brief defines upper limit of 64-bit floating point type */
#ifndef __max_fx64
#define __max_fx64 1.7976931348623158e+308
#endif

/*! \brief defines lower limit of 32-bit floating point type */
#ifndef __min_fx32
#define __min_fx32 1.175494351e-38
#endif

/*! \brief defines lower limit of 64-bit floating point type */
#ifndef __min_fx64
#define __min_fx64 2.22507e-308
#endif

/*!
* \enum complex_plain
* \brief defines the complex plain
*/
enum complex_plain
{
    REAL, ///< complex number real part
    IMAG  ///< complex number imaginary part
};

/*!
* \brief   complex numbers addition function
* \param[_c1] input complex number 1
* \param[_c2] input complex number 2
* \param[_c3] input complex number answer
* \return the function returns complex:
* \f[
*       real(_c3) = real(_c1) + real(_c2)
*       imag(_c3) = imag(_c1) + imag(_c2)
* \f]
*/
template<typename __type> __attribute__( (always_inline) ) inline
void __cadd__(const __type _c1[2], const  __type _c2[2], __type _c3[2] )
{
    _c3[REAL] = _c1[REAL] + _c2[REAL];
    _c3[IMAG] = _c1[IMAG] + _c2[IMAG];
}

/*!
* \brief   complex numbers substraction function
* \param[_c1] input complex number 1
* \param[_c2] input complex number 2
* \param[_c3] input complex number answer
* \return the function returns complex:
* \f[
*       real(_c3) = real(_c1) - real(_c2)
*       imag(_c3) = imag(_c1) - imag(_c2)
* \f]
*/
template<typename __type> inline __attribute__( (always_inline) )
void __csub__(const __type _c1[2], const  __type _c2[2], __type _c3[2] )
{
    _c3[REAL] = _c1[REAL] - _c2[REAL];
    _c3[IMAG] = _c1[IMAG] - _c2[IMAG];
}

/*!
* \brief   complex numbers multiplication function
* \param[_c1] input complex number 1
* \param[_c2] input complex number 2
* \param[_c3] input complex number answer
* \return the function returns complex:
* \f[
*       real(_c3) = real(_c1) * real(_c2) - imag(_c1) * imag(_c2)
*       imag(_c3) = real(_c1) * imag(_c2) + imag(_c1) * real(_c2)
* \f]
*/
template<typename __type> inline __attribute__( (always_inline) )
void __cmul__(const __type _c1[2], const  __type _c2[2], __type _c3[2] )
{
    _c3[REAL] = _c1[REAL] * _c2[REAL] - _c1[IMAG] * _c2[IMAG];
    _c3[IMAG] = _c1[REAL] * _c2[IMAG] + _c1[IMAG] * _c2[REAL];
}

/*!
* \brief   complex numbers division function
* \param[_c1] input complex number 1
* \param[_c2] input complex number 2
* \param[_c3] input complex number answer
* \return the function returns complex:
* \f[
*       real(_c3) = \frac{ real(_c1) * real(_c2) + imag(_c1) * imag(_c2)  }{ real(_c2) * real(_c2) + imag(_c2) * imag(_c2) }
*       imag(_c3) = \frac{ -real(_c1) * imag(_c2) + imag(_c1) * real(_c2) }{ real(_c2) * real(_c2) + imag(_c2) * imag(_c2) }
* \f]
*/
template<typename __type> inline __attribute__( (always_inline) )
void __cdiv__(const __type _c1[2], const  __type _c2[2], __type _c3[2] )
{
    __type scal =  _c2[REAL] * _c2[REAL] + _c2[IMAG] * _c2[IMAG];
    _c3[REAL]   =  _c1[REAL] * _c2[REAL] + _c1[IMAG] * _c2[IMAG];
    _c3[IMAG]   = -_c1[REAL] * _c2[IMAG] + _c1[IMAG] * _c2[REAL];

    if( scal > std::numeric_limits<__type>::min() )
    {
        _c3[REAL] /= scal;
        _c3[IMAG] /= scal;
    }
    else
    {
        _c3[REAL] = std::numeric_limits<__type>::max();
        _c3[IMAG] = std::numeric_limits<__type>::max();
    }
}

/*!
* \brief   Complex number modulus computation function
* \param[_c1] input complex number
* \return the function returns input 32-bit complex number modulus:
* \f[
*       abs(_c1) = \sqrt{ real\left( _c1 \right)^2 + imag\left( _c1 \right)^2 }
* \f]
*/
template<typename __type> inline __attribute__( (always_inline) )
__type __cabs__(const __type _c1[2] )
{
    return sqrt( _c1[REAL] * _c1[REAL] + _c1[IMAG] * _c1[IMAG] );
}

/*!
* \brief   Complex number angle computation function
* \param[_c1] input complex number
* \return the function returns input 32-bit complex number angle:
* \f[
*       arg(_c1) = atan2\left( real\left( _c1 \right) , imag\left( _c1 \right) \right)
* \f]
*/
template<typename __type> inline __attribute__( (always_inline) )
__type __carg__(const __type _c1[2] )
{
    return atan2( _c1[IMAG] , _c1[REAL] );
}

/*!
* \brief   Complex number greater function
* \param[_c1] input complex number 1
* \param[_c2] input complex number 2
* \return the function returns input 32-bit complex number angle:
* \f[
*       true  = abs(_c1) > abs(_c2) \newline
*       false = abs(_c1) > abs(_c2)
* \f]
*/
template<typename __type> inline __attribute__( (always_inline) )
bool __cgt__(const __type _c1[2], const  __type _c2[2] )
{
    return __cabs__(_c1) > __cabs__(_c2);
}

/*!
* \brief   Complex number lower function
* \param[_c1] input complex number 1
* \param[_c2] input complex number 2
* \return the function returns input 32-bit complex number angle:
* \f[
*       true  = abs(_c1) < abs(_c2) \newline
*       false = abs(_c1) < abs(_c2)
* \f]
*/
template<typename __type> inline __attribute__( (always_inline) )
bool __clt__(const __type _c1[2], const  __type _c2[2] )
{
    return __cabs__(_c1) < __cabs__(_c2);
}

/*!
* \brief   Complex number equal function
* \param[_c1] input complex number 1
* \param[_c2] input complex number 2
* \return the function returns input 32-bit complex number angle:
* \f[
*       true  = abs(_c1) == abs(_c2) \newline
*       false = abs(_c1) == abs(_c2)
* \f]
*/
template<typename __type> inline __attribute__( (always_inline) )
bool __ceq__(const __type _c1[2], const  __type _c2[2] )
{
    return __cabs__(_c1) >= __cabs__(_c2);
}

/*!
* \brief   Complex number greater/equal function
* \param[_c1] input complex number 1
* \param[_c2] input complex number 2
* \return the function returns input 32-bit complex number angle:
* \f[
*       true  = abs(_c1) >= abs(_c2) \newline
*       false = abs(_c1) >= abs(_c2)
* \f]
*/
template<typename __type> inline __attribute__( (always_inline) )
bool __cge__(const __type _c1[2], const  __type _c2[2] )
{
    return __cabs__(_c1) >= __cabs__(_c2);
}

/*!
* \brief   Complex number lower/equal function
* \param[_c1] input complex number 1
* \param[_c2] input complex number 2
* \return the function returns input 32-bit complex number angle:
* \f[
*       true  = abs(_c1) <= abs(_c2) \newline
*       false = abs(_c1) <= abs(_c2)
* \f]
*/
template<typename __type> inline __attribute__( (always_inline) )
bool __cle__(const __type _c1[2], const  __type _c2[2] )
{
    return __cabs__(_c1) <= __cabs__(_c2);
}


/*!
* \class fcomplex
* \brief Represents complex number
*/
template<typename __type = __fx32> class fcomplex
{
    /*! \brief complex number real and imaginary part buffer */
    __type m_data[2]{};
public:

    /*! \brief default constructor */
    fcomplex()
    {
        m_data[REAL] = 0;
        m_data[IMAG] = 0;
    }

    /*!
     *  \brief copy constructor
     *  \param[_c] input complex number
    */
    fcomplex(const fcomplex& _c)
    {
        m_data[REAL] = _c.m_data[REAL];
        m_data[IMAG] = _c.m_data[IMAG];
    }

    /*!
     *  \brief initializing constructor
     *  \param[_n] input real number
    */
    fcomplex( __type _n )
    {
        m_data[REAL] = _n;
        m_data[IMAG] = 0;
    }

    /*!
     *  \brief initializing constructor
     *  \param[_re] input real part of a complex number
     *  \param[_im] input imaginary of a complex number
    */
    fcomplex( __type _re , __type _im )
    {
        m_data[REAL] = _re;
        m_data[IMAG] = _im;
    }

    /*!
     *  \brief initializing constructor
     *  \param[_c] input 2D array with a real and imaginary part of a complex number
    */
    fcomplex( __type _c[2] )
    {
        m_data[REAL] = _c[REAL];
        m_data[IMAG] = _c[IMAG];
    }

    /*! \brief operator to convert fcomplex<__type>() -> fcomplex<__fx32>() */
    operator fcomplex<__fx32>()
    {
        return fcomplex<__fx32>( m_data[REAL], m_data[IMAG] );
    }

    /*! \brief operator to convert fcomplex<__type>() -> fcomplex<__fx64>() */
    operator fcomplex<__fx64>()
    {
        return fcomplex<__fx64>( m_data[REAL], m_data[IMAG] );
    }

    /*! \brief operator to convert fcomplex<__type>() -> fcomplex<__fxx64>() */
    operator fcomplex<__fxx64>()
    {
        return fcomplex<__fxx64>( m_data[REAL], m_data[IMAG] );
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
    inline void operator()( fcomplex<__type> _c )
    {
        m_data[REAL] = _c.m_data[REAL];
        m_data[IMAG] = _c.m_data[IMAG];
    }

    /*!
     *  \brief assigning operator
     *  \param[_c] input complex number
    */
    inline void operator = ( fcomplex<__type> _c )
    {
        m_data[REAL] = _c.m_data[REAL];
        m_data[IMAG] = _c.m_data[IMAG];
    };

    /*!
     *  \brief add operator
     *  \param[_c2] input complex number
    */
    inline fcomplex<__type> operator + (fcomplex<__type> _c2 )
    {
        fcomplex<__type> _c3;
        __cadd__( this->m_data , _c2.m_data , _c3.m_data );
        return _c3;
    }

    /*!
     *  \brief add operator
     *  \param[_c2] input complex number
    */
    inline fcomplex<__type> operator += (fcomplex<__type> _c2 )
    {
        fcomplex<__type> _c3;
        __cadd__( this->m_data , _c2.m_data , _c3.m_data );
        this->m_data[REAL] = _c3.m_data[REAL];
        this->m_data[IMAG] = _c3.m_data[IMAG];
        return *this;
    }

    /*!
     *  \brief substraction operator
     *  \param[_c2] input complex number
    */
    inline fcomplex<__type> operator - (fcomplex<__type> _c2 )
    {
        fcomplex<__type> _c3;
        __csub__( this->m_data , _c2.m_data , _c3.m_data ) ;
        return _c3;
    };

    /*!
     *  \brief substraction operator
     *  \param[_c2] input complex number
    */
    inline fcomplex<__type> operator -= ( fcomplex<__type> _c2 )
    {
        fcomplex<__type> _c3;
        __csub__( this->m_data , _c2.m_data , _c3.m_data );
        this->m_data[REAL] = _c3.m_data[REAL];
        this->m_data[IMAG] = _c3.m_data[IMAG];
        return *this;
    };

    /*!
     *  \brief multiply operator
     *  \param[_c2] input complex number
    */
    inline fcomplex<__type> operator * (fcomplex<__type> _c2 )
    {
        fcomplex<__type> _c3;
        __cmul__( this->m_data , _c2.m_data , _c3.m_data );
        return _c3;
    };

    /*!
     *  \brief multiply operator
     *  \param[_c2] input complex number
    */
    inline fcomplex<__type> operator * ( __type _c2 )
    {
        fcomplex<__type> _c3( this->m_data[REAL]*_c2 , this->m_data[IMAG]*_c2 );
        return _c3;
    };

    /*!
     *  \brief multiply operator
     *  \param[_c2] input complex number
    */
    inline fcomplex<__type> operator *= (fcomplex<__type> _c2 )
    {
        fcomplex<__type> _c3;
        __cmul__( this->m_data , _c2.m_data , _c3.m_data );
        this->m_data[REAL] = _c3.m_data[REAL];
        this->m_data[IMAG] = _c3.m_data[IMAG];
        return *this;
    };

    /*!
     *  \brief multiply operator
     *  \param[_c2] input real number
    */
    inline fcomplex<__type> operator *= (__type _c2 )
    {
        this->m_data[REAL] *=_c2;
        this->m_data[IMAG] *=_c2;
        return *this;
    };

    /*!
     *  \brief divide operator
     *  \param[_c2] input complex number
    */
    inline fcomplex<__type> operator / (fcomplex<__type> _c2 )
    {
        fcomplex<__type> _c3;
        __cdiv__( this->m_data , _c2.m_data , _c3.m_data );
        return _c3;
    };

    /*!
     *  \brief divide operator
     *  \param[_c2] input real number
    */
    inline fcomplex<__type> operator / ( __type _c2 )
    {
        fcomplex<__type> _c3( this->m_data[REAL] / _c2 ,this->m_data[IMAG] / _c2 );
        return _c3;
    };

    /*!
     *  \brief divide operator
     *  \param[_c2] input complex number
    */
    inline fcomplex<__type> operator /= (fcomplex<__type> _c2 )
    {
        fcomplex<__type> _c3;
        __cdiv__( this->m_data , _c2.m_data , _c3.m_data );
        this->m_data[REAL] = _c3.m_data[REAL];
        this->m_data[IMAG] = _c3.m_data[IMAG];
        return *this;
    };

    /*!
     *  \brief divide operator
     *  \param[_c2] input real number
    */
    inline fcomplex<__type> operator /= (__type _c2 )
    {
        this->m_data[REAL] /= _c2;
        this->m_data[IMAG] /= _c2;
        return *this;
    };

    /*!
     *  \brief greater operator
     *  \param[_c2] input complex number
    */
    inline bool operator > (fcomplex<__type> _c2 )
    {
        return __cgt__( this->m_data , _c2.m_data );
    };

    /*!
     *  \brief greater or equal operator
     *  \param[_c2] input complex number
    */
    inline bool operator >= (fcomplex<__type> _c2 )
    {
        return __cge__( this->m_data , _c2.m_data );
    };

    /*!
     *  \brief lower operator
     *  \param[_c2] input complex number
    */
    inline bool operator < (fcomplex<__type> _c2 )
    {
        return __clt__( this->m_data , _c2.m_data );
    };

    /*!
     *  \brief lower or equal operator
     *  \param[_c2] input complex number
    */
    inline bool operator <= (fcomplex<__type> _c2 )
    {
        return __cle__( this->m_data , _c2.m_data );
    };

    /*!
     *  \brief equal operator
     *  \param[_c2] input complex number
    */
    inline bool operator == (fcomplex<__type> _c2 )
    {
        return __ceq__( this->m_data , _c2.m_data );
    }

    /*!
     *  \brief friend real part extraction function
     *  \param[_complex] input complex number
    */
    template<typename T> friend T __realf__(fcomplex<T> _complex );

    /*!
     *  \brief friend imaginary part extraction function
     *  \param[_complex] input complex number
    */
    template<typename T> friend T __imagf__(fcomplex<T> _complex );

    /*!
     *  \brief friend complex number modulus computation function
     *  \param[_complex] input complex number
    */
    template<typename T> friend T __cabsf__ (fcomplex<T> _complex );

    /*!
     *  \brief friend complex number angle computation function
     *  \param[_complex] input complex number
    */
    template<typename T> friend T __cargf__ (fcomplex<T> _complex );
};

/*!
 *  \brief complex number angle computation function
 *  \param[_complex] input complex number
 *  \return The function returns the input complex number real part
*/
template<typename __type> __type
__realf__(fcomplex<__type> _complex )
{
    return _complex.m_data[REAL];
}

/*!
 *  \brief complex number angle computation function
 *  \param[_complex] input complex number
 *  \return The function returns the input complex number imaginary part
*/
template<typename __type> __type
__imagf__(fcomplex<__type> _complex )
{
    return _complex.m_data[IMAG];
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
__type __cabsf__(fcomplex<__type> _complex )
{
    return __cabs__<__type>(_complex.m_data );
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
__type __cargf__( fcomplex<__type> _complex )
{
    return __carg__<__type>(_complex.m_data );
}

/*!
* \brief complex number square root computation function
* \param[_complex] input complex number
* \return the function returns input 32-bit complex number square root:
* \f[
*       \sqrt{z} = \sqrt{ abs(z) } * cos \left( \frac{ arg(z) }{ 2 } \right) + j*\sqrt{ abs(z) } * sin \left( \frac{ arg(z) }{ 2 } \right)
* \f]
*/
template<typename __type> fcomplex<__type>
__csqrtf__(fcomplex<__type> _complex )
{
    __type abs = __cabsf__(_complex );
    __type arg = __cargf__(_complex );
    return fcomplex<__type>( sqrt( abs ) * cos( arg / 2) , sqrt( abs ) * sin( arg / 2) );
}

/*!
* \brief complex number conjugation functon
* \param[_complex] input complex number
* \return the function returns input 32-bit complex conjugated number:
* \f[
*       conj(z) = real(z) + j * imag(z)
* \f]
*/
template<typename __type> fcomplex<__type>
__conjf__( fcomplex<__type> _complex )
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
template<typename __type> fcomplex<__type>
__cnormf__( fcomplex<__type> _complex )
{
    return fcomplex<__type>( _complex / __cabsf__( _complex ) );
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
template<typename __type> fcomplex<__type>
__crotf__(__fx64 _arg, bool _mode )
{
    if(_mode ) { _arg = __TO_RADIANS( _arg ); }
    return fcomplex<__type>( cos(_arg) , sin(_arg) );
}

/*!
* \brief complex hyperbollic sinus function
* \param[_complex] input complex number
* \return the function returns 32-bit complex number hyperbollic sine:
* \f[
*       snih( z ) = \frac{ e^{z} - e^{-z} }{ 2 }
* \f]
*/
template<typename __type> fcomplex<__type>
__sinhf__( fcomplex<__type> _complex )
{
    __type re = ( exp( __realf__(_complex) ) * cos( __imagf__(_complex) ) - exp( -__realf__(_complex) ) * cos( -__imagf__(_complex) ) ) * (__type)0.5;
    __type im = ( exp( __realf__(_complex) ) * sin( __imagf__(_complex) ) - exp( -__realf__(_complex) ) * sin( -__imagf__(_complex) ) ) * (__type)0.5;
    return fcomplex<__type>( re , im );
}

/*!
* \brief complex hyperbollic cos function
* \param[_complex] input complex number
* \return the function returns 32-bit complex number hyperbollic cos:
* \f[
*       cosh( z ) = \frac{ e^{z} + e^{-z} }{ 2 }
* \f]
*/
template<typename __type> fcomplex<__type>
__coshf__( fcomplex<__type> _complex )
{
    __fx32 re = ( exp( __realf__(_complex) ) * cos( __imagf__(_complex) ) + exp( -__realf__(_complex) ) * cos( -__imagf__(_complex) ) ) * (__type)0.5;
    __fx32 im = ( exp( __realf__(_complex) ) * sin( __imagf__(_complex) ) + exp( -__realf__(_complex) ) * sin( -__imagf__(_complex) ) ) * (__type)0.5;
    return fcomplex<__type>( re , im );
}

/*!
* \brief complex hyperbollic tan function
* \param[_complex] input complex number
* \return the function returns 32-bit complex number hyperbollic tan:
* \f[
*       tanh( z ) = \frac{ snih(z) }{ cosh(z) }
* \f]
*/
template<typename __type> fcomplex<__type>
__tanhf__( fcomplex<__type> _complex ) { return __sinhf__(_complex) / __coshf__(_complex); }


/*!
* \brief complex hyperbollic catan function
* \param[_complex] input complex number
* \return the function returns 32-bit complex number hyperbollic catan:
* \f[
*       catanh( z ) = \frac{ cosh(z) }{ snih(z) }
* \f]
*/
template<typename __type> fcomplex<__type>
__ctnhf__( fcomplex<__type> _complex ) { return __coshf__(_complex) / __sinhf__(_complex); }

/*! @} */

#undef __TO_DEGREES
#undef __TO_RADIANS
#undef __CADD__
#undef __CSUB__
#undef __CMUL__
#undef __CDIV__
#undef __CCMP__
#undef __CMOV__
#undef __CINI__

// customized types names exclusion to avoid aliases during compilation:
#undef __ix16
#undef __ix32
#undef __ix64
#undef __uix16
#undef __uix32
#undef __uix64
#undef __fx32
#undef __fx64
#undef __fxx64

// customized upper limits exclusion to avoid aliases during compilation:
#undef __max_fx32
#undef __max_fx64
#undef __max_ix16
#undef __max_ix32
#undef __max_ix64
#undef __max_uix16
#undef __max_uix32
#undef __max_uix64

// customized lower limits exclusion to avoid aliases during compilation:
#undef __min_fx32
#undef __min_fx64
#undef __min_ix16
#undef __min_ix32
#undef __min_ix64
#undef __min_uix16
#undef __min_uix32
#undef __min_uix64


#endif // FCOMPLEX_H
