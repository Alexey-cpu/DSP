#ifndef GENERATORS_H
#define GENERATORS_H

// standart headers
#ifndef __ALG_PLATFORM // identify if the compilation is for ProsoftSystems IDE
#include "cmath"
#include <stdlib.h>
#endif

#ifndef __ix32
#define __ix32 int
#endif

#ifndef __fx32
#define __fx32 float
#endif

#ifndef __fx64
#define __fx64 double
#endif

#ifndef PI0 // PI
#define PI0 3.1415926535897932384626433832795
#endif

#ifndef PI2 // 2*PI
#define PI2 6.283185307179586476925286766559
#endif

#ifndef __TO_DEGREES
#define __TO_DEGREES(x) ( (x) * 57.295779513082320876798154814105)
#endif

#ifndef __TO_RADIANS
#define __TO_RADIANS(x) ( (x) * 0.01745329251994329576923690768489)
#endif

template<typename __type = __fx64>
class digital_clock
{
    __fx64 m_bound = 3600;
    __fx64 m_time  = 0;
    __fx64 m_Fs    = 4000;
    __fx64 m_Ts    = 1.0 / m_Fs;

public:

    // default constructor and destructor
     digital_clock(){}
    ~digital_clock(){}

     void init(__fx64 Fs, __fx64 bound = 3600 )
     {
         m_Fs    = Fs;
         m_Ts    = 1.0 / m_Fs;
         m_bound = bound;
         m_time  = 0;
     }

     // properties
     __type time()
     {
         return m_time;
     }

     __fx64 SamplingTimeRate()
     {
         return m_Ts;
     }

     // public functions

     __type tick()
     {
         if( m_time >= m_bound )
         {
             reset();
         }

         m_time += m_Ts;
         return m_time;
     }

    void reset()
    {
        m_time = 0;
    }
};

template < typename  __type >
class generator
{
public:

    /*! \brief test sine generator function
     *  \param[amp] - test signal amplitude
     *  \param[fn] - test signal frequency, Hz
     *  \param[phase] - test signal phase, degrees
     *  \param[time] - input time value, s
     *  \returns The function returns sine function output. Timer counter is dropped to a zero every hour
     *           to avoid overflow within the real time
    */
    __type sine( __fx64 amp , __fx64 fn , __fx64 phase , __fx64 time )
    {
        return fn == 0 ? amp : amp * sin( fn * PI2 * time + __TO_RADIANS( phase ) );
    }

    /*! \brief test sine generator function
     *  \param[amp] - test signal amplitude
     *  \param[fn] - test signal frequency, Hz
     *  \param[phase] - test signal phase, degrees
     *  \param[distortion] - total harmonic distortion, p.u
     *  \param[number_of_harmonics] - number of harmonics
     *  \param[time] - input time value, s
     *  \returns The function returns sine function output. Timer counter is dropped to a zero every hour
     *           to avoid overflow within the real time
    */
    __type distortion( __fx64 amp, __fx64 fn, __fx64 phase, __fx64 distortion, __ix32 number_of_harmonics, __fx64 time )
    {
        __fx64 ratio  = distortion / number_of_harmonics;
        __fx64 output = 0;

        for( __ix32 i = 0 ; i < number_of_harmonics ; i++ )
        {
            if( i != 1 )
            {
                output += amp * ratio * sine( amp, i * fn, 0, time );
            }
            else
            {
                output += amp * (1.0 - distortion) * sine( amp, i * fn, phase, time );
            }
        }

        return output;
    }

    /*! \brief test pulse signal generator
     *  \param[amp] - test pulse amplitude
     *  \param[fn] - test pulse frequency, Hz
     *  \param[phs] - phase angle between two neighboirung pulses , degrees
     *  \param[fs] - input time value, s
     *  \returns The function returns sine function output. Timer counter is dropped to a zero every hour
     *           to avoid overflow within the real time
    */
    __type pulse( __fx64 amp , __fx64 fn , __fx64 phase , __fx64 time )
    {
        __fx64 y1 = sin( fn * PI2 * time + __TO_RADIANS( 180.0 + phase ) );
        __fx64 y2 = sin( fn * PI2 * time );
        return fn == 0 ? amp : amp * (__fx64)( ( y1 > 0 ) || ( y2 > 0 ) );

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
#undef __TO_DEGREES

#endif
