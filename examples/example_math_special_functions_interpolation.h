#ifndef EXAMPLE_MATH_SPECIAL_FUNCTIONS_INTERPOLATION_H
#define EXAMPLE_MATH_SPECIAL_FUNCTIONS_INTERPOLATION_H

#include "../include/special_functions.h"

int interpolation_example()
{
    // logs directory:
    std::string directory = "C:\\Qt_projects\\DSP\\logs";

    // log files
    std::ofstream xt;
    std::ofstream yt;
    std::ofstream zt;

    // open files
    yt.open(directory + "\\yt.txt");
    xt.open(directory + "\\xt.txt");
    zt.open(directory + "\\zt.txt");

    int M = 80;
    int N = 128;
    int K = 128;
    double  Fn = 50;
    double  Fs = Fn * M;
    double* a = __alloc__<double>(M);
    double* b = __alloc__<double>(N);
    double* c = __alloc__<double>(K);

    for( int i = 0 ; i < M ; i++ )
    {
        a[i] = sin( 2.0 * 3.14 * 50.0 * (double)i / Fs );
        xt << a[i] << "\n";
    }

    interpolation<double, double>(a, b, 1, M, N, 1);
    interpolation<double, double>(a, c, 1, M, K, 3);

    for( int i = 0 ; i < N ; i++ )
    {
        yt << b[i] << "\n";
    }

    for( int i = 0 ; i < K ; i++ )
    {
        zt << c[i] << "\n";
    }

    // free memory
    __mfree__(a);
    __mfree__(b);
    __mfree__(c);

    // close files
    xt.close();
    yt.close();
    zt.close();

    return 0;
}

#endif // EXAMPLE_MATH_SPECIAL_FUNCTIONS_INTERPOLATION_H
