#ifndef EXAMPLE_MATH_SPECIAL_FUNCTIONS_FFT_H
#define EXAMPLE_MATH_SPECIAL_FUNCTIONS_FFT_H

#include "../../DSP/include/special_functions.h"
#include "config.h"
#include "../include/filters_real_time_spectrum_analyzer.h"

// Fast Fourier transform example
int fft_example()
{
    QElapsedTimer timer;
    int    N  = 128;
    double Fn = 50;
    Complex<double> *spectrum0 = __alloc__< Complex<double> >(N);

    // log files
    std::ofstream xt;
    std::ofstream yt;

    // open files
    xt.open( LOGS_DIRECTORY + OUTPUT_STREAM_INPUT);
    yt.open( LOGS_DIRECTORY + OUTPUT_STREAM_OUTPUT);


    for( int i = 0 ; i < N ; i++ )
    {
        spectrum0[i] = sin( 2.0 * 3.14 * 1 * Fn * (double)i / (N*Fn) );

        xt << __realf__( spectrum0[i] ) << "\n";

        /*
        sine[i] = 0.5 +
                  sin( 2.0 * 3.14 * 1 * 50.0 * (double)i / (M*50) ) +
                  sin( 2.0 * 3.14 * 2 * 50.0 * (double)i / (M*50) ) +
                  sin( 2.0 * 3.14 * 3 * 50.0 * (double)i / (M*50) );
                  */
    }

    // fft
    timer.start();

    fft0( spectrum0, N, 1 );

    double dt = timer.nsecsElapsed() / 1e9;

    // display info
    cout << "FFT has taken = " << dt << "s" << "\n\n";
    cout << "Spectrum magnitudes of direct transform: \n\n";
    for( int i = 0 ; i < N ; i++ ) cout << "S[" << i << "]" << spectrum0[i].to_string() << "\n";
    cout << "\n";


    //fft0( sine, spectrum0, M, N, 0, 3);
    fft1( spectrum0, N, 0);

    cout << "Spectrum magnitudes of inverse transform: \n\n";
    for( int i = 0 ; i < N ; i++ )
    {
        cout << "S[" << i << "]" << spectrum0[i].to_string() << "\n";
        yt << spectrum0[i].to_string() << "\n";
    }

    xt.close();
    yt.close();

    __mfree__(spectrum0);

    return 0;
}

#endif // EXAMPLE_MATH_SPECIAL_FUNCTIONS_FFT_H
