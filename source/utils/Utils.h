#ifndef UTILS_H
#define UTILS_H

// some special target platform features
#ifndef __ALG_PLATFORM

#define DEBUG

#ifdef _MALLOC_H
#include "malloc.h"
#endif

#ifndef _GLIBCXX_CHRONO
#include <chrono>
#endif

// STL
#include <float.h>
#include <bitset>
#include <limits.h>

// STL math and algorithms
#include <algorithm>
#include <random>
#include <cmath>
#include <iomanip>

// STL containers
#include "vector"
#include "list"
#include "map"
#include <set>

// STL input/output streams
#include <iostream>
#include <sstream>
#include <fstream>
#include <functional>

#endif

// define custom types
#ifndef STRING_LIBS_INCLUDED
#define STRING_LIBS_INCLUDED defined(_STRINGFWD_H) || defined(_LIBCPP_IOSFWD)
#endif

#ifndef __TO_DEGREES_CONVERSION_MULTIPLYER__
#define __TO_DEGREES_CONVERSION_MULTIPLYER__ 57.295779513082320876798154814105
#endif

#ifndef __TO_RADIANS_CONVERSION_MULTIPLYER__
#define __TO_RADIANS_CONVERSION_MULTIPLYER__ 0.01745329251994329576923690768489
#endif

#ifndef PI0
#define PI0 3.1415926535897932384626433832795
#endif

#ifndef PI2
#define PI2 6.283185307179586476925286766559
#endif

#ifndef PI_2
#define PI_2 1.5707963267948966192313216916398
#endif

#ifndef PI_4
#define PI_4 0.78539816339744830961566084581988
#endif

#ifndef STRINGIFY
#define STRINGIFY(INPUT) #INPUT
#endif

#ifndef CONCATENATE
#define CONCATENATE(X,Y) X##_##Y
#endif

/*! \defgroup <UTILITY> ( Utility functions and classes )
 *  \brief the module collaborates all the utility classes and functions
    @{
*/

template< typename __type >
struct Vector2D
{
    __type x = 0;
    __type y = 0;
};

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

        const Tuple<T2...>* m_Parent = static_cast< Tuple<T2...>* >( this );

        const uint64_t m_Index;

        template< uint64_t Index, typename Head, typename ... Tail >
        friend struct TupleUnpacker;

    public:

        typedef T1 return_type;

        // constructors
        Tuple() : m_Data( T1() ), m_Index( m_Parent->count() + 1 ){}

        Tuple( T1 _Data, T2 ... _Tail ) : Tuple<T2...>(_Tail...), m_Data( _Data ), m_Index( m_Parent->count() + 1 ){}

        // virtual destructor
        virtual ~Tuple(){}

        // public methods
        uint64_t count() const
        {
            return m_Index;
        }
    };

    // Tuple
    template<>
    struct Tuple<>
    {
    protected:
        const uint64_t m_Index = 0;

    public:

        // public methods
        uint64_t count() const
        {
            return m_Index;
        }
    };

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

#if defined(_STDINT_H) || defined(_STDINT_H_)

namespace BIT_UTILS
{
    template<typename __type>
    void __set_bit__(__type& _M, uint_fast8_t _N )
    {
        if( _N < sizeof (_M) * 8UL )
        {
            _M |= 1UL << _N;
        }
    }

    template<typename __type>
    void __reset_bit__(__type& _M, uint_fast8_t _N )
    {
        if( _N < sizeof( _M ) * 8UL )
        {
            _M &= ~( 1UL << _N );
        }
    }

    template<typename __type>
    uint_fast32_t __get_bit__( const __type& _M, uint_fast8_t _N )
    {
        return _N < sizeof (_M) * 8UL ? ( _M >> _N ) & 1 : 0;
    }

    template<typename __type>
    __type __bit_reverse__(__type M, uint_fast8_t MSB)
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
    T2 __retrieve_bits__( const T1& _Input, uint_fast8_t _S, uint_fast8_t _E )
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

#if defined(_STRINGFWD_H) || defined(_LIBCPP_IOSFWD)

