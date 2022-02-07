/*!
 * \file
 * \brief   Complex numbers template class and functions
 * \authors A.Tykvinskiy
 * \date    28.12.2021
 * \version 1.0
 *
 * The header declares template complex number class and functions
*/

#ifndef COMPLEX_H
#define COMPLEX_H

/*! \defgroup <COMPLEX_ARITHMETICS> ( Complex numbers functions )
 *  \brief the module describes complex numbers functions
    @{
*/

// standart headers
#ifndef __ALG_PLATFORM
#include "cmath"
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

// template complex number structure:
template< typename T > struct fcomplex;
/*!
 * \brief   32-bit floating point complex number data structure
 * \details 32-bit floating point complex number data structure contains 32-bit
 *          floating point real and imaginary component
 */
template<> struct fcomplex< __fx32  >  { __fx32  re , im; };

/*!
 * \brief   64-bit floating point complex number data structure
 * \details 64-bit floating point complex number data structure contains 64-bit
 *          floating point real and imaginary component
 */
template<> struct fcomplex< __fx64  >  { __fx64  re , im; };

/*!
 * \brief   Extended 64-bit floating point complex number data structure
 * \details Extended 64-bit floating point complex number data structure contains
 *          extended 64-bit floating point real and imaginary component
 */
template<> struct fcomplex< __fxx64 >  { __fxx64 re , im; };

// __cadd__ :
/*!
 * \brief   template complex numbers addition function
 * \returns template complex numbers addition function adds two complex numbers as follows:
 * \f[
 *          z_1 + z_2 = ( real( z_1 ) + real(z_2) ) + j * ( imag( z_1 ) + imag(z_2) )
 * \f]
 */
template< typename T>  fcomplex< T > __cadd__( T re0 , T im0 , T re1 , T im1 ) { return fcomplex<T>{ re0 + re1 , im0 + im1 }; }

// __csub__ :
/*!
 * \brief   template complex numbers substraction function
 * \returns template complex numbers add function sustracts two complex numbers as follows:
 * \f[
 *          z_1 - z_2 = ( real( z_1 ) - real(z_2) ) + j * ( imag( z_1 ) - imag(z_2) )
 * \f]
 */
template< typename T>  fcomplex< T > __csub__( T re0 , T im0 , T re1 , T im1 ) { return fcomplex<T>{ re0 - re1 , im0 - im1 }; }

// __cmul__ :
/*!
 * \brief   template complex numbers mutiplication function
 * \returns template complex numbers mutiplication function multiplies two complex numbers as follows:
 * \f[
 *          z_1 * z_2 = ( real( z_1 ) * real(z_2) - imag( z_1 ) * imag(z_2) )
 *          + j * ( imag( z_1 ) * real(z_2) + real( z_1 ) * imag(z_2) )
 * \f]
 */
template< typename T>  fcomplex< T > __cmul__( T re0 , T im0 , T re1 , T im1 ) { return fcomplex< T >{ re0 * re1 - im0 * im1 , im0 * re1 + re0 * im1 }; }

// __cdiv__ :
/*!
 * \brief   template complex numbers division function
 * \returns template complex numbers division function divides two complex numbers as follows:
 * \f[
 *          z_1 / z_2 = \frac{ ( real( z_1 ) * real(z_2) + imag( z_1 ) * imag(z_2) ) }{ real(z_2)^2 + imag(z_2)^2 }
 *          + j * \frac{ ( imag( z_1 ) * real(z_2) - real( z_1 ) * imag(z_2) ) }{ real(z_2)^2 + imag(z_2)^2 }
 * \f]
 */
template< typename T>  fcomplex< T > __cdiv__( T re0 , T im0 , T re1 , T im1 , T max )
{
    T den = re1 * re1 + im1 * im1;
    return ( den == 0 ) ? fcomplex< T >{ max , max } : fcomplex< T >{ ( re0 * re1 + im0 * im1 ) / den , ( im0 * re1 - re0 * im1 ) / den } ;
}

