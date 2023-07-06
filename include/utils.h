#ifndef UTILS_H
#define UTILS_H

// some special target platform features
#ifndef __ALG_PLATFORM

#ifdef _MALLOC_H
#include "malloc.h"
#endif

#ifndef _GLIBCXX_CHRONO
#include <chrono>
#endif

#include "QElapsedTimer"
#include <fstream>
#include <algorithm>
#include <random>
#include "iostream"
#include "cmath"
#include "vector"
#include "list"
#include "map"
#include <limits.h>
#include <float.h>
#include <set>
using namespace std;

#endif

// define custom types
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

#ifndef __TO_DEGREES_CONVERSION_MULTIPLYER__
#define __TO_DEGREES_CONVERSION_MULTIPLYER__ 57.295779513082320876798154814105
#endif

#ifndef __TO_RADIANS_CONVERSION_MULTIPLYER__
#define __TO_RADIANS_CONVERSION_MULTIPLYER__ 0.01745329251994329576923690768489
#endif

#ifndef STRINGIFY
#define STRINGIFY(INPUT) #INPUT
#endif

#ifndef CAT
#define CAT(X,Y) X##_##Y
#endif

/*! \defgroup <UTILITY> ( Utility functions and classes )
 *  \brief the module collaborates all the utility classes and functions
    @{
*/

/*!
    \struct vector2D
 *  \brief 2D vector
*/
template< typename __type >
struct vector2D
{
    __type x; ///< x coordinate
    __type y; ///< y coordinate
};

/*!
    \struct vector3D
 *  \brief 3D vector
*/
template< typename __type >
struct vector3D
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
template< typename __type >
inline __type __min__( const __type *input, __ix32 N )
{
    __ix32 imin = 0;
    __type vmin = input[imin];
    for( __ix32 i = 1 ; i < N; i++ )
    {
        if( input[i] < vmin )
        {
            vmin = input[i];
            imin = i;
        }
    }

    return vmin;
}

/*!
 *  \brief looks the minimum within the input array
 *  \param[input] input array
 *  \param[N] input array size
*/
template< typename __type > inline tuple_x2<__type, __ix32>
__min__( tuple_x2<__type*, __ix32> vector )
{
    __type *input = vector.item0;
    __ix32  imin = 0;
    __type  vmin = input[imin];
    for( __ix32 i = 1 ; i < vector.item1; i++ )
    {
        if( input[i] < vmin )
        {
            vmin = input[i];
            imin = i;
        }
    }

    return { vmin, imin };
}

/*!
 *  \brief returns an absolete value of an input
 *  \param[a] input
*/
template< typename __type >
inline __type __abs__ ( __type a )
{
    return ( a < 0 ) ? -a : a;
}

/*!
 *  \brief returns the number b with the sign of a number a
 *  \param[a] a value
 *  \param[b] b value
*/
template< typename __type >
inline __type __sign__( __type a , __type b )
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
 *  \brief converts input angle to degrees
 *  \param[angle] input angle
*/
inline __fx64 __to_degrees__(__fx64 angle)
{
    return angle * __TO_DEGREES_CONVERSION_MULTIPLYER__;
}

/*!
 *  \brief converts input angle to radians
 *  \param[angle] input angle
*/
inline __fx64 __to_radians__(__fx64 angle)
{
    return angle * __TO_RADIANS_CONVERSION_MULTIPLYER__;
}

/*!
 *  \brief returns -(a+1) i.e flips the sign and value of a number
 *  \param[a] a values
*/
inline __ix32 __flip__(__ix32 a)
{
    return (a >= 0) ? ~a : a;
}

/*!
 *  \brief returns -(a+1) of the flipped value i.e. flops the number
 *  \param[a] a values
*/
inline __ix32 __flop__(__ix32 a)
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
template<typename __type> __type
__saturation__(__type input, __type UpperLimit, __type LowerLimit)
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

template<typename __type> void
__cumulative_summ__( __type *input, __ix32 size )
{
    for( __ix32 i = 0, j = 0, k = 0 ; i < size ; i++ )
    {
        k += input[i];
        input[i] = j;
        j = k;
    }
}

/*!
 *  \brief bit set function
 *  \param[bitMask] input bit mask
 *  \param[bit] bit number
 *  \details The function sets the given bit of an input number
*/
template<typename __type>
void __set_bit__(__type &_Mask, size_t _Bit )
{
    if( _Bit < sizeof (_Mask) * 8UL )
        _Mask |= 1UL << _Bit;
}

