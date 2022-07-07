#ifndef UTILS_H
#define UTILS_H

#ifndef __ALG_PLATFORM

// include malloc file if it presents
// ( basically, needed by Windows )
#ifdef _MALLOC_H
#include "malloc.h"
#endif

#ifndef _GLIBCXX_CHRONO
#include <chrono>
#endif

#include "QElapsedTimer"
#include <fstream>
#include "iostream"
#include "cmath"
#include "vector"
#include "list"
#include "map"
using namespace std;

#endif

/*! \defgroup <UTILITY> ( Utility functions and classes )
 *  \brief the module collaborates all the utility classes and functions
    @{
*/

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

/*!
    \struct vector2D
 *  \brief 2D vector
*/
template< typename __type > struct vector2D
{
    __type x; ///< x coordinate
    __type y; ///< y coordinate
};

/*!
    \struct vector3D
 *  \brief 3D vector
*/
template< typename __type > struct vector3D
{
    __type x; ///< x coordinate
    __type y; ///< y coordinate
    __type z; ///< z coordinate
};

/*!
    \struct tuple_x2
 *  \brief tuple of 2 elements
*/
template< typename a, typename b>
struct tuple_x2
{
    a item0;
    b item1;

    tuple_x2(){}
    tuple_x2( a item0, b item1 ) :
    item0(item0), item1(item1){}
};

/*!
    \struct tuple_x3
 *  \brief tuple of 3 elements
*/
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

/*!
 *  \struct tuple_x4
 *  \brief tuple of 4 elements
*/
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

/*!
 *  \struct tuple_x5
 *  \brief tuple of 5 elements
*/
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

/*!
 *  \struct tuple_x6
 *  \brief tuple of 6 elements
*/
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

/*!
 *  \struct tuple_x7
 *  \brief tuple of 7 elements
*/
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

/*!
 *  \struct tuple_x8
 *  \brief tuple of 8 elements
*/
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

#ifdef _GLIBCXX_NUMERIC_LIMITS

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


#endif

/*!
 *  \brief looks the maximum among a, b
 *  \param[a] a value
 *  \param[b] b value
*/
template< typename __type > inline __type __max__ ( __type a , __type b )
{
    return ( a > b ) ? a : b;
}

/*!
 *  \brief looks the maximum among a, b, c
 *  \param[a] a value
 *  \param[b] b value
 *  \param[c] c value
*/
template< typename __type > inline __type __max__ ( __type a , __type b , __type c )
{
    return __max__< __type >( a , __max__< __type >( b , c ) );
}

/*!
 *  \brief looks the maximum among a, b, c, d
 *  \param[a] a value
 *  \param[b] b value
 *  \param[c] c value
 *  \param[d] d value
*/
template< typename __type > inline __type __max__ ( __type a , __type b , __type c , __type d )
{
    return __max__< __type >( a , __max__< __type >( b , c , d ) );
}

/*!
 *  \brief looks the maximum among a, b, c, d
 *  \param[a] a value
 *  \param[b] b value
 *  \param[c] c value
 *  \param[d] d value
*/
template< typename __type > inline __type __max__ ( __type a , __type b , __type c , __type d , __type e )
{
    return __max__< __type >( a , __max__< __type >( b , c , d , e ) );
}

/*!
 *  \brief looks the maximum among a, b, c, d, e, f
 *  \param[a] a value
 *  \param[b] b value
 *  \param[c] c value
 *  \param[d] d value
 *  \param[e] e value
 *  \param[f] f value
*/
template< typename __type > inline __type __max__ ( __type a , __type b , __type c , __type d , __type e , __type f )
{
    return __max__< __type >( a , __max__< __type >( b , c , d , e , f ) );
}

/*!
 *  \brief looks the maximum among a, b, c, d, e, f, g
 *  \param[a] a value
 *  \param[b] b value
 *  \param[c] c value
 *  \param[d] d value
 *  \param[e] e value
 *  \param[f] f value
 *  \param[g] g value
*/
template< typename __type > inline __type __max__ ( __type a , __type b , __type c , __type d , __type e , __type f , __type g)
{
    return __max__< __type >( a , __max__< __type >( b , c , d , e , f , g ) );
}

