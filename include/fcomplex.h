/*!
 * \file
 * \brief   Complex numbers template class and functions
 * \authors A.Tykvinskiy
 * \date    28.12.2021
 * \version 1.0
 *
 * The header declares template complex number class and functions
*/

#ifndef FCOMPLEX_H
#define FCOMPLEX_H

/*! \defgroup <COMPLEX_ARITHMETICS> ( Complex numbers functions )
 *  \brief the module describes complex numbers functions
    @{
*/

// standart headers
#ifndef __ALG_PLATFORM
#include "math.h"
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

// complex arithmetics functions realization macros:

// __cadd__:
#ifndef __CADD__
#define __CADD__                                                                                                                        \
    inline __attribute__( (always_inline) ) complex operator + ( complex _complex ) const { return complex( _complex.m_re + this->m_re , _complex.m_im + this->m_im ); } \
    inline __attribute__( (always_inline) ) complex operator + ( __type   number  ) const { return complex( this->m_re + number , this->m_im ); }                        \
    inline __attribute__( (always_inline) ) complex operator += ( complex _complex ) { this->m_re += _complex.m_re; this->m_im += _complex.m_im; return *this; }         \
    inline __attribute__( (always_inline) ) complex operator += ( __type number ) { this->m_re += number; return *this; }                                                \

#endif

// __csub__:
#ifndef __CSUB__
#define __CSUB__                                                                                                                    \
inline __attribute__( (always_inline) ) complex operator - ( complex _complex ) const { return complex( this->m_re - _complex.m_re , this->m_im - _complex.m_im ); } \
inline __attribute__( (always_inline) ) complex operator - ( __type number ) const { return complex( this->m_re - number , this->m_im ); }                           \
inline __attribute__( (always_inline) ) complex operator -= ( complex _complex ) { this->m_re -= _complex.m_re; this->m_im -= _complex.m_im; return *this; }         \
inline __attribute__( (always_inline) ) complex operator -= ( __type number ) { this->m_re -= number; return *this; }                                                \

#endif

// __cmul__:
#ifndef __CMUL__
#define __CMUL__                                                                                                                                                                             \
inline __attribute__( (always_inline) ) complex operator * (complex _complex ) const { return complex( this->m_re * _complex.m_re - this->m_im * _complex.m_im , this->m_im * _complex.m_re + this->m_re * _complex.m_im ); } \
inline __attribute__( (always_inline) ) complex operator * ( __type number ) const { return complex( this->m_re * number , this->m_im * number ); }                                                                           \
inline __attribute__( (always_inline) ) complex operator *= (complex _complex )                                                                                                                                               \
{                                                                                                                                                                                            \
    __type re0 = this->m_re; __type im0 = this->m_im;                                                                                                                                        \
    this->m_re = re0 * _complex.m_re - im0 * _complex.m_im;                                                                                                                                  \
    this->m_im = im0 * _complex.m_re + re0 * _complex.m_im;                                                                                                                                  \
    return *this;                                                                                                                                                                            \
}                                                                                                                                                                                            \
                                                                                                                                                                                             \
inline __attribute__( (always_inline) ) complex operator *= ( __type number ) { this->m_re *= number; this->m_im *= number; return *this; }                                                                                   \

#endif

// __cdiv__:
#ifndef __CDIV__
#define __CDIV__                                                                                                                                                                                        \
inline __attribute__( (always_inline) ) complex operator / ( complex _complex ) const                                                                                                                                                    \
{                                                                                                                                                                                                       \
    __type den = _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im;                                                                                                                         \
    return ( den == 0 ) ? complex( m_MAX , m_MAX ) : complex( ( this->m_re * _complex.m_re + this->m_im * _complex.m_im ) / den , ( this->m_im * _complex.m_re - this->m_re * _complex.m_im ) / den );  \
}                                                                                                                                                                                                       \
                                                                                                                                                                                                        \
