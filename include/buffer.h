#ifndef BUFFER_H
#define BUFFER_H

#ifndef __ALG_PLATFORM
#define BUFFER_DEBUG // debugging is not available if the algorithm is running on a device !!!
#endif

#ifndef __ALG_PLATFORM
#include <stdlib.h>
#include <iostream>
#endif

/*! \defgroup <DelayAndBuffer> ( Delay and buffer )
 *  \brief the module contains delay and ring buffer models
    @{
*/

#include "Complex.h"
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
template< typename T > class delay_base
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

        #ifdef BUFFER_DEBUG
        Debugger::Log("delay_base","allocate()","Memory allocation");
        #endif

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
        #ifdef BUFFER_DEBUG
        Debugger::Log("delay_base","deallocate()","Memory deallocation");
        #endif

        m_data = __mfree__(m_data);
    }

    /*! \brief default constructor */
    delay_base()
    {
        #ifdef BUFFER_DEBUG
        Debugger::Log("delay_base","delay_base()","Constructor call");
        #endif

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
    virtual ~delay_base()
    {
        #ifdef BUFFER_DEBUG
        Debugger::Log("delay_base","~delay_base()","Destructor call");
        #endif

        deallocate();
    }


    /*! \brief the function returns current lower/upper part pointer position */
    inline __ix32 getBuffPos()
    {
        return m_buffpos;
    }

    /*! \brief the function returns lower/upper part delay buffer size */
    inline __ix32 getBuffSize()
    {
        return m_nelem;
    }


    /*!
     *  \brief the operator invokation results in return of an n-th sample from the past values
     *  \param[n] sample number
    */
    inline __type operator [] ( __ix32 n )
    {
        return *( m_upper - n - 1 );
    }

    /*! \brief The operator invokation supposes to result in delay buffer fill */
    virtual inline void operator () ( __type *input ) = 0;
};

template< typename T > class buffer_base
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
        #ifdef BUFFER_DEBUG
        Debugger::Log("buffer_base","allocate()","Memory allocation");
        #endif

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
        #ifdef BUFFER_DEBUG
        Debugger::Log("buffer_base","deallocate()","Memory deallocation");
        #endif

        m_data = __mfree__(m_data);
    }

    /*! \brief default constructor */
    buffer_base()
    {
        #ifdef BUFFER_DEBUG
        Debugger::Log("buffer_base","buffer_base()","Constructor call");
        #endif

        m_lower   = nullptr;
        m_data    = nullptr;
        m_nelem   = 0;
        m_buffpos = 0;
    }

    /*!
     *  \brief default destructor
     *  \details The function clears buffer resources
    */
    virtual ~buffer_base()
    {
        #ifdef BUFFER_DEBUG
        Debugger::Log("buffer_base","~buffer_base()","Destructor call");
        #endif

        deallocate();
    }

    /*! \brief the function returns current lower/upper part pointer position */
    inline __ix32 getBuffPos()
    {
        return m_buffpos;
    }

    /*! \brief the function returns lower/upper part delay buffer size */
    inline __ix32 getBuffSize()
    {
        return m_nelem;
    }


    /*!
     *  \brief the operator invokation results in return of an n-th sample from the past values
     *  \param[n] sample number
    */
    inline __type operator [] ( __ix32 n )
    {
        return m_data[n];
    }

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

template<> class delay< __fx32  > final : public delay_base< __fx32 >
{
    typedef __fx32 __type;
public:
      delay() : delay_base(){}
     virtual ~delay(){}
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( __fx64  *input ) { fill_buff< __fx64 >( input ); }
     inline void operator () ( __fxx64 *input ) { fill_buff< __fxx64 >( input ); }
};

template<> class delay< __fx64  > final : public delay_base< __fx64 >
{
    typedef __fx64 __type;
public:
     delay() : delay_base(){}
    virtual ~delay(){}
     inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
     inline void operator () ( __fxx64 *input ) { fill_buff< __fxx64 >( input ); }
};

template<> class delay< __fxx64 > final : public delay_base< __fxx64 >
{
    typedef __fxx64 __type;
public:
    delay() : delay_base(){}
    virtual ~delay(){}
    inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
};

