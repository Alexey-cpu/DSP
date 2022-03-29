#include "include/special_functions.h"
#include "iostream"
	
int main
{
	// window functions generation:
	int     order  = 10;
	double *kaiser = Kaiser(2, order);
		
	printf("Kaiser: \t");
	for(int i = 0 ; i < order; i++ ) printf("%.4f ",kaiser[i]);
	printf("\n");
	
	// memory deallocation:
	free(kaiser);
	
	return 0;
}