inline __attribute__( (always_inline) ) complex operator / ( __type number ) const { return ( fabs( number ) == 0 ) ? complex( m_MAX , m_MAX )  : complex( this->m_re / number , this->m_im / number ); }                                \
inline __attribute__( (always_inline) ) complex operator /= ( complex _complex )                                                                                                                                                         \
{                                                                                                                                                                                                       \
    __type re0 = this->m_re; __type im0 = this->m_im;                                                                                                                                                   \
    __type den = _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im; \
    if( den == 0 ){ this->m_re = m_MAX; this->m_im = m_MAX; }                                                                                                                                           \
    else{ this->m_re = ( re0 * _complex.m_re + im0 * _complex.m_im ) / den; this->m_im = ( im0 * _complex.m_re - re0 * _complex.m_im ) / den; }                                                         \
    return *this;                                                                                                                                                                                       \
}                                                                                                                                                                                                       \
                                                                                                                                                                                                        \
inline __attribute__( (always_inline) ) complex operator /= ( __type number )                                                                                                                                                            \
{                                                                                                                                                                                                       \
    if( abs( number ) > 0 ) { this->m_re /= number; this->m_im /= number; }                                                                                                                             \
    else { this->m_re = m_MAX; this->m_im = m_MAX; }                                                                                                                                                    \
    return *this;                                                                                                                                                                                       \
}                                                                                                                                                                                                       \

#endif

// __cmove__:
#ifndef __CMOV__
#define __CMOV__                                                                                                               \
inline __attribute__( (always_inline) ) complex operator = ( const complex _complex ) { this->m_re = _complex.m_re; this->m_im = _complex.m_im; return *this; }  \
inline __attribute__( (always_inline) ) complex operator = ( const __type number ) { this->m_re = number; this->m_im = 0; return *this; }                        \

#endif

// __ccmp__:
#ifndef __CCMP__
#define __CCMP__                                                                                                                                                                                              \
inline __attribute__( (always_inline) ) bool operator == ( const complex _complex ) { return ( this->m_re == _complex.m_re ) && ( this->m_im == _complex.m_im ); }                                                                             \
inline __attribute__( (always_inline) ) bool operator > (  const complex _complex  ) { return ( ( sqrt( this->m_re * this->m_re + this->m_im * this->m_im ) ) > ( sqrt( _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im ) ) ); } \
inline __attribute__( (always_inline) ) bool operator < (  const complex _complex  ) { return ( ( sqrt( this->m_re * this->m_re + this->m_im * this->m_im ) ) < ( sqrt( _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im ) ) ); } \

#endif

#ifndef __CINI__
#define __CINI__ inline __attribute__( (always_inline) ) void operator () ( __type re , __type im ) { this->m_re = re; this->m_im = im; } \

#endif


template< typename T > class complex;

template<> class complex < __fx32 >
{
    typedef __fx32 __type;
    __type m_MAX = __max_fx32;
public:

    // real and imaginary part:
    __type m_re , m_im;

    // constructors:
    complex() { m_re  = m_im  = 0; }
    complex( __type re , __type im ) { m_re = re; m_im = im; }
    complex( __type number ) { m_re = number; m_im = 0; }
    complex( const complex &_complex ) { this->m_re = _complex.m_re; this->m_im = _complex.m_im; }

    // destructor:
    ~complex(){}

    // operators:
    __CADD__
    __CSUB__
    __CMUL__
    __CDIV__
    __CCMP__
    __CMOV__
    __CINI__
};

template<> class complex < __fx64 >
{
    typedef __fx64 __type;
    __type m_MAX = __max_fx64;
public:

    // real and imaginary part:
    __type m_re , m_im;

    // constructors:
    complex() { m_re  = m_im  = 0; }
    complex( __type re , __type im ) { m_re = re; m_im = im; }
    complex( __type number ) { m_re = number; m_im = 0; }
    complex( const complex &_complex ) { this->m_re = _complex.m_re; this->m_im = _complex.m_im; }

    // destructor:
    ~complex(){}

    // operators:
    __CADD__
    __CSUB__
    __CMUL__
    __CDIV__
    __CCMP__
    __CMOV__
    __CINI__
    inline operator complex< __fx32  >() { return complex< __fx32  >( this->m_re , this->m_im ); }
};

template<> class complex < __fxx64 >
{
    typedef __fxx64 __type;
    __type m_MAX = __max_fx64;
public:

    // real and imaginary part:
    __type m_re , m_im;

    // constructors:
    complex() { m_re  = m_im  = 0; }
    complex( __type re , __type im ) { m_re = re; m_im = im; }
    complex( __type number ) { m_re = number; m_im = 0; }
    complex( const complex &_complex ) { this->m_re = _complex.m_re; this->m_im = _complex.m_im; }

    // destructor:
    ~complex(){}

    // operators:
    __CADD__
    __CSUB__
    __CMUL__
    __CDIV__
    __CCMP__
    __CMOV__
    __CINI__
    inline operator complex< __fx32  >() { return complex< __fx32  >( this->m_re , this->m_im ); }
    inline operator complex< __fx64  >() { return complex< __fx64  >( this->m_re , this->m_im ); }
};

