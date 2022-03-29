#include "include/special_functions.h"
#include "iostream"
	
int main
{
	// window functions generation:
	int     order  = 10;
	double *nutall = Nutall(order);
		
	printf("Nutall: \t");
	for(int i = 0 ; i < order; i++ ) printf("%.4f ",nutall[i]);
	printf("\n");
	
	// memory deallocation:
	free(nutall);
	
	return 0;
}