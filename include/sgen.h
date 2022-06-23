/*!
 * file
 * brief   Test signal template generator
 * authors A.Tykvinskiy
 * date    28.01.2022
 * version 1.0
 *
 * The header declares template signal generator
*/

#ifndef SGEN_H
#define SGEN_H

// standart headers
#ifndef __ALG_PLATFORM // identify if the compilation is for ProsoftSystems IDE
#include "cmath"
#include <stdlib.h>
#endif

/*! \brief defines 32-bit integer type */
#ifndef __ix32
#define __ix32 int
#endif

/*! \brief defines 32-bit floating point type */
#ifndef __fx32
#define __fx32 float
#endif

/*! \brief defines 64-bit floating point type */
#ifndef __fx64
#define __fx64 double
#endif

/*! \brief defines Pi */
#ifndef PI0 // PI
#define PI0 3.1415926535897932384626433832795
#endif

/*! \brief defines  2*Pi */
#ifndef PI2 // 2*PI
#define PI2 6.283185307179586476925286766559
#endif

/*! \brief converts radians to degrees */
#ifndef __TO_DEGREES
#define __TO_DEGREES(x) ( (x) * 57.295779513082320876798154814105)
#endif

/*! \brief converts degrees to radians */
#ifndef __TO_RADIANS
#define __TO_RADIANS(x) ( (x) * 0.01745329251994329576923690768489)
#endif

/*! \brief template test signal generator */
template< typename T > class sgen
{
    typedef T __type;
private:
    /*! \brief time counter */
    __type m_t;
public:

    /*! \brief test signal generator output */
    __type m_out;

    /*! \brief test signal generator default constructor */
    sgen()
    {
        m_t    = 0;
        m_out  = 0;
    }

    /*! \brief test sine generator function
     *  \param[amp] - test signal amplitude
     *  \param[fn] - test signal frequency          , Hz
     *  \param[phs] - test signal phase              , degrees
     *  \param[fs] - test signal sampling frequency , Hz
     *  \returns The function returns sine function output. Timer counter is dropped to a zero every hour
     *           to avoid overflow within the real time
    */
    __type sine( __type amp , __type fn , __type phs , __type fs )
    {
        // signal generation:
        m_out = amp * sin( fn * PI2 * m_t + __TO_RADIANS( phs ) );
        if( m_t >= 3600 ) m_t = 0; // reset every hour
        else m_t += 1.0 / fs;
        return m_out;
    }

    /*! \brief test pulse signal generator
     *  \param[amp] - test pulse amplitude
     *  \param[fn] - test pulse frequency                        , Hz
     *  \param[phs] - phase angle between two neighboirung pulses , degrees
     *  \param[fs] - test signal sampling frequency              , Hz
     *  \returns The function returns sine function output. Timer counter is dropped to a zero every hour
     *           to avoid overflow within the real time
    */
    __type pulse( __type amp , __type fn , __type phs , __type fs )
    {
        __type y1 = sin( fn * PI2 * m_t + __TO_RADIANS( 180 + phs ) );
        __type y2 = sin( fn * PI2 * m_t );
        m_out     = amp * ( ( y1 > 0 ) || ( y2 > 0 ) );
        if( m_t >= 3600 ) m_t = 0; // reset every hour
        else m_t += 1.0 / fs;
        return m_out;

        return 0;
    }
};

// macro undefenition to avoid aliases during compilation
#undef __fx32
#undef __fx64
#undef __ix32
#undef PI0
#undef PI2
#undef __TO_RADIANS

#endif
