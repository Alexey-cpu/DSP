/*!
 * \file
 * \brief   Buffers
 * \authors A.Tykvinskiy
 * \date    21.12.2021
 * \version 1.0
 *
 * The header declares ring buffer and mirror ring buffer template classes
*/

#ifndef BUFFER_H
#define BUFFER_H

// identify if the compilation is for ProsoftSystems IDE
#ifndef __ALG_PLATFORM
#include "malloc.h"
#include <iostream>
#endif

/*! \defgroup <BUFFERS> ( Buffers )
 *  \brief the module contains mirror ring buffer and classic ring buffer template classes
    @{
*/

/*! \brief defines 16-bit integer type */
#ifndef __ix16
#define __ix16 short
#endif

/*! \brief defines 32-bit integer type */
#ifndef __ix32
#define __ix32 int
#endif

/*! \brief defines 64-bit integer type */
#ifndef __ix64
#define __ix64 long long
#endif

/*! \brief defines unsigned 16-bit integer type */
#ifndef __uix16
#define __uix16 unsigned short
#endif

/*! \brief defines unsigned 32-bit integer type */
#ifndef __uix32
#define __uix32 unsigned int
#endif

/*! \brief defines unsigned 64-bit integer type */
#ifndef __uix64
#define __uix64 unsigned long long
#endif

/*! \brief defines 32-bit floating point type */
#ifndef __fx32
#define __fx32 float
#endif

/*! \brief defines 64-bit floating point type */
#ifndef __fx64
#define __fx64 double
#endif

/*! \brief mirror ring buffer template class */
template< typename T > class mirror_ring_buffer;

/*! \brief classic ring buffer template class */
template< typename T > class ring_buffer;

