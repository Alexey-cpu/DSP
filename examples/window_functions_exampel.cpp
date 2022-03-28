#include "include/special_functions.h"
#include "iostream"

int main()
{
    // window functions generation:
    int     order = 10;
    double *bartlett        = Bartlett(order);
    double *bartlettHanning = BartlettHanning(order);
    double *blackman        = Blackman(order);
    double *blackmanHarris  = BlackmanHarris(order);
    double *bohman          = Bohman(order);
    double *chebyshev       = Chebyshev(60 , order);
    double *flatTop         = FlatTop(order);
    double *gaussian        = Gaussian(2, order);
    double *hamming         = Hamming(order);
    double *hann            = Hann(order);
    double *kaiser          = Kaiser(2, order);
    double *nutall          = Nutall(order);
    double *parzen          = Parzen(order);
    double *rectangular     = Rectangular(order);
    double *triangular      = Triangular(order);
    double *tukey           = Tukey(2, order);

    // window functions show:
    printf("Bartlett: \t");
    for(int i = 0 ; i < order; i++ ) printf("%.4f ",bartlett[i]);
    printf("\n");

    printf("Bartlett-Hanning: \t");
    for(int i = 0 ; i < order; i++ ) printf("%.4f ",bartlettHanning[i]);
    printf("\n");

    printf("Blackman: \t");
    for(int i = 0 ; i < order; i++ ) printf("%.4f ",blackman[i]);
    printf("\n");

    printf("Blackman-Harris: \t");
    for(int i = 0 ; i < order; i++ ) printf("%.4f ",blackmanHarris[i]);
    printf("\n");

    printf("Bohman:          \t");
    for(int i = 0 ; i < order; i++ ) printf("%.4f ",bohman[i]);
    printf("\n");

    printf("Chebyshev:       \t");
    for(int i = 0 ; i < order; i++ ) printf("%.4f ",chebyshev[i]);
    printf("\n");

    printf("FlatTop:         \t");
    for(int i = 0 ; i < order; i++ ) printf("%.4f ",flatTop[i]);
    printf("\n");

    printf("Gaussian:        \t");
    for(int i = 0 ; i < order; i++ ) printf("%.4f ",gaussian[i]);
    printf("\n");

    printf("Hamming:         \t");
    for(int i = 0 ; i < order; i++ ) printf("%.4f ",hamming[i]);
    printf("\n");

    printf("Hann:            \t");
    for(int i = 0 ; i < order; i++ ) printf("%.4f ",hann[i]);
    printf("\n");

    printf("Kaiser:          \t");
    for(int i = 0 ; i < order; i++ ) printf("%.4f ",kaiser[i]);
    printf("\n");

    printf("Nutall:          \t");
    for(int i = 0 ; i < order; i++ ) printf("%.4f ",nutall[i]);
    printf("\n");

    printf("Parzen:          \t");
    for(int i = 0 ; i < order; i++ ) printf("%.4f ",parzen[i]);
    printf("\n");

    printf("Rectangular:     \t");
    for(int i = 0 ; i < order; i++ ) printf("%.4f ",rectangular[i]);
    printf("\n");

    printf("Triangular:      \t");
    for(int i = 0 ; i < order; i++ ) printf("%.4f ",triangular[i]);
    printf("\n");

    printf("Tukey:           \t");
    for(int i = 0 ; i < order; i++ ) printf("%.4f ",tukey[i]);
    printf("\n");


    // Memory deallocation
    free(bartlett        );
    free(bartlettHanning );
    free(blackman        );
    free(blackmanHarris  );
    free(bohman          );
    free(chebyshev       );
    free(flatTop         );
    free(gaussian        );
    free(hamming         );
    free(hann            );
    free(kaiser          );
    free(nutall          );
    free(parzen          );
    free(rectangular     );
    free(triangular      );
    free(tukey           );

    return 0;
}