//--------------------------------------------------------------------------------
//A.M.Tykvinsky, 21.12.2021
//--------------------------------------------------------------------------------
// SPECIAL MATHEMATICAL FUNCTIONS CLASS
//--------------------------------------------------------------------------------

#ifndef SPECIAL_FUNCTIONS_H
#define SPECIAL_FUNCTIONS_H

// standart headers
#ifndef __ALG_PLATFORM // identify if the compilation is for ProsoftSystems IDE
#include "cmath"
#include "malloc.h"
#endif

// auxiliary header - files:
#include "complex.h"

// auxiliary macros:
#ifndef __TO_DEGREES
#define __TO_DEGREES(x) ( (x) * 57.295779513082320876798154814105)
#endif

#ifndef __TO_RADIANS
#define __TO_RADIANS(x) ( (x) * 0.01745329251994329576923690768489)
#endif

// maximum iterations number constant:
#ifndef __ITERATIONS_NUMBER__
#define __ITERATIONS_NUMBER__ 30
#endif

// customized C data types definition:
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

// customized upper limits:
#ifndef __max_fx32
#define __max_fx32 3.402823466e+38
#endif

#ifndef __max_fx64
#define __max_fx64 1.7976931348623158e+308
#endif

#ifndef __max_ix16
#define __max_ix16 32767
#endif

#ifndef __max_ix32
#define __max_ix32 2147483647
#endif

#ifndef __max_ix64
#define __max_ix64 9223372036854775807
#endif

#ifndef __max_uix16
#define __max_uix16 65535
#endif

#ifndef __max_uix32
#define __max_uix32 4294967295
#endif

#ifndef __max_uix64
#define __max_uix64 18446744073709551615
#endif

// customized lower limits:
#ifndef __min_fx32
#define __min_fx32 1.175494351e-38
#endif

#ifndef __min_fx64
#define __min_fx64 1.7976931348623158e+308
#endif

#ifndef __min_ix16
#define __min_ix16 -32767
#endif

#ifndef __min_ix32
#define __min_ix32 -2147483647
#endif

#ifndef __min_ix64
#define __min_ix64 -9223372036854775807
#endif

#ifndef __min_uix16
#define __min_uix16 0
#endif

#ifndef __min_uix32
#define __min_uix32 0
#endif

#ifndef __min_uix64
#define __min_uix64 0
#endif

// customized epsilon:
#ifndef __EPSILON_xx64__
#define __EPSILON_xx64__ 1.0842e-019
#endif

#ifndef __EPSILON_x64__
#define __EPSILON_x64__ 2.22045e-016
#endif

#ifndef __EPSILON_x32__
#define __EPSILON_x32__ 1.19209e-007
#endif

// customized PI:
#ifndef PI0
#define PI0 3.1415926535897932384626433832795
#endif

#ifndef PI2
#define PI2 6.283185307179586476925286766559
#endif

#ifndef PI_2
#define PI_2 1.5707963267948966192313216916398
#endif

#ifndef PI_4
#define PI_4 0.78539816339744830961566084581988
#endif

// elliptic functions:

// am fucntion:
extern __fx64 __am__( __fx64 u , __fx64 k )
{
    __fxx64 a[__ITERATIONS_NUMBER__ + 1];
    __fxx64 g[__ITERATIONS_NUMBER__ + 1];
    __fxx64 c[__ITERATIONS_NUMBER__ + 1];
    __fxx64 two_n;
    __fxx64 phi;
    __ix32  n;

    k = fabsl((__fxx64)k);

    // if k is out of range [-1 ; +1] we should set an edge value...
    if     ( k < -1 ) k = -1;
    else if( k > +1 ) k = +1;

    // special cases check: k = 1 and x = 0
    if (k == 0.0) return u;
    if (k == 1.0) return 2.0 * atan(exp(u)) - PI2;

    // arithmetico-geometric mean method variables initialization:
    a[0] = 1.0L;
    g[0] = sqrtl(1.0L - k * k);
    c[0] = k;

    /*
       Gauss arithmetic operations are continued untill the sequence converges to
       the answer that satisfies machinery epsilon accuracy
    */

    two_n = 1.0L;

    for (n = 0; n < __ITERATIONS_NUMBER__; n++)
    {
        if (fabsl(a[n] - g[n]) < (a[n] * __EPSILON_xx64__)) break;
        two_n += two_n;
        a[n + 1] = 0.5L * (a[n] + g[n]);
        g[n + 1] = sqrtl(a[n] * g[n]);
        c[n + 1] = 0.5L * (a[n] - g[n]);
    }

    // backward substitution implementation:
    phi = two_n * a[n] * u;
    for (; n > 0; n--) phi = 0.5L * (phi + asinl(c[n] * sinl(phi) / a[n]));
    return ( __fx64 )phi;
}

// function sn:
extern __fx64 __sn__( __fx64 u , __fx64 x ) { return sin( __am__( u , x ) ); }

// function cn:
extern __fx64 __cn__( __fx64 u , __fx64 x ) { return cos( __am__( u , x ) ); }

// function dn:
extern __fx64 __dn__( __fx64 u , __fx64 x ) { __fx64 SN = __sn__( u , x ); return sqrt(1.0 - x * x * SN * SN); }

// function cd:
extern __fx64 __cd__( __fx64 u , __fx64 x ) { return __cn__( u , x ) / __dn__( u , x ); }

// function sd:
extern __fx64 __sd__( __fx64 u , __fx64 x ) { return __sn__( u , x ) / __dn__( u , x ); }

// function nd:
extern __fx64 __nd__( __fx64 u , __fx64 x ) { return 1 / __dn__( u , x ); }

// function dc:
extern __fx64 __dc__( __fx64 u , __fx64 x ) { return __dn__( u , x ) / __cn__( u , x ); }

// function nc:
extern __fx64 __nc__( __fx64 u , __fx64 x ) { return 1 / __cn__(u, x); }

// function sc:
extern __fx64 __sc__( __fx64 u , __fx64 x ) { return __sn__( u , x ) / __cn__( u , x ); }

