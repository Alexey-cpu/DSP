#include "include/special_functions.h"
#include "iostream"
	
int main
{
	// window functions generation:
	int     order  = 10;
	double *parzen = Parzen(order);
		
	printf("Parzen: \t");
	for(int i = 0 ; i < order; i++ ) printf("%.4f ",parzen[i]);
	printf("\n");
	
	// memory deallocation:
	free(parzen);
	
	return 0;
}