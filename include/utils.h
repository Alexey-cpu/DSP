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

#include <bitset>
#include <sstream>
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
namespace TUPLE_PACKAGE
{
    // Tuple
    template< typename ... T >
    struct Tuple;

    template< typename T1, typename ... T2 >
    struct Tuple<T1, T2 ...> : public Tuple<T2...>
    {
    protected:

        T1 m_Data;

        template< uint64_t Index, typename Head, typename ... Tail >
        friend struct TupleUnpacker;

    public:

        typedef T1 return_type;

        // constructors
        Tuple() : m_Data( T1() ){}

        Tuple( T1 _Data, T2 ... _Tail ) : Tuple<T2...>(_Tail...), m_Data( _Data ){}

        // virtual destructor
        virtual ~Tuple(){}
    };

    // Tuple
    template<>
    class Tuple<>{};

    // Tuple unpacker
    template< uint64_t I, typename T1, typename ... T2 >
    struct TupleUnpacker
    {
        typedef typename TupleUnpacker<I-1, T2...>::return_type return_type;
        static return_type& get( Tuple< T1, T2... >& _Input )
        {
            return TupleUnpacker<I-1, T2...>::get(_Input);
        }
    };

    template< typename T1, typename ... T2 >
    struct TupleUnpacker< 0, T1, T2... >
    {
        typedef typename Tuple< T1, T2... >::return_type return_type;
        static return_type& get( Tuple< T1, T2... >& _Input )
        {
            return _Input.m_Data;
        }
    };

    template< uint64_t I, typename T1, typename ... T2 >
    typename TupleUnpacker<I, T1, T2...>::return_type& get( Tuple< T1, T2... >& _Input )
    {
        return TupleUnpacker< I, T1, T2... >::get( _Input );
    }
}

using namespace TUPLE_PACKAGE;

namespace BIT_UTILS
{
    template<typename __type>
    void __set_bit__(__type& _M, uint_fast32_t _N )
    {
        if( _N < sizeof (_M) * 8UL )
        {
            _M |= 1UL << _N;
        }
    }

    template<typename __type>
    void __reset_bit__(__type& _M, uint_fast32_t _N )
    {
        if( _N < sizeof( _M ) * 8UL )
        {
            _M &= ~( 1UL << _N );
        }
    }

    template<typename __type>
    uint_fast32_t __get_bit__( const __type& _M, uint_fast32_t _N )
    {
        return _N < sizeof (_M) * 8UL ? ( _M >> _N ) & 1 : 0;
    }

    template<typename __type>
    __type __bit_reverse__(__type M, uint_fast32_t MSB)
    {
        __type result = 0;
        for ( uint_fast32_t i = 0 ; i < MSB ; ++i)
        {
            if ( M & ( 1 << i ) ) // look for the set bits
            {
                result |= 1 << ( MSB - 1 - i ); // set the bits at the end of the resulting bit mask
            }
        }

        return result;
    }

    template< typename __type >
    uint_fast32_t __get_msb__( const __type& _M )
    {
        uint_fast32_t MSB = 0;

        while ( ( 1 << MSB ) < _M )
        {
            MSB++;
        }

        return MSB;
    }

    template<typename T1, typename T2>
    T2 __retrieve_bits__( const T1& _Input, uint_fast32_t _S, uint_fast32_t _E )
    {
        // check
        if( _E > sizeof(T1) * 8 )
            return 0;

        // main code
        T2 output = 0;

        for( uint_fast32_t i = _S, j = 0 ; i < _E ; i++, j++ )
        {
            T1 bit = BIT_UTILS::__get_bit__<T1>( _Input, i );

            if( bit > 0 )
            {
                BIT_UTILS::__set_bit__( output, j );
            }
        }

        return output;
    }

    inline uint_fast64_t __lfsr64__ ( uint_fast64_t _INPUT )
    {
        static uint_fast64_t S = _INPUT;
        S = ((((S >> 63) ^ (S >> 62) ^ (S >> 61) ^ (S >> 59) ^ (S >> 57) ^ S ) & (uint64_t)1 ) << 63 ) | (S >> 1);
        return S;
    }

#ifdef _STRINGFWD_H