// function ns:
extern __fx64 __ns__( __fx64 u , __fx64 x ) { return 1 / __sn__( u , x ); }

// function ds:
extern __fx64 __ds__( __fx64 u , __fx64 x ) { return __dn__( u , x ) / __sn__( u , x ); }

// function cs:
extern __fx64 __cs__( __fx64 u , __fx64 x ) { return __cn__( u , x ) / __sn__( u , x ); }

// inverse sn:
extern __fx64 __isn__( __fx64 sn , __fx64 x )
{
    // auxiliary variables and arrays:
    __fx64 a[__ITERATIONS_NUMBER__ + 1] ,  g[__ITERATIONS_NUMBER__ + 1] , s[__ITERATIONS_NUMBER__ + 1];
    __fx64 two_n , phi , phi_old , k;
    __ix32 n = 0;

    // initialization:
    k = fabs(x);
    a[0] = 1.0L;
    g[0] = sqrt(1.0 - k * k);

    // forward substitution:
    two_n = 1.0L;
    for (n = 0; n < __ITERATIONS_NUMBER__ - 1; n++)
    {
        s[n]     = 0;
        two_n   += two_n;
        a[n + 1] = 0.5 * (a[n] + g[n]);
        g[n + 1] = sqrt(a[n] * g[n]);
    }

    // backward substitution:
    phi = asin(sn);
    for (n = 1 ; n < __ITERATIONS_NUMBER__ ; n++)
    {

        phi_old = phi;
        phi     = atan( g[n-1]/a[n-1] * tan(phi)) + phi;

        if ( fabs(2*phi_old - phi) < 0.001 )
        {
            s[n] = 2 * s[n - 1];
        }
        else if (fabs(2 * phi_old - phi) > PI_2)
        {
            s[n] = 2 * s[n - 1] + 1;
        }

    }

    return  ( sn < 0 ) ? ( -(phi + s[n - 1] * PI0) / two_n / a[n - 1] ) : ( +(phi + s[n - 1] * PI0) / two_n / a[n - 1] );
}

// inverse cn:
extern __fx64 __icn__( __fx64 sn , __fx64 x )
{
    __fx64 a[ __ITERATIONS_NUMBER__ + 1 ] , g[ __ITERATIONS_NUMBER__ + 1 ] , s[ __ITERATIONS_NUMBER__ + 1 ];
    __fx64 two_n , phi , phi_old , k;
    __ix32 n = 0;

    // initialization:
    k    = fabs(x);
    a[0] = 1.0L;
    g[0] = sqrt(1.0 - k * k);

    // forward substitution:
    two_n = 1.0L;
    for (n = 0; n < __ITERATIONS_NUMBER__ - 1; n++)
    {
        s[n]     = 0;
        two_n   += two_n;
        a[n + 1] = 0.5 * (a[n] + g[n]);
        g[n + 1] = sqrt(a[n] * g[n]);
    }

    // backward substitution:
    phi = acos(sn);
    for (n = 1; n < __ITERATIONS_NUMBER__ ; n++)
    {
        phi_old = phi;
        phi = atan(g[n - 1] / a[n - 1] * tan(phi)) + phi;
        if (fabs(2 * phi_old - phi) < 5 * PI0 / 180 )
        {
            s[n] = 2 * s[n - 1];
        }
        else if (fabs(2 * phi_old - phi) > PI_2)
        {
            s[n] = 2 * s[n - 1] + 1;
        }
    }

    return  +(phi + s[n-1] * PI0) / two_n / a[n-1];
}

// inverse dn:
extern __fx64 __idn__( __fx64 dn , __fx64 x )
{
    __fx64 a[ __ITERATIONS_NUMBER__ + 1 ] , g[ __ITERATIONS_NUMBER__ + 1 ] , s[ __ITERATIONS_NUMBER__ + 1 ];
    __fx64 two_n , phi , phi_old , k;
    __ix32 n = 0;

    // initialization:
    k = fabs(x);
    a[0] = 1.0L;
    g[0] = sqrt(1.0 - k * k);

    // forward substitution:
    two_n = 1.0L;
    for (n = 0; n < __ITERATIONS_NUMBER__ - 1; n++)
    {
        s[n] = 0;
        two_n += two_n;
        a[n + 1] = 0.5 * (a[n] + g[n]);
        g[n + 1] = sqrt(a[n] * g[n]);
    }

    // backward substitution:
    phi = asin( sqrt( (1  - dn * dn) / k / k ) );
    for (n = 1; n < __ITERATIONS_NUMBER__; n++)
    {
        phi_old = phi;
        phi = atan(g[n - 1] / a[n - 1] * tan(phi)) + phi;

        if (fabs(2 * phi_old - phi) < 5 * PI0 / 180)
        {
            s[n] = 2 * s[n - 1];
        }
        else if (fabs(2 * phi_old - phi) > PI_2)
        {
            s[n] = 2 * s[n - 1] + 1;
        }
    }

    return  +(phi + s[n - 1] * PI0) / two_n / a[n - 1];
}

// inverse cd:
extern __fx64 __icd__( __fx64 cd , __fx64 x )
{
    __fx64 a[ __ITERATIONS_NUMBER__ + 1] , g[ __ITERATIONS_NUMBER__ + 1] , s[ __ITERATIONS_NUMBER__ + 1];
    __fx64 two_n , phi , phi_old , k;
    __ix32 n = 0;

    // initialization:
    k = fabs(x);
    a[0] = 1.0L;
    g[0] = sqrt(1.0 - k * k);

    // forward substitution:
    two_n = 1.0L;
    for (n = 0; n < __ITERATIONS_NUMBER__ - 1; n++)
    {
        s[n] = 0;
        two_n += two_n;
        a[n + 1] = 0.5 * (a[n] + g[n]);
        g[n + 1] = sqrt(a[n] * g[n]);
    }

    // backward substitution:
    phi = acos(sqrt(cd*cd*(x*x-1)/(cd*cd*x*x-1)));
    for (n = 1; n < __ITERATIONS_NUMBER__; n++)
    {
        phi_old = phi;
        phi = atan(g[n - 1] / a[n - 1] * tan(phi)) + phi;
        if (fabs(2 * phi_old - phi) < 5 * PI0 / 180)
        {
            s[n] = 2 * s[n - 1];
        }
        else if (fabs(2 * phi_old - phi) > PI_2)
        {
            s[n] = 2 * s[n - 1] + 1;
        }
    }

    return +(phi + s[n - 1] * PI0) / two_n / a[n - 1];
}

