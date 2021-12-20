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

    // add constant:
    inline complex operator + ( __type number ) const
    {
        return complex( this->m_re + number , this->m_im );
    }

    // substract another complex:
    inline complex operator - ( complex _complex ) const
    {
        return complex( this->m_re - _complex.m_re , this->m_im - _complex.m_im );
    }

    // substract constnat:
    inline complex operator - ( __type number ) const
    {
        return complex( this->m_re - number , this->m_im );
    }

    // multiply by another complex:
    inline complex operator * (complex _complex ) const
    {
        return complex( this->m_re * _complex.m_re - this->m_im * _complex.m_im , this->m_im * _complex.m_re + this->m_re * _complex.m_im);
    }

    // multiply by a constant:
    inline complex operator * ( __type number ) const
    {
        return complex( this->m_re * number ,this->m_im * number);
    }

    // divide by another complex:
    inline complex operator / ( complex _complex ) const
    {
        __type denum = _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im;

        if( denum == 0 )
        {
            return complex( __max_fx64 , __max_fx64 );
        }
        else
        {
            return complex( ( this->m_re * _complex.m_re + this->m_im * _complex.m_im ) / denum , ( this->m_im * _complex.m_re - this->m_re * _complex.m_im ) / denum);
        }
    }

    // divide by a constant:
    inline complex operator / ( __type number ) const // by value
    {
        return  ( number == 0 ) ? complex( __max_fx64 , __max_fx64 ) : complex( this->m_re / number , this->m_im / number ) ;
    }

    // assign another complex:
    inline void operator = ( const complex _complex ) // by reference
    {
        this->m_re = _complex.m_re;
        this->m_im = _complex.m_im;
    }

    // assign a constant:
    inline void operator = ( const __type number ) // by value
    {
        this->m_re = number;
        this->m_im = 0;
    }
};

// float x32:
template<> class complex< __fx32 >
{
    typedef __fx32 __type;
public:

    __type m_re;
    __type m_im;

    // default constructor:
    complex()
    {
        m_re  = 0;
        m_im  = 0;
    }

    // intialization constructors:

    // float x32:
    complex( __type re , __type im )
    {
        m_re = re;
        m_im = im;
    }

    // float x32:
    complex( __type number )
    {
        m_re = number;
    }

    // copying constructor:

    // float x32:
    complex( const complex<__type> &_complex )
    {
        this->m_re = _complex.m_re;
        this->m_im = _complex.m_im;
    }

    // float x64 ( CAUTION !!! ROUNDING ERROR OCCURS !!! ) :
    complex( const complex<__fx64> &_complex )
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

    // add constant:
    inline complex operator + ( __type number ) const
    {
        return complex( this->m_re + number , this->m_im );
    }

    // substract another complex:
    inline complex operator - ( complex _complex ) const
    {
        return complex( this->m_re - _complex.m_re , this->m_im - _complex.m_im );
    }

    // substract constnat:
    inline complex operator - ( __type number ) const
    {
        return complex( this->m_re - number , this->m_im );
    }

    // multiply by another complex:
    inline complex operator * (complex _complex ) const
    {
        return complex( this->m_re * _complex.m_re - this->m_im * _complex.m_im , this->m_im * _complex.m_re + this->m_re * _complex.m_im);
    }

    // multiply by a constant:
    inline complex operator * ( __type number ) const
    {
        return complex( this->m_re * number ,this->m_im * number);
    }

    // divide by another complex:
    inline complex operator / ( complex _complex ) const
    {
        __type denum = _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im;

        if( denum == 0 )
        {
            return complex( __max_fx32 , __max_fx32 );
        }
        else
        {
            return complex( ( this->m_re * _complex.m_re + this->m_im * _complex.m_im ) / denum , ( this->m_im * _complex.m_re - this->m_re * _complex.m_im ) / denum);
        }
    }

    // divide by a constant:
    inline complex operator / ( __type number ) const // by value
    {
        return  ( number == 0 ) ? complex( __max_fx32 , __max_fx32 ) : complex( this->m_re / number , this->m_im / number ) ;
    }

    // assign another complex:
    inline void operator = ( const complex _complex ) // by reference
    {
        this->m_re = _complex.m_re;
        this->m_im = _complex.m_im;
    }

    // assign a constant:
    inline void operator = ( const __type number ) // by value
    {
        this->m_re = number;
        this->m_im = 0;
    }
};

// int x64:
template<> class complex< __ix64 >
{
    typedef __ix64 __type;
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

    // add constant:
    inline complex operator + ( __type number ) const
    {
        return complex( this->m_re + number , this->m_im );
    }

    // substract another complex:
    inline complex operator - ( complex _complex ) const
    {
        return complex( this->m_re - _complex.m_re , this->m_im - _complex.m_im );
    }

    // substract constnat:
    inline complex operator - ( __type number ) const
    {
        return complex( this->m_re - number , this->m_im );
    }

    // multiply by another complex:
    inline complex operator * (complex _complex ) const
    {
        return complex( this->m_re * _complex.m_re - this->m_im * _complex.m_im , this->m_im * _complex.m_re + this->m_re * _complex.m_im);
    }

    // multiply by a constant:
    inline complex operator * ( __type number ) const
    {
        return complex( this->m_re * number ,this->m_im * number);
    }

    // divide by another complex:
    inline complex operator / ( complex _complex ) const
    {
        __type denum = _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im;

        if( denum == 0 )
        {
            return complex( __max_ix64 , __max_ix64 );
        }
        else
        {
            return complex( ( this->m_re * _complex.m_re + this->m_im * _complex.m_im ) / denum , ( this->m_im * _complex.m_re - this->m_re * _complex.m_im ) / denum);
        }
    }

    // divide by a constant:
    inline complex operator / ( __type number ) const // by value
    {
        return  ( number == 0 ) ? complex( __max_ix64 , __max_ix64 ) : complex( this->m_re / number , this->m_im / number ) ;
    }

    // assign another complex:
    inline void operator = ( const complex _complex ) // by reference
    {
        this->m_re = _complex.m_re;
        this->m_im = _complex.m_im;
    }

    // assign a constant:
    inline void operator = ( const __type number ) // by value
    {
        this->m_re = number;
        this->m_im = 0;
    }
};

