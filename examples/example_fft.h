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


    // logs directory:
    std::string directory = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // log files
    std::ofstream xt;
    std::ofstream yt;

    // open files
    yt .open(directory + "\\yt.txt");
    xt .open(directory + "\\xt.txt");


    for( int i = 0 ; i < M ; i++ )
    {
        sine[i] = sin( 2.0 * 3.14 * 1 * 50.0 * (double)i / (M*50) );

        xt << sine[i] << "\n";

        /*
        sine[i] = 0.5 +
                  sin( 2.0 * 3.14 * 1 * 50.0 * (double)i / (M*50) ) +
                  sin( 2.0 * 3.14 * 2 * 50.0 * (double)i / (M*50) ) +
                  sin( 2.0 * 3.14 * 3 * 50.0 * (double)i / (M*50) );
                  */
    }

    // fft
    timer.start();
    for( int i = 0 ; i < 1e3 ; i++ ) fft0( sine, spectrum0, M, N, 1 );
    //for( int i = 0 ; i < 1e3 ; i++ ) fft1( sine, spectrum0, M, N, 1 );
    double dt = timer.nsecsElapsed() / 1e9;

    // display info
    cout << "FFT has taken = " << dt << "s" << "\n\n";
    cout << "Spectrum magnitudes of direct transform: \n\n";
    for( int i = 0 ; i < N ; i++ ) cout << "S[" << i << "]" << spectrum0[i].to_string() << "\n";
    cout << "\n";

    for( int i = 0 ; i < M ; i++ )
    {
        sine[i] = 0;
    }

    //fft0( sine, spectrum0, M, N, 0, 3);
    fft1( sine, spectrum0, M, N, 0, 3);

    cout << "Spectrum magnitudes of inverse transform: \n\n";
    for( int i = 0 ; i < M ; i++ )
    {
        yt << sine[i] << "\n";
        //cout << "S[" << i << "]" << sine[i] << "\n";
    }

    xt.close();
    yt.close();

    __mfree__(spectrum0);
    __mfree__(sine);

    return 0;
}

#endif // EXAMPLE_FFT_H