// inverse sd:
extern __fx64 __isd__( __fx64 sd , __fx64 x )
{
    __fx64 a[ __ITERATIONS_NUMBER__ + 1 ] , g[ __ITERATIONS_NUMBER__ + 1 ] , s[ __ITERATIONS_NUMBER__ + 1 ];
    __fx64 two_n , phi , phi_old , k;
    __ix32 n = 0;

    // initialization:
    k = fabs(x);
    a[0] = 1.0L;
    g[0] = sqrt(1.0 - k * k);

    // forward substitution:
    two_n = 1.0L;
    for (n = 0; n < __ITERATIONS_NUMBER__ - 1; n++)
    {
        s[n] = 0;
        two_n += two_n;
        a[n + 1] = 0.5 * (a[n] + g[n]);
        g[n + 1] = sqrt(a[n] * g[n]);
    }

    // backward substitution:
    phi = asin(sqrt(sd*sd / (1 + sd * sd*k*k)));
    for (n = 1; n < __ITERATIONS_NUMBER__; n++)
    {
        phi_old = phi;
        phi = atan(g[n - 1] / a[n - 1] * tan(phi)) + phi;
        if (fabs(2 * phi_old - phi) < 5 * PI0 / 180)
        {
            s[n] = 2 * s[n - 1];
        }
        else if (fabs(2 * phi_old - phi) > PI_2)
        {
            s[n] = 2 * s[n - 1] + 1;
        }
    }

    return  ( sd < 0 ) ? ( -(phi + s[n - 1] * PI0) / two_n / a[n - 1] ) : ( +(phi + s[n - 1] * PI0) / two_n / a[n - 1] ) ;
}

// inverse nd:
extern __fx64 __ind__( __fx64 nd , __fx64 x ) { return __idn__( 1 / nd , x ); }

// inverse dc:
extern __fx64 __idc__( __fx64 dc , __fx64 x )
{
    __fx64 a[ __ITERATIONS_NUMBER__ + 1 ] , g[ __ITERATIONS_NUMBER__ + 1 ] , s[ __ITERATIONS_NUMBER__ + 1 ];
    __fx64 two_n , phi , phi_old , k;
    __ix32 n = 0;

    // initialization:
    k = fabs(x);
    a[0] = 1.0L;
    g[0] = sqrt(1.0 - k * k);

    // forward substitution:
    two_n = 1.0L;
    for (n = 0; n < __ITERATIONS_NUMBER__ - 1; n++)
    {
        s[n] = 0;
        two_n += two_n;
        a[n + 1] = 0.5 * (a[n] + g[n]);
        g[n + 1] = sqrt(a[n] * g[n]);
    }

    // backward substitution:
    phi = acos(sqrt((1-k*k)/(dc*dc-k*k)));
    for (n = 1; n < __ITERATIONS_NUMBER__ ; n++)
    {
        phi_old = phi;
        phi = atan(g[n - 1] / a[n - 1] * tan(phi)) + phi;

        if (fabs(2 * phi_old - phi) < 5 * PI0 / 180)
        {
            s[n] = 2 * s[n - 1];
        }
        else if (fabs(2 * phi_old - phi) > PI_2)
        {
            s[n] = 2 * s[n - 1] + 1;
        }
    }

    return  +(phi + s[n - 1] * PI0) / two_n / a[n - 1];
}

// inverse nc:
extern __fx64 __inc__( __fx64 nc , __fx64 x ) { return  __icn__( 1 / nc , x ); }

// inverse sc:
extern __fx64 __isc__( __fx64 sc , __fx64 x )
{
    __fx64 a[ __ITERATIONS_NUMBER__ + 1 ] , g[ __ITERATIONS_NUMBER__ + 1 ] , s[ __ITERATIONS_NUMBER__ + 1 ];
    __fx64 two_n , phi , phi_old , k;
    __ix32 n = 0;

    // initialization:
    k = fabs(x);
    a[0] = 1.0L;
    g[0] = sqrt(1.0 - k * k);

    // forward substitution:
    two_n = 1.0L;
    for (n = 0; n < __ITERATIONS_NUMBER__ - 1; n++)
    {
        s[n] = 0;
        two_n += two_n;
        a[n + 1] = 0.5 * (a[n] + g[n]);
        g[n + 1] = sqrt(a[n] * g[n]);
    }

    // backward substitution:
    phi = atan(sc);
    for (n = 1; n < __ITERATIONS_NUMBER__ ; n++)
    {
        phi_old = phi;
        phi = atan(g[n - 1] / a[n - 1] * tan(phi)) + phi;
        if (fabs(2 * phi_old - phi) < 5 * PI0 / 180)
        {
            s[n] = 2 * s[n - 1];
        }
        else if (fabs(2 * phi_old - phi) > PI_2)
        {
            s[n] = 2 * s[n - 1] + 1;
        }
    }

    return  ( sc < 0 ) ? ( -(phi + s[n - 1] * PI0) / two_n / a[n - 1] ) : ( +(phi + s[n - 1] * PI0) / two_n / a[n - 1] ) ;
}

// inverse ns:
extern __fx64 __ins__( __fx64 ns , __fx64 x ) { return  __isn__( 1 / ns , x ); }

// inverse ds:
extern __fx64 __ids__( __fx64 ds , __fx64 x ) { return  __isd__( 1 / ds , x ); }

// inverse cs:
extern __fx64 __ics__( __fx64 cs , __fx64 x ) { return  __isc__( 1 / cs , x ); }

