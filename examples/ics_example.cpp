#include "include/special_functions.h"
#include "iostream"

int main()
{
    // Matlab elliptic functions use M = k * k parameter. Be carefull !!!
    double M  = 0.7;
    double k  = sqrt(M);
    double u  = 2;
    double fc = __cs__( u , k );
    printf("cs  = %.4f  \n", fc );
    printf("ics = %.4f \n", __ics__( fc , k ) );

    return 0;
}