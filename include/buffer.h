//--------------------------------------------------------------------------------
//A.M.Tykvinsky, 21.12.2021
//--------------------------------------------------------------------------------
// TEMPLATE BUFFER CLASS
//--------------------------------------------------------------------------------

#ifndef BUFFER_H
#define BUFFER_H

// identify if the compilation is for ProsoftSystems IDE
#ifndef __ALG_PLATFORM
#include "malloc.h"
#include <iostream>
#endif

// customized C data types definition:
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

// ring buffer and mirror ring buffer class definition:
template< typename T > class mirror_ring_buffer;
template< typename T > class ring_buffer;

// template mirror_ring_buffer class realization for the different types:

// float 32 bit:
template<> class mirror_ring_buffer< __fx32 >
{
    typedef __fx32 __type;
public:
    __type *m_upper;
    __type *m_lower;
    __type *m_data;
    __ix32  m_nelem;
    __ix32  m_buffpos;

    // memory allocation function:
    int allocate( int nelem )
    {
        if( ( nelem > 0 ) && !m_data )
        {
            m_nelem = nelem;
            m_data = ( __type* ) calloc( ( 2 * m_nelem ) , sizeof ( __type ) );
            if( m_data )
            {
               m_lower = &m_data[0];
               m_upper = &m_data[m_nelem];
            }

            return ( m_data != 0 );
        }
        else return 0;
    }

    // memory deallocation function:
    void deallocate()
    {
        if( m_data != 0 )
        {
            free( m_data );
            m_data = 0;
        }
    }

    // default constructor:
    mirror_ring_buffer()
    {
        m_upper   = 0;
        m_lower   = 0;
        m_data    = 0;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    // default destructor:
    ~mirror_ring_buffer() { deallocate(); }

    // biffer filling function:

    // float x32 input:
    inline void fill_buff( __type *input )
    {
        *m_lower = *input;
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_lower = &m_data[0];
            m_upper = &m_data[m_nelem];
            m_buffpos = 0;
        }
        else
        {
            m_lower++;
            m_upper++;

        }
    }

    // float x64 input ( caution !!! The rounding error occurs !!! )
    inline void fill_buff( __fx64 *input )
    {
        *m_lower = *input;
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_lower = &m_data[0];
            m_upper = &m_data[m_nelem];
            m_buffpos = 0;
        }
        else
        {
            m_lower++;
            m_upper++;

        }
    }

    // operators:

    // get the value:
    inline __type operator [] ( __ix32 n ) { return *( m_upper - n - 1 ); }

    // put the value:
    inline void operator () ( __type *input ){ fill_buff( input ); }
    inline void operator () ( __fx64 *input ){ fill_buff( input ); }
};

// float 64 bit:
template<> class mirror_ring_buffer< __fx64 >
{
    typedef __fx64 __type;
public:
    __type *m_upper;
    __type *m_lower;
    __type *m_data;
    __ix32  m_nelem;
    __ix32  m_buffpos;

    // memory allocation function:
    int allocate( int nelem )
    {
        if( ( nelem > 0 ) && !m_data )
        {
            m_nelem = nelem;
            m_data = ( __type* ) calloc( ( 2 * m_nelem ) , sizeof ( __type ) );
            if( m_data )
            {
               m_lower = &m_data[0];
               m_upper = &m_data[m_nelem];
            }

            return ( m_data != 0 );
        }
        else return 0;
    }

    // memory deallocation function:
    void deallocate()
    {
        if( m_data != 0 )
        {
            free( m_data );
            m_data = 0;
        }
    }

    // default constructor:
    mirror_ring_buffer()
    {
        m_upper   = 0;
        m_lower   = 0;
        m_data    = 0;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    // default destructor:
    ~mirror_ring_buffer() { deallocate(); }

    // biffer filling function:
    inline void fill_buff( __type *input )
    {
        *m_lower = *input;
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_lower = &m_data[0];
            m_upper = &m_data[m_nelem];
            m_buffpos = 0;
        }
        else
        {
            m_lower++;
            m_upper++;

        }
    }

    // operators:

    // get the value:
    inline __type operator [] ( __ix32 n ) { return *( m_upper - n - 1 ); }

    // put the value:
    inline void operator () ( __type *input ){ fill_buff( input ); }
};

