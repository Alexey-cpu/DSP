#include "include/special_functions.h"
#include "iostream"
	
int main
{
	// window functions generation:
	int     order    = 10;
	double *hamming  = Hamming(order);
		
	printf("Hamming: \t");
	for(int i = 0 ; i < order; i++ ) printf("%.4f ",hamming[i]);
	printf("\n");
	
	// memory deallocation:
	free(hamming);
	
	return 0;
}