/*!
 *  \brief looks the maximum among a, b, c, d, e, f, g, h
 *  \param[a] a value
 *  \param[b] b value
 *  \param[c] c value
 *  \param[d] d value
 *  \param[e] e value
 *  \param[f] f value
 *  \param[g] g value
 *  \param[h] h value
*/
template< typename __type > inline __type __max__ ( __type a , __type b , __type c , __type d , __type e , __type f , __type g , __type h)
{
    return __max__< __type >( a , __max__< __type >( b , c , d , e , f , g , h ) );
}

/*!
 *  \brief looks the maximum within the input array
 *  \param[input] input array
 *  \param[N] input array size
*/
template< typename __type > inline __type __max__( __type *input , __ix32 N )
{
    __type vmax = input[0];
    for( __ix32 i = 1 ; i < N; i++ )
    {
        if( input[i] > vmax ) vmax = input[i];
    }
    return vmax;
}

/*!
 *  \brief looks the minimum among a, b
 *  \param[a] a value
 *  \param[b] b value
*/
template< typename __type > inline __type __min__ ( __type a , __type b )
{
    return ( a < b ) ? a : b;
}

/*!
 *  \brief looks the minimum among a, b, c
 *  \param[a] a value
 *  \param[b] b value
 *  \param[c] c value
*/
template< typename __type > inline __type __min__ ( __type a , __type b , __type c )
{
    return __min__< __type >( a , __min__< __type >( b , c ) );
}

/*!
 *  \brief looks the minimum among a, b, c, d
 *  \param[a] a value
 *  \param[b] b value
 *  \param[c] c value
 *  \param[d] d value
*/
template< typename __type > inline __type __min__ ( __type a , __type b , __type c , __type d )
{
    return __min__< __type >( a , __min__< __type >( b , c , d ) );
}

/*!
 *  \brief looks the minimum among a, b, c, d, e
 *  \param[a] a value
 *  \param[b] b value
 *  \param[c] c value
 *  \param[d] d value
 *  \param[e] e value
*/
template< typename __type > inline __type __min__ ( __type a , __type b , __type c , __type d , __type e )
{
    return __min__< __type >( a , __min__< __type >( b , c , d , e ) );
}

/*!
 *  \brief looks the minimum among a, b, c, d, e, f
 *  \param[a] a value
 *  \param[b] b value
 *  \param[c] c value
 *  \param[d] d value
 *  \param[e] e value
 *  \param[f] f value
*/
template< typename __type > inline __type __min__ ( __type a , __type b , __type c , __type d , __type e , __type f )
{
    return __min__< __type >( a , __min__< __type >( b , c , d , e , f ) );
}

/*!
 *  \brief looks the minimum among a, b, c, d, e, f, g
 *  \param[a] a value
 *  \param[b] b value
 *  \param[c] c value
 *  \param[d] d value
 *  \param[e] e value
 *  \param[f] f value
 *  \param[g] g value
*/
template< typename __type > inline __type __min__ ( __type a , __type b , __type c , __type d , __type e , __type f , __type g)
{
    return __min__< __type >( a , __min__< __type >( b , c , d , e , f , g ) );
}

/*!
 *  \brief looks the minimum among a, b, c, d, e, f, g, h
 *  \param[a] a value
 *  \param[b] b value
 *  \param[c] c value
 *  \param[d] d value
 *  \param[e] e value
 *  \param[f] f value
 *  \param[g] g value
 *  \param[h] h value
*/
template< typename __type > inline __type __min__ ( __type a , __type b , __type c , __type d , __type e , __type f , __type g , __type h)
{
    return __min__< __type >( a , __min__< __type >( b , c , d , e , f , g , h ) );
}

/*!
 *  \brief looks the minimum within the input array
 *  \param[input] input array
 *  \param[N] input array size
*/
template< typename __type > inline __type __min__( __type *input , __ix32 N )
{
    __type vmin = input[0];
    for( __ix32 i = 1 ; i < N; i++ )
    {
        if( input[i] < vmin ) vmin = input[i];
    }
    return vmin;
}

/*!
 *  \brief returns an absolete value of an input
 *  \param[a] input
*/
template< typename __type > inline __type __abs__ ( __type a )
{
    return ( a < 0 ) ? -a : a;
}

/*!
 *  \brief returns the number b with the sign of a number a
 *  \param[a] a value
 *  \param[b] b value
*/
template< typename __type > inline __type __sign__( __type a , __type b )
{
    return a * ( ( b > 0 ) ? (__type)1 : -(__type)1 );
}