// int x32:
template<> class complex< __ix32 >
{
    typedef __ix32 __type;
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

    // int x32:
    complex( const complex &_complex )
    {
        this->m_re = _complex.m_re;
        this->m_im = _complex.m_im;
    }

    // int x64 ( CAUTION !!! ROUNDING ERROR OCCURS !!! ):
    complex( const complex<__ix64> &_complex )
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

    // add constant:
    inline complex operator + ( __type number ) const
    {
        return complex( this->m_re + number , this->m_im );
    }

    // substract another complex:
    inline complex operator - ( complex _complex ) const
    {
        return complex( this->m_re - _complex.m_re , this->m_im - _complex.m_im );
    }

    // substract constnat:
    inline complex operator - ( __type number ) const
    {
        return complex( this->m_re - number , this->m_im );
    }

    // multiply by another complex:
    inline complex operator * (complex _complex ) const
    {
        return complex( this->m_re * _complex.m_re - this->m_im * _complex.m_im , this->m_im * _complex.m_re + this->m_re * _complex.m_im);
    }

    // multiply by a constant:
    inline complex operator * ( __type number ) const
    {
        return complex( this->m_re * number ,this->m_im * number);
    }

    // divide by another complex:
    inline complex operator / ( complex _complex ) const
    {
        __type denum = _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im;

        if( denum == 0 )
        {
            return complex( __max_ix32 , __max_ix32 );
        }
        else
        {
            return complex( ( this->m_re * _complex.m_re + this->m_im * _complex.m_im ) / denum , ( this->m_im * _complex.m_re - this->m_re * _complex.m_im ) / denum);
        }
    }

    // divide by a constant:
    inline complex operator / ( __type number ) const // by value
    {
        return  ( number == 0 ) ? complex( __max_ix32 , __max_ix32 ) : complex( this->m_re / number , this->m_im / number ) ;
    }

    // assign another complex:
    inline void operator = ( const complex _complex ) // by reference
    {
        this->m_re = _complex.m_re;
        this->m_im = _complex.m_im;
    }

    // assign a constant:
    inline void operator = ( const __type number ) // by value
    {
        this->m_re = number;
        this->m_im = 0;
    }
};

// int x16:
template<> class complex< __ix16 >
{
    typedef __ix16 __type;
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

    // int x16:
    complex( const complex &_complex )
    {
        this->m_re = _complex.m_re;
        this->m_im = _complex.m_im;
    }

    // int x32 ( CAUTION !!! ROUNDING ERROR OCCURS !!! ):
    complex( const complex<__ix32> &_complex )
    {
        this->m_re = _complex.m_re;
        this->m_im = _complex.m_im;
    }

    // int x64 ( CAUTION !!! ROUNDING ERROR OCCURS !!! ):
    complex( const complex<__ix64> &_complex )
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

    // add constant:
    inline complex operator + ( __type number ) const
    {
        return complex( this->m_re + number , this->m_im );
    }

    // substract another complex:
    inline complex operator - ( complex _complex ) const
    {
        return complex( this->m_re - _complex.m_re , this->m_im - _complex.m_im );
    }

    // substract constnat:
    inline complex operator - ( __type number ) const
    {
        return complex( this->m_re - number , this->m_im );
    }

    // multiply by another complex:
    inline complex operator * (complex _complex ) const
    {
        return complex( this->m_re * _complex.m_re - this->m_im * _complex.m_im , this->m_im * _complex.m_re + this->m_re * _complex.m_im);
    }

    // multiply by a constant:
    inline complex operator * ( __type number ) const
    {
        return complex( this->m_re * number ,this->m_im * number);
    }

    // divide by another complex:
    inline complex operator / ( complex _complex ) const
    {
        __type denum = _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im;

        if( denum == 0 )
        {
            return complex( __max_ix16 , __max_ix16 );
        }
        else
        {
            return complex( ( this->m_re * _complex.m_re + this->m_im * _complex.m_im ) / denum , ( this->m_im * _complex.m_re - this->m_re * _complex.m_im ) / denum);
        }
    }

    // divide by a constant:
    inline complex operator / ( __type number ) const // by value
    {
        return  ( number == 0 ) ? complex( __max_ix16 , __max_ix16 ) : complex( this->m_re / number , this->m_im / number ) ;
    }

    // assign another complex:
    inline void operator = ( const complex _complex ) // by reference
    {
        this->m_re = _complex.m_re;
        this->m_im = _complex.m_im;
    }

    // assign a constant:
    inline void operator = ( const __type number ) // by value
    {
        this->m_re = number;
        this->m_im = 0;
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


#endif // COMPLEX_H