    template< typename __type >
    std::string __to_hex_string__( const __type& _M, uint_fast32_t _N = 0 )
    {
        // initialize stream
        std::stringstream stream;
        stream << std::hex;

        // generate hex string
        uint_fast32_t N = _N < 4 ? sizeof(_M) * 8 : (uint_fast8_t)_N;

        for( uint_fast8_t i = 0 ; i < N ; i += 4 )
        {
            stream << BIT_UTILS::__retrieve_bits__< __type, uint_fast64_t >( _M, i, i + 4 );
        }

        return stream.str();
    }

#endif
}

using namespace BIT_UTILS;

#ifdef _GLIBCXX_BITSET

namespace BITSET_UTILS
{
    template< std::size_t N >
    void __read_bits_from_uint__( std::bitset<N>& _Target, const uint_fast64_t& _Source, uint_fast64_t _Start, uint_fast64_t _End )
    {
        // check
        if( N < _Start || N < _End )
            return;

        // main code
        for( uint_fast64_t i = _Start, j = 0 ; i < _End ; i++, j++ )
        {
            _Target[i] = BIT_UTILS::__get_bit__( _Source, j );
        }
    }

    template< std::size_t N1, std::size_t N2 >
    std::bitset<N2> __extract_bits__( std::bitset<N1>& _Source, uint_fast64_t _Start, uint_fast64_t _End )
    {
        // generate output
        std::bitset<N2> output;

        // check
        if( N1 < _Start || N1 < _End )
            return output;

        // fill output
        for( uint_fast64_t i = _Start, j = 0 ; i < _End ; i++, j++ )
        {
            output[j] = _Source[i];
        }

        return output;
    }

    template< std::size_t N >
    uint_fast32_t __get_msb__( std::bitset<N>& _Source )
    {
        uint_fast32_t MSB = 0;

        for( uint_fast32_t i = 0 ; i < N ; i++ )
        {
            if( _Source[i] > 0 )
            {
                MSB = i;
            }
        }

        return MSB;
    }

    template< std::size_t N >
    bool __big_unsigned_numbers_equal__( std::bitset<N> _A, std::bitset<N> _B )
    {
        return !( _A ^ _B ).any();
    }

    template< std::size_t N >
    bool __big_unsigned_number_is_greater__( std::bitset<N> _A, std::bitset<N> _B )
    {
        std::bitset<N> reference;
        reference[N-1] = 1;

        if( __big_unsigned_numbers_equal__( _A, _B ) )
            return false;

        while ( !( (_A ^ _B) & reference).any() )
        {
            _A <<= 1;
            _B <<= 1;
        }

        return (_A & reference).any();
    }

    template< std::size_t N >
    bool __big_unsigned_number_is_less__( std::bitset<N> _A, std::bitset<N> _B )
    {
        std::bitset<N> reference;
        reference[N-1] = 1;

        if( __big_unsigned_numbers_equal__( _A, _B ) )
            return false;

        while ( !( (_A ^ _B) & reference).any() )
        {
            _A <<= 1;
            _B <<= 1;
        }

        return !(_A & reference).any();
    }

    template< std::size_t N >
    struct BitSetsComparator
    {
        using is_transparent = std::bitset<N>;

    public:

        bool operator()( const std::bitset<N>& _A, const std::bitset<N>& _B ) const
        {
            return __big_unsigned_number_is_less__( _A, _B );
        }
    };

#ifdef _STRINGFWD_H

    template< std::size_t N >
    std::string __write_to_hex_string__( const std::bitset<N>& _Set )
    {
        // initialize stream
        std::stringstream stream;
        stream << std::hex;

        // generate hex string
        for( uint_fast64_t i = 0 ; i < N ; i += 4 )
        {
            uint_fast64_t hexNumber = 0;

            for( uint_fast64_t j = 0 ; j < 4 ; j++ )
            {
                if( _Set[ i + j ] > 0 )
                {
                    BIT_UTILS::__set_bit__( hexNumber, j );
                }
            }

            stream << hexNumber;
        }

        return stream.str();
    }

#endif
}

using namespace BITSET_UTILS;

#endif

