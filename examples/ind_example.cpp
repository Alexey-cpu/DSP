#include "include/special_functions.h"
#include "iostream"

int main()
{
    // Matlab elliptic functions use M = k * k parameter. Be carefull !!!
    double M  = 0.7;
    double k  = sqrt(M);
    double u  = 2;
    double fc = __nd__( u , k );
    printf("nd = %.4f  \n", fc );
    printf("ind = %.4f \n", __ind__( fc , k ) );

    return 0;
}