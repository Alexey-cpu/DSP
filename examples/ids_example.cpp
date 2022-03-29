#include "include/special_functions.h"
#include "iostream"

int main()
{
    // Matlab elliptic functions use M = k * k parameter. Be carefull !!!
    double M  = 0.7;
    double k  = sqrt(M);
    double u  = 2;
    double fc = __ds__( u , k );
    printf("ds  = %.4f  \n", fc );
    printf("ids = %.4f \n", __ids__( fc , k ) );

    return 0;
}