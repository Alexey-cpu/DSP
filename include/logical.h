/*!
 * \file
 * \brief   Logical components
 * \authors A.Tykvinskiy
 * \date    21.01.2022
 * \version 1.0
 *
 * The header declares DSP logical compenents
*/
#ifndef LOGICAL_H
#define LOGICAL_H

/*! \defgroup <DSP_TIMERS> ( Timers )
 *  \brief the module contains timers class
    @{
*/

/*! \brief defines 32-bit floating point type */
#ifndef __fx32
#define __fx32 float
#endif

/*! \brief defines 64-bit floating point type */
#ifndef __fx64
#define __fx64 double
#endif

/*! \brief defines 16-bit integer type */
#ifndef __ix16
#define __ix16 short
#endif

/*! \brief defines 32-bit integer type */
#ifndef __ix32
#define __ix32 int
#endif

/*! \brief timers class */
class timers
{
    typedef bool __bool ;
    typedef void __void ;

private:
    /*! \brief current time */
    __fx64 m_Time;
    /*! \brief sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief sampling period , s */
    __fx64 m_Ts;
    /*! \brief number of frames per IDE cycle */
    __ix32 m_FramesPerCycle;

    /*! \brief timer logical output */
    __bool m_Q;
    /*! \brief auxiliary variable */
    __bool m_CC;
    /*! \brief auxiliary variable */
    __bool m_QQ;

public:

    /*! \brief default constructor */
    timers()
    {
        m_Fs             = 4000;
        m_Ts             = 1 / m_Fs;
        m_FramesPerCycle = 16;
        m_Time           = 0;
        m_Q              = 0;
        m_CC			 = 0;
        m_QQ             = 0;
    }

    /*! \brief default destructor */
    ~timers();

    /*! \brief timers class initialization function
     *  \param[Fs            ] - sampling frequency , Hz
     *  \param[FramesPerCycle] - number frames per IDE cycle
    */
    __void init( __fx64 Fs, __ix32 FramesPerCycle )
    {
        m_Fs             = Fs;
        m_Ts             = 1 / m_Fs;
        m_FramesPerCycle = FramesPerCycle;
        m_Time           = 0;
        m_Q              = 0;
        m_Q              = 0;
        m_CC             = 0;
        m_QQ             = 0;
    }

    /*! \brief timer "on" function
     *  \param[ S  ] - controlled logical input signal
     *  \param[ dT ] - time to wait , s
     *  \return the function returns true if the input signal S stays true within the time greater than dT.
     *          The function returns false if the input signal S stays true within the time less than dT.
     *          The true output is dropped if the input S becomes false.
    */
    __bool ton( __bool S , __fx64 dT )
    {
        if ( ( S == 1 ) && ( m_Q == 0 ) )
        {
            for ( __ix32 n = 0 ; n < m_FramesPerCycle; n++)
            {
                m_Time = m_Time + m_Ts;
                if ( m_Time > dT ) break;
            }
        }
        else
        {
            m_Time = 0;
        }

        m_Q = ( ( m_Time > dT ) && S ) ? 1 : 0;

        return m_Q;
    }


    /*! \brief timer "off" function
     *  \param[ S  ] - controlled logical input signal
     *  \param[ dT ] - time to wait , s
     *  \return the function returns true if the input signal S is true. If the input S drops to false
     *          the true output exists during the time dT. Then, if dT has expired and the input signal S
     *          is false - the function output becomes false.
    */
    __bool tof( __bool S , __fx64 dT)
    {
        if ( S == 1 )
        {
            m_Q    = 1;
            m_Time = 0;
        }

        if ( m_Time > dT && S == 0 )
        {
            m_Q    = 0;
            m_Time = 0;
        }

        if( (m_Q == 1) && (S == 0) )
        {
            for ( __ix32 n = 0; n < m_FramesPerCycle; n++ )
            {
                m_Time = m_Time + 1 / m_Fs;
                if (m_Time > dT) break;
            }
        }

        return m_Q;
    }