/*!
 *  \brief bit reset function
 *  \param[bitMask] input bit mask
 *  \param[bit] bit number
 *  \details The function resets the given bit of an input number
*/
template<typename __type>
void __reset_bit__(__type &_Mask, size_t _N )
{
    if( _N < sizeof( _Mask ) * 8UL )
        _Mask &= ~(1UL << _N);
}

/*!
 *  \brief get bit function
 *  \param[bitMask] input bit mask
 *  \param[bit] bit number
 *  \details The function returns the given bit value of an input number
*/
inline char __get_bit__(size_t &_Mask, size_t _N )
{
    return _N < sizeof (_Mask) * 8UL ? ( _Mask >> _N ) & 1UL : 0UL;
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
__ix32 __bit_get_MSB__( __ix32 bitMask )
{
    __ix32 MSB = 0;
    while ( ( 1 << MSB ) < bitMask )
    {
        MSB++;
    }

    return MSB;
}

bool check_power_of_two( __ix32 _Number )
{
    return !(bool)( _Number & (_Number-1) );
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
__alloc__( __ix32 size, void (*__initializer__)(__type *memory, __ix32 size) )
{
    if(size <= 0) return nullptr;
    __type *memory = (__type*)calloc( size, sizeof(__type) );
    if(__initializer__ && memory)
        __initializer__(memory, size);
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
 *  \brief tuple based 1D array model memory allocation function
 *  \param[tuple] input tuple
 *  \details The function takes by pointer an input tuple and allocates it's memory pointer.
 *           The function is not safe. Yout MUST guarantee that the input tuple
 *           is empty or you get memory leak otherwise !!!
*/
template< typename __type > void
__alloc__( tuple_x2< __type*, __ix32 > *tuple, __ix32 n )
{
    tuple->item1 = n;
    tuple->item0 = __alloc__<__type>( tuple->item1 );
}

/*!
 *  \brief tuple based 1D array model memory free function
 *  \param[tuple] input tuple
 *  \details The function frees input tuple based 1D array memory.
*/
template< typename __type > inline tuple_x2< __type*, __ix32 >
__mfree__( tuple_x2< __type*, __ix32 > tuple )
{
    tuple.item0 = __mfree__(tuple.item0);
    tuple.item1 = -1;
    return tuple;
}
/*!
 *  \brief tuple based 2D array model memory allocation function
 *  \param[tuple] input tuple
 *  \details The function takes by pointer an input tuple and allocates it's memory pointer.
 *           The function is not safe. Yout MUST guarantee that the input tuple
 *           is empty or you get memory leak otherwise !!!
*/
template< typename __type > void
__alloc__( tuple_x3< __type**, __ix32, __ix32 > *tuple, __ix32 m, __ix32 n )
{
    tuple->item1 = m;
    tuple->item2 = n;
    tuple->item0 = __alloc__<__type*>(m);

    for( __ix32 i = 0 ; i < tuple->item1 ; i++ )
    {
        tuple->item0[i] = __alloc__<__type>(n);
    }
}

/*!
 *  \brief tuple based 2D array model memory free function
 *  \param[tuple] input tuple
 *  \details The function frees input tuple based 2D array memory.
*/
template< typename __type > inline tuple_x3< __type**, __ix32, __ix32 >
__mfree__( tuple_x3< __type**, __ix32, __ix32 > tuple )
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

#ifndef __ALG_PLATFORM

// radix sort with binary base 8 implementation for integer types
template< typename __type > void
__radix_sort__( __type *input, __type *output, __ix32 size, __ix32 descending = 1 )
{
    // chek the input type
    if( !is_same<__type, __ix16 >::value &&
        !is_same<__type, __ix32 >::value &&
        !is_same<__type, __ix64 >::value &&
        !is_same<__type, __uix16>::value &&
        !is_same<__type, __uix32>::value &&
        !is_same<__type, __uix64>::value)
        return;

      // check input:
      if( !input || !output )
          return;

      // auxiliary variables:
      __ix32 i = 0 , j = 0 , k = 0;
      __ix32 bit_shift = 0;
      __ix32 N = size - 1;

      // auxiliary workspace:
      __ix32 box[8];

      // looking for the maximum element within the input
      // and initialaling the elements mapping
      __ix32 max = input[0];
      for (i = 0; i < size; i++)
      {
          if ( input[i] > max)
              max = input[i];
      }

      while ( (max >> bit_shift) > 0 )
      {
         // initializing workspace:
         box[0] = 0;
         box[1] = 0;
         box[2] = 0;
         box[3] = 0;
         box[4] = 0;
         box[5] = 0;
         box[6] = 0;
         box[7] = 0;

         for (i = 0; i < size; i++)
         {
             j = ( input[i] >> bit_shift ) & 7;
             box[j]++;
         }

         // cumulative sum calculation:
         box[1] += box[0];
         box[2] += box[1];
         box[3] += box[2];
         box[4] += box[3];
         box[5] += box[4];
         box[6] += box[5];
         box[7] += box[6];

         for (i = N ; i >= 0; i--)
         {
             j = ( input[i] >> bit_shift ) & 7;
             k = --box[j];
             output [k] = input[i];
         }

         // ovewrite the input:
         for (i = 0; i < size; i++)
         {
             input[i] = output[i];
         }

         // incrementing the bitshift:
         bit_shift += 3;
     }

      if( !descending )
      {
          i = 0;
          j = size-1;
          while ( i < j )
              __swap__( input[i++], input[j--] );
      }
}

template< typename __key, typename __value > void
__radix_sort__( __key *keys, __value *values, void **workspace, __ix32 size, __ix32 descending = 1 )
{
    // chek the input type
    if( !is_same<__value, __ix16 >::value &&
        !is_same<__value, __ix32 >::value &&
        !is_same<__value, __ix64 >::value &&
        !is_same<__value, __uix16>::value &&
        !is_same<__value, __uix32>::value &&
        !is_same<__value, __uix64>::value)
        return;

      // check input:
      if( !values || !workspace )
          return;

      __value *sorted_values = (__value*)workspace[0];
      __key   *sorted_keys   = (__key*)workspace[1];

      // auxiliary variables:
      __ix32 i = 0 , j = 0 , k = 0;
      __ix32 bit_shift = 0;
      __ix32 N = size - 1;

      // auxiliary workspace:
      __ix32 box[8];

      // looking for the maximum element within the input
      // and initialaling the elements mapping
      __ix32 max = values[0];
      for (i = 0; i < size; i++)
      {
          if ( values[i] > max)
              max = values[i];
      }

      while ( (max >> bit_shift) > 0 )
      {
         // initializing workspace:
         box[0] = 0;
         box[1] = 0;
         box[2] = 0;
         box[3] = 0;
         box[4] = 0;
         box[5] = 0;
         box[6] = 0;
         box[7] = 0;

         for (i = 0; i < size; i++)
         {
             j = ( values[i] >> bit_shift ) & 7;
             box[j]++;
         }

         // cumulative sum calculation:
         box[1] += box[0];
         box[2] += box[1];
         box[3] += box[2];
         box[4] += box[3];
         box[5] += box[4];
         box[6] += box[5];
         box[7] += box[6];

         for (i = N ; i >= 0; i--)
         {
             j = ( values[i] >> bit_shift ) & 7;
             k = --box[j];
             sorted_values [k] = values[i];
             sorted_keys[k] = keys[i];
         }

         // ovewrite the input:
         for (i = 0; i < size; i++)
         {
             keys[i] = sorted_keys[i];
             values[i] = sorted_values[i];
         }

         // incrementing the bitshift:
         bit_shift += 3;
     }

     if( !descending )
     {
         i = 0;
         j = size-1;
         while ( i < j )
         {
             __swap__( keys[i], keys[j] );
             __swap__( values[i], values[j] );
             i++;
             j--;
         }
     }
}

inline __ix32 __generate_random_number__(__ix32 min, __ix32 max)
{
    return min + (rand() % max);
}

#endif

// pseudo random number generator
inline uint64_t __PRNG__ ( uint64_t _SEED = 1 )
{
  static uint64_t S = (uint64_t)&_SEED;
  S = ((((S >> 63) ^ (S >> 62) ^ (S >> 61) ^ (S >> 59) ^ (S >> 57) ^ S ) & (uint64_t)1 ) << 63 ) | (S >> 1);
  return S;
}

#ifdef _STRINGFWD_H

#define DEBUGGER

    namespace STRING_EXTENSION
    {
        inline map< size_t, string > __split__(string input, string delimeter = " ")
        {
            if( input.empty() )
                return map< size_t, string >();

            int start  = 0;
            int end    = 0;
            int size   = delimeter.size();
            map< size_t, string > output;

            while ( end >= 0 )
            {
                end = input.find(delimeter, start);
                output[ output.size() ] = input.substr(start, end-start);
                start = end + size;
            }

            return output;
        }

        inline bool __string_contains__(string input, char delimeter = ' ')
        {
            for( size_t i = 0 ; i < input.size() ; i++ )
            {
                if( input[i] == delimeter )
                    return true;
            }

            return false;
        }

        inline string __to_upper__( string _String )
        {
            transform(_String.begin(), _String.end(), _String.begin(), ::toupper);
            return _String;
        }

        inline string __to_lower__( string _String )
        {
            transform(_String.begin(), _String.end(), _String.begin(), ::tolower);
            return _String;
        }

        inline string __remove_symbol__( string _String, char _Symbol )
        {
            string output;

            // compute output string size
            size_t size = 0;
            for( size_t i = 0 ; i < _String.size() ; i++ )
            {
                if( _String[i] != _Symbol )
                    size++;
            }

            // edit output string
            output.resize(size);
            for( size_t i = 0, j = 0 ; i < _String.size() ; i++ )
            {
                if( _String[i] != _Symbol )
                    output[j++] = _String[i];
            }

            return output;
        }

        inline string __remove_symbols__( string _String, std::set< char > _Symbols )
        {
            string output;

            // compute output string size
            size_t size = 0;
            for( size_t i = 0 ; i < _String.size() ; i++ )
            {
                if( _Symbols.find( _String[i] ) == _Symbols.end() )
                    size++;
            }

            // edit output string
            output.resize(size);
            for( size_t i = 0, j = 0 ; i < _String.size() ; i++ )
            {
                if( _Symbols.find( _String[i] ) == _Symbols.end() )
                    output[j++] = _String[i];
            }

            return output;
        }

        template< typename __type >
        __type __from_string__( string _Input );

        template<> inline float __from_string__< float >( string _Input )
        {
            try
            {
                return std::stof( _Input );
            }
            catch(...)
            {
                return 0.0;
            }
        }

        template<> inline double __from_string__<double>( string _Input )
        {
            try
            {
                return std::stod( _Input );
            }
            catch(...)
            {
                return 0.0;
            }
        }

        template<> inline long double __from_string__<long double>( string _Input )
        {
            try
            {
                return std::stold( _Input );
            }
            catch(...)
            {
                return 0.0;
            }
        }

        template<> inline int __from_string__<int>( string _Input )
        {
            try
            {
                return std::stoi( _Input );
            }
            catch(...)
            {
                return 0.0;
            }
        }

        template<> inline long long __from_string__<long long>( string _Input )
        {
            try
            {
                return std::stoll( _Input );
            }
            catch(...)
            {
                return 0.0;
            }
        }

        template<> inline unsigned long __from_string__<unsigned long>( string _Input )
        {
            try
            {
                return std::stoul( _Input );
            }
            catch(...)
            {
                return 0.0;
            }
        }

        template<> inline bool __from_string__<bool>( string _Input )
        {
            try
            {
                return _Input == "true" || _Input == "1" ? true : false;
            }
            catch(...)
            {
                return false;
            }
        }

        template<> inline unsigned long long __from_string__<unsigned long long>( string _Input )
        {
            try
            {
                return std::stoull( _Input );
            }
            catch(...)
            {
                return 0.0;
            }            
        }

        template<> inline string __from_string__<string>( string _Input )
        {
            return _Input;
        }

        template< typename __type >
        string __to_string__( __type _Input );

        template<> inline string __to_string__<bool>( bool _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline string __to_string__< float >( float _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline string __to_string__<double>( double _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline string __to_string__<long double>( long double _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline string __to_string__<int>( int _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline string __to_string__<long long>( long long _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline string __to_string__<unsigned long>( unsigned long _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline string __to_string__<unsigned long long>( unsigned long long _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline string __to_string__<string>( string _Input )
        {
            return _Input;
        }
    }

    class Debugger
    {
        public:

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

        static string MessageFormat( string _OjectName, string _Message)
        {
            return "[ " + GetCurrentTime() + " ] " + "[ " + _OjectName + " ] " + _Message + "\n";
        }

        static string MessageFormat(string _Message)
        {
            return "[ " + GetCurrentTime() + " ] " + _Message + "\n";
        }

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

//------------------------------------------------------------------------------------------------------------------------------
// forget macro to avoid aliases
//------------------------------------------------------------------------------------------------------------------------------
#undef __fx32
#undef __fx64
#undef __fxx64
#undef __ix16
#undef __ix32
#undef __ix64
#undef __uix16
#undef __uix32
#undef __uix64

// forget the custom constants
#undef __TO_DEGREES_CONVERSION_MULTIPLYER__
#undef __TO_RADIANS_CONVERSION_MULTIPLYER__
//------------------------------------------------------------------------------------------------------------------------------

#endif // UTILS_H
