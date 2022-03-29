#include "include/special_functions.h"
#include "iostream"
	
int main
{
	// window functions generation:
	int     order    = 10;
	double *bartlettHanning = BartlettHanning(order);
		
	printf("BartlettHanning: \t");
	for(int i = 0 ; i < order; i++ ) printf("%.4f ",bartlettHanning[i]);
	printf("\n");
	
	// memory deallocation:
	free(bartlettHanning);
	
	return 0;
}