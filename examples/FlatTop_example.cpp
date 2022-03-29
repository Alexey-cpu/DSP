#include "include/special_functions.h"
#include "iostream"
	
int main
{
	// window functions generation:
	int     order   = 10;
	double *flatTop = FlatTop(order);
		
	printf("FlatTop: \t");
	for(int i = 0 ; i < order; i++ ) printf("%.4f ",flatTop[i]);
	printf("\n");
	
	// memory deallocation:
	free(flatTop);
	
	return 0;
}