template<typename a, typename b, typename c>
struct tuple_x3
{
    a item0;
    b item1;
    c item2;
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
template< typename __type > inline __type __max__( __type* input , int N )
{
    __type vmax = input[0];

    for( int i = 1 ; i < N; i++ )
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
inline __type __min__( const __type *input, int N )
{
    int imin = 0;
    __type vmin = input[imin];
    for( int i = 1 ; i < N; i++ )
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
/*
template< typename __type > inline tuple_x2<__type, int>
__min__( Tuple<__type*, int> vector )
{
    __type *input = get<0>( vector );
    int     imin  = 0;
    __type  vmin  = input[imin];

    for( int i = 1 ; i < get<1>(vector); i++ )
    {
        if( input[i] < vmin )
        {
            vmin = input[i];
            imin = i;
        }
    }

    return { vmin, imin };
}
*/

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
inline double __to_degrees__(double angle)
{
    return angle * __TO_DEGREES_CONVERSION_MULTIPLYER__;
}

/*!
 *  \brief converts input angle to radians
 *  \param[angle] input angle
*/
inline double __to_radians__(double angle)
{
    return angle * __TO_RADIANS_CONVERSION_MULTIPLYER__;
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

/*!
 *  \brief memory allocation function
 *  \param[size] the output array size
 *  \details The function allocates memory and returns the pointer pointing to the allocated memory segment.
 *           The function doesn't call the input type constructor. So don't use it to allocate customized
 *           object. Use new operator instead.
*/
template< typename __type > inline __type*
__alloc__(int size)
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
__alloc__(int size, __type value)
{
    if(size <= 0) return nullptr;
    __type *memory = (__type*)calloc( size, sizeof(__type) );
    for(int i = 0 ; i < size; i++ ) memory[i] = value;
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
__alloc__( int size, void (*__initializer__)(__type *memory, int size) )
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
__realloc__(__type *memory, int newsize)
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

/*
template< typename __type > void
__alloc__( tuple_x3< __type**, int, int > *tuple, int m, int n )
{
    tuple->item1 = m;
    tuple->item2 = n;
    tuple->item0 = __alloc__<__type*>(m);

    for( int i = 0 ; i < tuple->item1 ; i++ )
    {
        tuple->item0[i] = __alloc__<__type>(n);
    }
}
*/

#ifdef _STRINGFWD_H

#define DEBUGGER

    namespace STRING_EXTENSION
    {
        inline string __remove_prefix__( string _Input, string _Prefix )
        {
            size_t j = 0;
            for( size_t i = 0 ; i < _Prefix.size() ; i++ )
            {
                if( _Input[i] != _Prefix[i] )
                {
                    return _Input;
                }

                j++;
            }

            string output;
            for( size_t i = j ; i < _Input.size() ; i++ )
            {
                output += _Input[i];
            }

            return output;
        }

        inline string __format_camel_style_string__( string _Input )
        {
            string output;
            for( size_t i = 0 ; i < _Input.size() ; i++ )
            {
                output += _Input[i];

                if( i + 1 < _Input.size() && std::isupper( _Input[i+1] ) )
                    output += ' ';
            }

            return output;
        }

        inline string __class_field_to_string__( string _Input )
        {
            return __format_camel_style_string__( __remove_prefix__( _Input, "m_" ) );
        }

        inline std::vector< string > __split__(string input, string delimeter = " ")
        {
            if( input.empty() )
                return std::vector< string >();

            int start  = 0;
            int end    = 0;
            int size   = delimeter.size();
            std::vector< string > output;

            while ( end >= 0 )
            {
                end = input.find(delimeter, start);
                output.push_back( input.substr(start, end-start) );
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
            std::cout << MessageFormat(_Message);
        }

        static void Log(string _Typename, string _OjectName, string _Message)
        {
            std::cout << MessageFormat(_Typename, _OjectName,  _Message);
        }
    };

#endif

/*! @} */

//------------------------------------------------------------------------------------------------------------------------------
// forget macro to avoid aliases
//------------------------------------------------------------------------------------------------------------------------------
#undef __TO_DEGREES_CONVERSION_MULTIPLYER__
#undef __TO_RADIANS_CONVERSION_MULTIPLYER__
//------------------------------------------------------------------------------------------------------------------------------

#endif // UTILS_H
