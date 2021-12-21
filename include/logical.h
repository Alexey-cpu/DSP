#ifndef LOGICAL_H
#define LOGICAL_H

#ifndef __fx32
#define __fx32 float
#endif

#ifndef __fx64
#define __fx64 double
#endif

#ifndef __ix16
#define __ix16 short
#endif

#ifndef __ix32
#define __ix32 int
#endif

// timers class:
class timers
{
    typedef bool __bool ;
    typedef void __void ;
    __fx64 m_Time , m_Fs , m_Ts;
    __ix32 m_FramesPerCycle;
    __bool m_Q , m_CC , m_QQ;

public:

    // constructor:
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

    // destructor:
    ~timers();

    // timer initialization function:
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

    // timers functions:

    // ton:
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


    // tof:
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

    // tp:
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

    // get state and time functions:
    __bool getState() { return m_Q; }
    __fx64 getTime () { return m_Time; }

};

// triggers class:
class triggers
{
    typedef bool __bool ;

private:
    __bool m_CC;
    __bool m_QQ;
    __bool m_Q;

public:

    // constructor:
    triggers()
    {
        m_Q    = false;
        m_CC   = false;
        m_QQ   = false;
    }

    // destructor:
    ~triggers();

    // get trigger state function:
    __bool get_State() { return m_Q; }

    // rising edge detector
    __bool rr_trig( __bool CLK )
    {
        m_Q  = ( (CLK == true) && (m_CC == false) ) ? true : false;
        m_CC = CLK;
        return m_Q;
    }

    // falling edge detector
    __bool ff_trig( __bool CLK )
    {
        m_Q  = ( (CLK == false) && (m_CC == true) ) ? true : false;
        m_CC = CLK;
        return m_Q;
    }

    // bstable RS-trigger
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

    // bstable SR-trigger
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

    // Hit crossing template function:
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
};

#undef __fx32
#undef __fx64
#undef __ix32
#undef __ix16

#endif
