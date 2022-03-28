#include "include/special_functions.h"
#include "iostream"

int main()
{
    // Matlab elliptic functions use M = k * k parameter. Be carefull !!!
    double M = 0.7;
    double k = sqrt(M);
    double u = 2;

    printf("sn = %.4f \n", __sn__( u , k ) );
    printf("cn = %.4f \n", __cn__( u , k ) );
    printf("dn = %.4f \n", __dn__( u , k ) );
    printf("cd = %.4f \n", __cd__( u , k ) );
    printf("sd = %.4f \n", __sd__( u , k ) );
    printf("nd = %.4f \n", __nd__( u , k ) );
    printf("dc = %.4f \n", __dc__( u , k ) );
    printf("nc = %.4f \n", __nc__( u , k ) );
    printf("sc = %.4f \n", __sc__( u , k ) );
    printf("ns = %.4f \n", __ns__( u , k ) );
    printf("ds = %.4f \n", __ds__( u , k ) );
    printf("cs = %.4f \n", __cs__( u , k ) );

    return 0;
}