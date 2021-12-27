//--------------------------------------------------------------------------------
//A.M.Tykvinsky, 21.12.2021
//--------------------------------------------------------------------------------
// TEMPLATE COMPLEX NUMBER CLASS
//--------------------------------------------------------------------------------

#ifndef COMPLEX_H
#define COMPLEX_H

// customized C data types:
#ifndef __ix16
#define __ix16 short
#endif

#ifndef __ix32
#define __ix32 int
#endif

#ifndef __ix64
#define __ix64 long long
#endif

#ifndef __uix16
#define __uix16 unsigned short
#endif

#ifndef __uix32
#define __uix32 unsigned int
#endif

#ifndef __uix64
#define __uix64 unsigned long long
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

// customized upper limits:
#ifndef __max_fx32
#define __max_fx32 3.402823466e+38
#endif

#ifndef __max_fx64
#define __max_fx64 1.7976931348623158e+308
#endif

#ifndef __max_ix16
#define __max_ix16 32767
#endif

#ifndef __max_ix32
#define __max_ix32 2147483647
#endif

#ifndef __max_ix64
#define __max_ix64 9223372036854775807
#endif

#ifndef __max_uix16
#define __max_uix16 65535
#endif

#ifndef __max_uix32
#define __max_uix32 4294967295
#endif

#ifndef __max_uix64
#define __max_uix64 18446744073709551615
#endif

// customized lower limits:
#ifndef __min_fx32
#define __min_fx32 1.175494351e-38
#endif

#ifndef __min_fx64
#define __min_fx64 1.7976931348623158e+308
#endif

#ifndef __min_ix16
#define __min_ix16 -32767
#endif

#ifndef __min_ix32
#define __min_ix32 -2147483647
#endif

#ifndef __min_ix64
#define __min_ix64 -9223372036854775807
#endif

#ifndef __min_uix16
#define __min_uix16 0
#endif

#ifndef __min_uix32
#define __min_uix32 0
#endif

#ifndef __min_uix64
#define __min_uix64 0
#endif

// template complex number structure:
template< typename T > struct fcomplex;
template<> struct fcomplex< __fx32  >  { __fx32  re , im; };
template<> struct fcomplex< __fx64  >  { __fx64  re , im; };
template<> struct fcomplex< __fxx64 >  { __fxx64 re , im; };

// complex numbers main functions:

// __cadd__ :
template< typename T>  fcomplex< T > __cadd__( T re0 , T im0 , T re1 , T im1 ) { return fcomplex<T>{ re0 + re1 , im0 + im1 }; }

// __csub__ :
template< typename T>  fcomplex< T > __csub__( T re0 , T im0 , T re1 , T im1 ) { return fcomplex<T>{ re0 - re1 , im0 - im1 }; }

// __cmul__ :
template< typename T>  fcomplex< T > __cmul__( T re0 , T im0 , T re1 , T im1 ) { return fcomplex< T >{ re0 * re1 - im0 * im1 , im0 * re1 + re0 * im1 }; }

// __cdiv__ :
template< typename T>  fcomplex< T > __cdiv__( T re0 , T im0 , T re1 , T im1 , T max )
{
    __fx32 den = re1 * re1 + im1 * im1;
    return ( den == 0 ) ? fcomplex< T >{ max , max } : fcomplex< T >{ ( re0 * re1 + im0 * im1 ) / den , ( im0 * re1 - re0 * im1 ) / den } ;
}

/* template complex number class */

