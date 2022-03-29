#include "include/special_functions.h"
#include "iostream"
	
int main
{
	// window functions generation:
	int     order = 10;
	double *hann  = Hann(order);
		
	printf("Hann: \t");
	for(int i = 0 ; i < order; i++ ) printf("%.4f ",hann[i]);
	printf("\n");
	
	// memory deallocation:
	free(hann);
	
	return 0;
}