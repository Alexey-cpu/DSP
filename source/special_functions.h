#ifndef SPECIAL_FUNCTIONS_H
#define SPECIAL_FUNCTIONS_H

// standart headers
#ifndef __ALG_PLATFORM
#include "cmath"
#include <stdlib.h>
#include "cstring"
#endif

#include <Complex.h>
#include <Utils.h>

/*! \defgroup <SPECIAL_MATH_FUNCTIONS> ( Special functions )
 *  \brief the module collaborates all special math functions
    @{
*/

/*! \brief sets maximum number of iteration for elliptic functions computation */
#ifndef __ITERATIONS_NUMBER__
#define __ITERATIONS_NUMBER__ 30
#endif

/*! \defgroup <ELLIPTIC_FUNCTIONS> ( Elliptic functons )
 *  \ingroup SPECIAL_MATH_FUNCTIONS
 *  \brief the module collaborates all the elliptic functions and integrals
    @{
*/

/*!
    \brief Jacobi amplitude function
    \param[u] incomplete elliptic integral of the first kind
    \param[k] elliptic modulus
    \return  returns Jacobi amplitude computed using the method of the arithmetic-geometric mean.
*/
extern double __am__( double u , double k )
{
    long double a[__ITERATIONS_NUMBER__ + 1];
    long double g[__ITERATIONS_NUMBER__ + 1];
    long double c[__ITERATIONS_NUMBER__ + 1];
    long double two_n;
    long double phi;
    int  n;

    k = fabsl((long double)k);

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

    // Gauss arithmetic operations are continued untill the sequence converges to
    // the answer that satisfies machinery epsilon accuracy

    two_n = 1.0L;

    for (n = 0; n < __ITERATIONS_NUMBER__; n++)
    {
        if (fabsl(a[n] - g[n]) < (a[n] * __minexponent__<long double>() )) break;
        two_n += two_n;
        a[n + 1] = 0.5L * (a[n] + g[n]);
        g[n + 1] = sqrtl(a[n] * g[n]);
        c[n + 1] = 0.5L * (a[n] - g[n]);
    }

    // backward substitution implementation:
    phi = two_n * a[n] * u;
    for (; n > 0; n--) phi = 0.5L * (phi + asinl(c[n] * sinl(phi) / a[n]));
    return ( double )phi;
}

/*!
    \brief Elliptic Jacobi SN function
    \param[u] incomplete elliptic integral of the first kind
    \param[x] elliptic modulus
    \returs  returns Ellptic Jacobi SN( u , x ) = sin( am( u , x ) ) function
*/
extern double __sn__( double u , double x ) { return sin( __am__( u , x ) ); }

/*!  \example example_math_special_functions.h */


/*!
    \brief Elliptic Jacobi CN function
    \param[u] incomplete elliptic integral of the first kind
    \param[x] elliptic modulus
    \return  returns Ellptic Jacobi CN( u , x ) = cos( am( u , x ) ) function
*/
extern double __cn__( double u , double x ) { return cos( __am__( u , x ) ); }

/*!  \example example_math_special_functions.h */

/*!
    \brief Elliptic Jacobi DN function
    \param[u] incomplete elliptic integral of the first kind
    \param[x] elliptical modulus
    \return  returns Ellptic Jacobi DN( u , x ) = sqrt( 1 - x * x * SN( u , x ) * SN( u , x ) ) function
*/
extern double __dn__( double u , double x ) { double SN = __sn__( u , x ); return sqrt(1.0 - x * x * SN * SN); }

/*!  \example example_math_special_functions.h */

/*!
    \brief Elliptic Jacobi CD function
    \param[u] incomplete elliptic integral of the first kind
    \param[x] elliptical modulus
    \return  returns Ellptic Jacobi CD( u , x ) = CN( u , x ) / DN( u , x ) function
*/
extern double __cd__( double u , double x ) { return __cn__( u , x ) / __dn__( u , x ); }

/*!  \example example_math_special_functions.h */

/*!
    \brief Elliptic Jacobi SD function
    \param[u] incomplete elliptic integral of the first kind
    \param[x] elliptical modulus
    \return  returns Ellptic Jacobi SD( u , x ) = SN( u , x ) / DN( u , x ) function
*/
extern double __sd__( double u , double x ) { return __sn__( u , x ) / __dn__( u , x ); }

/*!  \example example_math_special_functions.h */

/*!
    \brief Elliptic Jacobi ND function
    \param[u] incomplete elliptic integral of the first kind
    \param[x] elliptical modulus
    \return  returns Ellptic Jacobi ND( u , x ) = SN( u , x ) / DN( u , x ) function
*/
extern double __nd__( double u , double x ) { return 1 / __dn__( u , x ); }

/*!  \example example_math_special_functions.h */

/*!
    \brief Elliptic Jacobi DC function
    \param[u] incomplete elliptic integral of the first kind
    \param[x] elliptical modulus
    \return  returns Ellptic Jacobi DC( u , x ) = 1 / ND( u , x ) function
*/
extern double __dc__( double u , double x ) { return __dn__( u , x ) / __cn__( u , x ); }

/*!  \example example_math_special_functions.h */

/*!
    \brief Elliptic Jacobi NC function
    \param[u] incomplete elliptic integral of the first kind
    \param[x] elliptical modulus
    \return  returns Ellptic Jacobi NC( u , x ) = 1 / CN( u , x ) function
*/
extern double __nc__( double u , double x ) { return 1 / __cn__(u, x); }

/*!  \example example_math_special_functions.h */

/*!
    \brief Elliptic Jacobi SC function
    \param[u] incomplete elliptic integral of the first kind
    \param[x] elliptical modulus
    \return  returns Ellptic Jacobi SC( u , x ) = SN( u , x ) / CN( u , x ) function
*/
extern double __sc__( double u , double x ) { return __sn__( u , x ) / __cn__( u , x ); }

/*!  \example example_math_special_functions.h */

/*!
    \brief Elliptic Jacobi NS function
    \param[u] incomplete elliptic integral of the first kind
    \param[x] elliptical modulus
    \return  returns Ellptic Jacobi NS( u , x ) = 1 / SN( u , x ) function
*/
extern double __ns__( double u , double x ) { return 1 / __sn__( u , x ); }

/*!  \example example_math_special_functions.h */

/*!
    \brief Elliptic Jacobi DS function
    \param[u] incomplete elliptic integral of the first kind
    \param[x] elliptical modulus
    \return  returns Ellptic Jacobi DS( u , x ) = DN( u , x ) / SN( u , x ) function
*/
extern double __ds__( double u , double x ) { return __dn__( u , x ) / __sn__( u , x ); }

