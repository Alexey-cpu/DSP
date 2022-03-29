#include "include/special_functions.h"
#include "iostream"

int main()
{
    // Matlab elliptic functions use M = k * k parameter. Be carefull !!!
    double M = 0.7;
    double k = sqrt(M);
    double u = 2;
    printf("cs = %.4f \n", __cs__( u , k ) );

    return 0;
}