/*!
 *  \brief swaps a and b
 *  \param[a] a value
 *  \param[b] b value
*/
template< typename __type > inline void __swap__( __type &a , __type &b )
{
    __type c = b;
    b = a;
    a = c;
}

/*!
 *  \brief returns -(a+1) i.e flips the sign and value of a number
 *  \param[a] a values
*/
__ix32 __flip__(__ix32 a)
{
    return (a >= 0) ? ~a : a;
}

/*!
 *  \brief returns -(a+1) of the flipped value i.e. flops the number
 *  \param[a] a values
*/
__ix32 __flop__(__ix32 a)
{
    return a < 0 ? ~a : a;
}

/*!
 *  \brief saturation function
 *  \param[input] input
 *  \param[UpperLimit] input upper limit
 *  \param[LowerLimit] input lower limit
 *  \details The function returns the input if it's value is between lower and upper limit.
 *           If the input value is above upper limit - the upper limit is returned.
 *           If the input value is below lower limit - the lower limit is returned.
*/
template<typename __type> __type __saturation__(__type input, __type UpperLimit, __type LowerLimit)
{
    if( input > UpperLimit )
    {
        return UpperLimit;
    }

    if( input < LowerLimit )
    {
        return LowerLimit;
    }

    return input;
}

/*!
 *  \brief bit set function
 *  \param[bitMask] input bit mask
 *  \param[bit] bit number
 *  \details The function sets the given bit of an input number
*/
template<typename __type>
void __set_bit__(__type &bitMask, __ix32 bit )
{
    if(bit < sizeof (bitMask)*8 )
    {
        bitMask |= (1 << bit);
    }
}

/*!
 *  \brief bit reset function
 *  \param[bitMask] input bit mask
 *  \param[bit] bit number
 *  \details The function resets the given bit of an input number
*/
template<typename __type>
void __reset_bit__(__type &bitMask, __ix32 bit )
{
    if(bit < sizeof (bitMask)*8 )
    {
        bitMask &= ~(1 << bit);
    }
}

/*!
 *  \brief get bit function
 *  \param[bitMask] input bit mask
 *  \param[bit] bit number
 *  \details The function returns the given bit value of an input number
*/
template<typename __type>
__type __get_bit__(__type &bitMask, __ix32 bit )
{
    __type result = 0;
    if(bit < sizeof (bitMask)*8 )
    {
        result = bitMask & (1 << bit);
    }
    return result;
}

/*!
 *  \brief bit mask revertion function
 *  \param[bitMask] input bit mask
 *  \param[MSB] input bit mask most significant bit
 *  \details The function returns the given bit value of an input number
*/
template<typename __type>
__type __bit_reverse__(__type bitMask, __ix32 MSB)
{
    __type result = 0;
    for ( __ix32 i = 0 ; i < MSB ; ++i)
    {
        if ( bitMask & ( 1 << i ) ) // look for the set bits
        {
            result |= 1 << ( MSB - 1 - i ); // set the bits at the end of the resulting bit mask
        }
    }

    return result;
}

/*!
 *  \brief MSB number computation function
 *  \param[bitMask] input bit mask
 *  \details The function returns the number of the most sifnificant bit
*/
template<typename __type>
__ix32 __bit_get_MSB__( __ix32 bitMask )
{
    __ix32 MSB = 0;
    while ( ( 1 << MSB ) < bitMask )
    {
        MSB++;
    }

    return MSB;
}

/*!
 *  \brief memory allocation function
 *  \param[size] the output array size
 *  \details The function allocates memory and returns the pointer pointing to the allocated memory segment.
 *           The function doesn't call the input type constructor. So don't use it to allocate customized
 *           object. Use new operator instead.
*/
template< typename __type > inline __type*
__alloc__(__ix32 size)
{
    if(size <= 0) return nullptr;
    return (__type*)calloc( size, sizeof(__type) );
}