/*!
* \brief Template sign function
* \details the function returns 1 if the input is positive and -1 otherwise
*/
template< typename T > __ix32 __sign__( T input ) { return ( input > 0 ) ? 1 : -1 ; }

// functions to work with complex numbers:

/*!
* \brief   32-bit floating point complex number modulus computation function
* \param[ _complex ] input complex number
* \return the function returns input 32-bit complex number modulus:
* \f[
*       abs(z) = \sqrt{ real\left( z \right)^2 + imag\left( z \right)^2 }
* \f]
*/
__fx32 __absf__( complex<__fx32> _complex ) { return sqrt( _complex.m_re*_complex.m_re + _complex.m_im*_complex.m_im ); }

/*!
* \brief   64-bit floating point complex number modulus computation function
* \param[ _complex ] input complex number
* \return the function returns input 64-bit complex number modulus:
* \f[
*       abs(z) = \sqrt{ real\left( z \right)^2 + imag\left( z \right)^2 }
* \f]
*/
__fx64 __absf__( complex<__fx64> _complex ) { return sqrt( _complex.m_re*_complex.m_re + _complex.m_im*_complex.m_im ); }

/*!
* \brief   extended 64-bit floating point complex number modulus computation function
* \param[ _complex ] input complex number
* \return the function returns input 64-bit complex number modulus:
* \f[
*       abs(z) = \sqrt{ real\left( z \right)^2 + imag\left( z \right)^2 }
* \f]
*/
__fxx64 __absf__( complex<__fxx64> _complex ) { return sqrt( _complex.m_re*_complex.m_re + _complex.m_im*_complex.m_im ); }

/*!
* \brief   32-bit floating point complex number angle computation function
* \param[ _complex ] input complex number
* \return the function returns input 32-bit complex number angle:
* \f[
*       arg(z) = atan2\left( real\left( z \right) , imag\left( z \right) \right)
* \f]
*/
__fx32 __argf__( complex<__fx32> _complex ) { return atan2( _complex.m_im , _complex.m_re ); }

/*!
* \brief   64-bit floating point complex number angle computation function
* \param[ _complex ] input complex number
* \return the function returns input 64-bit complex number angle:
* \f[
*       arg(z) = atan2\left( real\left( z \right) , imag\left( z \right) \right)
* \f]
*/
__fx64 __argf__( complex<__fx64> _complex ) { return atan2( _complex.m_im , _complex.m_re ); }

/*!
* \brief   extended 64-bit floating point complex number angle computation function
* \param[ _complex ] input complex number
* \return the function returns input 64-bit complex number angle:
* \f[
*       arg(z) = atan2\left( real\left( z \right) , imag\left( z \right) \right)
* \f]
*/
__fx64 __argf__( complex<__fxx64> _complex ) { return atan2( _complex.m_im , _complex.m_re ); }

/*!
* \brief   32-bit floating point complex number square root computation function
* \param[ _complex ] input complex number
* \return the function returns input 32-bit complex number square root:
* \f[
*       \sqrt{z} = \sqrt{ abs(z) } * cos \left( \frac{ arg(z) }{ 2 } \right) + j*\sqrt{ abs(z) } * sin \left( \frac{ arg(z) }{ 2 } \right)
* \f]
*/
complex<__fx32> __sqrtf__( complex<__fx32> _complex )
{
    __fx32 abs = sqrt ( _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im );
    __fx32 arg = atan2( _complex.m_im , _complex.m_re );
    return complex<__fx32>( sqrt( abs ) * cos( arg / 2) , sqrt( abs ) * sin( arg / 2) );
}

