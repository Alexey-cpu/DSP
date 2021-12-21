// --------------------------------------------------------------------------------------------------------------
// A.M. Tykvinsky, 04.02.2021
// --------------------------------------------------------------------------------------------------------------
// TRIGGERS CLASS
// --------------------------------------------------------------------------------------------------------------

#ifndef _TRIGGERS_HPP
#define _TRIGGERS_HPP

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

#endif 