/*!
 *  \brief memory allocation function
 *  \param[size] the output array size
 *  \param[value] the value that initializes the output memory segment
 *  \details The function allocates memory and returns the pointer pointing to the allocated memory segment initialized by the given value.
 *           The function doesn't call the input type constructor. So don't use it to allocate customized
 *           object. Use new operator instead.
*/
template< typename __type > inline __type*
__alloc__(__ix32 size, __type value)
{
    if(size <= 0) return nullptr;
    __type *memory = (__type*)calloc( size, sizeof(__type) );
    for(__ix32 i = 0 ; i < size; i++ ) memory[i] = value;
    return memory;
}

/*!
 *  \brief memory allocation function
 *  \param[size] the output array size
 *  \param[initializer] pointer to the function that initializes the output memory segment
 *  \details The function allocates memory and returns the pointer pointing to the allocated memory segment initialized by initializer function.
 *           The function doesn't call the input type constructor. So don't use it to allocate customized
 *           object. Use new operator instead.
*/
template< typename __type > inline __type*
__alloc__( __ix32 size, void (*initializer)(__type *memory, __ix32 size) )
{
    if(size <= 0) return nullptr;
    __type *memory = (__type*)calloc( size, sizeof(__type) );
    if(initializer && memory)
        initializer(memory, size);
    return memory;
}

/*!
 *  \brief memory reallocation function
 *  \param[memory] the input memory segment
 *  \param[newsize] the new size of the input memory segment
 *  \details The function reallocates memory and returns the pointer pointing to the reallocated memory segment.
 *           The function returbs nullptr if the new size is lower or equal to zero or the memory reallocation has failed.
*/
template< typename __type > inline __type*
__realloc__(__type *memory, __ix32 newsize)
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

/*!
 *  \brief memory clean function
 *  \param[memory] the input memory segment
 *  \details The function frees memory and returns nullptr
*/
template< typename __type > inline __type*
__mfree__(__type *memory)
{
    if(memory)
    {
        free(memory);
        memory = nullptr;
    }

    return memory;
}

/*!
 *  \brief memory clean function
 *  \param[tuple] two element tuple tuple.item0
 *  \details The function frees memory of *tuple.item0
*/
template< typename __type > inline tuple_x2< __type*, __ix32 >
__mfree__( tuple_x2< __type*, __ix32 > tuple)
{
    tuple.item0 = __mfree__(tuple.item0);
    tuple.item1 = -1;
    return tuple;
}

/*!
 *  \brief memory clean function
 *  \param[tuple] two element tuple
 *  \details The function frees memory of **tuple.item0
*/
inline tuple_x2< void**, __ix32 >
__mfree__( tuple_x2< void**, __ix32 > tuple)
{
    for(__ix32 i = 0 ; i < tuple.item1 ; i++)
    {
        tuple.item0[i] = __mfree__( tuple.item0[i] );
    }

    tuple.item0 = __mfree__( tuple.item0 );
    tuple.item1 = -1;

    return tuple;
}

/*!
 *  \brief memory clean function
 *  \param[tuple] three element tuple
 *  \details The function frees memory of **tuple.item0
*/
inline tuple_x3< void**, __ix32, __ix32 >
__mfree__( tuple_x3< void**, __ix32, __ix32 > tuple)
{
    for(__ix32 i = 0 ; i < tuple.item1 ; i++)
    {
        tuple.item0[i] = __mfree__( tuple.item0[i] );
    }

    tuple.item0 = __mfree__( tuple.item0 );
    tuple.item1 = -1;
    tuple.item2 = -1;
    return tuple;
}

#ifdef _STRINGFWD_H

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

    class Debugger
    {
        static string GetCurrentTime()
        {
            time_t _time = chrono::system_clock::to_time_t(chrono::system_clock::now());
            string output = ctime(&_time);
            return output.substr(0, output.length()-1);
        }

        static string MessageFormat(string _Typename, string _OjectName, string _Message)
        {
            return "[ " + GetCurrentTime() + " ] " + "[ " + _Typename + " ] " + "[ " + _OjectName + " ] " + _Message + "\n";
        }

        static string MessageFormat(string _Message)
        {
            return "[ " + GetCurrentTime() + " ] " + _Message + "\n";
        }

        public:

        // logging functions
        static void Log(string _Message)
        {
            cout << MessageFormat(_Message);
        }

        static void Log(string _Typename, string _OjectName, string _Message)
        {
            cout << MessageFormat(_Typename, _OjectName,  _Message);
        }
    };

#endif

/*! @} */


// custom macroses undefenition to avoid aliases
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