/*!  \example example_math_special_functions.h */

/*!
 * \brief Elliptic Jacobi CS function
 * \param[u] incomplete elliptic integral of the first kind
 * \param[x] elliptical modulus
 * \return  returns Ellptic Jacobi CS( u , x ) = CN( u , x ) / SN( u , x ) function
*/
extern double __cs__( double u , double x ) { return __cn__( u , x ) / __sn__( u , x ); }

/*!  \example example_math_special_functions.h */

/*!
 * \brief Elliptic Jacobi inverse SN function
 * \param[sn] Elliptic Jacobi SN function
 * \param[x] elliptical modulus
 * \return  returns Ellptic Jacobi inverse SN function
*/
extern double __isn__( double sn , double x )
{
    // auxiliary variables and arrays:
    double a[__ITERATIONS_NUMBER__ + 1] ,  g[__ITERATIONS_NUMBER__ + 1] , s[__ITERATIONS_NUMBER__ + 1];
    double two_n , phi , phi_old , k;
    int n = 0;

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

/*!
 * \brief Elliptic Jacobi inverse CN function
 * \param[cn] elliptic Jacobi CN function
 * \param[x] elliptical modulus
 * \return  returns Ellptic Jacobi inverse CN function
*/
extern double __icn__( double cn , double x )
{
    double a[ __ITERATIONS_NUMBER__ + 1 ] , g[ __ITERATIONS_NUMBER__ + 1 ] , s[ __ITERATIONS_NUMBER__ + 1 ];
    double two_n , phi , phi_old , k;
    int n = 0;

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
    phi = acos(cn);
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

/*!
 * \brief Elliptic Jacobi inverse DN function\
 * \param[dn] Elliptic Jacobi DN function
 * \param[x] elliptical modulus
 * \return  returns Ellptic Jacobi inverse DN function
*/
extern double __idn__( double dn , double x )
{
    double a[ __ITERATIONS_NUMBER__ + 1 ] , g[ __ITERATIONS_NUMBER__ + 1 ] , s[ __ITERATIONS_NUMBER__ + 1 ];
    double two_n , phi , phi_old , k;
    int n = 0;

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

/*!
 * \brief Elliptic Jacobi inverse CD function\
 * \param[cd] Elliptic Jacobi CD function
 * \param[x] elliptical modulus
 * \return  returns Ellptic Jacobi inverse CD function
*/
extern double __icd__( double cd , double x )
{
    double a[ __ITERATIONS_NUMBER__ + 1] , g[ __ITERATIONS_NUMBER__ + 1] , s[ __ITERATIONS_NUMBER__ + 1];
    double two_n , phi , phi_old , k;
    int n = 0;

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

/*!
 * \brief Elliptic Jacobi inverse SD function\
 * \param[sd] Elliptic Jacobi SD function
 * \param[x] elliptical modulus
 * \return  returns Ellptic Jacobi inverse SD function
*/
extern double __isd__( double sd , double x )
{
    double a[ __ITERATIONS_NUMBER__ + 1 ] , g[ __ITERATIONS_NUMBER__ + 1 ] , s[ __ITERATIONS_NUMBER__ + 1 ];
    double two_n , phi , phi_old , k;
    int n = 0;

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

/*!
* \brief Elliptic Jacobi inverse ND function
* \param[nd] Elliptic Jacobi ND function
* \param[x] elliptical modulus
* \return  returns Ellptic Jacobi inverse ND function
*/
extern double __ind__( double nd , double x ) { return __idn__( 1 / nd , x ); }

/*!
* \brief Elliptic Jacobi inverse DC function
* \param[dc] Elliptic Jacobi DC function
* \param[x] elliptical modulus
* \return  returns Ellptic Jacobi inverse DC function
*/
extern double __idc__( double dc , double x )
{
    double a[ __ITERATIONS_NUMBER__ + 1 ] , g[ __ITERATIONS_NUMBER__ + 1 ] , s[ __ITERATIONS_NUMBER__ + 1 ];
    double two_n , phi , phi_old , k;
    int n = 0;

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

/*!
* \brief Elliptic Jacobi inverse NC function
* \param[nc] Elliptic Jacobi NC function
* \param[x] elliptical modulus
* \return  returns Ellptic Jacobi inverse NC function
*/
extern double __inc__( double nc , double x ) { return  __icn__( 1 / nc , x ); }

/*!
* \brief Elliptic Jacobi inverse SC function
* \param[sc] Elliptic Jacobi SC function
* \param[x] elliptical modulus
* \return  returns Ellptic Jacobi inverse SC function
*/
extern double __isc__( double sc , double x )
{
    double a[ __ITERATIONS_NUMBER__ + 1 ] , g[ __ITERATIONS_NUMBER__ + 1 ] , s[ __ITERATIONS_NUMBER__ + 1 ];
    double two_n , phi , phi_old , k;
    int n = 0;

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

/*!
* \brief Elliptic Jacobi inverse NS function
* \param[ns] Elliptic Jacobi NS function
* \param[x] elliptical modulus
* \return  returns Ellptic Jacobi inverse NS function
*/
extern double __ins__( double ns , double x ) { return  __isn__( 1 / ns , x ); }

/*!
* \brief Elliptic Jacobi inverse DS function
* \param[ds] Elliptic Jacobi DS function
* \param[x] elliptical modulus
* \return  returns Ellptic Jacobi inverse DS function
*/
extern double __ids__( double ds , double x ) { return  __isd__( 1 / ds , x ); }

/*!
* \brief Elliptic Jacobi inverse CS function
* \param[cs] Elliptic Jacobi CS function
* \param[x] elliptical modulus
* \return  returns Ellptic Jacobi inverse CS function
*/
extern double __ics__( double cs , double x ) { return  __isc__( 1 / cs , x ); }

/*!
* \brief Elliptic integral of the first kind
* \param[k] elliptical modulus
* \return  returns Ellptic integral of the first kind
*/
extern double __ellip_k__( double k )
{
    long double a[ __ITERATIONS_NUMBER__ + 1 ] , g[ __ITERATIONS_NUMBER__ + 1 ];
    long double two_n;
    int  n;

    k = fabsl( ( long double )k);

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
        if (fabsl(a[n] - g[n]) < (a[n] * __minexponent__<long double>() )) break;
        two_n += two_n;
        a[n + 1] = 0.5L * (a[n] + g[n]);
        g[n + 1] = sqrtl(a[n] * g[n]);
    }

    return PI_2 / a[n];
}

/*!
* \brief Elliptic integral of the second kind
* \param[k] elliptical modulus
* \return  returns Ellptic integral of the second kind
*/
extern double __ellip_e__( double k )
{
    long double a[ __ITERATIONS_NUMBER__ + 1 ] ,  g[ __ITERATIONS_NUMBER__ + 1 ] , c[ __ITERATIONS_NUMBER__ + 1 ];
    long double two_n = 1.0L , sum = 0;
    int  n;

    k = fabsl( ( long double ) k );

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
        if (fabsl(a[n] - g[n]) < (a[n] * __minexponent__<long double>() ) ) break;
        two_n += two_n;
        a[n + 1] = 0.5L * (a[n] + g[n]);
        g[n + 1] = sqrtl(a[n] * g[n]);
        c[n + 1] = 0.5L * (a[n] - g[n]);
        sum = 0.5 * c[n + 1] * c[n + 1] * two_n + sum;
    }

    return PI_2*(1 - (sum + 0.5 * c[0] * c[0]))/ a[n];
}

/*! @} */


/*! \defgroup <BESSEL_FUNCTIONS> ( Bessel functions )
 *  \ingroup SPECIAL_MATH_FUNCTIONS
 *  \brief the module collaborates Bessel functions and factorial
    @{
*/

/*!
* \brief Factorial computation function
* \param[n] input n
* \return  returns n!
*/

extern long double __factorial__( long long n )
{
    long double out = 1;
    for( long long i = 1 ; i <= n ; i++ ) out *= i;
    return out;
}

/*!
* \brief Integer gamma function
* \param[n] input n
* \return  returns ( n - 1 )!
*/
extern unsigned long long __gamma_integer__( unsigned long long n ) { return __factorial__(n-1); }

/*!
* \brief Zero order Bessel function
* \param[x] input x
* \return  returns zero order Bessel function value
*/
extern long double __bessel_i0__( long double x )
{
    long double sum = 0 , fact_n = 0;
    for ( int n = 0 ; n < __ITERATIONS_NUMBER__ ; n++)
    {
        fact_n = powl( __factorial__( n ), 2);
        if ( fact_n == 0 ) break;
        sum = sum + pow(-1, n) * pow(x / 2, 2 * n) / fact_n;
    }

    return sum;
}

/*!
* First order Bessel function
* \param[x] input x
* \return  returns first order Bessel function value
*/
extern long double __bessel_i1__( long double x )
{
    long double sum = 0 , fact_n = 0 , fact_n_1 = 0;
    for (int n = 0; n < __ITERATIONS_NUMBER__ ; n++)
    {
        fact_n   = __factorial__(n);
        fact_n_1 = __factorial__(n + 1);
        if (fact_n == 0 || fact_n_1 == 0) break;
        sum = sum + pow(-1, n) * powl(x / 2, 2 * n) / fact_n / fact_n_1;
    }
    return sum;
}

/*!
* N-th order Bessel function
* \param[x] input x
* \param[order] Bessel function order
* \return  returns N-th order Bessel function value
*/
extern long double __bessel_in__( long double x  , int order )
{
    long double sum = 0 , fact_n = 0 , fact_n_order = 0;
    for ( int n = 0; n < __ITERATIONS_NUMBER__ ; n++)
    {
        fact_n       = __factorial__(n);
        fact_n_order = __factorial__(n + order);
        if (fact_n == 0 || fact_n_order == 0) break;
        sum = sum + powl(-1, n) * powl(x / 2, 2 * n + order) / fact_n / fact_n_order;
    }
    return sum;
}

/*!
* N-th order modified Bessel function
* \param[x] input x
* \param[order] Modified Bessel function order
* \return  returns N-th order modified Bessel function value
*/
extern long double __modified_bessel_in__( long double x  , int order )
{
    long double  sum = 0 , gamma1 = 0 , gamma2    = 0;
    for (int n = 0; n < 16; n++)
    {
        if ( ( long double )gamma2 / __maxexponent__<long double>() > 0.9 )
            break;

        gamma1    = __gamma_integer__(n + 1);
        gamma2    = __gamma_integer__(n + order + 1);
        sum       = sum + powl(x / 2, 2 * n + order) / gamma1 / gamma2;
    }
    return sum;
}

/*! @} */

/*! \defgroup <WINDOW_FUNCTIONS> ( Window functions )
 *  \ingroup SPECIAL_MATH_FUNCTIONS
 *  \brief the module describes windows functions class
    @{
*/

/*!
 * \brief Barlett window function computation
 * \param[_order] window function size
 * \return The function allocates memory sets, m_wind_ready = 1 and computes Barlett window coefficients as follows:
 *  \f[
 *      Ns = order \newline
 *      n = 0 \dots Ns \newline
 *      y( n ) = \begin{equation}
 *      \begin{cases}
 *      2 * n / ( Ns - 1 ) , n <= ( Ns - 1 ) / 2
 *      \\
 *      2 - 2 * n / ( Ns - 1 ) , n > ( Ns - 1 ) / 2
 *      \end{cases}
 *      \end{equation}
 *  \f]
*/
double* __Bartlett__( int _order )
{
    // memory allocation:
    double *buff = (double*)calloc( _order, sizeof(double) );
    // window function coefficients computation:
    double Ns = ( double ) _order;
    for ( int n = 0; n < Ns ; n++) { buff[n] = ( n <= ( Ns-1 ) / 2 ) ? ( 2*n / (Ns-1) ) : ( 2 - 2 * n / (Ns-1) ); }
    return buff;
}

/*!
 * \brief Barlett Hanning window function computation
 * \param[_order] window function size
 * \return The function allocates memory, sets m_wind_ready = 1 and computes Barlett-Hanning window coefficients as follows:
 *  \f[
 *      Ns = order \newline
 *      n  = 0 \dots Ns \newline
 *      y( n ) = 0.62 - 0.48 * \left|  \frac{ n }{ Ns - 1 } - 0.5 \right| + 0.38 * cos \left[ \ 2 * \pi * \left( \frac{ n }{ Ns - 1 } - 0.5 \right) \right]
 *  \f]
*/
double* __BartlettHanning__( int _order )
{
    // memory allocation:
    double *buff = (double*)calloc( _order, sizeof(double) );
    // window function coefficients computation:
    double Ns = (double)_order;
    for (int n = 0; n < Ns ; n++) { buff[n] = 0.62 - 0.48 * fabs( n / (Ns-1)-0.5) + 0.38 * cos(PI2 * (n / (Ns - 1) - 0.5)); }
    // window is ready to use:
    return buff;
}


/*!
     * \brief Balckman window function computation
     * \param[_order] window function size
     * \return The function allocates memory, sets m_wind_ready = 1 and computes Balckman window coefficients as follows:
     *  \f[
     *      Ns = order \newline
     *      n  = 0 \dots Ns \newline
     *      y( n ) = 0.42 - 0.50 * cos \left( 2 * \pi * \frac { n } { Ns - 1 } \right ) + 0.08 * cos \left( 4 * \pi * \frac{ n } { Ns - 1 } \right )
     *  \f]
*/
double* __Blackman__(int _order)
{
    // memory allocation:
    double *buff = (double*)calloc( _order, sizeof(double) );
    // window function coefficients computation:
    double Ns = (double)_order;
    for (int n = 0; n < Ns ; n++) { buff[n] = 0.42 - 0.50 * cos(2 * PI0 * n / (Ns - 1) ) + 0.08 * cos(4 * PI0 * n / (Ns - 1)); }
    // window is ready to use:
    return buff;
}

/*!
     * \brief Balckman-Harris window function computation
     * \param[_order] window function size
     * \return The function allocates memory, sets m_wind_ready = 1 and computes Balckman-Harris window coefficients as follows:
     *  \f[
     *      Ns = order \newline
     *      n  = 0 \dots Ns \newline
     *      y( n ) = 0.35875 - 0.48829 * cos \left( 2 * \pi * \frac{ n } { Ns - 1 } \right ) + 0.14128 * cos\left( 4 * \pi * \frac{ n } { Ns - 1 } \right ) - 0.01168 * cos \left( 6 * \pi * \frac{ n } { Ns - 1 } \right )
     *  \f]
    */
double* __BlackmanHarris__(int _order)
{
    // memory allocation:
    double *buff = (double*)calloc( _order, sizeof(double) );
    // window function coefficients computation:
    double Ns = (double)_order;
    for (int n = 0; n < Ns; n++) { buff[n] = 0.35875 - 0.48829 * cos(2 * PI0 * n / (Ns - 1)) + 0.14128 * cos(4 * PI0 * n / (Ns - 1)) - 0.01168 * cos(6 * PI0 * n / (Ns - 1)); }
    // window is ready to use:
    return buff;
}


/*!
     * \brief Bohman window function computation
     * \param[_order] window function size
     * \return The function allocates memory, sets m_wind_ready = 1 and computes Bohman window coefficients as follows:
     *  \f[
     *      Ns = ceil \left( \frac{ order } { 2 } \right ) \quad , \quad n  = 0 \dots Ns \newline
     *      y( n ) = \left( 1 - \left| \frac{ n } { Ns + 1} \right| \right) * cos \left( \pi * \left| \frac{ n } { Ns + 1 } \right| \right ) + \frac{ 1 } { \pi } * sin \left( \pi * \left| \frac { n } { Ns + 1 } \right| \right) \quad , \quad
     *      n = \begin{cases}
     *      n = k - Ns \quad \quad \quad , \quad n = 0
     *      \\
     *      n = k - Ns - 1 \quad , \quad n >= 0
     *      \end{cases}
     *  \f]
    */
double* __Bohman__(int _order)
{
    // memory allocation:
    double *buff = (double*)calloc( _order, sizeof(double) );

    // window function coefficients computation:
    double Ns = ceil( (double)_order / 2) , n = 0;
    for (int k = 0; k < _order; k++)
    {
        n = k - Ns;
        if ( n >= 0 )  n = k - Ns + 1;
        buff[k] = ( 1 - fabs( n / (Ns + 1) ) ) * cos( PI0 * fabs(n / (Ns + 1))) + 1 / PI0 * sin(PI0 * fabs(n / (Ns + 1)));
    }
    // window is ready to use:
    return buff;
}

/*!
     * \brief Auxiliary Chebhshev poly computation function
     * \param[n] Chebyshev poly order
     * \param[x] x-value at which n-th order Chebyshev poly value is computed
     * \return The function returns Chebyshev n-th order poly value
     *  \f[
     *      y( x , n ) =
     *      \begin{cases}
     *      cos \left( n * acos \left( x \right ) \right ) \quad , \quad |x| <= 1
     *      \\
     *      cos \left( n * acosh \left( x \right ) \right ) \quad , \quad |x| > 1
     *      \end{cases}
     *  \f]
    */
double __Chebyshev_poly__(int n, double x)
{
    double res;
    if (fabs(x) <= 1) res = cos(n*acos(x));
    else              res = cosh(n*acosh(x));
    return res;
}

/*!
     * \brief Chebyshev window computation function
     * \param[_atten] sidelobe attenuation , Db
     * \param[_order] window function size
     * \return The function allocates memory, sets m_wind_ready = 1 and computes Chebyshev window coefficients
    */
double* __Chebyshev__( double _atten , int _order )
{
    // memory allocation:
    double *buff = (double*)calloc( _order, sizeof(double) );

    // window function coefficients computation:
    int Ns = _order , nn, kk;
    double M, n, sum = 0, max = 0;
    double tg = pow(10, _atten / 20);
    double x0 = cosh((1.0 / (Ns - 1))*acosh(tg));
    M = (Ns - 1) / 2;

    if (Ns % 2 == 0) M = M + 0.5;
    for (nn = 0; nn < (Ns / 2 + 1); nn++)
    {
        n = nn - M;
        sum = 0;
        for (kk = 1; kk <= M; kk++)  { sum += __Chebyshev_poly__(Ns - 1, x0*cos(PI0*kk / Ns))*cos(2.0*n*PI0*kk / Ns); }
        buff[nn] = tg + 2 * sum;
        buff[(int)Ns - nn - 1] = buff[nn];
        if (buff[nn] > max)max = buff[nn];
    }
    for (nn = 0; nn < Ns; nn++) buff[nn] /= max;

    // window is ready to use:
    return buff;
}


/*!
     * \brief Flat-Top window computation function
     * \param[_order] window function size
     * \return The function allocates memory, sets m_wind_ready = 1 and computes Flat-Top window coefficients as follows:
     *  \f[
     *      Ns = order \newline
     *      n  = 0 \dots Ns \newline
     *      y( n ) = 0.21557895 - 0.41663158 * cos \left( ( 2 * \pi * \frac{ n } { Ns - 1 } \right)
     *      + 0.277263158 * cos \left( 4 * \pi * \frac{ n } { Ns - 1 } \right)
     *      - 0.083578947 * cos \left( 6 * \pi * \frac{ n } { Ns - 1 } \right)
     *      + 0.006947368 * cos \left( 8 * \pi * \frac{ n } { Ns - 1 } \right)
     *  \f]
    */
double* __FlatTop__(int _order)
{
    // memory allocation:
    double *buff = (double*)calloc( _order, sizeof(double) );

    // window coefficients computation:
    double Ns = (double)_order;
    for (int n = 0 ; n < Ns; n++)
    {
        buff[n] = 0.21557895 - 0.41663158 * cos(PI2 * n / (Ns - 1) ) +
                0.277263158 * cos(4 * PI0 * n / (Ns - 1) ) -
                0.083578947 * cos(6 * PI0 * n / (Ns - 1) ) +
                0.006947368 * cos(8 * PI0 * n / (Ns - 1) );
    }

    // window is ready to use:
    return buff;
}


/*!
     * \brief Gaussian window computation function
     * \param[_alpha] Gaussian window parameter
     * \param[_order] window function size
     * \return The function allocates memory, sets m_wind_ready = 1 and computes Gaussian window coefficients as follows:
     *  \f[
     *      Ns = order      \newline
     *      \sigma = 0      \newline
     *      n  = 0 \dots Ns \newline
     *      y( n ) = e^ \left( \frac{ n^2 }{ 2 * \sigma^2 } \right) \newline
     *      \sigma = \frac{ Ns-1 }{ 2 * \alpha } \newline
     *      n =
     *      \begin{cases}
     *      k - \frac{ Ns }{ 2 } \quad , \quad n < 0
     *      \\
     *      k - \frac{ Ns }{ 2 } + 1 \quad , \quad n \geq 0
     *      \end{cases}
     *  \f]
    */
double* __Gaussian__( double _alpha, int _order )
{
    // memory allocation:
    double *buff = (double*)calloc( _order, sizeof(double) );

    // window function coefficients computation:
    int Ns     = _order;
    int n      = 0;
    double sigma  = 0;

    for (int k = 0 ; k < Ns; k++)
    {
        n = k - Ns/2;
        if (n >= 0)  n = k - Ns/2 + 1;
        sigma = ((double)Ns-1) / 2 / _alpha;
        buff[k] = exp(-(double)n * (double)n / 2 / sigma / sigma);
    }

    // wind is ready to use:
    return buff;
}

/*!
     * \brief Hamming window computation function
     * \param[_order] window function size
     * \return The function allocates memory, sets m_wind_ready = 1 and computes Hamming window coefficients as follows:
     *  \f[
     *      Ns = order      \newline
     *      n  = 0 \dots Ns \newline
     *      y( n ) = 0.54 - 0.46 * cos \left( 2 * \pi * \frac{ n } { Ns-1 } \right)
     *  \f]
    */
double* __Hamming__(int _order )
{
    // memory allocation:
    double *buff = (double*)calloc( _order, sizeof(double) );

    // window function coefficients computation:
    int Ns = _order;
    for (int n = 0 ; n < Ns ; n++) buff[n] = 0.54 - 0.46 * cos(PI2 * n / (Ns-1));

    // window is ready to use:
    return buff;
}

/*!
     * \brief Hann window computation function
     * \param[_order] window function size
     * \return The function allocates memory, sets m_wind_ready = 1 and computes Hann window coefficients as follows:
     *  \f[
     *      Ns = order      \newline
     *      n  = 0 \dots Ns \newline
     *      y( n ) = 0.5 - 0.5 * cos \left( 2 * \pi * \frac{ n } { Ns-1 } \right)
     *  \f]
    */
double* __Hann__(int _order)
{
    // memory allocation:
    double *buff = (double*)calloc( _order, sizeof(double) );

    // window function coefficients computation:
    int Ns = _order;
    for (int n = 0; n < Ns; n++) buff[n] = 0.5 - 0.5 * cos( PI2 * n / (Ns-1) );

    // window is ready to use:
    return buff;
}


/*!
     * \brief Kaiser window computation function
     * \param[_betta] Kaiser window function parameter
     * \param[_order] window function size
     * \return The function allocates memory, sets m_wind_ready = 1 and computes Kaiser window coefficients as follows:
     *  \f[
     *      Ns = order                                                                      \newline
     *      n  = 0 \dots Ns                                                                 \newline
     *      A  = \frac{ \left( n - \frac{ Ns - 1 } { 2 } \right) }{ \frac{ Ns - 1 } { 2 } } \newline
     *      B  = \beta * \sqrt( 1 - A^2 )                                                   \newline
     *      C  = \beta                                                                      \newline
     *      y(n) = \frac{ ModifiedBessel( B , 0 ) }{ ModifiedBessel( C , 0 ) }
     *  \f]
    */
double* __Kaiser__(double _betta, int _order )
{
    // memory allocation:
    double *buff = (double*)calloc( _order, sizeof(double) );

    // winfow function coefficients computation:
    int Ns = _order;
    double A  = 0;
    double B  = 0;
    double C  = 0;
    for (int n = 0; n < Ns; n++)
    {
        A = ((double)n - ( (double)Ns - 1 ) / 2) / ( ((double)Ns - 1 ) / 2);
        B = _betta * sqrt(1 - A * A);
        C = _betta;
        buff[n] = __modified_bessel_in__( B , 0 ) / __modified_bessel_in__( C , 0 );
    }

    // window is ready to use:
    return buff;
}

/*!
     * \brief Nutall window computation function
     * \param[_order] window function size
     * \return The function allocates memory, sets m_wind_ready = 1 and computes Nutall window coefficients as follows:
     *  \f[
     *      Ns = order      \newline
     *      n  = 0 \dots Ns \newline
     *      y(n) = 0.3635819
     *      - 0.4891775 * cos \left( 2 * \pi * \frac{ n }{ Ns - 1 } \right)
     *      + 0.1365995 * cos \left( 4 * \pi * \frac{ n }{ Ns - 1 } \right)
     *      - 0.0106411 * cos \left( 6 * \pi * \frac{ n }{ Ns - 1 } \right)
     *  \f]
    */
double* __Nutall__(int _order)
{
    // memory allocation:
    double *buff = (double*)calloc( _order, sizeof(double) );

    // window function coefficients computation:
    int Ns = _order;
    for (int n = 0; n < Ns; n++)
    {
        buff[n] = 0.3635819 -
                0.4891775 * cos(2 * PI0 * n / (Ns - 1) ) +
                0.1365995 * cos(4 * PI0 * n / (Ns - 1)) -
                0.0106411 * cos(6 * PI0 * n / (Ns - 1));
    }

    // window is ready to use:
    return buff;
}

/*!
     * \brief Parzen window computation function
     * \param[_order] window function size
     * \return The function allocates memory, sets m_wind_ready = 1 and computes Parzen window coefficients as follows:
     *  \f[
     *      Ns = order      \newline
     *      n  = 0 \dots Ns \newline
     *      y(n) =
     *      \begin{cases}
     *
     *      1 - 6 * \left( \frac{ |n| } { \left( Ns / 2 \right) } \right)^2
     *      + 6 * \left( \frac{ |n| } { \left( Ns / 2 \right) } \right)^3 \quad , \quad 0 \leq |n| \leq \frac{ Ns - 1 }{ 4 }
     *      \\
     *      2 * \left( \frac{ |n| } { \left( Ns / 2 \right) } \right)^3 \quad  , \quad \frac{ Ns - 1 }{ 4 } < |n| \leq \frac{ Ns - 1 }{ 2 }
     *
     *      \end{cases}
     *
     *  \f]
    */
double* __Parzen__(int _order)
{
    // memory allocation:
    double *buff = (double*)calloc( _order, sizeof(double) );

    // window function coefficients computation:
    int    Ns = _order;
    int    n = 0;
    for (int k = 0; k < Ns; k++)
    {
        n = k - Ns / 2;
        if (n >= 0)  n = k - Ns / 2 + 1;

        if ( abs(n)>=0 && abs(n) <= (Ns-1)/4 )
        {
            buff[k] = 1 - 6 * fabs((double)n)*fabs((double)n) / ((double)Ns*(double)Ns / 4) +
                    6 * fabs((double)n)*fabs((double)n)*fabs((double)n) / ((double)Ns*(double)Ns*(double)Ns / 8);
        }
        else if( abs(n) < Ns/2 && abs(n) > (Ns - 1) / 4 )
        {
            buff[k] = 2 * pow( 1 - fabs((double)n)/((double)Ns/2) , 3);
        }
    }

    // window is ready:
    return buff;
}

/*!
     * \brief Rectangular window computation function
     * \param[_order] window function size
     * \return The function allocates memory, sets m_wind_ready = 1 and computes Rectangular window coefficients as follows:
     *  \f[
     *      Ns = order      \newline
     *      n  = 0 \dots Ns \newline
     *      y(n) = 1
     *  \f]
    */
double* __Rectangular__(int _order)
{
    // memory allocation:
    double *buff = (double*)calloc( _order, sizeof(double) );

    // window coefficients vomputation:
    for (int n = 0; n < _order; n++) buff[n] = 1;

    // window is ready to use:
    return buff;
}

/*!
     * \brief Triangular window computation function
     * \param[_order] window function size
     * \return The function allocates memory, sets m_wind_ready = 1 and computes Triangular window coefficients as follows:
     *  \f[
     *      Ns = order      \newline
     *      n  = 0 \dots Ns \newline
     *      y(n) =
     *      \begin{cases}
     *      1 - \left| \frac{ n - \left( Ns - 1 \right) / 2 }{ \left( Ns / 2 \right) } \right| \quad , \quad Ns \mod 2 = 0
     *      \\
     *      1 - \left| \frac{ n - \left( Ns - 1 \right) / 2 }{ \left( ( Ns + 1 ) / 2 \right) } \right| \quad , \quad Ns \mod 2 \neq 0
     *      \end{cases}
     *  \f]
    */
double* __Triangular__(int _order)
{
    // memory allocation:
    double *buff = (double*)calloc( _order, sizeof(double) );

    // window function coefficients computation:
    int Ns = _order;
    if ( Ns % 2 == 0)
    {
        for (int n = 0; n < Ns; n++)
        {
            buff[n] = (1.0 - fabs(((double)n - ((double)Ns - 1) / 2) / ((((double)Ns - 1) + 1) / 2)));
        }
    }
    else
    {
        for (int n = 0; n < Ns; n++)
        {
            buff[n] = (1.0 - fabs(((double)n - ((double)Ns - 1) / 2) / ((((double)Ns - 1) + 2) / 2)));
        }
    }

    // window is ready to use:
    return buff;
}

/*!
     * \brief Tukey window computation function
     * \param[_R] Tukey window function parameter
     * \param[_order] window function size
     * \return The function allocates memory, sets m_wind_ready = 1 and computes Tukey window coefficients as follows:
     *  \f[
     *      Ns = order      \newline
     *      n  = 0 \dots Ns \newline
     *      y \left( x = \frac{ n }{ Ns - 1 } \right) =
     *      \begin{cases}
     *      0.5 + 0.5 * cos \left( \frac{ 2 * \pi }{ R * ( x - 0.5*R ) } \right) \quad , \quad  0 \geq x < R / 2;
     *
     *      \\
     *
     *      0.5 + 0.5 * cos \left( \frac{ 2 * \pi }{ R * ( x - 1 + 0.5*R ) } \right) \quad , \quad  1 - 0.5*R \geq x \leq 1;
     *
     *      \\
     *
     *      1
     *
     *      \end{cases}
     *  \f]
    */
double* __Tukey__(double _R, int _order )
{
    // memory allocation:
    double *buff = (double*)calloc( _order, sizeof(double) );

    // window function coefficients computation:
    int Ns = _order;
    double x  = 0;
    for(int n = 0 ; n < Ns ; n++)
    {
        x = (double)n / ((double)Ns - 1);
        if ( x >=0 && x < _R / 2 )
        {
            buff[n] = 0.5 + 0.5 * cos( PI2 / _R * ( x - 0.5*_R ) );
        }
        else if ( x >= 1 - 0.5 * _R && x <= 1 )
        {
            buff[n] = 0.5 + 0.5 * cos(PI2 / _R * (x - 1 + 0.5*_R) );
        }
        else
        {
            buff[n] = 1;
        }
    }

    // window is ready to use:
    return buff;
}

/*!
     * \brief Linear convolution function
     * \param[_Na] size of the first input polynom of a linear convolution function
     * \param[_Nb] size of the second input polynom of a linear convolution function
*/
int __conv_size__(int _Na,  int _Nb)
{
    return _Na + _Nb - 1;
}

/*!
     * \brief Linear convolution function
     * \param[_a] pointer to the poly a array
     * \param[_b] pointer to the poly b array
     * \param[_c] pointer to the poly c array
     * \param[_Na] size of poly a array
     * \param[_Nb] size of poly b array
     * \param[_Nc] size of poly c array
*/
template< typename __type > void
__convf__( const __type *_a, const __type *_b, __type *_c, int _Na,  int _Nb, int _Nc )
{
    if( ( _Nc >= _Na + _Nb - 1 ) && ( _a && _b && _c ) )
    {
        for( int i = 0 ; i < ( _Na + _Nb ) ; i++ )
        {
            for( int j = ( i - _Na < 0 ) ? 0 : i-_Na+1 , k = ( i < _Na ) ? i : _Na-1 ; ( j < _Nb ) && ( k >= 0 ) ; j++ , k-- )
            {
                _c[i] += _a[k] * _b[j];
            }
        }
    }
}

/*!
     * \brief Linear convolution function
     * \param[_a] pointer to the poly a array
     * \param[_b] pointer to the poly b array
     * \param[_Na] size of poly a array
     * \param[_Nb] size of poly b array
     * \returns The function returns the tuple_x2
     *          containing resulting convulution vector and it's size
*/
template< typename __type > Tuple<__type*, int>
__convf__( const __type *_a, const __type *_b, int _Na,  int _Nb )
{
    int _Nc = _Na + _Nb - 1;
    __type *_c = __alloc__<__type>(_Nc);
    __convf__( _a, _b, _c , _Na, _Nb, _Nc );

    return { _c, _Nc };
}

/*!
     * \brief Numeric fraction/fraction substitution function
     * \param[AN] numerator of the first fraction
     * \param[AD] denominator of the first fraction
     * \param[BN] numerator of the fraction to substitute into the first fraction
     * \param[BD] denominator of the fraction to substitute into the first fraction
     * \param[N] the numerator/denominator size of the first fraction
     * \param[P] the numerator/denominator size of the fraction to substitute into the first fraction
     * \returns The function returns the tuple_x3
     *          containing resulting fraction array and it's first and second dimention sizes
*/

template< typename __type >
struct fraction
{
    __type** data      = nullptr;
    int      positions = -1;
    int      sections  = -1;
};

template< typename __type > fraction<__type>
__mfree__( fraction<__type> tuple )
{
    for(int i = 0 ; i < tuple.positions ; i++)
    {
        tuple.data[i] = __mfree__( tuple.data[i] );
    }

    tuple.data      = __mfree__( tuple.data );
    tuple.positions = -1;
    tuple.sections  = -1;

    return tuple;
}

template<typename __type> fraction<__type>
__fraction_numeric_substitution__(__type *AN, __type *AD, __type *BN, __type *BD, int N, int P)
{
    // decrement orders to omit zero power elements
    N--;
    P--;

    // identify martix dimensions
    int nrows = N+1;
    int ncols = N*P+1;

    // allocate memory
    int *Ap = __alloc__<int>( ncols );
    int *Bp = __alloc__<int>( ncols );
    __type *Ax = __alloc__<__type>( nrows * ncols );
    __type *Bx = __alloc__<__type>( nrows * ncols );
    __type *Cx = __alloc__<__type>( nrows * ncols );
    __type *Nx = __alloc__<__type>( ncols );
    __type *Dx = __alloc__<__type>( ncols );

    // Compute numerator/denominator powers matrixes
    Ax[0] = 1;
    Bx[0] = 1;
    Ap[0] = 1;
    Bp[0] = 1;
    for(int i = 1, Na = 1, Nb = (P+1), Nc = ncols ; i < nrows ; i++ )
    {
        __convf__( &Ax[(i-1)*ncols], BN, &Ax[i*ncols], Na, Nb, Nc );
        __convf__( &Bx[(i-1)*ncols], BD, &Bx[i*ncols], Na, Nb, Nc );
        Na = __conv_size__(Na, Nb);
        Ap[i] = Na;
        Bp[i] = Na;
    }

    // convolve
    for(int i = 0; i < nrows ; i++)
    {
        __convf__( &Bx[(N-i)*ncols], &Ax[i*ncols], &Cx[i*ncols], Ap[N-i], Bp[i], ncols );
    }

    // compute the resulting poly coefficients
    for( int j = 0 ; j < ncols ; j++ )
    {
        for( int i = 0 ; i < nrows ; i++ )
        {
            Nx[j] += Cx[i*ncols+j] * AN[i];
            Dx[j] += Cx[i*ncols+j] * AD[i];
        }
    }

    // free memory
    __mfree__(Ap);
    __mfree__(Bp);
    __mfree__(Ax);
    __mfree__(Bx);
    __mfree__(Cx);

    // generate the output
    __type** output = __alloc__<__type*>(2);
    output[0] = Nx;
    output[1] = Dx;
    return { output, 2, ncols };
}

/*!
     * \brief Numeric fraction/fraction substitution function
     * \param[input] input samples array
     * \param[output] output samples array
     * \param[Gain] output gain
     * \param[M] input array size
     * \param[N] output array size
     * \param[order] interpolation order
     * \details The function generates interpolated output of input signal multiplied by gain.
     *          The function supports linear, quad and cubic interpolation.
*/
template< typename __type >
__type get_data( __type* _Data, int _Size, int _Index )
{
    if( _Data == nullptr || _Size <= 0 )
        return 0.0;

    if( _Index < 0 )
        return _Data[ _Index + _Size ];

    if( _Index >= _Size )
        return _Data[ _Index - _Size ];

    return _Data[ _Index ];
}

double lagrange_polynom_interpolation( double* _X, double* _Y, double _Point, int _N )
{
    // check
    if( _X == nullptr || _N <= 0 )
        return 0.0;

    double result = 0.0;

    for( int i = 0 ; i < _N ; i++ )
    {
        double product = 1;

        for( int j = 0 ; j < _N ; j++ )
        {
            if( i != j )
            {
                product *= ( _Point - _X[j] ) / ( _X[i] - _X[j] );
            }
        }

        result += product * _Y[i];
    }

    return result;
}

template<typename T1, typename T2, typename T3> void
samples_range_interpolation(T1 _Input, T2 _Output, T3 _Gain, int _M, int _N, int _K, int _Order)
{
    // check
    //if( _N < _M * _K || _K == 1 ) return;

    // time step
    double dN = (double)_M / (double)_N;

    for( int i = _Order, k = 0 ; i < _M ; i++ )
    {
        double X[_Order+1];
        double Y[_Order+1];

        for( int j = _Order, m = 0 ; j >= 0 ; j--, m++ )
        {
            X[m] = (double)( i - j );
            Y[m] = _Input[ (int)X[m] ];
        }

        for( int m = 0 ; m < _Order+1 ; m++ )
        {
            for( int j = 0 ; j < _K ; j++, k++ )
            {
                _Output[k] = lagrange_polynom_interpolation( X, Y, ( X[m] + (double)j * dN ), _Order + 1 ) * _Gain;
            }

            if( i != _M-1 )
                break;
        }
    }
}

template<typename __InputType, typename __OutputType> void
samples_range_decimation(__InputType _Input, __OutputType _Output, int _M, int _N, int _K)
{
    // check
    if( _Input == nullptr || _Output == nullptr || _N > _M / _K || _K == 1 )
        return;

    // main code
    for( int i = 0, j = 0 ; i < _M ; i += _K, j++ )
    {
        _Output[j] = _Input[i];
    }
}

/*!
     * \brief Discrete Fourier transform
     * \param[_Input] input samples array
     * \param[_Spectrum] output samples array
     * \param[_N] input/spectrum array size
     * \param[_Direct] direct / inverse transform
     * \details The function computes discrete Fourier transform
*/
template<typename __type> void
dft( Complex<__type>* _Input, Complex<__type>* _Spectrum, int _N, int _Direct )
{
    // check
    if( _Input == nullptr || _Spectrum == nullptr || _N <= 0 )
        return;

    // compute
    for( int i = 0 ; i < _N ; i++ )
    {
        double angle = PI2 * (float)i / (float)_N * (_Direct ? -1.0 : +1.0);
        Complex<__type> Wn = Complex<__type>( cos( angle ), sin( angle ) );
        Complex<__type> W1(1,0);

        for( int j = 0 ; j < _N ; j++ )
        {
            _Spectrum[i] += _Input[j] * W1;
            W1 *= Wn;
        }
    }

    // scale zero component
    _Spectrum[0] /= 2.0;
}

inline bool check_power_of_two( int _Number )
{
    return !(bool)( _Number & (_Number-1) );
}

/*!
     * \brief Fast Fourier transform
     * \param[_Spectrum] input samples array
     * \param[_N] signal array size
     * \param[direct] direct / inverse transform
     * \details The function uses fast Fourier transform algorithm with frequency domain decimation.
     *          If the flag direct == 1 the function computes direct Fourier transform and fills input [spectrum] array
     *          with the [spectrum] array signal spectrum. if the flag direct == 0 the function computes inverse Fourier transform
     *          and fills [spectrum] array with the values of the result. If [_Spectrum] array size is not power-of-two function quits.
     *          It such case use dft() function.
*/
template<typename __type> void
fft0( Complex<__type>* _Spectrum, int _N, int _Direct )
{
    // check
    if( _Spectrum == nullptr || _N <= 0 || !check_power_of_two(_N) )
        return;

    // initialization
    double G = _Direct ? 2.0 / ( double )_N : 0.5;

    // scale spectrum
    for( int i = 0 ; i < _N ; i++ )
    {
        _Spectrum[i] = _Spectrum[i] * G;
    }

    // compute FFT
    for(int L = _N; L >= 2 ; L /= 2 )
    {
        double angle = PI2/L * (_Direct ? -1.0 : +1.0);
        Complex<__type> Wn( cos(angle), sin(angle) );
        for( int i = 0; i < _N; i += L )
        {
            Complex<__type> *pointer = &_Spectrum[i];
            Complex<__type> W1(1,0);
            for( int j = 0, k = L / 2 ; j < L / 2 ; j++, k++ )
            {
                Complex<__type> S0 = pointer[j] + pointer[k];
                Complex<__type> S1 = pointer[j] - pointer[k];
                pointer[j] = S0;
                pointer[k] = S1 * W1;
                W1 *= Wn;
            }
        }
    }

    // spectrum reordering
    for (int i = 1, j = 0 ; i < _N ; i++)
    {
        int bit = _N >> 1;

        for ( ; j >= bit; bit >>= 1 )
        {
            j -= bit;
        }

        j += bit;

        if ( i < j )
        {
            swap ( _Spectrum[i], _Spectrum[j] );
        }
    }

    // scale zero component
    _Spectrum[0] /= 2.0;
}

/*!  \example example_math_special_functions_fft.h */

/*!
     * \brief Fast Fourier transform
     * \param[_Spectrum] input samples array
     * \param[_N] signal array size
     * \param[direct] direct / inverse transform
     * \details The function uses fast Fourier transform algorithm with time domain decimation.
     *          If the flag direct == 1 the function computes direct Fourier transform and fills input [spectrum] array
     *          with the [spectrum] array signal spectrum. if the flag direct == 0 the function computes inverse Fourier transform
     *          and fills [spectrum] array with the values of the result. If [_Spectrum] array size is not power-of-two function quits.
     *          It such case use dft() function.
*/
template<typename __type> void
fft1( Complex<__type>* _Spectrum, int _N, int _Direct )
{
    // check
    if( _Spectrum == nullptr || _N <= 0 || !check_power_of_two(_N) )
        return;

    // initialization
    double G = _Direct ? 2.0 / _N : 0.5;

    // scale spectrum
    for( int i = 0 ; i < _N ; i++ )
    {
        _Spectrum[i] = _Spectrum[i] * G;
    }

    // spectrum reordering
    for (int i = 1, j = 0 ; i < _N ; i++)
    {
        int bit = _N >> 1;

        for ( ; j >= bit; bit >>= 1 )
        {
            j -= bit;
        }

        j += bit;

        if ( i < j )
        {
            swap ( _Spectrum[i], _Spectrum[j] );
        }
    }

    // compute FFT
    for( int L = 2; L <= _N; L<<=1 )
    {
        double angle = PI2/L * (_Direct ? -1.0 : +1.0);
        Complex<__type> Wn ( cos(angle), sin(angle) );
        for ( int i = 0; i < _N; i+=L)
        {
            Complex<__type> W1(1,0);

            for ( int j=0 ; j < L / 2 ; j++ )
            {
                Complex<__type> S0 = _Spectrum[i+j];
                Complex<__type> S1 = _Spectrum[i+j+L/2] * W1;
                _Spectrum[i+j] = S0 + S1;
                _Spectrum[i+j+L/2] = S0 - S1;
                W1 *= Wn;
            }
        }
    }

    // scale zero component
    _Spectrum[0] /= 2.0;
}

/*!  \example example_math_special_functions_fft.h */
extern long __euclide_algorithm__(long a, long b)
{
    while (a && b)
    {
        if (a >= b)
        {
            a %= b;
        }
        else
        {
            b %= a;
        }
    }

    return a | b;
}

/*! @} */

// customized variables exclusion to avloid aliasing during compilation:
#undef __ITERATIONS_NUMBER__

#endif // SPECIAL_FUNCTIONS_H
