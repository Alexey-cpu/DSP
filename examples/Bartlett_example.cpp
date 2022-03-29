#include "include/special_functions.h"
#include "iostream"
	
int main
{
	// window functions generation:
	int     order    = 10;
	double *bartlett = Bartlett(order);
		
	printf("Bartlett: \t");
	for(int i = 0 ; i < order; i++ ) printf("%.4f ",bartlett[i]);
	printf("\n");
	
	// memory deallocation:
	free(bartlett);
	
	return 0;
}