// ellitic intergral 1:
extern __fx64 __ellip_k__( __fx64 k )
{
    __fxx64 a[ __ITERATIONS_NUMBER__ + 1 ] , g[ __ITERATIONS_NUMBER__ + 1 ];
    __fxx64 two_n;
    __ix32  n;

    k = fabsl( ( __fxx64 )k);

    // if k is out of range [-1 ; +1] then the edge value is set:
    if      ( k < -1 ) k = -1;
    else if ( k > +1 ) k = +1;

    // arithmetico-geometric mean method variables initialization:
    a[0] = 1.0L;
    g[0] = sqrtl(1.0L - k * k);

    /*
       Gauss arithmetic operations are continued untill the sequence converges to
       the answer that satisfies machinery epsilon accuracy
    */

    two_n = 1.0L;
    for (n = 0; n < __ITERATIONS_NUMBER__ ; n++)
    {
        if (fabsl(a[n] - g[n]) < (a[n] * __EPSILON_xx64__ )) break;
        two_n += two_n;
        a[n + 1] = 0.5L * (a[n] + g[n]);
        g[n + 1] = sqrtl(a[n] * g[n]);
    }

    return PI_2 / a[n];
}

// elliptic integral 2:
extern __fx64 __ellip_e__( __fx64 k )
{
    __fxx64 a[ __ITERATIONS_NUMBER__ + 1 ] ,  g[ __ITERATIONS_NUMBER__ + 1 ] , c[ __ITERATIONS_NUMBER__ + 1 ];
    __fxx64 two_n = 1.0L , sum = 0;
    __ix32  n;

    k = fabsl( ( __fxx64 ) k );

    // if k is out of range [-1 ; +1] then the edge value is set:
    if      ( k < -1 ) k = -1;
    else if ( k > +1 ) k = +1;

    // arithmetico-geometric mean method variables initialization:
    a[0] = 1.0L;
    g[0] = sqrtl(1.0L - k * k);
    c[0] = k;

    /*
       Gauss arithmetic operations are continued untill the sequence converges to
       the answer that satisfies machinery epsilon accuracy
    */

    for (n = 0; n < __ITERATIONS_NUMBER__ ; n++)
    {
        if (fabsl(a[n] - g[n]) < (a[n] * __EPSILON_xx64__ ) ) break;
        two_n += two_n;
        a[n + 1] = 0.5L * (a[n] + g[n]);
        g[n + 1] = sqrtl(a[n] * g[n]);
        c[n + 1] = 0.5L * (a[n] - g[n]);
        sum = 0.5 * c[n + 1] * c[n + 1] * two_n + sum;
    }

    return PI_2*(1 - (sum + 0.5 * c[0] * c[0]))/ a[n];
}

// factorial function:
extern __fxx64 __factorial__( __ix64 n )
{
    //return (n == 1 || n == 0) ? 1 : __factorial__(n - 1) * n;
    __fxx64 out = 1;
    for( __ix64 i = 1 ; i <= n ; i++ ) out *= i;
    return out;
}

// integer gamma:
extern __uix64 __gamma_integer__( __uix64 n ) { return __factorial__(n-1); }

// zero order Bessel:
extern __fxx64 __bessel_i0__( __fxx64 x )
{
    __fxx64 sum = 0 , fact_n = 0;
    for ( __ix32 n = 0 ; n < __ITERATIONS_NUMBER__ ; n++)
    {
        fact_n = powl( __factorial__( n ), 2);
        if ( fact_n == 0 ) break;
        sum = sum + pow(-1, n) * pow(x / 2, 2 * n) / fact_n;
    }

    return sum;
}

// first order Bessel:
extern __fxx64 __bessel_i1__( __fxx64 x )
{
    __fxx64 sum = 0 , fact_n = 0 , fact_n_1 = 0;
    for (__ix32 n = 0; n < __ITERATIONS_NUMBER__ ; n++)
    {
        fact_n   = __factorial__(n);
        fact_n_1 = __factorial__(n + 1);
        if (fact_n == 0 || fact_n_1 == 0) break;
        sum = sum + pow(-1, n) * powl(x / 2, 2 * n) / fact_n / fact_n_1;
    }
    return sum;
}

// n order Bessel:
extern __fxx64 __bessel_in__( __fxx64 x  , __ix32 order )
{
    __fxx64 sum = 0 , fact_n = 0 , fact_n_order = 0;
    for ( __ix32 n = 0; n < __ITERATIONS_NUMBER__ ; n++)
    {
        fact_n       = __factorial__(n);
        fact_n_order = __factorial__(n + order);
        if (fact_n == 0 || fact_n_order == 0) break;
        sum = sum + powl(-1, n) * powl(x / 2, 2 * n + order) / fact_n / fact_n_order;
    }
    return sum;
}

// modified n-order Bessel:
extern __fxx64 __modified_bessel_in__( __fxx64 x  , __ix32 order )
{
    __fxx64  sum = 0 , gamma1 = 0 , gamma2    = 0;
    for (int n = 0; n < 16; n++)
    {
        if ( ( __fx64 )gamma2 / (__fx64)__max_fx64 > 0.9 ) break;
        gamma1    = __gamma_integer__(n + 1);
        gamma2    = __gamma_integer__(n + order + 1);
        sum       = sum + powl(x / 2, 2 * n + order) / gamma1 / gamma2;
    }
    return sum;
}

// window functions class:
class wind_fcn
{
    typedef bool __bool ;
protected:
    __fx64 m_order;
    __bool m_wind_ready;
    __fx64 *m_buff;
public:

    // constructor and destructor:
    wind_fcn()
    {
        m_wind_ready = 0;
        m_buff       = 0;
    }

    ~wind_fcn() { deallocate(); }

    // initialization function:
    void init( __fx64 order )
    {
        m_order = order;
        m_buff  = 0;
    }

    // memory allocation and deallocation functions:
    __ix32 allocate()
    {
        m_buff = ( __fx64* ) calloc( m_order , sizeof ( __fx64 ) );
        return ( !m_buff );
    }

    void deallocate()
    {
        if ( m_buff != 0 )
        {
            free( m_buff );
            m_buff = 0;
        }
    }

    // window functions:

