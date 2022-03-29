#include "include/special_functions.h"
#include "iostream"
	
int main
{
	// window functions generation:
	int     order       = 10;
	double *rectangular = Rectangular(order);
		
	printf("Rectangular: \t");
	for(int i = 0 ; i < order; i++ ) printf("%.4f ",rectangular[i]);
	printf("\n");
	
	// memory deallocation:
	free(rectangular);
	
	return 0;
}