    template< typename __type >
    std::string __to_hex_string__( const __type& _M, uint_fast8_t _N = 0 )
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

#if defined(_GLIBCXX_BITSET) || defined(_LIBCPP_BITSET)

namespace BITSET_UTILS
{
    template< std::size_t N >
    void __read_bits_from_uint__( std::bitset<N>& _Target, const uint_fast64_t& _Source, uint_fast8_t _Start, uint_fast8_t _End )
    {
        // check
        if( N < _Start || N < _End )
            return;

        // main code
        for( uint_fast8_t i = _Start, j = 0 ; i < _End ; i++, j++ )
        {
            _Target[i] = BIT_UTILS::__get_bit__( _Source, j );
        }
    }

    template< std::size_t N1, std::size_t N2 >
    std::bitset<N2> __extract_bits__( const std::bitset<N1>& _Source, uint_fast8_t _Start, uint_fast8_t _End )
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
    uint_fast8_t __get_msb__( std::bitset<N>& _Source )
    {
        uint_fast8_t MSB = 0;

        for( uint_fast8_t i = 0 ; i < N ; i++ )
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

#if defined(_STRINGFWD_H) || defined(_LIBCPP_IOSFWD)

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

#endif

#if defined(_GLIBCXX_NUMERIC_LIMITS) || defined(_LIBCPP_LIMITS)

    // Fortran analogues functions
    template< typename __type > __type __digits__()  { return std::numeric_limits<__type>::digits; }

    template< typename __type > __type __epsilon__() { return std::numeric_limits<__type>::epsilon(); }

    template< typename __type > __type __huge__() { return std::numeric_limits<__type>::max(); }

    template< typename __type > __type __maxexponent__() { return std::numeric_limits<__type>::max_exponent; }

    template< typename __type > __type __minexponent__() { return std::numeric_limits<__type>::min_exponent; }

    template< typename __type > __type __radix__() { return std::numeric_limits<__type>::radix; }

    template< typename __type > __type __tiny__() { return std::numeric_limits<__type>::min(); }

    template< typename __type >
    class PseudoRandomNumberGenerator
    {
    private:

        std::mt19937_64 m_PseudoRandomNumberGenerator = std::mt19937_64( (uint_fast64_t)this );

    public:

        // constructors
        PseudoRandomNumberGenerator(){}

        // virtual destructor
        ~PseudoRandomNumberGenerator(){}

        // getters
        std::mt19937_64& get_pseudo_random_number_generator()
        {
            return m_PseudoRandomNumberGenerator;
        }

        // operators
        __type operator()(uint_fast64_t _Min = 0, uint_fast64_t _Max = __huge__<__type>() )
        {            
            long double integer  = (long double)( _Min + m_PseudoRandomNumberGenerator() % ( ( _Max + 1 ) - _Min ) );
            long double floating = m_PseudoRandomNumberGenerator() % 1024;
            while( floating > 1.0 )
                floating /= 1024;
            return (__type)( integer + floating );
        }

    };

#endif

template< typename Head >
inline __attribute__((always_inline)) Head __max__( Head _A, Head _B )
{
    return _A > _B ? _A : _B;
}

template< typename Head, typename ... Args >
Head __max__( Head _A, Head _B, Args... _Args )
{
    return __max__( __max__( _A, _B ), _Args... );
}

template< typename __type > inline __type
__max__( __type* _Input , uint64_t _Size )
{
    __type vmax = _Input[0];

    for( uint64_t i = 1 ; i < _Size; i++ )
    {
        if( _Input[i] > vmax )
            vmax = _Input[i];
    }

    return vmax;
}

template< typename Head >
inline __attribute__((always_inline)) Head __min__( Head _A, Head _B )
{
    return _A < _B ? _A : _B;
}

template< typename Head, typename ... Args >
Head __min__( Head _A, Head _B, Args... _Args )
{
    return __min__( __min__( _A, _B ), _Args... );
}

template< typename __type >
inline __type __min__( const __type* _Input, int _Size )
{
    int    imin = 0;
    __type vmin = _Input[imin];

    for( int i = 1 ; i < _Size; i++ )
    {
        if( _Input[i] < vmin )
        {
            vmin = _Input[i];
            imin = i;
        }
    }

    return vmin;
}

template< typename __type >
inline __attribute__((always_inline)) __type __abs__ ( __type _A )
{
    return ( _A < 0 ) ? -_A : _A;
}

template< typename __type >
inline __attribute__((always_inline)) __type __sign__( __type _A, __type _B )
{
    return _A * ( ( _B > 0 ) ? (__type)1 : -(__type)1 );
}

template< typename __type >
inline __attribute__((always_inline)) __type __sign__( __type _A )
{
    return _A >= 0 ? (__type)1.0 : -(__type)1.0;
}

template< typename __type >
inline __attribute__((always_inline)) void __swap__( __type& _A, __type& _B )
{
    __type c = _B;
    _B = _A;
    _A = c;
}

inline double __to_degrees__(double _Angle)
{
    return _Angle * __TO_DEGREES_CONVERSION_MULTIPLYER__;
}

inline double __to_radians__(double _Angle)
{
    return _Angle * __TO_RADIANS_CONVERSION_MULTIPLYER__;
}

template<typename __type> __type
__saturation__(__type _Input, __type _UpperLimit, __type _LowerLimit)
{
    if( _Input > _UpperLimit )
        return _UpperLimit;

    if( _Input < _LowerLimit )
        return _LowerLimit;

    return _Input;
}

template< typename __type > inline __type*
__alloc__(int _Size)
{
    if(_Size <= 0)
        return nullptr;

    return (__type*)calloc( _Size, sizeof(__type) );
}

template< typename __type > inline __type*
__alloc__(int _Size, __type _Value)
{
    if( _Size <= 0 )
        return nullptr;

    __type *memory = (__type*)calloc( _Size, sizeof(__type) );

    for(int i = 0 ; i < _Size; i++ )
        memory[i] = _Value;

    return memory;
}

template< typename __type > inline __type*
__alloc__( int _Size, void (*__initializer__)( __type* _Memory, int _Size ) )
{
    if(_Size <= 0)
        return nullptr;

    __type *memory = (__type*)calloc( _Size, sizeof(__type) );

    if(__initializer__ && memory)
        __initializer__(memory, _Size);

    return memory;
}

template< typename __type > inline __type*
__realloc__( __type* _Memory, int _Newsize )
{
    if(_Newsize <= 0 && _Memory)
    {
        free(_Memory);
        return nullptr;
    }

    __type *data = (__type*)realloc(_Memory, sizeof (__type)*_Newsize );

    if(!data && _Memory)
        free(_Memory);

    return data;
}

template< typename __type > inline __type*
__mfree__( __type* _Memory )
{
    if(_Memory)
    {
        free(_Memory);
        _Memory = nullptr;
    }

    return _Memory;
}

inline std::chrono::high_resolution_clock::time_point tic()
{
    return std::chrono::high_resolution_clock::now();
}

template< typename __type = std::chrono::nanoseconds >
double elapsed( std::chrono::high_resolution_clock::time_point _Then, std::chrono::high_resolution_clock::time_point _Now )
{
    return (double)std::chrono::duration_cast<__type>(_Now - _Then).count();
}

#if defined(_STRINGFWD_H) || defined(_LIBCPP_IOSFWD)

#define DEBUGGER

    namespace STRING_EXTENSION
    {
        // utility functions
        inline std::string __remove_prefix__( std::string _Input, std::string _Prefix )
        {
            size_t j = 0;
            for( size_t i = 0 ; i < _Prefix.size() ; i++ )
            {
                if( _Input[i] != _Prefix[i] )
                {
                    break;
                }

                j++;
            }

            std::string output;

            for( size_t i = j ; i < _Input.size() ; i++ )
            {
                output += _Input[i];
            }

            return output;
        }

        inline std::string __format_camel_style_string__( std::string _Input )
        {
            std::string output;
            for( size_t i = 0 ; i < _Input.size() ; i++ )
            {
                output += _Input[i];

                if( i + 1 < _Input.size() && std::isupper( _Input[i+1] ) )
                    output += ' ';
            }

            return output;
        }

        inline std::string __class_field_to_string__( std::string _Input )
        {
            return __format_camel_style_string__( __remove_prefix__( _Input, "m_" ) );
        }

        inline std::vector< std::string > __split__( std::string _Input, std::string _Delimeter = " " )
        {
            if( _Input.empty() )
                return std::vector< std::string >();

            int start  = 0;
            int end    = 0;
            int size   = _Delimeter.size();
            std::vector< std::string > output;

            while ( end >= 0 )
            {
                end = _Input.find(_Delimeter, start);
                output.push_back( _Input.substr(start, end-start) );
                start = end + size;
            }

            return output;
        }

        inline bool __string_contains__( std::string _Input, char _Delimeter = ' ')
        {
            for( size_t i = 0 ; i < _Input.size() ; i++ )
            {
                if( _Input[i] == _Delimeter )
                    return true;
            }

            return false;
        }

        inline std::string __to_upper__( std::string _String )
        {
            std::transform(_String.begin(), _String.end(), _String.begin(), ::toupper);
            return _String;
        }

        inline std::string __to_lower__( std::string _String )
        {
            std::transform(_String.begin(), _String.end(), _String.begin(), ::tolower);
            return _String;
        }

        inline std::string __remove_symbol__( std::string _String, char _Symbol )
        {
            std::string output;

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

        inline std::string __remove_symbols__( std::string _String, std::set< char > _Symbols )
        {
            std::string output;

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

        inline std::string __replace_all__( std::string _String, std::string _Substring, std::string _NewSubstring )
        {
            // reserve buffer for substring
            std::string buffer;
            buffer.reserve( _Substring.size() );

            // generate output string
            std::string output;
            output.reserve( _String.size() );

            // main code
            for( size_t i = 0 ; i < _String.size() ;  )
            {
                for( size_t j = 0, k = i ; j < _Substring.size() ; j++, k++ )
                {
                    buffer.push_back( _String[k] );
                }

                if( buffer == _Substring )
                {
                    output.append(_NewSubstring);
                    i += _Substring.size();
                }
                else
                {
                    output.push_back( _String[i] );
                    i++;
                }

                buffer.clear();
            }

            return output;
        }

        inline void __replace_symbol__( std::string& _Input, char _Symbol )
        {
            for( size_t i = 0 ; i < _Input.size() ; i++ )
            {
                if( _Input[i] == _Symbol )
                {
                    _Input[i] =  _Symbol;
                }
            }
        }

        // from string conversion
        template< typename __type >
        __type __from_string__( std::string _Input );

        template<> inline float __from_string__< float >( std::string _Input )
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

        template<> inline double __from_string__<double>( std::string _Input )
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

        template<> inline long double __from_string__<long double>( std::string _Input )
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

        template<> inline int __from_string__<int>( std::string _Input )
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

        template<> inline long long __from_string__<long long>( std::string _Input )
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

        template<> inline unsigned long __from_string__<unsigned long>( std::string _Input )
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

        template<> inline unsigned int __from_string__<unsigned int>( std::string _Input )
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

        template<> inline bool __from_string__<bool>( std::string _Input )
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

        template<> inline unsigned long long __from_string__<unsigned long long>( std::string _Input )
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

        template<> inline std::string __from_string__< std::string >( std::string _Input )
        {
            return _Input;
        }

        template<> inline char __from_string__<char>( std::string _Input )
        {
            return _Input.empty() ? ' ' : _Input[0];
        }

        // to string conversion
        template< typename __type >
        std::string __to_string__( __type _Input );

        template<> inline std::string __to_string__<bool>( bool _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__< float >( float _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__<double>( double _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__<long double>( long double _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__<int>( int _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__<long long>( long long _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__<unsigned long>( unsigned long _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__<unsigned long long>( unsigned long long _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__< std::string >( std::string _Input )
        {
            return _Input;
        }
    }

    class Debugger
    {
    protected:

        static std::string GetCurrentTime()
        {
            time_t _time = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
            std::string output = ctime(&_time);
            return output.substr(0, output.length()-1);
        }

    public:

        static std::string MessageFormat( std::string _Typename, std::string _OjectName, std::string _Message)
        {
            return "[ " + GetCurrentTime() + " ] " + "[ " + _Typename + " ] " + "[ " + _OjectName + " ] " + _Message;
        }

        static std::string MessageFormat( std::string _OjectName, std::string _Message)
        {
            return "[ " + GetCurrentTime() + " ] " + "[ " + _OjectName + " ] " + _Message;
        }

        static std::string MessageFormat( std::string _Message)
        {
            return "[ " + GetCurrentTime() + " ] " + _Message;
        }

        // logging functions
        static void Log( std::string _Typename, std::string _OjectName, std::string _Message )
        {
            std::cout << MessageFormat(_Typename, _OjectName,  _Message) + "\n";
        }

        static void LogError( std::string _Typename, std::string _OjectName, std::string _Message )
        {
            std::cerr << MessageFormat(_Typename, _OjectName,  _Message) + "\n";
        }
    };

#endif

/*! @} */

#endif // UTILS_H