/*! \brief complex number template class */
template< typename T > class complex;

/*! \brief complex number 32-bit realization */
template<> class complex< __fx32 >
{
    typedef __fx32 __type;
    /*! \brief maximum complex number real or imaginary component value */
    __type m_MAX = __max_fx32;
public:

    /*! \brief complex number real component */
    __type m_re;

    /*! \brief complex number imaginary component */
    __type m_im;

    /*! \brief complex number default constructor */
    complex() { m_re  = m_im  = 0; }

    /*!
     *  \brief template complex number initialization constructor
     *  \param[re] complex number real component
     *  \param[im] imaginary number real component
    */
    complex( __type re , __type im )
    {
        m_re = re;
        m_im = im;
    }

    /*!
     *  \brief complex number initialization constructor
     *  \param[ number ] - real constant that initializes complex number real component
    */
    complex( __type number )
    {
        m_re = number;
        m_im = 0;
    }

    /*! \brief complex number copying constructor
     *  \param[_complex] input template complex number
    */
    complex( const complex &_complex )
    {
        this->m_re = _complex.m_re;
        this->m_im = _complex.m_im;
    }

    /*!
     *  \brief complex number + operator
     *  \param[_complex] input template complex number
     *  \return the operator result is the summ of the current templatre complex number object and the input
     *          complex number template object. The summation is implemented by calling
     *          \f[ cadd<T>( re_1 , im_1 , re_2 , im_2 ) \f] template function
    */
    inline complex operator + ( complex _complex ) const
    {
        fcomplex< __type > c = __cadd__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        return complex( c.re , c.im ) ;
    }

    /*!
     *  \brief complex number += operator
     *  \param[_complex] input template complex number
     *  \return the operator result is the summ of the current templatre complex number object and the input
     *          complex number template object. The summation is implemented by calling
     *          \f[ cadd<T>( re_1 , im_1 , re_2 , im_2 ) \f] template function. The result is stored within
     *          current complex number template object and returned as an output
    */
    inline complex operator += ( complex _complex )
    {
        fcomplex< __type > c = __cadd__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        this->m_re = c.re;
        this->m_im = c.im;
        return *this;
    }

    /*!
     *  \brief complex number + operator
     *  \param[number] input real constant
     *  \return the operator result is the summ of the current templatre complex number object and the input
     *          real constant. The summation is implemented by calling \f[ cadd<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function.
    */
    inline complex operator + ( __type number ) const
    {
        fcomplex< __type > c = __cadd__< __type >( this->m_re , this->m_im , number , 0.0 );
        return complex( c.re , c.im );
    }

    /*!
     *  \brief complex number += operator
     *  \param[number] input real constant
     *  \return the operator result is the summ of the current templatre complex number object and the input
     *          real constant. The summation is implemented by calling \f[ cadd<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within current complex number template object and
     *          returned as an output
    */
    inline complex operator += ( __type number )
    {
        //this->m_re += number;
        fcomplex< __type > c = __cadd__< __type >( this->m_re , this->m_im , number , 0.0 );
        this->m_re = c.re; this->m_im = c.im;
        return *this;
    }

    /*!
     *  \brief complex number - operator
     *  \param[_complex] input template complex number
     *  \return the operator result is the sustraction of the current templatre complex number object and the input
     *          template complex number. The substraction is implemented by calling \f[ csub<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function.
    */
    inline complex operator - ( complex _complex ) const
    {
        fcomplex< __type > c = __csub__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        return complex( c.re , c.im );
    }

    /*!
     *  \brief complex number -= operator
     *  \param[_complex] input template complex number
     *  \return the operator result is the sustraction of the current templatre complex number object and the input
     *          template complex number. The substraction is implemented by calling \f[ csub<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator -= ( complex _complex )
    {

        fcomplex< __type > c = __csub__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        this->m_re = c.re; this->m_im = c.im;
        return *this;
    }

    /*!
     *  \brief complex number - operator
     *  \param[number] input real constant
     *  \return the operator result is the sustraction of the current templatre complex number object and the input
     *          real constant. The substraction is implemented by calling \f[ csub<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function.
    */
    inline complex operator - ( __type number ) const
    {
        fcomplex< __type > c = __csub__< __type >( this->m_re , this->m_im , number , 0.0 );
        return complex( c.re , c.im );
    }

    /*!
     *  \brief complex number - operator
     *  \param[number] input real constant
     *  \return the operator result is the sustraction of the current template complex number object and the input
     *          real constant. The substraction is implemented by calling \f[ csub<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator -= ( __type number )
    {
        fcomplex< __type > c = __csub__< __type >( this->m_re , this->m_im , number , 0.0 );
        return complex( c.re , c.im );
    }

    /*!
     *  \brief complex number * operator
     *  \param[_complex] input template complex nuber object
     *  \return the operator result is the multiplication of the current template complex number object
     *          by another complex number. The multiplication is implemented by calling \f[ cmul<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator * (complex _complex ) const
    {
        fcomplex< __type > c = __cmul__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        return complex( c.re , c.im );
    }

    /*!
     *  \brief complex number *= operator
     *  \param[_complex] input template complex nuber object
     *  \return the operator result is the multiplication of the current template complex number object
     *          by another complex number. The multiplication is implemented by calling \f[ cmul<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator *= (complex _complex )
    {
        fcomplex< __type > c = __cmul__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        this->m_re = c.re;
        this->m_im = c.im;
        return *this;
    }

    /*!
     *  \brief complex number * operator
     *  \param[number] input real constant
     *  \return the operator result is the multiplication of the current template complex number object
     *          by a real constant. The multiplication is implemented by calling \f[ cmul<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator * ( __type number ) const
    {
        fcomplex< __type > c = __cmul__< __type >( this->m_re , this->m_im , number , 0.0 );
        return complex( c.re , c.im );
    }

    /*!
     *  \brief complex number *= operator
     *  \param[number] input real constant
     *  \return the operator result is the multiplication of the current template complex number object
     *          by a real constant. The multiplication is implemented by calling \f[ cmul<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator *= ( __type number )
    {
        fcomplex< __type > c = __cmul__< __type >( this->m_re , this->m_im , number , 0.0 );
        this->m_re = c.re;
        this->m_im = c.im;
        return *this;
    }

    /*!
     *  \brief complex number / operator
     *  \param[_complex] input template complex nuber object
     *  \return the operator result is the division of the current template complex number object
     *          by another complex number. The multiplication is implemented by calling \f[ cdiv<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator / ( complex _complex ) const
    {
        fcomplex< __type > c = __cdiv__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im , m_MAX );
        return complex( c.re , c.im );
    }

    /*!
     *  \brief complex number /= operator
     *  \param[_complex] input template complex nuber object
     *  \return the operator result is the division of the current template complex number object
     *          by another complex number. The multiplication is implemented by calling \f[ cdiv<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator /= ( complex _complex )
    {

        fcomplex< __type > c = __cdiv__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im , m_MAX );
        this->m_re = c.re;
        this->m_im = c.im;
        return *this;
    }

    /*!
     *  \brief complex number / operator
     *  \param[number] input real constant
     *  \return the operator result is the division of the current template complex number object
     *          by a real constant. The multiplication is implemented by calling \f[ cdiv<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator / ( __type number ) const
    {
        fcomplex< __type > c = __cdiv__< __type >( this->m_re , this->m_im , number , 0.0 , m_MAX );
        return complex( c.re , c.im );
    }

    /*!
     *  \brief complex number /= operator
     *  \param[number] input real constant
     *  \return the operator result is the division of the current template complex number object
     *          by a real constant. The multiplication is implemented by calling \f[ cdiv<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator /= ( __type number )
    {
        fcomplex< __type > c = __cdiv__< __type >( this->m_re , this->m_im , number , 0.0 , m_MAX );
        this->m_re = c.re;
        this->m_im = c.im;
        return *this;
    }

    /*!
     *  \brief complex number /= operator
     *  \param[_complex] input template complex nuber object
     *  \return the operator assigns current complex number another complex number
    */
    inline complex operator = ( const complex _complex )
    {
        this->m_re = _complex.m_re;
        this->m_im = _complex.m_im;
        return *this;
    }

    /*!
     *  \brief complex number /= operator
     *  \param[number] input real constant
     *  \return the operator assigns current complex a real constant
    */
    inline complex operator = ( const __type number )
    {
        this->m_re = number;
        this->m_im = 0;
        return *this;
    }

    /*!
     *  \brief equality operator
     *  \param[_complex] input template complex nuber object
     *  \return the operator checks if the two complex numbers are equal
    */
    inline bool operator == ( const complex _complex )
    {
        return ( this->m_re == _complex.m_re ) && ( this->m_im == _complex.m_im );
    }

    /*!
     *  \brief operator >
     *  \param[_complex] input template complex nuber object
     *  \return the operator checks if the current complex number is greater than another
    */
    inline bool operator > (  const complex _complex  )
    {
        __type abs1 = sqrt( this->m_re * this->m_re + this->m_im * this->m_im );
        __type abs2 = sqrt( _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im );
        return (abs1 > abs2);
    }

    /*!
     *  \brief operator <
     *  \param[_complex] input template complex nuber object
     *  \return the operator checks if the current complex number is greater than another
    */
    inline bool operator < (  const complex _complex  )
    {
        __type abs1 = sqrt( this->m_re * this->m_re + this->m_im * this->m_im );
        __type abs2 = sqrt( _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im );
        return (abs1 > abs2);
    }

    /*!
     *  \brief equality operator
     *  \param[re] input template complex nuber object
     *  \param[im]
     *  \return the operator checks if the two complex numbers are equal
    */
    inline void operator () ( __type re , __type im )
    {
        this->m_re = re;
        this->m_im = im;
    }
};