/*!
* \brief   64-bit floating point complex number square root computation function
* \param[ _complex ] input complex number
* \return the function returns input 64-bit complex number square root:
* \f[
*       \sqrt{z} = \sqrt{ abs(z) } * cos \left( \frac{ arg(z) }{ 2 } \right) + j*\sqrt{ abs(z) } * sin \left( \frac{ arg(z) }{ 2 } \right)
* \f]
*/
complex<__fx64> __sqrtf__( complex<__fx64> _complex )
{
    __fx64 abs = sqrt ( _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im );
    __fx64 arg = atan2( _complex.m_im , _complex.m_re );
    return complex<__fx64>( sqrt( abs ) * cos( arg / 2) , sqrt( abs ) * sin( arg / 2) );
}

/*!
* \brief   extended 64-bit floating point complex number square root computation function
* \param[ _complex ] input complex number
* \return the function returns input 64-bit complex number square root:
* \f[
*       \sqrt{z} = \sqrt{ abs(z) } * cos \left( \frac{ arg(z) }{ 2 } \right) + j*\sqrt{ abs(z) } * sin \left( \frac{ arg(z) }{ 2 } \right)
* \f]
*/
complex<__fxx64> __sqrtf__( complex<__fxx64> _complex )
{
    __fxx64 abs = sqrt ( _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im );
    __fxx64 arg = atan2( _complex.m_im , _complex.m_re );
    return complex<__fxx64>( sqrt( abs ) * cos( arg / 2) , sqrt( abs ) * sin( arg / 2) );
}

/*!
* \brief   32-bit floating point complex number conjugation functon
* \param[ _complex ] input complex number
* \return the function returns input 32-bit complex conjugated number:
* \f[
*       conj(z) = real(z) + j * imag(z)
* \f]
*/
complex<__fx32> __conjf__( complex<__fx32> _complex ) { _complex.m_im *= -1.0; return _complex; }

/*!
* \brief   64-bit floating point complex number conjugation functon
* \param[ _complex ] input complex number
* \return the function returns input 64-bit complex conjugated number:
* \f[
*       conj(z) = real(z) + j * imag(z)
* \f]
*/
complex<__fx64> __conjf__( complex<__fx64> _complex ) { _complex.m_im *= -1.0; return _complex; }

/*!
* \brief   extended 64-bit floating point complex number conjugation functon
* \param[ _complex ] input complex number
* \return the function returns input 64-bit complex conjugated number:
* \f[
*       conj(z) = real(z) + j * imag(z)
* \f]
*/
complex<__fxx64> __conjf__( complex<__fxx64> _complex ) { _complex.m_im *= -1.0; return _complex; }

/*!
* \brief   32-bit floating point complex number normalization
* \param[ _complex ] input complex number
* \return the function returns 32-bit normalized complex number:
* \f[
*       norm(z) = \frac{ real(z) + j * imag(z) }{ abs( z ) }
* \f]
*/
complex<__fx32> __normf__( complex<__fx32> _complex ) { __fx32 m = __absf__( _complex ); return complex<__fx32>( _complex / m ); }

/*!
* \brief   64-bit floating point complex number normalization
* \param[ _complex ] input complex number
* \return the function returns 64-bit normalized complex number:
* \f[
*       norm(z) = \frac{ real(z) + j * imag(z) }{ abs( z ) }
* \f]
*/
complex<__fx64> __normf__( complex<__fx64> _complex ) { __fx64 m = __absf__( _complex ); return complex<__fx64>( _complex / m ); }

/*!
* \brief   extended 64-bit floating point complex number normalization
* \param[ _complex ] input complex number
* \return the function returns 64-bit normalized complex number:
* \f[
*       norm(z) = \frac{ real(z) + j * imag(z) }{ abs( z ) }
* \f]
*/
complex<__fxx64> __normf__( complex<__fxx64> _complex ) { __fxx64 m = __absf__( _complex ); return complex<__fxx64>( _complex / m ); }

/*!
* \brief   32-bit floating point complex rotation vector form function
* \param[ arg  ] input unit vector angle
* \param[ mode ] input unit vector angle format ( degrees / radians )
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
complex<__fx32> __rotf__( __fx32 arg , bool mode )
{
    if( mode ) { arg = __TO_RADIANS( arg ); }
    return complex<__fx32>( cos(arg) , sin(arg) );
}

/*!
* \brief   64-bit floating point complex rotation vector form function
* \param[ arg  ] input unit vector angle
* \param[ mode ] input unit vector angle format ( degrees / radians )
* \return the function returns 64-bit unit vector represented by a complex number with argument arg:
* \f[
*       rot( arg ) =
*       \begin{cases}
*       cos( arg ) + j*sin( arg ) \quad , \quad mode = 0
*       \\
*       cos( arg * \pi / 180 ) + j*sin( arg * \pi / 180 ) \quad , \quad mode = 1
*       \end{cases}
* \f]
*/
complex<__fx64> __rotf__( __fx64 arg , bool mode )
{
    if( mode ) { arg = __TO_RADIANS( arg ); }
    return complex<__fx64>( cos(arg) , sin(arg) );
}

