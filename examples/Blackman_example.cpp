#include "include/special_functions.h"
#include "iostream"
	
int main
{
	// window functions generation:
	int     order    = 10;
	double *blackman = Blackman(order);
		
	printf("Blackman: \t");
	for(int i = 0 ; i < order; i++ ) printf("%.4f ",blackman[i]);
	printf("\n");
	
	// memory deallocation:
	free(blackman);
	
	return 0;
}