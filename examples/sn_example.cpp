#include "include/special_functions.h"
#include "iostream"

int main()
{
    // Matlab elliptic functions use M = k * k parameter. Be carefull !!!
    double M  = 0.7;
    double k  = sqrt(M);
    double u  = 2;
	double fc = __sn__( u , k );
    printf("sn = %.4f  \n", fc );
	printf("isn = %.4f \n", __isn__( sn , k ) );

    return 0;
}