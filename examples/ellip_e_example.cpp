#include "include/special_functions.h"
#include "iostream"

int main()
{
    // Matlab elliptic functions use M = k * k parameter. Be carefull !!!
    double M  = 0.7;
    double k  = sqrt(M);
    double u  = 2;
    double fc = __ellip_e__( u , k );
    printf("ellip_e  = %.4f  \n", fc );

    return 0;
}