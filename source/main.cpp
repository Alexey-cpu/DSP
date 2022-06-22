#include "include/special_functions.h"

// examples:
#include "iostream"
#include "include/fcomplex.h"
#include "include/f_iir.h"
#include "include/utils.h"

int main()
{
    int PolynomsNumber = 4;
    int PolynomsSize   = 3;

    int    *S  = __alloc__<int>(PolynomsNumber);
    double **P = __alloc__<double*>(PolynomsNumber);

    double *c = nullptr;
    int     Nc = 0;

    // fill
    for( int i = 0 ; i < PolynomsNumber; i++ )
    {
        P[i] = __alloc__<double>(PolynomsSize);
        S[i] = PolynomsSize;
        for( int j = 0 ; j < PolynomsSize ; j++ )
        {
            P[i][j] = j+1;
        }
    }

    __convf__( P, S, 4, &c, &Nc );

    for( int i = 0 ; i < Nc; i++ )
    {
        cout << c[i] << "\n";
    }

    // memory free
    __mfree__(S);
    __mfree__(P);
    __mfree__(c);

    return 0;
}