    // Barlett:
    __ix32 Bartlett()
    {
        // memory allocation:
        allocate();

        // window function coefficients computation:
        __fx64 Ns = ( __fx64 ) m_order;
        for ( __ix32 n = 0; n < Ns ; n++) { m_buff[n] = ( n <= ( Ns-1 ) / 2 ) ? ( 2*n / (Ns-1) ) : ( 2 - 2 * n / (Ns-1) ); }

        // window is ready to use:
        m_wind_ready = 1;
        return m_wind_ready;
    }

    // Barlett-Hanning:
    __ix32 BartlettHanning()
    {
        // memory allocation:
        allocate();

        // window function coefficients computation:
        __fx64 Ns = (__fx64)m_order;
        for (__ix32 n = 0; n < Ns ; n++) { m_buff[n] = 0.62 - 0.48 * fabs( n / (Ns-1)-0.5) + 0.38 * cos(PI2 * (n / (Ns - 1) - 0.5)); }

        // window is ready to use:
        m_wind_ready = 1;
        return m_wind_ready;
    }

    // Blackman:
    __ix32 Blackman()
    {
        // memory allocation:
        allocate();

        // window function coefficients computation:
        __fx64 Ns = (__fx64)m_order;
        for (__ix32 n = 0; n < Ns ; n++) { m_buff[n] = 0.42 - 0.50 * cos(2 * PI0 * n / (Ns - 1) ) + 0.08 * cos(4 * PI0 * n / (Ns - 1)); }

        // window is ready to use:
        m_wind_ready = 1;

        return 0;
    }

    // Blackman-Harris:
    __ix32 BlackmanHarris()
    {
        // memory allocation:
        allocate();

        // window function coefficients computation:
        __fx64 Ns = (__fx64)m_order;
        for (__ix32 n = 0; n < Ns; n++) { m_buff[n] = 0.35875 - 0.48829 * cos(2 * PI0 * n / (Ns - 1)) + 0.14128 * cos(4 * PI0 * n / (Ns - 1)) - 0.01168 * cos(6 * PI0 * n / (Ns - 1)); }

        // window is ready to use:
        m_wind_ready = 1;
        return m_wind_ready;
    }

    // Bohman:
    __ix32 Bohman()
    {
        // memory allocation:
        allocate();

        // window function coefficients computation:
        __fx64 Ns = ceil( (__fx64)m_order / 2) , n = 0;
        for (__ix32 k = 0; k < m_order; k++)
        {
            n = k - Ns;
            if ( n >= 0 )  n = k - Ns + 1;
            m_buff[k] = (1 - fabs(n / (Ns + 1))) * cos(PI0 * fabs(n / (Ns + 1))) + 1 / PI0 * sin(PI0 * fabs(n / (Ns + 1)));
        }

        // window is ready to use:
        m_wind_ready = 1;
        return m_wind_ready;
    }

    // Chebyshev poly coefficients computation function:
    __fx64 cheby_poly(__ix32 n, __fx64 x)
    {
        __fx64 res;
        if (fabs(x) <= 1) res = cos(n*acos(x));
        else              res = cosh(n*acosh(x));
        return res;
    }

    // Chebyshev:
    __ix32 Chebyshev( __fx64 atten )
    {
        // memory allocation:
        allocate();

        // window function coefficients computation:
        __ix32    Ns = m_order , nn, kk;
        __fx64 M, n, sum = 0, max = 0;
        __fx64 tg = pow(10, atten / 20);
        __fx64 x0 = cosh((1.0 / (Ns - 1))*acosh(tg));
        M = (Ns - 1) / 2;

        if (Ns % 2 == 0) M = M + 0.5;
        for (nn = 0; nn < (Ns / 2 + 1); nn++)
        {
            n = nn - M;
            sum = 0;
            for (kk = 1; kk <= M; kk++)  { sum += cheby_poly(Ns - 1, x0*cos(PI0*kk / Ns))*cos(2.0*n*PI0*kk / Ns); }
            m_buff[nn] = tg + 2 * sum;
            m_buff[(__ix32)Ns - nn - 1] = m_buff[nn];
            if (m_buff[nn] > max)max = m_buff[nn];
        }
        for (nn = 0; nn < Ns; nn++) m_buff[nn] /= max;

        // window is ready to use:
        m_wind_ready = true;
        return m_wind_ready;
    }

    // FlatTop:
    __ix32 FlatTop()
    {
        // memory allocation:
        allocate();

        // window coefficients computation:
        __fx64 Ns = (__fx64)m_order;
        for (__ix32 n = 0 ; n < Ns; n++) { m_buff[n] = 0.21557895 - 0.41663158 * cos(PI2 * n / (Ns - 1) ) + 0.277263158 * cos(4 * PI0 * n / (Ns - 1) ) - 0.083578947 * cos(6 * PI0 * n / (Ns - 1) ) + 0.006947368 * cos(8 * PI0 * n / (Ns - 1) ); }

        // window is ready to use:
        m_wind_ready = true;
        return m_wind_ready;
    }

    // Gaussian:
    __ix32 Gaussian( __fx64 alpha )
    {
        // memory allocation:
        allocate();

        // window function coefficients computation:
        __ix32    Ns = m_order;
        __ix32    n     = 0;
        __fx64 sigma = 0;
        for (__ix32 k = 0 ; k < Ns; k++)
        {
            n = k - Ns/2;
            if (n >= 0)  n = k - Ns/2 + 1;
            sigma = ((__fx64)Ns-1) / 2 / alpha;
            m_buff[k] = exp(-(__fx64)n * (__fx64)n / 2 / sigma / sigma);
        }

        // wind is ready to use:
        m_wind_ready = true;
        return m_wind_ready;
    }

    // Hamming:
    __ix32 Hamming()
    {
        // memory allocation:
        allocate();

        // window function coefficients computation:
        __ix32 Ns = m_order;
        for (__ix32 n = 0 ; n < Ns ; n++)  { m_buff[n] = 0.54 - 0.46 * cos(PI2 * n / (Ns-1)); }

        // window is ready to use:
        m_wind_ready = true;
        return m_wind_ready;
    }

    // Hann:
    __ix32 Hann()
    {
        // memory allocation:
        allocate();

        // window function coefficients computation:
        __ix32 Ns = m_order;
        for (__ix32 n = 0; n < Ns; n++) { m_buff[n] = 0.5 - 0.5 * cos( PI2 * n / (Ns-1) ); }

        // window is ready to use:
        m_wind_ready = true;
        return m_wind_ready;
    }

