#include "include/special_functions.h"
#include "iostream"

int main()
{
    // allocating polynoms arrays:
    int Na = 10;
    int Nb = 5;
    int Nc = Na + Nb - 1;
    double *a = (double*)calloc(Na,sizeof (double));
    double *b = (double*)calloc(Nb,sizeof (double));
    double *c = (double*)calloc(Nc,sizeof (double));

    // initialization of polynos arrays:
    for(int i = 0 ; i < Na; i++   ) a[i] = i;
    for(int i = Nb-1 ; i >= 0; i--) b[i] = i;

    // convolution:
    __convf__(a,b,c,Na,Nb,Nc);

    // show the result:
    for(int i = 0 ; i < Nc; i++ ) printf("%f.4 \n", c[i] );

    free(a);
    free(b);
    free(c);

	return 0;	
}