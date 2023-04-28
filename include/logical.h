#ifndef LOGICAL_H
#define LOGICAL_H

/*! \defgroup <DSP_LOGICAL> ( Logical )
 *  \brief the module contains DSP logical components
    @{
*/

/*! \defgroup <DSP_TIMERS> ( Timers )
 *  \ingroup DSP_LOGICAL
 *  \brief the module contains timers class
    @{
*/

/*!
 *  \class timers
 *  \brief timers class
*/
class timers final
{
private:

    double m_Time;
    double m_Fs;
    double m_Ts;
    int64_t m_FramesPerCycle;
    bool m_Q;
    bool m_CC;
    bool m_QQ;

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
    virtual ~timers(){}

    /*! \brief timers class initialization function
     *  \param[Fs] - sampling frequency , Hz
     *  \param[FramesPerCycle] - number frames per IDE cycle
    */
    void init( double Fs, int64_t FramesPerCycle )
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
     *  \param[S] - controlled logical input signal
     *  \param[dT] - time to wait , s
     *  \return the function returns true if the input signal S stays true within the time greater than dT.
     *          The function returns false if the input signal S stays true within the time less than dT.
     *          The true output is dropped if the input S becomes false.
    */
    bool ton( bool S , double dT )
    {
        if ( ( S == 1 ) && ( m_Q == 0 ) )
        {
            for ( int64_t n = 0 ; n < m_FramesPerCycle; n++)
            {
                m_Time = m_Time + m_Ts;

                if ( m_Time > dT )
                    break;
            }
        }

        if( ( m_Time > dT ) && S )
        {
            m_Q = 1;
        }
        else if( !S )
        {
            m_Q    = 0;
            m_Time = 0;
        }

        return m_Q;
    }


    /*! \brief timer "off" function
     *  \param[S] - controlled logical input signal
     *  \param[dT] - time to wait , s
     *  \return the function returns true if the input signal S is true. If the input S drops to false
     *          the true output exists during the time dT. Then, if dT has expired and the input signal S
     *          is false - the function output becomes false.
    */
    bool tof( bool S , double dT)
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
            for ( int64_t n = 0; n < m_FramesPerCycle; n++ )
            {
                m_Time = m_Time + 1 / m_Fs;

                if (m_Time > dT)
                    break;
            }
        }

        return m_Q;
    }

    /*! \brief pulse timer function
     *  \param[S] - controlled logical input signal
     *  \param[dT] - time to wait , s
     *  \return The function outputs true if the input signal changes from 0 to 1.
     *          The true output exists untill time dT is expired
    */
    bool tp( bool S , double dT)
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
            for ( int64_t n = 0; n < m_FramesPerCycle; n++)
            {
                m_Time += m_Ts;

                if (m_Time > dT)
                    break;
            }
        }

        return m_Q;
    }

    /*! \brief timer state control function
     *  \return The function returns current timer object state
    */
    bool getState()
    {
        return m_Q;
    }

    /*! \brief timer time control function
     *  \return The function returns current timer time
    */
    double getTime ()
    {
        return m_Time;
    }

};

/*! @} */

/*! @} */

/*! \defgroup <DSP_TRIGGERS> ( Triggers )
 *  \ingroup DSP_LOGICAL
 *  \brief the module contains triggers class
    @{
*/

/*!
 *  \class triggers
 *  \brief triggers class */
class triggers final
{
private:

    bool m_CC;
    bool m_QQ;
    bool m_Q;

public:

    /*! \brief default trigger constructor */
    triggers()
    {
        m_Q    = false;
        m_CC   = false;
        m_QQ   = false;
    }

    /*! \brief default trigger destructor */
    virtual ~triggers(){}

    /*! \brief rising edge detecting function
     *  \param[CLK] - controlled logical input
     *  \return The function returns true single frame ( IDE cycle ) pulse if the input changes from 0 to 1.
    */
    bool rr_trig( bool CLK )
    {
        m_Q  = CLK && m_CC;
        m_CC = CLK;
        return m_Q;
    }

    /*! \brief falling edge detecting function
     *  \param[CLK] - controlled logical input
     *  \return The function returns true single frame ( IDE cycle ) pulse if the input changes from 1 to 0.
    */
    bool ff_trig( bool CLK )
    {
        m_Q  = !CLK && m_CC;
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
    bool rs_trig( bool S , bool R )
    {
        if ( ( S && !R ) || ( m_QQ && !R ) )
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
    bool sr_trig( bool S , bool R )
    {
        if ( S || ( S && !R ) )
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
    template< typename T > bool hit_crossing( T input , T offset , bool falling )
    {
        return ( !falling ) ? rr_trig( ( input > offset ) ) : ff_trig( ( input > offset ) );
    }

    /*! \brief trigger state control function
     *  \return The function returns current trigger state
    */
    bool getState()
    {
        return m_Q;
    }
};

/*! @} */

#endif