    // Kaiser:
    __ix32 Kaiser( __fx64 betta )
    {
        // memory allocation:
        allocate();

        // winfow function coefficients computation:
        __ix32    Ns = m_order;
        __fx64 A  = 0;
        __fx64 B  = 0;
        __fx64 C  = 0;
        for (__ix32 n = 0; n < Ns; n++)
        {
            A = ((__fx64)n - ( (__fx64)Ns - 1 ) / 2) / ( ((__fx64)Ns - 1 ) / 2);
            B = betta * sqrt(1 - A * A);
            C = betta;
            m_buff[n] = __modified_bessel_in__( B , 0 ) / __modified_bessel_in__( C , 0 );
        }

        // window is ready to use:
        m_wind_ready = true;
        return m_wind_ready;
    }

    // Nutall:
    __ix32 Nutall()
    {
        // memory allocation:
        allocate();

        // window function coefficients computation:
        __ix32 Ns = m_order;
        for (__ix32 n = 0; n < Ns; n++)
        {
            m_buff[n] = 0.3635819 - 0.4891775 * cos(2 * PI0 * n / (Ns - 1) ) + 0.1365995 * cos(4 * PI0 * n / (Ns - 1)) - 0.0106411 * cos(6 * PI0 * n / (Ns - 1));
        }

        // window is ready to use:
        m_wind_ready = true;
        return m_wind_ready;
    }

    // Parzen:
    __ix32 Parzen()
    {
        // memory allocation:
        allocate();

        // window function coefficients computation:
        __ix32    Ns = m_order;
        __ix32    n = 0;
        for (__ix32 k = 0; k < Ns; k++)
        {
            n = k - Ns / 2;
            if (n >= 0)  n = k - Ns / 2 + 1;

            if ( abs(n)>=0 && abs(n) <= (Ns-1)/4 )
            {
                m_buff[k] = 1 - 6 * fabs((__fx64)n)*fabs((__fx64)n) / ((__fx64)Ns*(__fx64)Ns / 4) + 6 * fabs((__fx64)n)*fabs((__fx64)n)*fabs((__fx64)n) / ((__fx64)Ns*(__fx64)Ns*(__fx64)Ns / 8);
            }
            else if( abs(n) < Ns/2 && abs(n) > (Ns - 1) / 4 )
            {
                m_buff[k] = 2 * pow( 1 - fabs((__fx64)n)/((__fx64)Ns/2) , 3);
            }
        }

        // window is ready:
        m_wind_ready = true;
        return m_wind_ready;
    }

    // Rectangular:
    __ix32 Rectangular()
    {
        // memory allocation:
        allocate();

        // window coefficients vomputation:
        for (__ix32 n = 0; n < m_order; n++)  m_buff[n] = 1;

        // window is ready to use:
        m_wind_ready = true;
        return m_wind_ready;
    }

    // Triangular:
    __ix32 Triangular()
    {
        // memory allocation:
        allocate();

        // window function coefficients computation:
        __ix32 Ns = m_order;
        if ( Ns % 2 == 0)
        {
            for (__ix32 n = 0; n < Ns; n++) { m_buff[n] = (1.0 - fabs(((__fx64)n - ((__fx64)Ns - 1) / 2) / ((((__fx64)Ns - 1) + 1) / 2))); }
        }
        else
        {
            for (__ix32 n = 0; n < Ns; n++) { m_buff[n] = (1.0 - fabs(((__fx64)n - ((__fx64)Ns - 1) / 2) / ((((__fx64)Ns - 1) + 2) / 2))); }
        }

        // window is ready to use:
        m_wind_ready = true;
        return m_wind_ready;
    }

    // Tukey:
    __ix32 Tukey( __fx64 R )
    {
        // memory allocation:
        allocate();

        // window function coefficients computation:
        __ix32 Ns = m_order;
        __fx64 x  = 0;
        for(__ix32 n = 0 ; n < Ns ; n++)
        {
            x = (__fx64)n / ((__fx64)Ns - 1);
            if ( x >=0 && x < R / 2 )
            {
                m_buff[n] = 0.5 + 0.5 * cos( PI2 / R * ( x - 0.5*R ) );
            }
            else if ( x >= 1 - 0.5 * R && x <= 1 )
            {
                m_buff[n] = 0.5 + 0.5 * cos(PI2 / R * (x - 1 + 0.5*R) );
            }
            else
            {
                m_buff[n] = 1;
            }
        }

        // window is ready to use:
        m_wind_ready = true;
        return m_wind_ready;
    }

    // ready or not:
    bool is_ready() { return m_wind_ready; }

    // operators:
    inline __fx64 operator [] ( __ix32 n ) { return m_buff[n]; }
};

// template sign function:
template< typename T > __ix32 __sign__( T input ) { return ( input > 0 ) ? 1 : -1 ; }

// functions to work with complex numbers:

#ifdef COMPLEX_H

// __absf__ :
__fx64 __absf__( complex<__fx64> _complex ) { return sqrt( _complex.m_re*_complex.m_re + _complex.m_im*_complex.m_im ); }
__fx32 __absf__( complex<__fx32> _complex ) { return sqrt( _complex.m_re*_complex.m_re + _complex.m_im*_complex.m_im ); }
__ix64 __absf__( complex<__ix64> _complex ) { return sqrt( _complex.m_re*_complex.m_re + _complex.m_im*_complex.m_im ); }
__ix32 __absf__( complex<__ix32> _complex ) { return sqrt( _complex.m_re*_complex.m_re + _complex.m_im*_complex.m_im ); }
__ix16 __absf__( complex<__ix16> _complex ) { return sqrt( _complex.m_re*_complex.m_re + _complex.m_im*_complex.m_im ); }

