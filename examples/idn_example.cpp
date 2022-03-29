#include "include/special_functions.h"
#include "iostream"

int main()
{
    // Matlab elliptic functions use M = k * k parameter. Be carefull !!!
    double M  = 0.7;
    double k  = sqrt(M);
    double u  = 2;
    double fc = __dn__( u , k );
    printf("sn = %.4f  \n", fc );
    printf("idn = %.4f \n", __idn__( fc , k ) );

    return 0;
}