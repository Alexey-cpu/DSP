#include "include/special_functions.h"
#include "iostream"
	
int main
{
	// window functions generation:
	int     order       = 10;
	double *triangular  = Triangular(order);
		
	printf("Triangular: \t");
	for(int i = 0 ; i < order; i++ ) printf("%.4f ",triangular[i]);
	printf("\n");
	
	// memory deallocation:
	free(triangular);
	
	return 0;
}