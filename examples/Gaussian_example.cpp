#include "include/special_functions.h"
#include "iostream"
	
int main
{
	// window functions generation:
	int     order    = 10;
	double *gaussian = Gaussian(2, order);
		
	printf("Gaussian: \t");
	for(int i = 0 ; i < order; i++ ) printf("%.4f ",gaussian[i]);
	printf("\n");
	
	// memory deallocation:
	free(gaussian);
	
	return 0;
}