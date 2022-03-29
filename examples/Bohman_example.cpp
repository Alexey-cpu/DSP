#include "include/special_functions.h"
#include "iostream"
	
int main
{
	// window functions generation:
	int     order  = 10;
	double *bohman = Bohman(order);
		
	printf("Bohman: \t");
	for(int i = 0 ; i < order; i++ ) printf("%.4f ",bohman[i]);
	printf("\n");
	
	// memory deallocation:
	free(bohman);
	
	return 0;
}