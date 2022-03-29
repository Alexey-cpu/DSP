#include "include/special_functions.h"
#include "iostream"

int main()
{
    // Matlab elliptic functions use M = k * k parameter. Be carefull !!!
    double M  = 0.7;
    double k  = sqrt(M);
    double u  = 2;
    double fc = __nc__( u , k );
    printf("nc  = %.4f  \n", fc );
    printf("inc = %.4f \n", __inc__( fc , k ) );

    return 0;
}