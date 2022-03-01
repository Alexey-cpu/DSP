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
#include <stdlib.h>
//#include "malloc.h"
#include <iostream>
#endif

/*! \defgroup <BUFFERS> ( Buffers )
 *  \brief the module contains mirror ring buffer and classic ring buffer template classes
    @{
*/

#include "fcomplex.h"

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

/*! \brief defines extended 64-bit floating point type */
#ifndef __fxx64
#define __fxx64 long double
#endif

/*! \brief mirror ring buffer template abstract class */
template< typename T > class mirror_ring_buffer_abstract
{
    typedef T __type;
protected:
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

    /*! \brief mirror ring buffer template filling function
     * \param[input] pointer to the input data
    */
    template< typename F > inline void fill_buff( F *input )
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

public:


    /*! \brief mirror ring buffer memory allocation function
     *  \param[nelem] the mirror ring buffer size
    */
    __ix32 allocate( __ix32 nelem )
    {
        if( ( nelem > 0 ) && !m_data )
        {
            m_nelem = nelem;
            m_data = new __type[ 2 * m_nelem ];
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
            delete [] m_data;
            m_data = 0;
        }
    }

    /*! \brief default constructor */
    mirror_ring_buffer_abstract()
    {
        m_upper   = 0;
        m_lower   = 0;
        m_data    = 0;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    /*! \brief virtual destructor */
    virtual ~mirror_ring_buffer_abstract(){ deallocate(); }

    /*!
     *  \brief  mirror ring buffer position function
     *  \return The function returns the buffer filling pointer position
    */
    inline __ix32 getBuffPos() { return m_buffpos; }

    /*!
     *  \brief  mirror ring buffer size function
     *  \return The function returns the buffer size
    */
    inline __ix32 getBuffSize(){ return m_nelem;   }

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
    virtual inline void operator () ( __type *input ) = 0;
};

/*! \brief ring buffer template abstract class */
template< typename T > class ring_buffer_abstract
{
    typedef T __type;
protected:
    /*! \brief mirror ring buffer lower half pointer */
    __type *m_lower;
    /*! \brief mirror ring buffer data pointer */
    __type *m_data;
     /*! \brief mirror ring buffer size */
    __ix32  m_nelem;
     /*! \brief mirror ring buffer position */
    __ix32  m_buffpos;

    /*!
     *  \brief mirror ring buffer template filling function
     * \param[input] pointer to the input data
    */
    template< typename F > inline void fill_buff( F *input )
    {
        *m_lower = *input;
        if( ++m_buffpos >= m_nelem )
        {
            m_lower   = &m_data[0];
            m_buffpos = 0;
        }
        else m_lower++;
    }

public:

    /*! \brief mirror ring buffer memory allocation function
     *  \param[nelem] the mirror ring buffer size
    */
    __ix32 allocate( __ix32 nelem )
    {
        if( ( nelem > 0 ) && !m_data )
        {
            m_nelem = nelem;
            m_data  = ( __type* ) calloc( ( m_nelem ) , sizeof ( __type ) );
            m_lower = ( m_data ) ? &m_data[0] : 0;
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
    ring_buffer_abstract()
    {
        m_lower   = 0;
        m_data    = 0;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    /*! \brief mirror ring buffer destructor */
    virtual ~ring_buffer_abstract() { deallocate(); }

    /*!
     *  \brief  mirror ring buffer position function
     *  \return The function returns the buffer filling pointer position
    */
    inline __ix32 getBuffPos() { return m_buffpos; }

    /*!
     *  \brief  mirror ring buffer size function
     *  \return The function returns the buffer size
    */
    inline __ix32 getBuffSize(){ return m_nelem;   }

    /*! \brief mirror ring buffer [] operator
     * \param[n] sample number
     * \return the operator call returns a sample that is on the left from the current
     *         mirror ring buffer position
    */
    inline __type operator [] ( __ix32 n ) { return m_data[n]; }

    /*! \brief mirror ring buffer () operator
     * \param[input] pointer to the input data
     * \return the operator call puts the data into the buffer by
     *         means of calling fill_buff( __type *input ) function
    */
    virtual inline void operator () ( __type *input ) = 0;
};

/*! \brief mirror ring buffer template class */
template< typename T > class mirror_ring_buffer;

/*! \brief classic ring buffer template class */
template< typename T > class ring_buffer;

/*! \brief mirror ring buffer class floating point 32-bit realization */
template<> class mirror_ring_buffer< __fx32 > : public mirror_ring_buffer_abstract< __fx32 >
{
    typedef __fx32 __type;
public:
      mirror_ring_buffer() : mirror_ring_buffer_abstract(){}
     ~mirror_ring_buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( __fx64  *input ) { fill_buff< __fx64 >( input ); }
     inline void operator () ( __fxx64 *input ) { fill_buff< __fxx64 >( input ); }
};

/*! \brief mirror ring buffer class floating point 64-bit realization */
template<> class mirror_ring_buffer< __fx64 > : public mirror_ring_buffer_abstract< __fx64 >
{
    typedef __fx64 __type;
public:
     mirror_ring_buffer() : mirror_ring_buffer_abstract(){}
    ~mirror_ring_buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( __fxx64 *input ) { fill_buff< __fxx64 >( input ); }
};

/*! \brief mirror ring buffer class floating point extended 64-bit realization */
template<> class mirror_ring_buffer< __fxx64 > : public mirror_ring_buffer_abstract< __fxx64 >
{
    typedef __fxx64 __type;
public:
     mirror_ring_buffer() : mirror_ring_buffer_abstract(){}
    ~mirror_ring_buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
};

#ifdef FCOMPLEX_H
/*! \brief mirror ring buffer class complex floating point 32-bit realization */
template<> class mirror_ring_buffer< complex< __fx32 > > : public mirror_ring_buffer_abstract< complex< __fx32 > >
{
    typedef complex< __fx32 > __type;
public:
      mirror_ring_buffer() : mirror_ring_buffer_abstract(){}
     ~mirror_ring_buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( complex< __fx64  > *input ) { fill_buff< complex< __fx64  > >( input ); }
     inline void operator () ( complex< __fxx64 > *input ) { fill_buff< complex< __fxx64  > >( input ); }
};

/*! \brief mirror ring buffer class complex floating point 64-bit realization */
template<> class mirror_ring_buffer< complex< __fx64 > > : public mirror_ring_buffer_abstract< complex< __fx64 > >
{
    typedef complex< __fx64 > __type;
public:
      mirror_ring_buffer() : mirror_ring_buffer_abstract(){}
     ~mirror_ring_buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( complex< __fxx64 > *input ) { fill_buff< complex< __fxx64  > >( input ); }
};

/*! \brief mirror ring buffer class complex floating point extended 64-bit realization */
template<> class mirror_ring_buffer< complex< __fxx64 > > : public mirror_ring_buffer_abstract< complex< __fxx64 > >
{
    typedef complex< __fxx64 > __type;
public:
      mirror_ring_buffer() : mirror_ring_buffer_abstract(){}
     ~mirror_ring_buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
};

#endif

/*! \brief mirror ring buffer class integer 32-bit realization */
template<> class mirror_ring_buffer< __ix32 > : public mirror_ring_buffer_abstract< __ix32 >
{
    typedef __ix32 __type;
public:
      mirror_ring_buffer() : mirror_ring_buffer_abstract(){}
     ~mirror_ring_buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( __ix64  *input ) { fill_buff< __ix64 >( input ); }
};

/*! \brief mirror ring buffer class integer 64-bit realization */
template<> class mirror_ring_buffer< __ix64 > : public mirror_ring_buffer_abstract< __ix64 >
{
    typedef __ix64 __type;
public:
      mirror_ring_buffer() : mirror_ring_buffer_abstract(){}
     ~mirror_ring_buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
};

/*! \brief ring buffer class floating point 32-bit realization */
template<> class ring_buffer< __fx32 > : public ring_buffer_abstract< __fx32 >
{
    typedef __fx32 __type;
public:
      ring_buffer() : ring_buffer_abstract(){}
     ~ring_buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( __fx64  *input ) { fill_buff< __fx64 >( input ); }
     inline void operator () ( __fxx64 *input ) { fill_buff< __fxx64 >( input ); }
};

/*! \brief ring buffer class floating point 64-bit realization */
template<> class ring_buffer< __fx64 > : public ring_buffer_abstract< __fx64 >
{
    typedef __fx64 __type;
public:
      ring_buffer() : ring_buffer_abstract(){}
     ~ring_buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( __fxx64 *input ) { fill_buff< __fxx64 >( input ); }
};

/*! \brief ring buffer class floating point extended 64-bit realization */
template<> class ring_buffer< __fxx64 > : public ring_buffer_abstract< __fxx64 >
{
    typedef __fxx64 __type;
public:
      ring_buffer() : ring_buffer_abstract(){}
     ~ring_buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
};

#ifdef FCOMPLEX_H

/*! \brief ring buffer class complex floating point 32-bit realization */
template<> class ring_buffer< complex< __fx32 > > : public ring_buffer_abstract< complex< __fx32 > >
{
    typedef complex< __fx32 > __type;
public:
      ring_buffer() : ring_buffer_abstract(){}
     ~ring_buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( complex< __fx64  > *input ) { fill_buff< complex< __fx64  > >( input ); }
     inline void operator () ( complex< __fxx64 > *input ) { fill_buff< complex< __fxx64 > >( input ); }
};

/*! \brief ring buffer class complex floating point 64-bit realization */
template<> class ring_buffer< complex< __fx64 > > : public ring_buffer_abstract< complex< __fx64 > >
{
    typedef complex< __fx64 > __type;
public:
      ring_buffer() : ring_buffer_abstract(){}
     ~ring_buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( complex< __fxx64 > *input ) { fill_buff< complex< __fxx64 > >( input ); }
};

/*! \brief ring buffer class complex floating point extended 64-bit realization */
template<> class ring_buffer< complex< __fxx64 > > : public ring_buffer_abstract< complex< __fxx64 > >
{
    typedef complex< __fxx64 > __type;
public:
      ring_buffer() : ring_buffer_abstract(){}
     ~ring_buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
};

#endif

/*! \brief ring buffer class complex integer 32-bit realization */
template<> class ring_buffer< __ix32 > : public ring_buffer_abstract< __ix32 >
{
    typedef __ix32 __type;
public:
      ring_buffer() : ring_buffer_abstract(){}
     ~ring_buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( __ix64  *input ) { fill_buff< __ix64 >( input ); }
};

/*! \brief ring buffer class complex integer 64-bit realization */
template<> class ring_buffer< __ix64 > : public ring_buffer_abstract< __ix64 >
{
    typedef __ix64 __type;
public:
      ring_buffer() : ring_buffer_abstract(){}
     ~ring_buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
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