// int 16 bit:
template<> class mirror_ring_buffer< __ix16 >
{
    typedef __ix16 __type;
public:
    __type *m_upper;
    __type *m_lower;
    __type *m_data;
    __ix32  m_nelem;
    __ix32  m_buffpos;

    // memory allocation function:
    __ix32 allocate( int nelem )
    {
        if( ( nelem > 0 ) && !m_data )
        {
            m_nelem = nelem;
            m_data = ( __type* ) calloc( ( 2 * m_nelem ) , sizeof ( __type ) );
            if( m_data )
            {
               m_lower = &m_data[0];
               m_upper = &m_data[m_nelem];
            }

            return ( m_data != 0 );
        }
        else return 0;
    }

    // memory deallocation function:
    void deallocate()
    {
        if( m_data != 0 )
        {
            free( m_data );
            m_data = 0;
        }
    }

    // default constructor:
    mirror_ring_buffer()
    {
        m_upper   = 0;
        m_lower   = 0;
        m_data    = 0;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    // default destructor:
    ~mirror_ring_buffer() { deallocate(); }

    // biffer filling function:

    // int x16 input:
    inline void fill_buff( __type *input )
    {
        *m_lower = *input;
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_lower = &m_data[0];
            m_upper = &m_data[m_nelem];
            m_buffpos = 0;
        }
        else
        {
            m_lower++;
            m_upper++;

        }
    }

    // int x32 input ( caution !!! The rounding error occurs !!! )
    inline void fill_buff( __ix32 *input )
    {
        *m_lower = *input;
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_lower = &m_data[0];
            m_upper = &m_data[m_nelem];
            m_buffpos = 0;
        }
        else
        {
            m_lower++;
            m_upper++;

        }
    }

    // int x64 input ( caution !!! The rounding error occurs !!! )
    inline void fill_buff( __ix64 *input )
    {
        *m_lower = *input;
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_lower = &m_data[0];
            m_upper = &m_data[m_nelem];
            m_buffpos = 0;
        }
        else
        {
            m_lower++;
            m_upper++;

        }
    }

    // operators:

    // get the value:
    inline __type operator [] ( __ix32 n ) { return *( m_upper - n - 1 ); }

    // put the value:
    inline void operator () ( __type *input ){ fill_buff( input ); }
    inline void operator () ( __ix32 *input ){ fill_buff( input ); }
    inline void operator () ( __ix64 *input ){ fill_buff( input ); }
};

// int 32 bit:
template<> class mirror_ring_buffer< __ix32 >
{
    typedef __ix32 __type;
public:
    __type *m_upper;
    __type *m_lower;
    __type *m_data;
    __ix32  m_nelem;
    __ix32  m_buffpos;

    // memory allocation function:
    int allocate( int nelem )
    {
        if( ( nelem > 0 ) && !m_data )
        {
            m_nelem = nelem;
            m_data = ( __type* ) calloc( ( 2 * m_nelem ) , sizeof ( __type ) );
            if( m_data )
            {
               m_lower = &m_data[0];
               m_upper = &m_data[m_nelem];
            }

            return ( m_data != 0 );
        }
        else return 0;
    }

    // memory deallocation function:
    void deallocate()
    {
        if( m_data != 0 )
        {
            free( m_data );
            m_data = 0;
        }
    }

    // default constructor:
    mirror_ring_buffer()
    {
        m_upper   = 0;
        m_lower   = 0;
        m_data    = 0;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    // default destructor:
    ~mirror_ring_buffer() { deallocate(); }

    // biffer filling function:

    // int x32 input:
    inline void fill_buff( __type *input )
    {
        *m_lower = *input;
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_lower = &m_data[0];
            m_upper = &m_data[m_nelem];
            m_buffpos = 0;
        }
        else
        {
            m_lower++;
            m_upper++;

        }
    }

    // int x64 input ( Caution !!! The rounding error occurs !!! )
    inline void fill_buff( __ix64 *input )
    {
        *m_lower = *input;
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_lower = &m_data[0];
            m_upper = &m_data[m_nelem];
            m_buffpos = 0;
        }
        else
        {
            m_lower++;
            m_upper++;
        }
    }

    // operators:

    // get the value:
    inline __type operator [] ( __ix32 n ) { return *( m_upper - n - 1 ); }

    // put the value:
    inline void operator () ( __type *input ){ fill_buff( input ); }
    inline void operator () ( __ix64 *input ){ fill_buff( input ); }
};

