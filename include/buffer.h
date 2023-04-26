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

#include "utils.h"

/*! \defgroup <DelayAndBufferAbstractModels> ( Delay and buffer abstract models )
 *  \ingroup DelayAndBuffer
 *  \brief the module contains buffer and delay abstract models
    @{
*/

template< typename __type >
class delay
{
protected:
    __type *m_upper;   ///< delay buffer upper part pointer
    __type *m_lower;   ///< delay buffer lower part pointer
    __type *m_data;    ///< delay buffer data samples buffer
    int     m_nelem;   ///< delay buffer single ( upper/lower ) part size
    int     m_buffpos; ///< delay buffer current upper/lower part pointer position

    /*!
     *  \brief The function fills delay buffer
     *  \param[input] input sample pointer
    */
    template< typename T > inline void fill_buff( T *input )
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
    int allocate( int nelem )
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
    delay()
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
    virtual ~delay()
    {
        #ifdef BUFFER_DEBUG
        Debugger::Log("delay_base","~delay_base()","Destructor call");
        #endif

        deallocate();
    }


    /*! \brief the function returns current lower/upper part pointer position */
    inline int getBuffPos()
    {
        return m_buffpos;
    }

    /*! \brief the function returns lower/upper part delay buffer size */
    inline int getBuffSize()
    {
        return m_nelem;
    }


    /*!
     *  \brief the operator invokation results in return of an n-th sample from the past values
     *  \param[n] sample number
    */
    inline __type operator [] ( int n )
    {
        return *( m_upper - n - 1 );
    }

    /*! \brief The operator invokation supposes to result in delay buffer fill */
    template< typename T > inline void operator () ( T *input )
    {
        this->fill_buff<T>(input);
    }
};

template< typename __type >
class buffer
{
protected:

    __type *m_lower;   ///< buffer pointer
    __type *m_data;    ///< data samples buffer
    int     m_nelem;   ///< buffer size
    int     m_buffpos; ///< current buffer pointer position

    /*!
     *  \brief The function fills buffer
     *  \param[input] input sample pointer
    */
    template< typename T > inline void fill_buff( T *input )
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
    int allocate( int nelem )
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
    buffer()
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
    virtual ~buffer()
    {
        #ifdef BUFFER_DEBUG
        Debugger::Log("buffer_base","~buffer_base()","Destructor call");
        #endif

        deallocate();
    }

    /*! \brief the function returns current lower/upper part pointer position */
    inline int getBuffPos()
    {
        return m_buffpos;
    }

    /*! \brief the function returns lower/upper part delay buffer size */
    inline int getBuffSize()
    {
        return m_nelem;
    }


    /*!
     *  \brief the operator invokation results in return of an n-th sample from the past values
     *  \param[n] sample number
    */
    inline __type operator [] ( int n )
    {
        return m_data[n];
    }

    /*! \brief The operator invokation supposes to result in delay buffer fill */
    template< typename T > inline void operator () ( T *input )
    {
        this->fill_buff<T>(input);
    }
};

/*! @} */

#endif // BUFFER_H
