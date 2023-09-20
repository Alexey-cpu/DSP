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

// STL containers
#include "vector"
#include "list"
#include "map"
#include <set>

// STL input/output streams
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

// Qt includes
#include <QElapsedTimer>

#endif

// define custom types
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
    struct Tuple<>{};

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

#ifdef _STDINT_H

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

#endif

#ifdef _GLIBCXX_NUMERIC_LIMITS

    // Fortran analogues functions
    template< typename __type > __type __digits__()  { return numeric_limits<__type>::digits; }

    template< typename __type > __type __epsilon__() { return numeric_limits<__type>::epsilon(); }

    template< typename __type > __type __huge__() { return numeric_limits<__type>::max(); }

    template< typename __type > __type __maxexponent__() { return numeric_limits<__type>::max_exponent; }

    template< typename __type > __type __minexponent__() { return numeric_limits<__type>::min_exponent; }

    template< typename __type > __type __radix__() { return numeric_limits<__type>::radix; }

    template< typename __type > __type __tiny__() { return numeric_limits<__type>::min(); }

#endif

template< typename Head >
Head __max__( Head _A, Head _B )
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

    for( int i = 1 ; i < _Size; i++ )
    {
        if( _Input[i] > vmax )
            vmax = _Input[i];
    }

    return vmax;
}

template< typename Head >
Head __min__( Head _A, Head _B )
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
inline __type __abs__ ( __type _A )
{
    return ( _A < 0 ) ? -_A : _A;
}

template< typename __type >
inline __type __sign__( __type _A, __type _B )
{
    return _A * ( ( _B > 0 ) ? (__type)1 : -(__type)1 );
}

template< typename __type >
inline void __swap__( __type& _A, __type& _B )
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

        inline std::vector< string > __split__( string _Input, string _Delimeter = " " )
        {
            if( _Input.empty() )
                return std::vector< string >();

            int start  = 0;
            int end    = 0;
            int size   = _Delimeter.size();
            std::vector< string > output;

            while ( end >= 0 )
            {
                end = _Input.find(_Delimeter, start);
                output.push_back( _Input.substr(start, end-start) );
                start = end + size;
            }

            return output;
        }

        inline bool __string_contains__(string _Input, char _Delimeter = ' ')
        {
            for( size_t i = 0 ; i < _Input.size() ; i++ )
            {
                if( _Input[i] == _Delimeter )
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
    protected:

        static string GetCurrentTime()
        {
            time_t _time = chrono::system_clock::to_time_t(chrono::system_clock::now());
            string output = ctime(&_time);
            return output.substr(0, output.length()-1);
        }

    public:

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

#endif // UTILS_H