    /*! \brief pulse timer function
     *  \param[ S  ] - controlled logical input signal
     *  \param[ dT ] - time to wait , s
     *  \return The function outputs true if the input signal changes from 0 to 1.
     *          The true output exists untill time dT is expired
    */
    __bool tp( __bool S , __fx64 dT)
    {
        // rising front detection:
        m_QQ = ( (S == 1) && (m_CC == 0) ) ? 1 : 0;
        m_CC = S;

        if ( m_QQ == 1 )
        {
            m_Q    = 1;
            m_Time = 0;
        }

        // timer increment:
        if ( m_Time > dT )
        {
            m_Q    = 0;
            m_Time = 0;
        }
        else if( m_Q == 1 )
        {
            for ( __ix32 n = 0; n < m_FramesPerCycle; n++)
            {
                m_Time += m_Ts;
                if (m_Time > dT) break;
            }
        }

        return m_Q;
    }

    /*! \brief timer state control function
     *  \return The function returns current timer object state
    */
    __bool getState() { return m_Q; }

    /*! \brief timer time control function
     *  \return The function returns current timer time
    */
    __fx64 getTime () { return m_Time; }

};

/*! @} */

/*! \defgroup <DSP_TRIGGERS> ( Triggers )
 *  \brief the module contains triggers class
    @{
*/

/*! \brief triggers class */
class triggers
{
    typedef bool __bool ;

private:
    /*! \brief trigger logical output */
    __bool m_CC;
    /*! \brief auxiliary variable */
    __bool m_QQ;
    /*! \brief auxiliary variable */
    __bool m_Q;

public:

    /*! \brief default trigger constructor */
    triggers()
    {
        m_Q    = false;
        m_CC   = false;
        m_QQ   = false;
    }

    /*! \brief default trigger destructor */
    ~triggers();

    /*! \brief rising edge detecting function
     *  \param[CLK] - controlled logical input
     *  \return The function returns true single frame ( IDE cycle ) pulse if the input changes from 0 to 1.
    */
    __bool rr_trig( __bool CLK )
    {
        m_Q  = ( (CLK == true) && (m_CC == false) ) ? true : false;
        m_CC = CLK;
        return m_Q;
    }

    /*! \brief falling edge detecting function
     *  \param[CLK] - controlled logical input
     *  \return The function returns true single frame ( IDE cycle ) pulse if the input changes from 1 to 0.
    */
    __bool ff_trig( __bool CLK )
    {
        m_Q  = ( (CLK == false) && (m_CC == true) ) ? true : false;
        m_CC = CLK;
        return m_Q;
    }

    /*! \brief bistable RS trigger function
     *  \param[S] - controlled logical set   input
     *  \param[R] - controlled logical reset input
     *  \return The function returns true if the input S is true.
     *          The output becomes false if the input R becomes false.
     *          If S and R inputs become true at the same time the output is false.
    */
    __bool rs_trig( __bool S , __bool R )
    {
        if ( ( (S == true) && (R == false) ) || ( (m_QQ == true) && (R == false) ) )
        {
            m_QQ = true;
            m_Q  = m_QQ;
        }
        else
        {
            m_QQ = false;
            m_Q  = m_QQ;
        }

        return m_Q;
    }

    /*! \brief bistable SR trigger function
     *  \param[S] - controlled logical set   input
     *  \param[R] - controlled logical reset input
     *  \return The function returns true if the input S is true.
     *          The output becomes false if the input R becomes false.
     *          If S and R inputs become true at the same time the output is true.
    */
    __bool sr_trig( __bool S , __bool R )
    {
        if ( (S == true) || ( (m_QQ == true) && (R == false) ) )
        {
            m_QQ = true;
            m_Q  = m_QQ;
        }
        else
        {
            m_QQ = false;
            m_Q  = m_QQ;
        }

        return m_Q;
    }

    /*! \brief bistable SR trigger function
     *  \param[input]   - controlled input signal
     *  \param[offset]  - the input signal offset
     *  \param[falling] - falling edge control flag
     *  \return If falling == false: the function returns true if the input signal increases the offset.
     *          If falling == true : the function returns true if the input signal becomes bellow the offset.
    */
    template< typename T > __bool hit_crossing( T input , T offset , __bool falling )
    {
        if (falling == false)
        {
            return rr_trig( ( input > offset ) );
        }
        else
        {
            return ff_trig( ( input > offset ) );
        }
    }

    /*! \brief trigger state control function
     *  \return The function returns current trigger state
    */
    __bool get_State() { return m_Q; }
};

/*! @} */

#undef __fx32
#undef __fx64
#undef __ix32
#undef __ix16

#endif