// __argf__ :
__fx64 __argf__( complex<__fx64> _complex ) { return atan2( _complex.m_im , _complex.m_re ); }
__fx32 __argf__( complex<__fx32> _complex ) { return atan2( _complex.m_im , _complex.m_re ); }
__ix64 __argf__( complex<__ix64> _complex ) { return atan2( _complex.m_im , _complex.m_re ); }
__ix32 __argf__( complex<__ix32> _complex ) { return atan2( _complex.m_im , _complex.m_re ); }
__ix16 __argf__( complex<__ix16> _complex ) { return atan2( _complex.m_im , _complex.m_re ); }

// __sqrtf__:
complex<__fx64> __sqrtf__( complex<__fx64> _complex )
{
    __fx64 abs = sqrt ( _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im );
    __fx64 arg = atan2( _complex.m_im , _complex.m_re );
    return complex<__fx64>( sqrt( abs ) * cos( arg / 2) , sqrt( abs ) * sin( arg / 2) );
}

complex<__fx32> __sqrtf__( complex<__fx32> _complex )
{
    __fx32 abs = sqrt ( _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im );
    __fx32 arg = atan2( _complex.m_im , _complex.m_re );
    return complex<__fx32>( sqrt( abs ) * cos( arg / 2) , sqrt( abs ) * sin( arg / 2) );
}

complex<__ix64> __sqrtf__( complex<__ix64> _complex )
{
    __ix64 abs = sqrt ( _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im );
    __ix64 arg = atan2( _complex.m_im , _complex.m_re );
    return complex<__ix64>( sqrt( abs ) * cos( arg / 2) , sqrt( abs ) * sin( arg / 2) );
}

complex<__ix32> __sqrtf__( complex<__ix32> _complex )
{
    __ix32 abs = sqrt ( _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im );
    __ix32 arg = atan2( _complex.m_im , _complex.m_re );
    return complex<__ix32>( sqrt( abs ) * cos( arg / 2) , sqrt( abs ) * sin( arg / 2) );
}

complex<__ix16> __sqrtf__( complex<__ix16> _complex )
{
    __ix16 abs = sqrt ( _complex.m_re * _complex.m_re + _complex.m_im * _complex.m_im );
    __ix16 arg = atan2( _complex.m_im , _complex.m_re );
    return complex<__ix16>( sqrt( abs ) * cos( arg / 2) , sqrt( abs ) * sin( arg / 2) );
}

// __conjf__ :
complex<__fx64> __conjf__( complex<__fx64> _complex ) { _complex.m_im *= -1.0; return _complex; }
complex<__fx32> __conjf__( complex<__fx32> _complex ) { _complex.m_im *= -1.0; return _complex; }
complex<__ix64> __conjf__( complex<__ix64> _complex ) { _complex.m_im *= -1.0; return _complex; }
complex<__ix32> __conjf__( complex<__ix32> _complex ) { _complex.m_im *= -1.0; return _complex; }
complex<__ix16> __conjf__( complex<__ix16> _complex ) { _complex.m_im *= -1.0; return _complex; }

// __normf__ :
complex<__fx64> __normf__( complex<__fx64> _complex ) { __fx64 m = __absf__( _complex ); return complex<__fx64>( _complex / m ); }
complex<__fx32> __normf__( complex<__fx32> _complex ) { __fx32 m = __absf__( _complex ); return complex<__fx32>( _complex / m ); }
complex<__ix64> __normf__( complex<__ix64> _complex ) { __ix64 m = __absf__( _complex ); return complex<__ix64>( _complex / m ); }
complex<__ix32> __normf__( complex<__ix32> _complex ) { __ix32 m = __absf__( _complex ); return complex<__ix32>( _complex / m ); }
complex<__ix16> __normf__( complex<__ix16> _complex ) { __ix16 m = __absf__( _complex ); return complex<__ix16>( _complex / m ); }

// __rotf__ :
complex<__fx64> __rotf__( __fx64 arg , bool mode )
{
    if( mode ) { arg = __TO_RADIANS( arg ); }
    return complex<__fx64>( cos(arg) , sin(arg) );
}

complex<__fx32> __rotf__( __fx32 arg , bool mode )
{
    if( mode ) { arg = __TO_RADIANS( arg ); }
    return complex<__fx32>( cos(arg) , sin(arg) );
}

complex<__ix64> __rotf__( __ix64 arg , bool mode )
{
    if( mode ) { arg = __TO_RADIANS( arg ); }
    return complex<__ix64>( cos(arg) , sin(arg) );
}

complex<__ix32> __rotf__( __ix32 arg , bool mode )
{
    if( mode ) { arg = __TO_RADIANS( arg ); }
    return complex<__ix32>( cos(arg) , sin(arg) );
}

complex<__ix16> __rotf__( __ix16 arg , bool mode )
{
    if( mode ) { arg = __TO_RADIANS( arg ); }
    return complex<__ix16>( cos(arg) , sin(arg) );
}

// __sinhf__   :
complex<__fx64> __sinhf__( complex<__fx64> _complex )
{
    __fx64 re = ( exp( _complex.m_re ) * cos( _complex.m_im ) - exp( -_complex.m_re ) * cos( -_complex.m_im ) ) * 0.5;
    __fx64 im = ( exp( _complex.m_re ) * sin( _complex.m_im ) - exp( -_complex.m_re ) * sin( -_complex.m_im ) ) * 0.5;
    return complex<__fx64>( re , im );
}

complex<__fx32> __sinhf__( complex<__fx32> _complex )
{
    __fx32 re = ( exp( _complex.m_re ) * cos( _complex.m_im ) - exp( -_complex.m_re ) * cos( -_complex.m_im ) ) * 0.5;
    __fx32 im = ( exp( _complex.m_re ) * sin( _complex.m_im ) - exp( -_complex.m_re ) * sin( -_complex.m_im ) ) * 0.5;
    return complex<__fx32>( re , im );
}

complex<__ix64> __sinhf__( complex<__ix64> _complex )
{
    __ix64 re = ( exp( _complex.m_re ) * cos( _complex.m_im ) - exp( -_complex.m_re ) * cos( -_complex.m_im ) ) * 0.5;
    __ix64 im = ( exp( _complex.m_re ) * sin( _complex.m_im ) - exp( -_complex.m_re ) * sin( -_complex.m_im ) ) * 0.5;
    return complex<__ix64>( re , im );
}

