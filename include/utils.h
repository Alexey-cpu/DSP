#ifndef UTILS_H
#define UTILS_H

#ifndef __ALG_PLATFORM

// include malloc file if it presents
// ( basically, needed by Windows )
#ifdef _MALLOC_H
#include "malloc.h"
#endif

#include "QElapsedTimer"
#include "iostream"
#include "cmath"
#include "vector"
#include "list"
#include "map"
#endif

using namespace std;

// data types:
#ifndef __fx32
#define __fx32 float
#endif

#ifndef __fx64
#define __fx64 double
#endif

#ifndef __fxx64
#define __fxx64 long double
#endif

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

// utility macros
#define __STRINGIFY__(INPUT) #INPUT

// utilities:

// 2D vector
template< typename __type > struct vector2
{
    __type x;
    __type y;
};

// 3D vector
template< typename __type > struct vector3
{
    __type x;
    __type y;
    __type z;
};

// x2 tuple
template< typename a, typename b>
struct tuple_x2
{
    a item0;
    b item1;

    tuple_x2(){}
    tuple_x2( a item0, b item1 ) :
    item0(item0), item1(item1){}
};

// x3 tuple
template<typename a, typename b, typename c>
struct tuple_x3
{
    a item0;
    b item1;
    c item2;

    tuple_x3(){}
    tuple_x3( a item0, b item1, c item2 ) :
    item0(item0), item1(item1), item2(item2){}
};

// x4 tuple
template<typename a, typename b, typename c, typename d>
struct tuple_x4
{
    a item0;
    b item1;
    c item2;
    d item3;

    tuple_x4(){}
    tuple_x4( a item0, b item1, c item2, d item3 ) :
    item0(item0), item1(item1), item2(item2), item3(item3){}
};

// x5 tuple
template< typename a, typename b, typename c, typename d, typename e>
struct tuple_x5
{
    a item0;
    b item1;
    c item2;
    d item3;
    e item4;

    tuple_x5(){}
    tuple_x5( a item0, b item1, c item2, d item3 , e item4 ) :
    item0(item0), item1(item1), item2(item2), item3(item3), item4(item4){}
};

// x6 tuple
template< typename a, typename b, typename c, typename d, typename e, typename f>
struct tuple_x6
{
    a item0;
    b item1;
    c item2;
    d item3;
    e item4;
    f item5;

    tuple_x6(){}
    tuple_x6( a item0, b item1, c item2, d item3, e item4, f item5 ) :
    item0(item0), item1(item1), item2(item2), item3(item3), item4(item4), item5(item5){}
};

// x7 tuple
template< typename a, typename b, typename c, typename d, typename e, typename f, typename g>
struct tuple_x7
{
    a item0;
    b item1;
    c item2;
    d item3;
    e item4;
    f item5;
    g item6;

    tuple_x7(){}
    tuple_x7( a item0, b item1, c item2, d item3, e item4, f item5, g item6 ) :
    item0(item0), item1(item1), item2(item2), item3(item3), item4(item4), item5(item5), item6(item6){}
};

// x8 tuple
template< typename a, typename b, typename c, typename d, typename e, typename f, typename g, typename h>
struct tuple_x8
{
    a item0;
    b item1;
    c item2;
    d item3;
    e item4;
    f item5;
    g item6;
    h item7;

    tuple_x8(){}
    tuple_x8( a item0, b item1, c item2, d item3, e item4, f item5, g item6, h item7 ) :
    item0(item0), item1(item1), item2(item2), item3(item3), item4(item4), item5(item5), item6(item6), item7(item7){}
};

// Fortran analogues functions:

// digits:
template< typename __type > __type __digits__()  { return numeric_limits<__type>::digits; }

// epsilon:
template< typename __type > __type __epsilon__() { return numeric_limits<__type>::epsilon(); }

// huge:
template< typename __type > __type __huge__() { return numeric_limits<__type>::max(); }

// maxexponent:
template< typename __type > __type __maxexponent__() { return numeric_limits<__type>::max_exponent; }

// minexponent:
template< typename __type > __type __minexponent__() { return numeric_limits<__type>::min_exponent; }

// radix:
template< typename __type > __type __radix__() { return numeric_limits<__type>::radix; }

// tiny:
template< typename __type > __type __tiny__() { return numeric_limits<__type>::min(); }

// maximum:
template< typename __type > inline __type __max__ ( __type a , __type b )
{
    return ( a > b ) ? a : b;
}

template< typename __type > inline __type __max__ ( __type a , __type b , __type c )
{
    return __max__< __type >( a , __max__< __type >( b , c ) );
}

template< typename __type > inline __type __max__ ( __type a , __type b , __type c , __type d )
{
    return __max__< __type >( a , __max__< __type >( b , c , d ) );
}

template< typename __type > inline __type __max__ ( __type a , __type b , __type c , __type d , __type e )
{
    return __max__< __type >( a , __max__< __type >( b , c , d , e ) );
}

template< typename __type > inline __type __max__ ( __type a , __type b , __type c , __type d , __type e , __type f )
{
    return __max__< __type >( a , __max__< __type >( b , c , d , e , f ) );
}

