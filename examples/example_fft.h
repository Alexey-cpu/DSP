#ifndef EXAMPLE_FFT_H
#define EXAMPLE_FFT_H

#include "include/special_functions.h"

// Recursive Fourier filter
int fft_example()
{
    QElapsedTimer timer;
    int N = 128;
    int M = 80;
    double *sine = __alloc__<double>(M);
    fcomplex<double> *spectrum0 = __alloc__< fcomplex<double> >(N);

    for( int i = 0 ; i < M ; i++ )
    {
        sine[i] = 0.5 +
                  sin( 2.0 * 3.14 * 1 * 50.0 * (double)i / (M*50) ) +
                  sin( 2.0 * 3.14 * 2 * 50.0 * (double)i / (M*50) ) +
                  sin( 2.0 * 3.14 * 3 * 50.0 * (double)i / (M*50) );
    }

    // fft
    timer.start();
    for( int i = 0 ; i < 1e3 ; i++ ) fft( sine, spectrum0, M, N );
    double dt = timer.nsecsElapsed() / 1e9;

    // display info
    cout << "FFT has taken = " << dt << "s" << "\n\n";
    cout << "Spectrum magnitudes : \n\n";
    for( int i = 0 ; i < N ; i++ )
    {
        cout << "S[" << i << "]" << spectrum0[i].to_string() << "\n";
    }

    __mfree__(spectrum0);
    __mfree__(sine);

    return 0;
}

#endif // EXAMPLE_FFT_H