// int 64 bit:
template<> class mirror_ring_buffer< __ix64 >
{
    typedef __ix64 __type;
public:
    __type *m_upper;
    __type *m_lower;
    __type *m_data;
    __ix32  m_nelem;
    __ix32  m_buffpos;

    // memory allocation function:
    int allocate( int nelem )
    {
        if( ( nelem > 0 ) && !m_data )
        {
            m_nelem = nelem;
            m_data = ( __type* ) calloc( ( 2 * m_nelem ) , sizeof ( __type ) );
            if( m_data )
            {
               m_lower = &m_data[0];
               m_upper = &m_data[m_nelem];
            }

            return ( m_data != 0 );
        }
        else return 0;
    }

    // memory deallocation function:
    void deallocate()
    {
        if( m_data != 0 )
        {
            free( m_data );
            m_data = 0;
        }
    }

    // default constructor:
    mirror_ring_buffer()
    {
        m_upper   = 0;
        m_lower   = 0;
        m_data    = 0;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    // default destructor:
    ~mirror_ring_buffer() { deallocate(); }

    // biffer filling function:
    inline void fill_buff( __type *input )
    {
        *m_lower = *input;
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_lower = &m_data[0];
            m_upper = &m_data[m_nelem];
            m_buffpos = 0;
        }
        else
        {
            m_lower++;
            m_upper++;

        }
    }

    // operators:

    // get the value:
    inline __type operator [] ( __ix32 n ) { return *( m_upper - n - 1 ); }

    // put the value:
    inline void operator () ( __type *input ){ fill_buff( input ); }
};


// template buffer class realization for the different types:

// float 32 bit:
template<> class ring_buffer< __fx32 >
{
    typedef __fx32 __type;
    __type *m_upper;
    __type *m_data;
    __ix32  m_nelem;
    __ix32  m_buffpos;

public:

    // memory allocation function:
    int allocate( int nelem )
    {
        if( ( nelem > 0 ) && !m_data )
        {
            m_nelem = nelem;
            m_data = ( __type* ) calloc( ( m_nelem ) , sizeof ( __type ) );
            if( m_data )
            {
                m_upper = &m_data[0];
            }
            return ( m_data != 0 );
        }
        else return 0;
    }

    // memory deallocation function:
    void deallocate()
    {
        if( m_data != 0 )
        {
            delete [] m_data;
            m_data = 0;
        }
    }

    // default constructor:
    ring_buffer()
    {
        m_upper   = 0;
        m_data    = 0;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    // default destructor:
    ~ring_buffer() { deallocate(); }

    // biffer filling function:

    // float x32 input:
    inline void fill_buff( __type *input )
    {
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_upper = &m_data[0];
            m_buffpos = 0;
        }
        else
        {
            m_upper++;
        }
    }

    // float x64 input ( CAUTION !!! THE ROUNDING ERROR OCCURS !!! )
    inline void fill_buff( __fx64 *input )
    {
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_upper = &m_data[0];
            m_buffpos = 0;
        }
        else
        {
            m_upper++;
        }
    }

    // operators:

    // get the value:
    inline __type operator [] ( __ix32 n ) { return m_data[n]; }

    // put the value:
    inline void operator() ( __type *input ) { fill_buff( input ); }
    inline void operator() ( __fx64 *input ) { fill_buff( input ); }
};

// float 64 bit:
template<> class ring_buffer< __fx64 >
{
    typedef __fx64 __type;
    __type *m_upper;
    __type *m_data;
    __ix32  m_nelem;
    __ix32  m_buffpos;

public:

    // memory allocation function:
    int allocate( int nelem )
    {
        if( ( nelem > 0 ) && !m_data )
        {
            m_nelem = nelem;
            m_data = ( __type* ) calloc( ( m_nelem ) , sizeof ( __type ) );
            if( m_data )
            {
                m_upper = &m_data[0];
            }
            return ( m_data != 0 );
        }
        else return 0;
    }

    // memory deallocation function:
    void deallocate()
    {
        if( m_data != 0 )
        {
            free( m_data );
            m_data = 0;
        }
    }

    // default constructor:
    ring_buffer()
    {
        m_upper   = 0;
        m_data    = 0;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    // default destructor:
    ~ring_buffer() { deallocate(); }

    // biffer filling function:
    inline void fill_buff( __type *input )
    {
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_upper = &m_data[0];
            m_buffpos = 0;
        }
        else
        {
            m_upper++;
        }
    }

    // operators:

    // get the value:
    inline __type operator [] ( __ix32 n ) { return m_data[n]; }

    // put the value:
    inline void operator() ( __type *input ) { fill_buff( input ); }
};

