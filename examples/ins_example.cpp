#include "include/special_functions.h"
#include "iostream"

int main()
{
    // Matlab elliptic functions use M = k * k parameter. Be carefull !!!
    double M  = 0.7;
    double k  = sqrt(M);
    double u  = 2;
    double fc = __ns__( u , k );
    printf("ns  = %.4f  \n", fc );
    printf("ins = %.4f \n", __ins__( fc , k ) );

    return 0;
}