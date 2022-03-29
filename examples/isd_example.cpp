#include "include/special_functions.h"
#include "iostream"

int main()
{
    // Matlab elliptic functions use M = k * k parameter. Be carefull !!!
    double M  = 0.7;
    double k  = sqrt(M);
    double u  = 2;
    double fc = __sd__( u , k );
    printf("sd = %.4f  \n", fc );
    printf("isd = %.4f \n", __isd__( fc , k ) );

    return 0;
}