/*!
* \brief   extended 64-bit floating point complex rotation vector form function
* \param[ arg  ] input unit vector angle
* \param[ mode ] input unit vector angle format ( degrees / radians )
* \return the function returns 64-bit unit vector represented by a complex number with argument arg:
* \f[
*       rot( arg ) =
*       \begin{cases}
*       cos( arg ) + j*sin( arg ) \quad , \quad mode = 0
*       \\
*       cos( arg * \pi / 180 ) + j*sin( arg * \pi / 180 ) \quad , \quad mode = 1
*       \end{cases}
* \f]
*/
complex<__fxx64> __rotf__( __fxx64 arg , bool mode )
{
    if( mode ) { arg = __TO_RADIANS( arg ); }
    return complex<__fxx64>( cos(arg) , sin(arg) );
}

/*!
* \brief   32-bit floating point complex hyperbollic sinus function
* \param[ _complex  ] input complex number
* \return the function returns 32-bit complex number hyperbollic sine:
* \f[
*       snih( z ) = \frac{ e^{z} - e^{-z} }{ 2 }
* \f]
*/
complex<__fx32> __sinhf__( complex<__fx32> _complex )
{
    __fx32 re = ( exp( _complex.m_re ) * cos( _complex.m_im ) - exp( -_complex.m_re ) * cos( -_complex.m_im ) ) * 0.5;
    __fx32 im = ( exp( _complex.m_re ) * sin( _complex.m_im ) - exp( -_complex.m_re ) * sin( -_complex.m_im ) ) * 0.5;
    return complex<__fx32>( re , im );
}

/*!
* \brief   64-bit floating point complex hyperbollic sinus function
* \param[ _complex  ] input complex number
* \return the function returns 64-bit complex number hyperbollic sine:
* \f[
*       snih( z ) = \frac{ e^{z} - e^{-z} }{ 2 }
* \f]
*/
complex<__fx64> __sinhf__( complex<__fx64> _complex )
{
    __fx64 re = ( exp( _complex.m_re ) * cos( _complex.m_im ) - exp( -_complex.m_re ) * cos( -_complex.m_im ) ) * 0.5;
    __fx64 im = ( exp( _complex.m_re ) * sin( _complex.m_im ) - exp( -_complex.m_re ) * sin( -_complex.m_im ) ) * 0.5;
    return complex<__fx64>( re , im );
}

/*!
* \brief   extended 64-bit floating point complex hyperbollic sinus function
* \param[ _complex  ] input complex number
* \return the function returns 64-bit complex number hyperbollic sine:
* \f[
*       snih( z ) = \frac{ e^{z} - e^{-z} }{ 2 }
* \f]
*/
complex<__fxx64> __sinhf__( complex<__fxx64> _complex )
{
    __fxx64 re = ( exp( _complex.m_re ) * cos( _complex.m_im ) - exp( -_complex.m_re ) * cos( -_complex.m_im ) ) * 0.5;
    __fxx64 im = ( exp( _complex.m_re ) * sin( _complex.m_im ) - exp( -_complex.m_re ) * sin( -_complex.m_im ) ) * 0.5;
    return complex<__fxx64>( re , im );
}

/*!
* \brief   32-bit floating point complex hyperbollic cos function
* \param[ _complex  ] input complex number
* \return the function returns 32-bit complex number hyperbollic cos:
* \f[
*       cosh( z ) = \frac{ e^{z} + e^{-z} }{ 2 }
* \f]
*/
complex<__fx32> __coshf__( complex<__fx32> _complex )
{
    __fx32 re = ( exp( _complex.m_re ) * cos( _complex.m_im ) + exp( -_complex.m_re ) * cos( -_complex.m_im ) ) * 0.5;
    __fx32 im = ( exp( _complex.m_re ) * sin( _complex.m_im ) + exp( -_complex.m_re ) * sin( -_complex.m_im ) ) * 0.5;
    return complex<__fx32>( re , im );
}

