#include "include/special_functions.h"
#include "iostream"

int main()
{
    // Matlab elliptic functions use M = k * k parameter. Be carefull !!!
    double M  = 0.7;
    double k  = sqrt(M);
    double u  = 2;
    double fc = __cn__( u , k );
    printf("sn = %.4f  \n", fc );
    printf("icn = %.4f \n", __icn__( fc , k ) );

    return 0;
}