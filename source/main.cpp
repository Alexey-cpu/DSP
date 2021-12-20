
#include <iostream>
#include "include/special_fcn.h"
#include "include/special_functions.hpp"
#include "include/buffer.h"

int test0()
{
    special_functions f_ellip;

    // arguments:
    double u = 0.25;
    double k = 0.5;

    // elliptic functions:
    std::cout << " arguments : " << "\n";
    std::cout << "u = " << u << "\t" << "k = " << k << "\n" << "\n";
    std::cout << " elliptic functions: " << "\n";
    std::cout << " CN = " << f_ellip.cn( u , k ) <<  "\t" << __cn__( u , k ) << "\n";
    std::cout << " SN = " << f_ellip.sn( u , k ) <<  "\t" << __sn__( u , k )  << "\n";
    std::cout << " DN = " << f_ellip.dn( u , k ) <<  "\t" << __dn__( u , k )  << "\n";
    std::cout << " CD = " << f_ellip.cd( u , k ) <<  "\t" << __cd__( u , k )  << "\n";
    std::cout << " SD = " << f_ellip.sd( u , k ) <<  "\t" << __sd__( u , k )  << "\n";
    std::cout << " ND = " << f_ellip.nd( u , k ) <<  "\t" << __nd__( u , k )  << "\n";
    std::cout << " DC = " << f_ellip.dc( u , k ) <<  "\t" << __dc__( u , k )  << "\n";
    std::cout << " NC = " << f_ellip.nc( u , k ) <<  "\t" << __nc__( u , k )  << "\n";
    std::cout << " SC = " << f_ellip.sc( u , k ) <<  "\t" << __sc__( u , k )  << "\n";
    std::cout << " NS = " << f_ellip.ns( u , k ) <<  "\t" << __ns__( u , k )  << "\n";
    std::cout << " DS = " << f_ellip.ds( u , k ) <<  "\t" << __ds__( u , k )  << "\n";
    std::cout << " CS = " << f_ellip.cs( u , k ) <<  "\t" << __cs__( u , k )  << "\n";
    std::cout << "\n";

    // inverse elliptic functions:
    std::cout << " inverse elliptic functions: " << "\n";
    std::cout << " inv_CN = " << f_ellip.inv_cn(f_ellip.cn( u , k ) , k ) << "\t" << __icn__( __cn__( u , k ) , k ) <<  "\n";
    std::cout << " inv_SN = " << f_ellip.inv_sn(f_ellip.sn( u , k ) , k ) << "\t" << __isn__( __sn__( u , k ) , k ) << "\n";
    std::cout << " inv_DN = " << f_ellip.inv_dn(f_ellip.dn( u , k ) , k ) << "\t" << __idn__( __dn__( u , k ) , k ) << "\n";
    std::cout << " inv_CD = " << f_ellip.inv_cd(f_ellip.cd( u , k ) , k ) << "\t" << __icd__( __cd__( u , k ) , k ) << "\n";
    std::cout << " inv_SD = " << f_ellip.inv_sd(f_ellip.sd( u , k ) , k ) << "\t" << __isd__( __sd__( u , k ) , k ) << "\n";
    std::cout << " inv_ND = " << f_ellip.inv_nd(f_ellip.nd( u , k ) , k ) << "\t" << __ind__( __nd__( u , k ) , k ) << "\n";
    std::cout << " inv_DC = " << f_ellip.inv_dc(f_ellip.dc( u , k ) , k ) << "\t" << __idc__( __dc__( u , k ) , k ) << "\n";
    std::cout << " inv_NC = " << f_ellip.inv_nc(f_ellip.nc( u , k ) , k ) << "\t" << __inc__( __nc__( u , k ) , k ) << "\n";
    std::cout << " inv_SC = " << f_ellip.inv_sc(f_ellip.sc( u , k ) , k ) << "\t" << __isc__( __sc__( u , k ) , k ) << "\n";
    std::cout << " inv_NS = " << f_ellip.inv_ns(f_ellip.ns( u , k ) , k ) << "\t" << __ins__( __ns__( u , k ) , k ) << "\n";
    std::cout << " inv_DS = " << f_ellip.inv_ds(f_ellip.ds( u , k ) , k ) << "\t" << __ids__( __ds__( u , k ) , k ) << "\n";
    std::cout << " inv_CS = " << f_ellip.inv_cs(f_ellip.cs( u , k ) , k ) << "\t" << __ics__( __cs__( u , k ) , k ) << "\n";
    std::cout << "\n";

    // elliptic integrals:
    std::cout << " elliptic integrals: " << "\n";
    std::cout << " K = " << f_ellip.ellip_K( u ) << "\t" << __ellip_k__( u ) << "\n";
    std::cout << " E = " << f_ellip.ellip_E( u ) << "\t" << __ellip_e__( u ) << "\n";
    std::cout << "\n";

    // Bessel functions:
    double ba = 10;
    int    nn = 5;
    std::cout << " bessel_I0          = " << f_ellip.bessel_I0( ba ) << "\t" << __bessel_i0__( ba ) << "\n";
    std::cout << " bessel_I1          = " << f_ellip.bessel_I1( ba ) << "\t" << __bessel_i1__( ba ) << "\n";
    std::cout << " bessel_In          = " << f_ellip.bessel_In( ba , nn ) << "\t" << __bessel_in__( ba , nn ) << "\n";
    std::cout << " modified_bessel_In = " << f_ellip.mod_bessel_In( ba , nn ) << "\t" << __modified_bessel_in__( ba , nn ) << "\n";
    std::cout << "\n";

    return 0;
}

int test1()
{
    typedef double __type;
    int N0 = 15;
    int N1 = 10;
    __type *a = new __type [N0];

    mirror_ring_buffer<__type> buff;
    buff.allocate( N1 );

    std::cout << "a: " << "\n";
    for( int i = 0 ; i < N0 ; i++ ) std::cout << ( a[i] = i ) << "\t";
    std::cout << "\n";

    for( int i = 0 ; i < N0 ; i++ )
    {
        buff.fill_buff( &a[i] );
    }

    std::cout << "buffer: " << "\n";
    for( int i = 0 ; i < N1 ; i++ ) std::cout <<  buff[i] << "\t";
    std::cout << "\n";

    buff.deallocate();
    delete [] a;
    a= nullptr;

    return 0;
}

int main()
{
    test1();
    return 0;
}
