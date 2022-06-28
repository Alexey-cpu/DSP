#ifndef EXAMPLE_SPECIAL_FUNCTIONS_H
#define EXAMPLE_SPECIAL_FUNCTIONS_H

#ifndef WRITE_LOGS
#define WRITE_LOGS
#endif

#include "include/utils.h"
#include "include/special_functions.h"

// special functions examples
int special_functions_example()
{
    double M = 0.7;
    double k = sqrt(M);
    double u = 5;

    printf("__sn__ = %.4f", __sn__( u , k ) );
    printf("__cn__ = %.4f", __cn__( u , k ) );
    printf("__dn__ = %.4f", __dn__( u , k ) );
    printf("__cd__ = %.4f", __cd__( u , k ) );
    printf("__sd__ = %.4f", __sd__( u , k ) );
    printf("__dc__ = %.4f", __dc__( u , k ) );
    printf("__nc__ = %.4f", __nc__( u , k ) );
    printf("__sc__ = %.4f", __sc__( u , k ) );
    printf("__sc__ = %.4f", __sc__( u , k ) );
    printf("__ns__ = %.4f", __ns__( u , k ) );
    printf("__ds__ = %.4f", __ds__( u , k ) );
    printf("__cs__ = %.4f", __cs__( u , k ) );

    return 0;
}


#endif // EXAMPLE_SPECIAL_FUNCTIONS_H