complex<__ix32> __sinhf__( complex<__ix32> _complex )
{
    __ix32 re = ( exp( _complex.m_re ) * cos( _complex.m_im ) - exp( -_complex.m_re ) * cos( -_complex.m_im ) ) * 0.5;
    __ix32 im = ( exp( _complex.m_re ) * sin( _complex.m_im ) - exp( -_complex.m_re ) * sin( -_complex.m_im ) ) * 0.5;
    return complex<__ix32>( re , im );
}

complex<__ix16> __sinhf__( complex<__ix16> _complex )
{
    __ix16 re = ( exp( _complex.m_re ) * cos( _complex.m_im ) - exp( -_complex.m_re ) * cos( -_complex.m_im ) ) * 0.5;
    __ix16 im = ( exp( _complex.m_re ) * sin( _complex.m_im ) - exp( -_complex.m_re ) * sin( -_complex.m_im ) ) * 0.5;
    return complex<__ix16>( re , im );
}

// __coshf__   :
complex<__fx64> __coshf__( complex<__fx64> _complex )
{
    __fx64 re = ( exp( _complex.m_re ) * cos( _complex.m_im ) + exp( -_complex.m_re ) * cos( -_complex.m_im ) ) * 0.5;
    __fx64 im = ( exp( _complex.m_re ) * sin( _complex.m_im ) + exp( -_complex.m_re ) * sin( -_complex.m_im ) ) * 0.5;
    return complex<__fx64>( re , im );
}

complex<__fx32> __coshf__( complex<__fx32> _complex )
{
    __fx32 re = ( exp( _complex.m_re ) * cos( _complex.m_im ) + exp( -_complex.m_re ) * cos( -_complex.m_im ) ) * 0.5;
    __fx32 im = ( exp( _complex.m_re ) * sin( _complex.m_im ) + exp( -_complex.m_re ) * sin( -_complex.m_im ) ) * 0.5;
    return complex<__fx32>( re , im );
}

complex<__ix64> __coshf__( complex<__ix64> _complex )
{
    __ix64 re = ( exp( _complex.m_re ) * cos( _complex.m_im ) + exp( -_complex.m_re ) * cos( -_complex.m_im ) ) * 0.5;
    __ix64 im = ( exp( _complex.m_re ) * sin( _complex.m_im ) + exp( -_complex.m_re ) * sin( -_complex.m_im ) ) * 0.5;
    return complex<__ix64>( re , im );
}

complex<__ix32> __coshf__( complex<__ix32> _complex )
{
    __ix32 re = ( exp( _complex.m_re ) * cos( _complex.m_im ) + exp( -_complex.m_re ) * cos( -_complex.m_im ) ) * 0.5;
    __ix32 im = ( exp( _complex.m_re ) * sin( _complex.m_im ) + exp( -_complex.m_re ) * sin( -_complex.m_im ) ) * 0.5;
    return complex<__ix32>( re , im );
}

complex<__ix16> __coshf__( complex<__ix16> _complex )
{
    __ix16 re = ( exp( _complex.m_re ) * cos( _complex.m_im ) + exp( -_complex.m_re ) * cos( -_complex.m_im ) ) * 0.5;
    __ix16 im = ( exp( _complex.m_re ) * sin( _complex.m_im ) + exp( -_complex.m_re ) * sin( -_complex.m_im ) ) * 0.5;
    return complex<__ix16>( re , im );
}

// __tanhf__   :
complex<__fx64> __tanhf__( complex<__fx64> _complex ) { return __sinhf__(_complex) / __coshf__(_complex); }
complex<__fx32> __tanhf__( complex<__fx32> _complex ) { return __sinhf__(_complex) / __coshf__(_complex); }
complex<__ix64> __tanhf__( complex<__ix64> _complex ) { return __sinhf__(_complex) / __coshf__(_complex); }
complex<__ix32> __tanhf__( complex<__ix32> _complex ) { return __sinhf__(_complex) / __coshf__(_complex); }
complex<__ix16> __tanhf__( complex<__ix16> _complex ) { return __sinhf__(_complex) / __coshf__(_complex); }

// __catanhf__ :
complex<__fx64> __ctnhf__( complex<__fx64> _complex ) { return __coshf__(_complex) / __sinhf__(_complex); }
complex<__fx32> __ctnhf__( complex<__fx32> _complex ) { return __coshf__(_complex) / __sinhf__(_complex); }
complex<__ix64> __ctnhf__( complex<__ix64> _complex ) { return __coshf__(_complex) / __sinhf__(_complex); }
complex<__ix32> __ctnhf__( complex<__ix32> _complex ) { return __coshf__(_complex) / __sinhf__(_complex); }
complex<__ix16> __ctnhf__( complex<__ix16> _complex ) { return __coshf__(_complex) / __sinhf__(_complex); }

#endif

// customized types names exclusion to avloid aliasing during compilation:
#undef __ix16
#undef __ix32
#undef __ix64
#undef __uix16
#undef __uix32
#undef __uix64
#undef __fx32
#undef __fx64
#undef __fxx64

// customized upper limits exclusion to avloid aliasing during compilation:
#undef __max_fx32
#undef __max_fx64
#undef __max_ix16
#undef __max_ix32
#undef __max_ix64
#undef __max_uix16
#undef __max_uix32
#undef __max_uix64

// customized lower limits exclusion to avloid aliasing during compilation:
#undef __min_fx32
#undef __min_fx64
#undef __min_ix16
#undef __min_ix32
#undef __min_ix64
#undef __min_uix16
#undef __min_uix32
#undef __min_uix64

// customized epsilon exclusion to avloid aliasing during compilation:
#undef __EPSILON_xx64__
#undef __EPSILON_x64__
#undef __EPSILON_x32__

#undef __TO_DEGREES
#undef __TO_RADIANS

// customized pi undef:
#undef PI0
#undef PI2
#undef PI_2
#undef PI_4

// customized variables exclusion to avloid aliasing during compilation:
#undef __ITERATIONS_NUMBER__

#endif // SPECIAL_FUNCTIONS_H
