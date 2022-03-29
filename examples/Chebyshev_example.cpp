#include "include/special_functions.h"
#include "iostream"
	
int main
{
	// window functions generation:
	int     order  = 10;
	double *chebyshev = Chebyshev(60 , order);
		
	printf("Chebyshev: \t");
	for(int i = 0 ; i < order; i++ ) printf("%.4f ",chebyshev[i]);
	printf("\n");
	
	// memory deallocation:
	free(chebyshev);
	
	return 0;
}