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

// complex number type:
template< typename T > class complex;

// float x64:
template<> class complex< __fx64 >
{
    typedef __fx64 __type;
    __type m_MAX = __max_fx64;
public:

    __type m_re;
    __type m_im;

    // default constructor:
    complex()
    {
        m_re  = 0;
        m_im  = 0;
    }

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
        return complex( this->m_re + _complex.m_re , this->m_im + _complex.m_im );
    }

    inline complex operator += ( complex _complex )
    {
        this->m_re += _complex.m_re;
        this->m_im += _complex.m_im;
        return *this;
    }

    // add constant:
    inline complex operator + ( __type number ) const
    {
        return complex( this->m_re + number , this->m_im );
    }

    inline complex operator += ( __type number )
    {
        this->m_re += number;
        return *this;
    }

    // substract another complex:
    inline complex operator - ( complex _complex ) const
    {
        return complex( this->m_re - _complex.m_re , this->m_im - _complex.m_im );
    }

    inline complex operator -= ( complex _complex )
    {
        this->m_re -= _complex.m_re;
        this->m_im -= _complex.m_im;
        return *this;
    }

    // substract constnat:
    inline complex operator - ( __type number ) const
    {
        return complex( this->m_re - number , this->m_im );
    }

    inline complex operator -= ( __type number )
    {
        this->m_re -= number;
        return *this;
    }

    // multiply by another complex:
    inline complex operator * (complex _complex ) const
    {
        return complex( this->m_re * _complex.m_re - this->m_im * _complex.m_im , this->m_im * _complex.m_re + this->m_re * _complex.m_im);
    }

    inline complex operator *= (complex _complex )
    {
        __type re = this->m_re * _complex.m_re - this->m_im * _complex.m_im;
        __type im = this->m_im * _complex.m_re + this->m_re * _complex.m_im;
        this->m_re = re;
        this->m_im = im;
        return *this;
    }

    // multiply by a constant:
    inline complex operator * ( __type number ) const
    {
        return complex( this->m_re * number ,this->m_im * number);
    }

    inline complex operator *= ( __type number )
    {
        this->m_re *= number;
        this->m_im *= number;
        return *this;
    }

    // divide by another complex:
    inline complex operator / ( complex _complex ) const
    {
        __type denum = _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im;

        if( denum == 0 )
        {
            return complex( m_MAX , m_MAX );
        }
        else
        {
            return complex( ( this->m_re * _complex.m_re + this->m_im * _complex.m_im ) / denum , ( this->m_im * _complex.m_re - this->m_re * _complex.m_im ) / denum);
        }
    }

    inline complex operator /= ( complex _complex )
    {
        __type denum = _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im;
        if( denum == 0 )
        {
            this->m_re = m_MAX;
            this->m_im = m_MAX;
            return *this;
        }
        else
        {
            __type re = ( this->m_re * _complex.m_re + this->m_im * _complex.m_im ) / denum;
            __type im = ( this->m_im * _complex.m_re - this->m_re * _complex.m_im ) / denum;
            this->m_re = re;
            this->m_im = im;
            return *this;
        }
    }

    // divide by a constant:
    inline complex operator / ( __type number ) const
    {
        return  ( number == 0 ) ? complex( m_MAX , m_MAX ) : complex( this->m_re / number , this->m_im / number ) ;
    }

    inline complex operator /= ( __type number )
    {
        if( number == 0 )
        {
            this->m_re = m_MAX;
            this->m_im = m_MAX;
        }
        else
        {
            this->m_re /= number;
            this->m_im /= number;
        }

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

// float x32:
template<> class complex< __fx32 >
{
    typedef __fx32 __type;
    __type m_MAX = __max_fx32;
public:

    __type m_re;
    __type m_im;

    // default constructor:
    complex()
    {
        m_re  = 0;
        m_im  = 0;
    }

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
        return complex( this->m_re + _complex.m_re , this->m_im + _complex.m_im );
    }

    inline complex operator += ( complex _complex )
    {
        this->m_re += _complex.m_re;
        this->m_im += _complex.m_im;
        return *this;
    }

    // add constant:
    inline complex operator + ( __type number ) const
    {
        return complex( this->m_re + number , this->m_im );
    }

    inline complex operator += ( __type number )
    {
        this->m_re += number;
        return *this;
    }

    // substract another complex:
    inline complex operator - ( complex _complex ) const
    {
        return complex( this->m_re - _complex.m_re , this->m_im - _complex.m_im );
    }

    inline complex operator -= ( complex _complex )
    {
        this->m_re -= _complex.m_re;
        this->m_im -= _complex.m_im;
        return *this;
    }

    // substract constnat:
    inline complex operator - ( __type number ) const
    {
        return complex( this->m_re - number , this->m_im );
    }

    inline complex operator -= ( __type number )
    {
        this->m_re -= number;
        return *this;
    }

    // multiply by another complex:
    inline complex operator * (complex _complex ) const
    {
        return complex( this->m_re * _complex.m_re - this->m_im * _complex.m_im , this->m_im * _complex.m_re + this->m_re * _complex.m_im);
    }

    inline complex operator *= (complex _complex )
    {
        __type re = this->m_re * _complex.m_re - this->m_im * _complex.m_im;
        __type im = this->m_im * _complex.m_re + this->m_re * _complex.m_im;
        this->m_re = re;
        this->m_im = im;
        return *this;
    }

    // multiply by a constant:
    inline complex operator * ( __type number ) const
    {
        return complex( this->m_re * number ,this->m_im * number);
    }

    inline complex operator *= ( __type number )
    {
        this->m_re *= number;
        this->m_im *= number;
        return *this;
    }

    // divide by another complex:
    inline complex operator / ( complex _complex ) const
    {
        __type denum = _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im;

        if( denum == 0 )
        {
            return complex( m_MAX , m_MAX );
        }
        else
        {
            return complex( ( this->m_re * _complex.m_re + this->m_im * _complex.m_im ) / denum , ( this->m_im * _complex.m_re - this->m_re * _complex.m_im ) / denum);
        }
    }

    inline complex operator /= ( complex _complex )
    {
        __type denum = _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im;
        if( denum == 0 )
        {
            this->m_re = m_MAX;
            this->m_im = m_MAX;
            return *this;
        }
        else
        {
            __type re = ( this->m_re * _complex.m_re + this->m_im * _complex.m_im ) / denum;
            __type im = ( this->m_im * _complex.m_re - this->m_re * _complex.m_im ) / denum;
            this->m_re = re;
            this->m_im = im;
            return *this;
        }
    }

    // divide by a constant:
    inline complex operator / ( __type number ) const
    {
        return  ( number == 0 ) ? complex( m_MAX , m_MAX ) : complex( this->m_re / number , this->m_im / number ) ;
    }

    inline complex operator /= ( __type number )
    {
        if( number == 0 )
        {
            this->m_re = m_MAX;
            this->m_im = m_MAX;
        }
        else
        {
            this->m_re /= number;
            this->m_im /= number;
        }

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

// int x64:
template<> class complex< __ix64 >
{
    typedef __ix64 __type;
    __type m_MAX = __max_ix64;
public:

    __type m_re;
    __type m_im;

    // default constructor:
    complex()
    {
        m_re  = 0;
        m_im  = 0;
    }

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
        return complex( this->m_re + _complex.m_re , this->m_im + _complex.m_im );
    }

    inline complex operator += ( complex _complex )
    {
        this->m_re += _complex.m_re;
        this->m_im += _complex.m_im;
        return *this;
    }

    // add constant:
    inline complex operator + ( __type number ) const
    {
        return complex( this->m_re + number , this->m_im );
    }

    inline complex operator += ( __type number )
    {
        this->m_re += number;
        return *this;
    }

    // substract another complex:
    inline complex operator - ( complex _complex ) const
    {
        return complex( this->m_re - _complex.m_re , this->m_im - _complex.m_im );
    }

    inline complex operator -= ( complex _complex )
    {
        this->m_re -= _complex.m_re;
        this->m_im -= _complex.m_im;
        return *this;
    }

    // substract constnat:
    inline complex operator - ( __type number ) const
    {
        return complex( this->m_re - number , this->m_im );
    }

    inline complex operator -= ( __type number )
    {
        this->m_re -= number;
        return *this;
    }

    // multiply by another complex:
    inline complex operator * (complex _complex ) const
    {
        return complex( this->m_re * _complex.m_re - this->m_im * _complex.m_im , this->m_im * _complex.m_re + this->m_re * _complex.m_im);
    }

    inline complex operator *= (complex _complex )
    {
        __type re = this->m_re * _complex.m_re - this->m_im * _complex.m_im;
        __type im = this->m_im * _complex.m_re + this->m_re * _complex.m_im;
        this->m_re = re;
        this->m_im = im;
        return *this;
    }

    // multiply by a constant:
    inline complex operator * ( __type number ) const
    {
        return complex( this->m_re * number ,this->m_im * number);
    }

    inline complex operator *= ( __type number )
    {
        this->m_re *= number;
        this->m_im *= number;
        return *this;
    }

    // divide by another complex:
    inline complex operator / ( complex _complex ) const
    {
        __type denum = _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im;

        if( denum == 0 )
        {
            return complex( m_MAX , m_MAX );
        }
        else
        {
            return complex( ( this->m_re * _complex.m_re + this->m_im * _complex.m_im ) / denum , ( this->m_im * _complex.m_re - this->m_re * _complex.m_im ) / denum);
        }
    }

    inline complex operator /= ( complex _complex )
    {
        __type denum = _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im;
        if( denum == 0 )
        {
            this->m_re = m_MAX;
            this->m_im = m_MAX;
            return *this;
        }
        else
        {
            __type re = ( this->m_re * _complex.m_re + this->m_im * _complex.m_im ) / denum;
            __type im = ( this->m_im * _complex.m_re - this->m_re * _complex.m_im ) / denum;
            this->m_re = re;
            this->m_im = im;
            return *this;
        }
    }

    // divide by a constant:
    inline complex operator / ( __type number ) const
    {
        return  ( number == 0 ) ? complex( m_MAX , m_MAX ) : complex( this->m_re / number , this->m_im / number ) ;
    }

    inline complex operator /= ( __type number )
    {
        if( number == 0 )
        {
            this->m_re = m_MAX;
            this->m_im = m_MAX;
        }
        else
        {
            this->m_re /= number;
            this->m_im /= number;
        }

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

// int x32:
template<> class complex< __ix32 >
{
    typedef __ix32 __type;
    __type m_MAX = __max_ix32;
public:

    __type m_re;
    __type m_im;

    // default constructor:
    complex()
    {
        m_re  = 0;
        m_im  = 0;
    }

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
        return complex( this->m_re + _complex.m_re , this->m_im + _complex.m_im );
    }

    inline complex operator += ( complex _complex )
    {
        this->m_re += _complex.m_re;
        this->m_im += _complex.m_im;
        return *this;
    }

    // add constant:
    inline complex operator + ( __type number ) const
    {
        return complex( this->m_re + number , this->m_im );
    }

    inline complex operator += ( __type number )
    {
        this->m_re += number;
        return *this;
    }

    // substract another complex:
    inline complex operator - ( complex _complex ) const
    {
        return complex( this->m_re - _complex.m_re , this->m_im - _complex.m_im );
    }

    inline complex operator -= ( complex _complex )
    {
        this->m_re -= _complex.m_re;
        this->m_im -= _complex.m_im;
        return *this;
    }

    // substract constnat:
    inline complex operator - ( __type number ) const
    {
        return complex( this->m_re - number , this->m_im );
    }

    inline complex operator -= ( __type number )
    {
        this->m_re -= number;
        return *this;
    }

    // multiply by another complex:
    inline complex operator * (complex _complex ) const
    {
        return complex( this->m_re * _complex.m_re - this->m_im * _complex.m_im , this->m_im * _complex.m_re + this->m_re * _complex.m_im);
    }

    inline complex operator *= (complex _complex )
    {
        __type re = this->m_re * _complex.m_re - this->m_im * _complex.m_im;
        __type im = this->m_im * _complex.m_re + this->m_re * _complex.m_im;
        this->m_re = re;
        this->m_im = im;
        return *this;
    }

    // multiply by a constant:
    inline complex operator * ( __type number ) const
    {
        return complex( this->m_re * number ,this->m_im * number);
    }

    inline complex operator *= ( __type number )
    {
        this->m_re *= number;
        this->m_im *= number;
        return *this;
    }

    // divide by another complex:
    inline complex operator / ( complex _complex ) const
    {
        __type denum = _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im;

        if( denum == 0 )
        {
            return complex( m_MAX , m_MAX );
        }
        else
        {
            return complex( ( this->m_re * _complex.m_re + this->m_im * _complex.m_im ) / denum , ( this->m_im * _complex.m_re - this->m_re * _complex.m_im ) / denum);
        }
    }

    inline complex operator /= ( complex _complex )
    {
        __type denum = _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im;
        if( denum == 0 )
        {
            this->m_re = m_MAX;
            this->m_im = m_MAX;
            return *this;
        }
        else
        {
            __type re = ( this->m_re * _complex.m_re + this->m_im * _complex.m_im ) / denum;
            __type im = ( this->m_im * _complex.m_re - this->m_re * _complex.m_im ) / denum;
            this->m_re = re;
            this->m_im = im;
            return *this;
        }
    }

    // divide by a constant:
    inline complex operator / ( __type number ) const
    {
        return  ( number == 0 ) ? complex( m_MAX , m_MAX ) : complex( this->m_re / number , this->m_im / number ) ;
    }

    inline complex operator /= ( __type number )
    {
        if( number == 0 )
        {
            this->m_re = m_MAX;
            this->m_im = m_MAX;
        }
        else
        {
            this->m_re /= number;
            this->m_im /= number;
        }

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

// int x16:
template<> class complex< __ix16 >
{
    typedef __ix16 __type;
    __type m_MAX = __max_ix16;
public:

    __type m_re;
    __type m_im;

    // default constructor:
    complex()
    {
        m_re  = 0;
        m_im  = 0;
    }

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
        return complex( this->m_re + _complex.m_re , this->m_im + _complex.m_im );
    }

    inline complex operator += ( complex _complex )
    {
        this->m_re += _complex.m_re;
        this->m_im += _complex.m_im;
        return *this;
    }

    // add constant:
    inline complex operator + ( __type number ) const
    {
        return complex( this->m_re + number , this->m_im );
    }

    inline complex operator += ( __type number )
    {
        this->m_re += number;
        return *this;
    }

    // substract another complex:
    inline complex operator - ( complex _complex ) const
    {
        return complex( this->m_re - _complex.m_re , this->m_im - _complex.m_im );
    }

    inline complex operator -= ( complex _complex )
    {
        this->m_re -= _complex.m_re;
        this->m_im -= _complex.m_im;
        return *this;
    }

    // substract constnat:
    inline complex operator - ( __type number ) const
    {
        return complex( this->m_re - number , this->m_im );
    }

    inline complex operator -= ( __type number )
    {
        this->m_re -= number;
        return *this;
    }

    // multiply by another complex:
    inline complex operator * (complex _complex ) const
    {
        return complex( this->m_re * _complex.m_re - this->m_im * _complex.m_im , this->m_im * _complex.m_re + this->m_re * _complex.m_im);
    }

    inline complex operator *= (complex _complex )
    {
        __type re = this->m_re * _complex.m_re - this->m_im * _complex.m_im;
        __type im = this->m_im * _complex.m_re + this->m_re * _complex.m_im;
        this->m_re = re;
        this->m_im = im;
        return *this;
    }

    // multiply by a constant:
    inline complex operator * ( __type number ) const
    {
        return complex( this->m_re * number ,this->m_im * number);
    }

    inline complex operator *= ( __type number )
    {
        this->m_re *= number;
        this->m_im *= number;
        return *this;
    }

    // divide by another complex:
    inline complex operator / ( complex _complex ) const
    {
        __type denum = _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im;

        if( denum == 0 )
        {
            return complex( m_MAX , m_MAX );
        }
        else
        {
            return complex( ( this->m_re * _complex.m_re + this->m_im * _complex.m_im ) / denum , ( this->m_im * _complex.m_re - this->m_re * _complex.m_im ) / denum);
        }
    }

    inline complex operator /= ( complex _complex )
    {
        __type denum = _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im;
        if( denum == 0 )
        {
            this->m_re = m_MAX;
            this->m_im = m_MAX;
            return *this;
        }
        else
        {
            __type re = ( this->m_re * _complex.m_re + this->m_im * _complex.m_im ) / denum;
            __type im = ( this->m_im * _complex.m_re - this->m_re * _complex.m_im ) / denum;
            this->m_re = re;
            this->m_im = im;
            return *this;
        }
    }

    // divide by a constant:
    inline complex operator / ( __type number ) const
    {
        return  ( number == 0 ) ? complex( m_MAX , m_MAX ) : complex( this->m_re / number , this->m_im / number ) ;
    }

    inline complex operator /= ( __type number )
    {
        if( number == 0 )
        {
            this->m_re = m_MAX;
            this->m_im = m_MAX;
        }
        else
        {
            this->m_re /= number;
            this->m_im /= number;
        }

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
