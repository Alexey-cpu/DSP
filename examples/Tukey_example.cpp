#include "include/special_functions.h"
#include "iostream"
	
int main
{
	// window functions generation:
	int     order = 10;
	double *tukey = Tukey(2, order);
		
	printf("Tukey: \t");
	for(int i = 0 ; i < order; i++ ) printf("%.4f ",tukey[i]);
	printf("\n");
	
	// memory deallocation:
	free(tukey);
	
	return 0;
}