template< typename __type > inline __type __max__ ( __type a , __type b , __type c , __type d , __type e , __type f , __type g)
{
    return __max__< __type >( a , __max__< __type >( b , c , d , e , f , g ) );
}

template< typename __type > inline __type __max__ ( __type a , __type b , __type c , __type d , __type e , __type f , __type g , __type h)
{
    return __max__< __type >( a , __max__< __type >( b , c , d , e , f , g , h ) );
}

template< typename __type > inline __type __max__( __type *input , __ix32 N )
{
    __type vmax = input[0];
    for( __ix32 i = 1 ; i < N; i++ )
    {
        if( input[i] > vmax ) vmax = input[i];
    }
    return vmax;
}

// minimum:
template< typename __type > inline __type __min__ ( __type a , __type b )
{
    return ( a < b ) ? a : b;
}

template< typename __type > inline __type __min__ ( __type a , __type b , __type c )
{
    return __min__< __type >( a , __min__< __type >( b , c ) );
}

template< typename __type > inline __type __min__ ( __type a , __type b , __type c , __type d )
{
    return __min__< __type >( a , __min__< __type >( b , c , d ) );
}

template< typename __type > inline __type __min__ ( __type a , __type b , __type c , __type d , __type e )
{
    return __min__< __type >( a , __min__< __type >( b , c , d , e ) );
}

template< typename __type > inline __type __min__ ( __type a , __type b , __type c , __type d , __type e , __type f )
{
    return __min__< __type >( a , __min__< __type >( b , c , d , e , f ) );
}

template< typename __type > inline __type __min__ ( __type a , __type b , __type c , __type d , __type e , __type f , __type g)
{
    return __min__< __type >( a , __min__< __type >( b , c , d , e , f , g ) );
}

template< typename __type > inline __type __min__ ( __type a , __type b , __type c , __type d , __type e , __type f , __type g , __type h)
{
    return __min__< __type >( a , __min__< __type >( b , c , d , e , f , g , h ) );
}

template< typename __type > inline __type __min__( __type *input , __ix32 N )
{
    __type vmin = input[0];
    for( __ix32 i = 1 ; i < N; i++ )
    {
        if( input[i] < vmin ) vmin = input[i];
    }
    return vmin;
}

// modulus
template< typename __type > inline __type __abs__ ( __type a )
{
    return ( a < 0 ) ? -a : a;
}

// sign function
template< typename __type > inline __type __sign__( __type a , __type b )
{
    return a * ( ( b > 0 ) ? (__type)1 : -(__type)1 );
}

// swap function
template< typename __type > inline void __swap__( __type &a , __type &b )
{
    __type c = b;
    b = a;
    a = c;
}

// number flip function
__ix32 __flip__(__ix32 a)
{
    return (a >= 0) ? ~a : a;
}

// number flop function
__ix32 __flop__(__ix32 a)
{
    return a < 0 ? ~a : a;
}

// set bit
void __sbit__(__uix32 &bits, __uix32 bit )
{
    if(bit < sizeof (bit)*8 ) bits |= (1 << bit);
}

// reset bit
void __rbit__(__uix32 &bits, __uix32 bit )
{
    if(bit < sizeof (bit)*8 ) bits &= ~(1 << bit);
}

// get bit
void __gbit__(__uix32 &bits, __uix32 bit )
{
    if(bit < sizeof (bit)*8 ) bits &= (1 << bit);
}

// memory allocation
template< typename __type > inline __type* __alloc__(__ix32 size)
{
    if(size <= 0) return nullptr;
    return (__type*)calloc( size, sizeof(__type) );
}

template< typename __type > inline __type* __alloc__(__ix32 size, __type value)
{
    if(size <= 0) return nullptr;
    __type *memory = (__type*)calloc( size, sizeof(__type) );
    for(__ix32 i = 0 ; i < size; i++ ) memory[i] = value;
    return memory;
}

template< typename __type > inline __type* __alloc__( __ix32 size, void (*initializer)(__type *memory, __ix32 size) )
{
    if(size <= 0) return nullptr;
    __type *memory = (__type*)calloc( size, sizeof(__type) );
    if(initializer && memory)
        initializer(memory, size);
    return memory;
}

// memory reallocation
template< typename __type > inline __type* __realloc__(__type *memory, __ix32 newsize)
{
    if(newsize <= 0 && memory)
    {
        free(memory);
        return nullptr;
    }
    __type *data = (__type*)realloc(memory, sizeof (__type)*newsize );
    if(!data && memory) free(memory);
    return data;
}

// memory free
template< typename __type > inline __type* __mfree__(__type *memory)
{
    if(memory)
    {
        free(memory);
        memory = nullptr;
    }

    return memory;
}

namespace STRING_EXTENSION
{
    vector<string> __split__(string input, string delimeter = " ")
    {
        int start  = 0;
        int end    = 0;
        int size   = delimeter.size();
        vector<string> output;

        while ( end >= 0 )
        {
            end = input.find(delimeter, start);
            output.push_back( input.substr(start, end-start) );
            start = end + size;
        }

        return output;
    }
}

#undef __uix16
#undef __uix32
#undef __uix64

#undef __ix16
#undef __ix32
#undef __ix64

#undef __fx32
#undef __fx64
#undef __fxx64

#endif // UTILS_H
