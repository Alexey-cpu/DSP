#ifndef SGEN_H
#define SGEN_H

#ifndef __fx32
#define __fx32 float
#endif

#ifndef __fx64
#define __fx64 double
#endif

#ifndef __ix32
#define __ix32 int
#endif

// signal generator template:
template< typename T > class sgen;

// implementations:
template<> class sgen<__fx32>
{

};

#undef __fx32
#undef __fx64
#undef __ix32

#endif