// complex number type:
template< typename T > class complex
{
    typedef T __type;
    __type m_MAX = __max_fx32;
public:

    __type m_re , m_im;

    // default constructor:
    complex() { m_re  = m_im  = 0; }

    // intialization constructor:
    complex( __type re , __type im )
    {
        m_re = re;
        m_im = im;
    }

    complex( __type number )
    {
        m_re = number;
    }

    // copying constructor:
    complex( const complex &_complex )
    {
        this->m_re = _complex.m_re;
        this->m_im = _complex.m_im;
    }

    // operators:

    // add another complex:
    inline complex operator + ( complex _complex ) const
    {
        fcomplex< __type > c = __cadd__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        return complex( c.re , c.im ) ;
    }

    inline complex operator += ( complex _complex )
    {
        fcomplex< __type > c = __cadd__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        this->m_re = c.re;
        this->m_im = c.im;
        return *this;
    }

    // add constant:
    inline complex operator + ( __type number ) const
    {
        fcomplex< __type > c = __cadd__< __type >( this->m_re , this->m_im , number , 0.0 );
        return complex( c.re , c.im );
    }

    inline complex operator += ( __type number )
    {
        //this->m_re += number;
        fcomplex< __type > c = __cadd__< __type >( this->m_re , this->m_im , number , 0.0 );
        this->m_re = c.re; this->m_im = c.im;
        return *this;
    }

    // substract another complex:
    inline complex operator - ( complex _complex ) const
    {
        fcomplex< __type > c = __csub__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        return complex( c.re , c.im );
    }

    inline complex operator -= ( complex _complex )
    {

        fcomplex< __type > c = __csub__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        this->m_re = c.re; this->m_im = c.im;
        return *this;
    }

    // substract constnat:
    inline complex operator - ( __type number ) const
    {
        fcomplex< __type > c = __csub__< __type >( this->m_re , this->m_im , number , 0.0 );
        return complex( c.re , c.im );
    }

    inline complex operator -= ( __type number )
    {
        fcomplex< __type > c = __csub__< __type >( this->m_re , this->m_im , number , 0.0 );
        return complex( c.re , c.im );
    }

    // multiply by another complex:
    inline complex operator * (complex _complex ) const
    {
        fcomplex< __type > c = __cmul__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        return complex( c.re , c.im );
    }

    inline complex operator *= (complex _complex )
    {
        fcomplex< __type > c = __cmul__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im );
        this->m_re = c.re;
        this->m_im = c.im;
        return *this;
    }

    // multiply by a constant:
    inline complex operator * ( __type number ) const
    {
        fcomplex< __type > c = __cmul__< __type >( this->m_re , this->m_im , number , 0.0 );
        return complex( c.re , c.im );
    }

    inline complex operator *= ( __type number )
    {
        fcomplex< __type > c = __cmul__< __type >( this->m_re , this->m_im , number , 0.0 );
        this->m_re = c.re;
        this->m_im = c.im;
        return *this;
    }

    // divide by another complex:
    inline complex operator / ( complex _complex ) const
    {
        fcomplex< __type > c = __cdiv__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im , m_MAX );
        return complex( c.re , c.im );
    }

    inline complex operator /= ( complex _complex )
    {

        fcomplex< __type > c = __cdiv__< __type >( this->m_re , this->m_im , _complex.m_re , _complex.m_im , m_MAX );
        this->m_re = c.re;
        this->m_im = c.im;
        return *this;
    }

    // divide by a constant:
    inline complex operator / ( __type number ) const
    {
        fcomplex< __type > c = __cdiv__< __type >( this->m_re , this->m_im , number , 0.0 , m_MAX );
        return complex( c.re , c.im );
    }

    inline complex operator /= ( __type number )
    {
        fcomplex< __type > c = __cdiv__< __type >( this->m_re , this->m_im , number , 0.0 , m_MAX );
        this->m_re = c.re;
        this->m_im = c.im;
        return *this;
    }

    // assign another complex:
    inline complex operator = ( const complex _complex )
    {
        this->m_re = _complex.m_re;
        this->m_im = _complex.m_im;
        return *this;
    }

    // assign a constant:
    inline complex operator = ( const __type number )
    {
        this->m_re = number;
        this->m_im = 0;
        return *this;
    }

    // comparison operator:
    inline bool operator == ( const complex _complex )
    {
        return ( this->m_re == _complex.m_re ) && ( this->m_im == _complex.m_im );
    }
};

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