/*! \brief 32-bit floating point mirror ring buffer */
template<> class mirror_ring_buffer< __fx32 >
{
    typedef __fx32 __type;
public:
    /*! \brief mirror ring buffer upper half pointer */
    __type *m_upper;
    /*! \brief mirror ring buffer lower half pointer */
    __type *m_lower;
    /*! \brief mirror ring buffer data pointer */
    __type *m_data;
     /*! \brief mirror ring buffer size */
    __ix32  m_nelem;
     /*! \brief mirror ring buffer position */
    __ix32  m_buffpos;

    /*! \brief mirror ring buffer memory allocation function
     *  \param[nelem] the mirror ring buffer size
    */
    __ix32 allocate( __ix32 nelem )
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

    /*! \brief mirror ring buffer memory deallocation function */
    void deallocate()
    {
        if( m_data != 0 )
        {
            free( m_data );
            m_data = 0;
        }
    }

    /*! \brief mirror ring buffer default constructor */
    mirror_ring_buffer()
    {
        m_upper   = 0;
        m_lower   = 0;
        m_data    = 0;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    /*! \brief mirror ring buffer destructor */
    ~mirror_ring_buffer() { deallocate(); }

    // biffer filling function:

    /*! \brief mirror ring buffer filling function
     * \param[input] pointer to the input data
    */
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

    /*! \brief mirror ring buffer filling function
     * \param[input] pointer to the input data
    */
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

    /*! \brief mirror ring buffer [] operator
     * \param[n] sample number
     * \return the operator call returns a sample that is on the left from the current
     *         mirror ring buffer position
    */
    inline __type operator [] ( __ix32 n ) { return *( m_upper - n - 1 ); }

    /*! \brief mirror ring buffer () operator
     * \param[input] pointer to the input data
     * \return the operator call puts the data into the buffer by
     *         means of calling fill_buff( __type *input ) function
    */
    inline void operator () ( __type *input ){ fill_buff( input ); }

    /*! \brief mirror ring buffer () operator
     * \param[input] pointer to the input data
     * \return the operator call puts the data into the buffer by
     *         means of calling fill_buff( __type *input ) function
    */
    inline void operator () ( __fx64 *input ){ fill_buff( input ); }
};

/*! \brief 64-bit floating point mirror ring buffer */
template<> class mirror_ring_buffer< __fx64 >
{
    typedef __fx64 __type;
public:
    /*! \brief mirror ring buffer upper half pointer */
    __type *m_upper;
    /*! \brief mirror ring buffer lower half pointer */
    __type *m_lower;
    /*! \brief mirror ring buffer data pointer */
    __type *m_data;
     /*! \brief mirror ring buffer size */
    __ix32  m_nelem;
     /*! \brief mirror ring buffer position */
    __ix32  m_buffpos;

    /*! \brief mirror ring buffer memory allocation function
     *  \param[nelem] the mirror ring buffer size
    */
    __ix32 allocate( __ix32 nelem )
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

    /*! \brief mirror ring buffer memory deallocation function */
    void deallocate()
    {
        if( m_data != 0 )
        {
            free( m_data );
            m_data = 0;
        }
    }

    /*! \brief mirror ring buffer default constructor */
    mirror_ring_buffer()
    {
        m_upper   = 0;
        m_lower   = 0;
        m_data    = 0;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    /*! \brief mirror ring buffer destructor */
    ~mirror_ring_buffer() { deallocate(); }

    // biffer filling function:

    /*! \brief mirror ring buffer filling function
     * \param[input] pointer to the input data
    */
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

    /*! \brief mirror ring buffer [] operator
     * \param[n] sample number
     * \return the operator call returns a sample that is on the left from the current
     *         mirror ring buffer position
    */
    inline __type operator [] ( __ix32 n ) { return *( m_upper - n - 1 ); }

    /*! \brief mirror ring buffer () operator
     * \param[input] pointer to the input data
     * \return the operator call puts the data into the buffer by
     *         means of calling fill_buff( __type *input ) function
    */
    inline void operator () ( __type *input ){ fill_buff( input ); }
};

/*! \brief 32-bit integer mirror ring buffer */
template<> class mirror_ring_buffer< __ix32 >
{
    typedef __ix32 __type;
public:
    /*! \brief mirror ring buffer upper half pointer */
    __type *m_upper;
    /*! \brief mirror ring buffer lower half pointer */
    __type *m_lower;
    /*! \brief mirror ring buffer data pointer */
    __type *m_data;
     /*! \brief mirror ring buffer size */
    __ix32  m_nelem;
     /*! \brief mirror ring buffer position */
    __ix32  m_buffpos;

    /*! \brief mirror ring buffer memory allocation function
     *  \param[nelem] the mirror ring buffer size
    */
    __ix32 allocate( __ix32 nelem )
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

    /*! \brief mirror ring buffer memory deallocation function */
    void deallocate()
    {
        if( m_data != 0 )
        {
            free( m_data );
            m_data = 0;
        }
    }

    /*! \brief mirror ring buffer default constructor */
    mirror_ring_buffer()
    {
        m_upper   = 0;
        m_lower   = 0;
        m_data    = 0;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    /*! \brief mirror ring buffer destructor */
    ~mirror_ring_buffer() { deallocate(); }

    // biffer filling function:

    /*! \brief mirror ring buffer filling function
     * \param[input] pointer to the input data
    */
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

    /*! \brief mirror ring buffer filling function
     * \param[input] pointer to the input data
    */
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

    /*! \brief mirror ring buffer [] operator
     * \param[n] sample number
     * \return the operator call returns a sample that is on the left from the current
     *         mirror ring buffer position
    */
    inline __type operator [] ( __ix32 n ) { return *( m_upper - n - 1 ); }

    /*! \brief mirror ring buffer () operator
     * \param[input] pointer to the input data
     * \return the operator call puts the data into the buffer by
     *         means of calling fill_buff( __type *input ) function
    */
    inline void operator () ( __type *input ){ fill_buff( input ); }

    /*! \brief mirror ring buffer () operator
     * \param[input] pointer to the input data
     * \return the operator call puts the data into the buffer by
     *         means of calling fill_buff( __type *input ) function
    */
    inline void operator () ( __ix64 *input ){ fill_buff( input ); }
};

/*! \brief 32-bit integer mirror ring buffer */
template<> class mirror_ring_buffer< __ix64 >
{
    typedef __ix64 __type;
public:
    /*! \brief mirror ring buffer upper half pointer */
    __type *m_upper;
    /*! \brief mirror ring buffer lower half pointer */
    __type *m_lower;
    /*! \brief mirror ring buffer data pointer */
    __type *m_data;
     /*! \brief mirror ring buffer size */
    __ix32  m_nelem;
     /*! \brief mirror ring buffer position */
    __ix32  m_buffpos;

    /*! \brief mirror ring buffer memory allocation function
     *  \param[nelem] the mirror ring buffer size
    */
    __ix32 allocate( __ix32 nelem )
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

    /*! \brief mirror ring buffer memory deallocation function */
    void deallocate()
    {
        if( m_data != 0 )
        {
            free( m_data );
            m_data = 0;
        }
    }

    /*! \brief mirror ring buffer default constructor */
    mirror_ring_buffer()
    {
        m_upper   = 0;
        m_lower   = 0;
        m_data    = 0;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    /*! \brief mirror ring buffer destructor */
    ~mirror_ring_buffer() { deallocate(); }

    // biffer filling function:

    /*! \brief mirror ring buffer filling function
     * \param[input] pointer to the input data
    */
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

    /*! \brief mirror ring buffer [] operator
     * \param[n] sample number
     * \return the operator call returns a sample that is on the left from the current
     *         mirror ring buffer position
    */
    inline __type operator [] ( __ix32 n ) { return *( m_upper - n - 1 ); }

    /*! \brief mirror ring buffer () operator
     * \param[input] pointer to the input data
     * \return the operator call puts the data into the buffer by
     *         means of calling fill_buff( __type *input ) function
    */
    inline void operator () ( __type *input ){ fill_buff( input ); }
};

/*! @} */

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
