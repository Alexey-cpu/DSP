#include "include/special_functions.h"
#include "iostream"
	
int main
{
	// window functions generation:
	int     order    = 10;
	double *blackmanHarris  = BlackmanHarris(order);
		
	printf("BlackmanHarris: \t");
	for(int i = 0 ; i < order; i++ ) printf("%.4f ",blackmanHarris[i]);
	printf("\n");
	
	// memory deallocation:
	free(blackmanHarris);
	
	return 0;
}