/*! \brief complex number 64-bit realization */
template<> class complex< __fx64 >
{
    typedef __fx64 __type;
    /*! \brief maximum complex number real or imaginary component value */
    __type m_MAX = __max_fx64;
public:

    /*! \brief complex number real component */
    __type m_re;

    /*! \brief complex number imaginary component */
    __type m_im;

    /*! \brief complex number default constructor */
    complex() { m_re  = m_im  = 0; }

    /*!
     *  \brief template complex number initialization constructor
     *  \param[re] complex number real component
     *  \param[im] imaginary number real component
    */
    complex( __type re , __type im )
    {
        m_re = re;
        m_im = im;
    }

    /*!
     *  \brief complex number initialization constructor
     *  \param[ number ] - real constant that initializes complex number real component
    */
    complex( __type number )
    {
        m_re = number;
        m_im = 0;
    }

    /*! \brief complex number copying constructor
     *  \param[_complex] input template complex number
    */
    complex( const complex &_complex )
    {
        this->m_re = _complex.m_re;
        this->m_im = _complex.m_im;
    }

    /*!
     *  \brief complex number + operator
     *  \param[_complex] input template complex number
     *  \return the operator result is the summ of the current templatre complex number object and the input
     *          complex number template object. The summation is implemented by calling
     *          \f[ cadd<T>( re_1 , im_1 , re_2 , im_2 ) \f] template function
    */
    inline complex operator + ( complex _complex ) const
    {
        fcomplex< __type > c = __cadd__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        return complex( c.re , c.im ) ;
    }

    /*!
     *  \brief complex number += operator
     *  \param[_complex] input template complex number
     *  \return the operator result is the summ of the current templatre complex number object and the input
     *          complex number template object. The summation is implemented by calling
     *          \f[ cadd<T>( re_1 , im_1 , re_2 , im_2 ) \f] template function. The result is stored within
     *          current complex number template object and returned as an output
    */
    inline complex operator += ( complex _complex )
    {
        fcomplex< __type > c = __cadd__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        this->m_re = c.re;
        this->m_im = c.im;
        return *this;
    }

    /*!
     *  \brief complex number + operator
     *  \param[number] input real constant
     *  \return the operator result is the summ of the current templatre complex number object and the input
     *          real constant. The summation is implemented by calling \f[ cadd<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function.
    */
    inline complex operator + ( __type number ) const
    {
        fcomplex< __type > c = __cadd__< __type >( this->m_re , this->m_im , number , 0.0 );
        return complex( c.re , c.im );
    }

    /*!
     *  \brief complex number += operator
     *  \param[number] input real constant
     *  \return the operator result is the summ of the current templatre complex number object and the input
     *          real constant. The summation is implemented by calling \f[ cadd<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within current complex number template object and
     *          returned as an output
    */
    inline complex operator += ( __type number )
    {
        //this->m_re += number;
        fcomplex< __type > c = __cadd__< __type >( this->m_re , this->m_im , number , 0.0 );
        this->m_re = c.re; this->m_im = c.im;
        return *this;
    }

    /*!
     *  \brief complex number - operator
     *  \param[_complex] input template complex number
     *  \return the operator result is the sustraction of the current templatre complex number object and the input
     *          template complex number. The substraction is implemented by calling \f[ csub<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function.
    */
    inline complex operator - ( complex _complex ) const
    {
        fcomplex< __type > c = __csub__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        return complex( c.re , c.im );
    }

    /*!
     *  \brief complex number -= operator
     *  \param[_complex] input template complex number
     *  \return the operator result is the sustraction of the current templatre complex number object and the input
     *          template complex number. The substraction is implemented by calling \f[ csub<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator -= ( complex _complex )
    {

        fcomplex< __type > c = __csub__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        this->m_re = c.re; this->m_im = c.im;
        return *this;
    }

    /*!
     *  \brief complex number - operator
     *  \param[number] input real constant
     *  \return the operator result is the sustraction of the current templatre complex number object and the input
     *          real constant. The substraction is implemented by calling \f[ csub<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function.
    */
    inline complex operator - ( __type number ) const
    {
        fcomplex< __type > c = __csub__< __type >( this->m_re , this->m_im , number , 0.0 );
        return complex( c.re , c.im );
    }

    /*!
     *  \brief complex number - operator
     *  \param[number] input real constant
     *  \return the operator result is the sustraction of the current template complex number object and the input
     *          real constant. The substraction is implemented by calling \f[ csub<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator -= ( __type number )
    {
        fcomplex< __type > c = __csub__< __type >( this->m_re , this->m_im , number , 0.0 );
        return complex( c.re , c.im );
    }

    /*!
     *  \brief complex number * operator
     *  \param[_complex] input template complex nuber object
     *  \return the operator result is the multiplication of the current template complex number object
     *          by another complex number. The multiplication is implemented by calling \f[ cmul<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator * (complex _complex ) const
    {
        fcomplex< __type > c = __cmul__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        return complex( c.re , c.im );
    }

    /*!
     *  \brief complex number *= operator
     *  \param[_complex] input template complex nuber object
     *  \return the operator result is the multiplication of the current template complex number object
     *          by another complex number. The multiplication is implemented by calling \f[ cmul<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator *= (complex _complex )
    {
        fcomplex< __type > c = __cmul__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        this->m_re = c.re;
        this->m_im = c.im;
        return *this;
    }

    /*!
     *  \brief complex number * operator
     *  \param[number] input real constant
     *  \return the operator result is the multiplication of the current template complex number object
     *          by a real constant. The multiplication is implemented by calling \f[ cmul<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator * ( __type number ) const
    {
        fcomplex< __type > c = __cmul__< __type >( this->m_re , this->m_im , number , 0.0 );
        return complex( c.re , c.im );
    }

    /*!
     *  \brief complex number *= operator
     *  \param[number] input real constant
     *  \return the operator result is the multiplication of the current template complex number object
     *          by a real constant. The multiplication is implemented by calling \f[ cmul<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator *= ( __type number )
    {
        fcomplex< __type > c = __cmul__< __type >( this->m_re , this->m_im , number , 0.0 );
        this->m_re = c.re;
        this->m_im = c.im;
        return *this;
    }

    /*!
     *  \brief complex number / operator
     *  \param[_complex] input template complex nuber object
     *  \return the operator result is the division of the current template complex number object
     *          by another complex number. The multiplication is implemented by calling \f[ cdiv<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator / ( complex _complex ) const
    {
        fcomplex< __type > c = __cdiv__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im , m_MAX );
        return complex( c.re , c.im );
    }

    /*!
     *  \brief complex number /= operator
     *  \param[_complex] input template complex nuber object
     *  \return the operator result is the division of the current template complex number object
     *          by another complex number. The multiplication is implemented by calling \f[ cdiv<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator /= ( complex _complex )
    {

        fcomplex< __type > c = __cdiv__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im , m_MAX );
        this->m_re = c.re;
        this->m_im = c.im;
        return *this;
    }

    /*!
     *  \brief complex number / operator
     *  \param[number] input real constant
     *  \return the operator result is the division of the current template complex number object
     *          by a real constant. The multiplication is implemented by calling \f[ cdiv<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator / ( __type number ) const
    {
        fcomplex< __type > c = __cdiv__< __type >( this->m_re , this->m_im , number , 0.0 , m_MAX );
        return complex( c.re , c.im );
    }

    /*!
     *  \brief complex number /= operator
     *  \param[number] input real constant
     *  \return the operator result is the division of the current template complex number object
     *          by a real constant. The multiplication is implemented by calling \f[ cdiv<T>( re_1 , im_1 , re_2 , im_2 ) \f]
     *          template function. The result is stored within the current complex number template object and returned as
     *          an output.
    */
    inline complex operator /= ( __type number )
    {
        fcomplex< __type > c = __cdiv__< __type >( this->m_re , this->m_im , number , 0.0 , m_MAX );
        this->m_re = c.re;
        this->m_im = c.im;
        return *this;
    }

    /*!
     *  \brief complex number /= operator
     *  \param[_complex] input template complex nuber object
     *  \return the operator assigns current complex number another complex number
    */
    inline complex operator = ( const complex _complex )
    {
        this->m_re = _complex.m_re;
        this->m_im = _complex.m_im;
        return *this;
    }

    /*!
     *  \brief complex number /= operator
     *  \param[number] input real constant
     *  \return the operator assigns current complex a real constant
    */
    inline complex operator = ( const __type number )
    {
        this->m_re = number;
        this->m_im = 0;
        return *this;
    }

    /*!
     *  \brief equality operator
     *  \param[_complex] input template complex nuber object
     *  \return the operator checks if the two complex numbers are equal
    */
    inline bool operator == ( const complex _complex )
    {
        return ( this->m_re == _complex.m_re ) && ( this->m_im == _complex.m_im );
    }

    /*!
     *  \brief operator >
     *  \param[_complex] input template complex nuber object
     *  \return the operator checks if the current complex number is greater than another
    */
    inline bool operator > (  const complex _complex  )
    {
        __type abs1 = sqrt( this->m_re * this->m_re + this->m_im * this->m_im );
        __type abs2 = sqrt( _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im );
        return (abs1 > abs2);
    }

    /*!
     *  \brief operator <
     *  \param[_complex] input template complex nuber object
     *  \return the operator checks if the current complex number is greater than another
    */
    inline bool operator < (  const complex _complex  )
    {
        __type abs1 = sqrt( this->m_re * this->m_re + this->m_im * this->m_im );
        __type abs2 = sqrt( _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im );
        return (abs1 > abs2);
    }

    /*!
     *  \brief equality operator
     *  \param[re] input template complex nuber object
     *  \param[im]
     *  \return the operator checks if the two complex numbers are equal
    */
    inline void operator () ( __type re , __type im )
    {
        this->m_re = re;
        this->m_im = im;
    }
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

/*! @} */

#undef __TO_DEGREES
#undef __TO_RADIANS

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


#endif // COMPLEX_H