/*!
* \brief   64-bit floating point complex hyperbollic cos function
* \param[ _complex  ] input complex number
* \return the function returns 64-bit complex number hyperbollic cos:
* \f[
*       cosh( z ) = \frac{ e^{z} + e^{-z} }{ 2 }
* \f]
*/
complex<__fx64> __coshf__( complex<__fx64> _complex )
{
    __fx64 re = ( exp( _complex.m_re ) * cos( _complex.m_im ) + exp( -_complex.m_re ) * cos( -_complex.m_im ) ) * 0.5;
    __fx64 im = ( exp( _complex.m_re ) * sin( _complex.m_im ) + exp( -_complex.m_re ) * sin( -_complex.m_im ) ) * 0.5;
    return complex<__fx64>( re , im );
}

/*!
* \brief   64-bit floating point complex hyperbollic cos function
* \param[ _complex  ] input complex number
* \return the function returns 64-bit complex number hyperbollic cos:
* \f[
*       cosh( z ) = \frac{ e^{z} + e^{-z} }{ 2 }
* \f]
*/
complex<__fxx64> __coshf__( complex<__fxx64> _complex )
{
    __fxx64 re = ( exp( _complex.m_re ) * cos( _complex.m_im ) + exp( -_complex.m_re ) * cos( -_complex.m_im ) ) * 0.5;
    __fxx64 im = ( exp( _complex.m_re ) * sin( _complex.m_im ) + exp( -_complex.m_re ) * sin( -_complex.m_im ) ) * 0.5;
    return complex<__fxx64>( re , im );
}

/*!
* \brief   32-bit floating point complex hyperbollic tan function
* \param[ _complex  ] input complex number
* \return the function returns 32-bit complex number hyperbollic tan:
* \f[
*       tanh( z ) = \frac{ snih(z) }{ cosh(z) }
* \f]
*/
complex<__fx32> __tanhf__( complex<__fx32> _complex ) { return __sinhf__(_complex) / __coshf__(_complex); }

/*!
* \brief   64-bit floating point complex hyperbollic tan function
* \param[ _complex  ] input complex number
* \return the function returns 64-bit complex number hyperbollic tan:
* \f[
*       tanh( z ) = \frac{ snih(z) }{ cosh(z) }
* \f]
*/
complex<__fx64> __tanhf__( complex<__fx64> _complex ) { return __sinhf__(_complex) / __coshf__(_complex); }

/*!
* \brief   64-bit floating point complex hyperbollic tan function
* \param[ _complex  ] input complex number
* \return the function returns 64-bit complex number hyperbollic tan:
* \f[
*       tanh( z ) = \frac{ snih(z) }{ cosh(z) }
* \f]
*/
complex<__fxx64> __tanhf__( complex<__fxx64> _complex ) { return __sinhf__(_complex) / __coshf__(_complex); }

/*!
* \brief   32-bit floating point complex hyperbollic catan function
* \param[ _complex  ] input complex number
* \return the function returns 32-bit complex number hyperbollic catan:
* \f[
*       catanh( z ) = \frac{ cosh(z) }{ snih(z) }
* \f]
*/
complex<__fx32> __ctnhf__( complex<__fx32> _complex ) { return __coshf__(_complex) / __sinhf__(_complex); }

/*!
* \brief   64-bit floating point complex hyperbollic catan function
* \param[ _complex  ] input complex number
* \return the function returns 64-bit complex number hyperbollic catan:
* \f[
*       catanh( z ) = \frac{ cosh(z) }{ snih(z) }
* \f]
*/
complex<__fx64> __ctnhf__( complex<__fx64> _complex ) { return __coshf__(_complex) / __sinhf__(_complex); }

/*!
* \brief   64-bit floating point complex hyperbollic catan function
* \param[ _complex  ] input complex number
* \return the function returns 64-bit complex number hyperbollic catan:
* \f[
*       catanh( z ) = \frac{ cosh(z) }{ snih(z) }
* \f]
*/
complex<__fxx64> __ctnhf__( complex<__fxx64> _complex ) { return __coshf__(_complex) / __sinhf__(_complex); }

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