template<> class delay< __ix32  > final : public delay_base< __ix32 >
{
    typedef __ix32 __type;
public:
    delay() : delay_base(){}
    virtual ~delay(){}
    inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
    inline void operator () ( __ix64  *input ) { fill_buff< __ix64 >( input ); }
};

template<> class delay< __ix64  > final : public delay_base< __ix64 >
{
    typedef __ix64 __type;
public:
    delay() : delay_base(){}
    virtual ~delay(){}
    inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
};

#ifdef COMPLEX_H

template<> class delay< Complex< __fx32  > > final : public delay_base< Complex< __fx32 > >
{
    typedef Complex< __fx32 > __type;
public:
    delay() : delay_base(){}
    virtual ~delay(){}
    inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
    inline void operator () ( Complex< __fx64  > *input ) { fill_buff< Complex< __fx64  > >( input ); }
    inline void operator () ( Complex< __fxx64 > *input ) { fill_buff< Complex< __fxx64  > >( input ); }
};

template<> class delay< Complex< __fx64  > > final : public delay_base< Complex< __fx64 > >
{
    typedef Complex< __fx64 > __type;
public:
    delay() : delay_base(){}
    virtual ~delay(){}
    inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
    inline void operator () ( Complex< __fxx64 > *input ) { fill_buff< Complex< __fxx64  > >( input ); }
};

template<> class delay< Complex< __fxx64 > > final : public delay_base< Complex< __fxx64 > >
{
    typedef Complex< __fxx64 > __type;
public:
    delay() : delay_base(){}
    virtual ~delay(){}
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

template<> class buffer< __fx32  > final : public buffer_base< __fx32 >
{
    typedef __fx32 __type;
public:
    buffer() : buffer_base(){}
    virtual ~buffer(){}
    inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
    inline void operator () ( __fx64  *input ) { fill_buff< __fx64 >( input ); }
    inline void operator () ( __fxx64 *input ) { fill_buff< __fxx64 >( input ); }
};

template<> class buffer< __fx64  > final : public buffer_base< __fx64 >
{
    typedef __fx64 __type;
public:
    buffer() : buffer_base(){}
    virtual ~buffer(){}
    inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
    inline void operator () ( __fxx64 *input ) { fill_buff< __fxx64 >( input ); }
};

template<> class buffer< __fxx64 > final : public buffer_base< __fxx64 >
{
    typedef __fxx64 __type;
public:
    buffer() : buffer_base(){}
    virtual ~buffer(){}
    inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
};

template<> class buffer< __ix32  > final : public buffer_base< __ix32 >
{
    typedef __ix32 __type;
public:
    buffer() : buffer_base(){}
    virtual ~buffer(){}
    inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
    inline void operator () ( __ix64  *input ) { fill_buff< __ix64 >( input ); }
};

template<> class buffer< __ix64  > final : public buffer_base< __ix64 >
{
    typedef __ix64 __type;
public:
    buffer() : buffer_base(){}
    virtual ~buffer(){}
    inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
};

#ifdef COMPLEX_H

template<> class buffer< Complex< __fx32  > > final : public buffer_base< Complex< __fx32 > >
{
    typedef Complex< __fx32 > __type;
public:
    buffer() : buffer_base(){}
    virtual ~buffer(){}
    inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
    inline void operator () ( Complex< __fx64  > *input ) { fill_buff< Complex< __fx64  > >( input ); }
    inline void operator () ( Complex< __fxx64 > *input ) { fill_buff< Complex< __fxx64 > >( input ); }
};

template<> class buffer< Complex< __fx64  > > final : public buffer_base< Complex< __fx64 > >
{
    typedef Complex< __fx64 > __type;
public:
    buffer() : buffer_base(){}
    virtual ~buffer(){}
    inline void operator () ( __type  *input ) override { fill_buff< __type >( input ); }
    inline void operator () ( Complex< __fxx64 > *input ) { fill_buff< Complex< __fxx64 > >( input ); }
};

template<> class buffer< Complex< __fxx64 > > final : public buffer_base< Complex< __fxx64 > >
{
    typedef Complex< __fxx64 > __type;
public:
    buffer() : buffer_base(){}
    virtual ~buffer(){}
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
