#ifndef SPECIAL_FCN_H
#define SPECIAL_FCN_H

// identify if the compilation is for ProsoftSystems IDE
#ifndef __ALG_PLATFORM
#include "cmath"
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
    return ( __fx32 )phi;
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

    // СЂР°СЃС‡РµС‚ РЅСѓР»РµРІРѕРіРѕ Р·РЅР°С‡РµРЅРёСЏ СѓРіР»Р°:

    phi = asin(sn);

    // СЂР°СЃС‡РµС‚ РєРѕРЅРµС‡РЅРѕРіРѕ Р·РЅР°С‡РµРЅРёСЏ СѓРіР»Р°:

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

// customized variables exclusion to avloid aliasing during compilation:
#undef __ITERATIONS_NUMBER__

#endif // SPECIAL_FCN_H
