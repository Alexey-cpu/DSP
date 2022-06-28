#ifndef BUFFER_H
#define BUFFER_H

// identify if the compilation is for ProsoftSystems IDE
#ifndef __ALG_PLATFORM
#include <stdlib.h>
#include <iostream>
#endif

/*! \defgroup <DelayAndBuffer> ( Delay and buffer )
 *  \brief the module contains delay and ring buffer models
    @{
*/

#include "fcomplex.h"
#include "utils.h"

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

#ifndef __fxx64
#define __fxx64 long double
#endif

/*! \defgroup <DelayAndBufferAbstractModels> ( Delay and buffer abstract models )
 *  \ingroup DelayAndBuffer
 *  \brief the module contains buffer and delay abstract models
    @{
*/

// ABSTRACT MODELS
template< typename T > class delay_abstract
{
    typedef T __type;
protected:
    __type *m_upper;   ///< delay buffer upper part pointer
    __type *m_lower;   ///< delay buffer lower part pointer
    __type *m_data;    ///< delay buffer data samples buffer
    __ix32  m_nelem;   ///< delay buffer single ( upper/lower ) part size
    __ix32  m_buffpos; ///< delay buffer current upper/lower part pointer position

    /*!
     *  \brief The function fills delay buffer
     *  \param[input] input sample pointer
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

    /*!
     *  \brief The function allocates delay buffer
     *  \param[nelem] delay buffer upper/lower part size
     *  \details The function is supposed to be called explicitly by the user
    */
    __ix32 allocate( __ix32 nelem )
    {
        if( ( nelem > 0 ) && !m_data )
        {
            m_nelem = nelem;
            m_data = __alloc__<__type>( 2 * m_nelem );

            if( m_data )
            {
               m_lower = &m_data[0];
               m_upper = &m_data[m_nelem];
            }

            return ( m_data != 0 );
        }
        else return 0;
    }

    /*! \brief The function frees delay resources */
    void deallocate()
    {
        m_data = __mfree__(m_data);
    }

    /*! \brief default constructor */
    delay_abstract()
    {
        m_upper   = nullptr;
        m_lower   = nullptr;
        m_data    = nullptr;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    /*!
     *  \brief default constructor
     *  \details The functions clears delay resources at the end of it's lifetime
    */
    virtual ~delay_abstract()
    {
        deallocate();
    }


    /*! \brief the function returns current lower/upper part pointer position */
    inline __ix32 getBuffPos() { return m_buffpos; }

    /*! \brief the function returns lower/upper part delay buffer size */
    inline __ix32 getBuffSize(){ return m_nelem;   }


    /*!
     *  \brief the operator invokation results in return of an n-th sample from the past values
     *  \param[n] sample number
    */
    inline __type operator [] ( __ix32 n ) { return *( m_upper - n - 1 ); }

    /*! \brief The operator invokation supposes to result in delay buffer fill */
    virtual inline void operator () ( __type *input ) = 0;
};

template< typename T > class buffer_abstract
{
    typedef T __type;
protected:

    __type *m_lower;   ///< buffer pointer
    __type *m_data;    ///< data samples buffer
    __ix32  m_nelem;   ///< buffer size
    __ix32  m_buffpos; ///< current buffer pointer position


    /*!
     *  \brief The function fills buffer
     *  \param[input] input sample pointer
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

    /*!
     *  \brief The function allocates buffer
     *  \param[nelem] delay buffer upper/lower part size
     *  \details The function is supposed to be called explicitly by the user
    */
    __ix32 allocate( __ix32 nelem )
    {
        if( ( nelem > 0 ) && !m_data )
        {
            m_nelem = nelem;
            m_data  = __alloc__<__type>( m_nelem );
            m_lower = ( m_data ) ? &m_data[0] : 0;
            return ( m_data != 0 );
        }
        else return 0;
    }

    /*! \brief The function frees delay resources */
    void deallocate()
    {
        m_data = __mfree__(m_data);
    }

    /*! \brief default constructor */
    buffer_abstract()
    {
        m_lower   = nullptr;
        m_data    = nullptr;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    /*!
     *  \brief default destructor
     *  \details The function clears buffer resources
    */
    virtual ~buffer_abstract()
    {
        deallocate();
    }

    /*! \brief the function returns current lower/upper part pointer position */
    inline __ix32 getBuffPos() { return m_buffpos; }

    /*! \brief the function returns lower/upper part delay buffer size */
    inline __ix32 getBuffSize(){ return m_nelem;   }


    /*!
     *  \brief the operator invokation results in return of an n-th sample from the past values
     *  \param[n] sample number
    */
    inline __type operator [] ( __ix32 n ) { return m_data[n]; }

    /*! \brief The operator invokation supposes to result in delay buffer fill */
    virtual inline void operator () ( __type *input ) = 0;
};

/*! @} */

/*! \defgroup <DelayImplementation> ( Delay abstract model implementation )
 *  \ingroup DelayAndBuffer
 *  \brief the module contains delay abstract model implementation
    @{
*/

template< typename T > class delay;

template<> class delay< __fx32 > : public delay_abstract< __fx32 >
{
    typedef __fx32 __type;
public:
      delay() : delay_abstract(){}
     ~delay(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( __fx64  *input ) { fill_buff< __fx64 >( input ); }
     inline void operator () ( __fxx64 *input ) { fill_buff< __fxx64 >( input ); }
};

template<> class delay< __fx64 > : public delay_abstract< __fx64 >
{
    typedef __fx64 __type;
public:
     delay() : delay_abstract(){}
    ~delay(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( __fxx64 *input ) { fill_buff< __fxx64 >( input ); }
};

template<> class delay< __fxx64 > : public delay_abstract< __fxx64 >
{
    typedef __fxx64 __type;
public:
     delay() : delay_abstract(){}
    ~delay(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
};

template<> class delay< __ix32 > : public delay_abstract< __ix32 >
{
    typedef __ix32 __type;
public:
      delay() : delay_abstract(){}
     ~delay(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( __ix64  *input ) { fill_buff< __ix64 >( input ); }
};

template<> class delay< __ix64 > : public delay_abstract< __ix64 >
{
    typedef __ix64 __type;
public:
      delay() : delay_abstract(){}
     ~delay(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
};

#ifdef FCOMPLEX_H

template<> class delay< fcomplex< __fx32 > > : public delay_abstract< fcomplex< __fx32 > >
{
    typedef fcomplex< __fx32 > __type;
public:
      delay() : delay_abstract(){}
     ~delay(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( fcomplex< __fx64  > *input ) { fill_buff< fcomplex< __fx64  > >( input ); }
     inline void operator () ( fcomplex< __fxx64 > *input ) { fill_buff< fcomplex< __fxx64  > >( input ); }
};

template<> class delay< fcomplex< __fx64 > > : public delay_abstract< fcomplex< __fx64 > >
{
    typedef fcomplex< __fx64 > __type;
public:
      delay() : delay_abstract(){}
     ~delay(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( fcomplex< __fxx64 > *input ) { fill_buff< fcomplex< __fxx64  > >( input ); }
};

template<> class delay< fcomplex< __fxx64 > > : public delay_abstract< fcomplex< __fxx64 > >
{
    typedef fcomplex< __fxx64 > __type;
public:
      delay() : delay_abstract(){}
     ~delay(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
};

#endif

/*! @} */

/*! \defgroup <BufferImplementation> ( Buffer abstract model implementation )
 *  \ingroup DelayAndBuffer
 *  \brief The module contains buffer abstract model implementation
    @{
*/

template< typename T > class buffer;

template<> class buffer< __fx32 > : public buffer_abstract< __fx32 >
{
    typedef __fx32 __type;
public:
      buffer() : buffer_abstract(){}
     ~buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( __fx64  *input ) { fill_buff< __fx64 >( input ); }
     inline void operator () ( __fxx64 *input ) { fill_buff< __fxx64 >( input ); }
};

template<> class buffer< __fx64 > : public buffer_abstract< __fx64 >
{
    typedef __fx64 __type;
public:
      buffer() : buffer_abstract(){}
     ~buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( __fxx64 *input ) { fill_buff< __fxx64 >( input ); }
};

template<> class buffer< __fxx64 > : public buffer_abstract< __fxx64 >
{
    typedef __fxx64 __type;
public:
      buffer() : buffer_abstract(){}
     ~buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
};

template<> class buffer< __ix32 > : public buffer_abstract< __ix32 >
{
    typedef __ix32 __type;
public:
      buffer() : buffer_abstract(){}
     ~buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( __ix64  *input ) { fill_buff< __ix64 >( input ); }
};

template<> class buffer< __ix64 > : public buffer_abstract< __ix64 >
{
    typedef __ix64 __type;
public:
      buffer() : buffer_abstract(){}
     ~buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
};

#ifdef FCOMPLEX_H

template<> class buffer< fcomplex< __fx32 > > : public buffer_abstract< fcomplex< __fx32 > >
{
    typedef fcomplex< __fx32 > __type;
public:
      buffer() : buffer_abstract(){}
     ~buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( fcomplex< __fx64  > *input ) { fill_buff< fcomplex< __fx64  > >( input ); }
     inline void operator () ( fcomplex< __fxx64 > *input ) { fill_buff< fcomplex< __fxx64 > >( input ); }
};

template<> class buffer< fcomplex< __fx64 > > : public buffer_abstract< fcomplex< __fx64 > >
{
    typedef fcomplex< __fx64 > __type;
public:
      buffer() : buffer_abstract(){}
     ~buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( fcomplex< __fxx64 > *input ) { fill_buff< fcomplex< __fxx64 > >( input ); }
};

template<> class buffer< fcomplex< __fxx64 > > : public buffer_abstract< fcomplex< __fxx64 > >
{
    typedef fcomplex< __fxx64 > __type;
public:
      buffer() : buffer_abstract(){}
     ~buffer(){};
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
};

#endif

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
#undef __fxx64

#endif // BUFFER_H
