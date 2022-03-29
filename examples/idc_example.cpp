#include "include/special_functions.h"
#include "iostream"

int main()
{
    // Matlab elliptic functions use M = k * k parameter. Be carefull !!!
    double M  = 0.7;
    double k  = sqrt(M);
    double u  = 2;
    double fc = __dc__( u , k );
    printf("dc  = %.4f  \n", fc );
    printf("idc = %.4f \n", __idc__( fc , k ) );

    return 0;
}