// int 16 bit:
template<> class ring_buffer< __ix16 >
{
    typedef __ix16 __type;
    __type *m_upper;
    __type *m_data;
    __ix32  m_nelem;
    __ix32  m_buffpos;

public:

    // memory allocation function:
    int allocate( int nelem )
    {
        if( ( nelem > 0 ) && !m_data )
        {
            m_nelem = nelem;
            m_data = ( __type* ) calloc( ( m_nelem ) , sizeof ( __type ) );
            if( m_data )
            {
                m_upper = &m_data[0];
            }
            return ( m_data != 0 );
        }
        else return 0;
    }

    // memory deallocation function:
    void deallocate()
    {
        if( m_data != 0 )
        {
            free( m_data );
            m_data = 0;
        }
    }

    // default constructor:
    ring_buffer()
    {
        m_upper   = 0;
        m_data    = 0;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    // default destructor:
    ~ring_buffer() { deallocate(); }

    // biffer filling function:
    inline void fill_buff( __type *input )
    {
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_upper = &m_data[0];
            m_buffpos = 0;
        }
        else
        {
            m_upper++;
        }
    }

    // int x32 input ( CAUTION !!! THE ROUNDING ERROR OCCURS !!! )
    inline void fill_buff( __ix32 *input )
    {
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_upper = &m_data[0];
            m_buffpos = 0;
        }
        else
        {
            m_upper++;
        }
    }

    // int x64 input ( CAUTION !!! THE ROUNDING ERROR OCCURS !!! )
    inline void fill_buff( __ix64 *input )
    {
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_upper = &m_data[0];
            m_buffpos = 0;
        }
        else
        {
            m_upper++;
        }
    }

    // operators:

    // get the value:
    inline __type operator [] ( __ix32 n ) { return m_data[n]; }

    // put the value:
    inline void operator() ( __type *input ) { fill_buff( input ); }
    inline void operator() ( __ix32 *input ) { fill_buff( input ); }
    inline void operator() ( __ix64 *input ) { fill_buff( input ); }
};

// int 32 bit:
template<> class ring_buffer< __ix32 >
{
    typedef __ix32 __type;
    __type *m_upper;
    __type *m_data;
    __ix32  m_nelem;
    __ix32  m_buffpos;

public:

    // memory allocation function:
    int allocate( int nelem )
    {
        if( ( nelem > 0 ) && !m_data )
        {
            m_nelem = nelem;
            m_data = ( __type* ) calloc( ( m_nelem ) , sizeof ( __type ) );
            if( m_data )
            {
                m_upper = &m_data[0];
            }
            return ( m_data != 0 );
        }
        else return 0;
    }

    // memory deallocation function:
    void deallocate()
    {
        if( m_data != 0 )
        {
            free( m_data );
            m_data = 0;
        }
    }

    // default constructor:
    ring_buffer()
    {
        m_upper   = 0;
        m_data    = 0;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    // default destructor:
    ~ring_buffer() { deallocate(); }

    // biffer filling function:
    inline void fill_buff( __type *input )
    {
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_upper = &m_data[0];
            m_buffpos = 0;
        }
        else
        {
            m_upper++;
        }
    }

    // int x64 input ( CAUTION !!! THE ROUNDING ERROR OCCURS !!! )
    inline void fill_buff( __ix64 *input )
    {
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_upper = &m_data[0];
            m_buffpos = 0;
        }
        else
        {
            m_upper++;
        }
    }

    // operators:

    // get the value:
    inline __type operator [] ( __ix32 n ) { return m_data[n]; }

    // put the value:
    inline void operator() ( __type *input ) { fill_buff( input ); }
    inline void operator() ( __ix64 *input ) { fill_buff( input ); }
};

// int 64 bit:
template<> class ring_buffer< __ix64 >
{
    typedef __ix64 __type;
    __type *m_upper;
    __type *m_data;
    __ix32  m_nelem;
    __ix32  m_buffpos;

public:

    // memory allocation function:
    int allocate( int nelem )
    {
        if( ( nelem > 0 ) && !m_data )
        {
            m_nelem = nelem;
            m_data = ( __type* ) calloc( ( m_nelem ) , sizeof ( __type ) );
            if( m_data )
            {
                m_upper = &m_data[0];
            }
            return ( m_data != 0 );
        }
        else return 0;
    }

    // memory deallocation function:
    void deallocate()
    {
        if( m_data != 0 )
        {
            free( m_data );
            m_data = 0;
        }
    }

    // default constructor:
    ring_buffer()
    {
        m_upper   = 0;
        m_data    = 0;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    // default destructor:
    ~ring_buffer() { deallocate(); }

    // biffer filling function:
    inline void fill_buff( __type *input )
    {
        *m_upper = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_upper = &m_data[0];
            m_buffpos = 0;
        }
        else
        {
            m_upper++;
        }
    }

    // operators:

    // get the value:
    inline __type operator [] ( __ix32 n ) { return m_data[n]; }

    // put the value:
    inline void operator() ( __type *input ) { fill_buff( input ); }
};

// customized C data typed exclusion to avoid aliases:
#undef __ix16
#undef __ix32
#undef __ix64
#undef __uix16
#undef __uix32
#undef __uix64
#undef __fx32
#undef __fx64

#endif // BUFFER_H
