#ifndef SGEN_H
#define SGEN_H

// standart headers
#ifndef __ALG_PLATFORM // identify if the compilation is for ProsoftSystems IDE
#include "cmath"
#include "malloc.h"
#endif

#ifndef __fx32
#define __fx32 float
#endif

#ifndef __fx64
#define __fx64 double
#endif

#ifndef __ix32
#define __ix32 int
#endif

// customized PI:
#ifndef PI0
#define PI0 3.1415926535897932384626433832795
#endif

#ifndef PI2
#define PI2 6.283185307179586476925286766559
#endif

#ifndef __TO_RADIANS
#define __TO_RADIANS(x) ( (x) * 0.01745329251994329576923690768489)
#endif

// signal generator template:
template< typename T > class sgen
{
    typedef T __type;
    __type m_t;

public:

    __type m_out;

    sgen()
    {
        m_t    = 0;
        m_out  = 0;
    }

    __type gen( __type amp , __type fn , __type phs , __type fs )
    {
        // signal generation:
        m_out = amp * sin( fn * PI2 * m_t + __TO_RADIANS( phs ) );

        // time increment:
        if( ( fn > 0 ) && ( m_t == 1 ) ) m_t = 0;
        else m_t += 1 / fs;

        // output:
        return m_out;
    }

    // operators:
    inline __type operator() ( __type amp , __type fn , __type phs , __type fs ) { return gen( amp , fn , phs , fs ); }
};

#undef __fx32
#undef __fx64
#undef __ix32
#undef PI0
#undef PI2
#undef __TO_RADIANS

#endif
