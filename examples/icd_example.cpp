#include "include/special_functions.h"
#include "iostream"

int main()
{
    // Matlab elliptic functions use M = k * k parameter. Be carefull !!!
    double M  = 0.7;
    double k  = sqrt(M);
    double u  = 2;
    double fc = __cd__( u , k );
    printf("cd = %.4f  \n", fc );
    printf("icd = %.4f \n", __icd__( fc , k ) );

    return 0;
}