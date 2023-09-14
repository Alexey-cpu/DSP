#ifndef EXAMPLE_MATH_SPECIAL_FUNCTIONS_INTERPOLATION_H
#define EXAMPLE_MATH_SPECIAL_FUNCTIONS_INTERPOLATION_H

#include "../source/special_functions.h"
#include "../source/filters_transfer_functions.h"
#include "../source/filters_real_time_spectrum_analyzer.h"
#include "../source/filters_iir.h"
#include "../source/filters_fir.h"

#include "config.h"

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

    double k = 8;
    double m = 5;
    int M = 80;
    int N = k*M;
    int K = N / m;
    double  Fn = 50;
    double  Fs = Fn * M;
    double* a = __alloc__<double>(M);
    double* b = __alloc__<double>(N);
    double* c = __alloc__<double>(K);

    for( int i = 0 ; i < M ; i++ )
    {
        a[i] = sin( 2.0 * 3.14 * 45.0 * (double)i / Fs + 30.0 * 3.14 / 180.0 );
        xt << a[i] << "\n";
    }

    samples_range_interpolation<double*, double*, double>(a, b, 1, M, N, k, 1);
    samples_range_decimation<double*, double*>(b, c, N, K, m);

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

int interpolator_example()
{
    typedef double __type;

    // emulation parameters:
    double Fs                = 4000;
    double time              = 0;
    double EmulationDuration = 0.02;
    int    CycleWidth        = 5;
    int    cycles_num        = 1000 * EmulationDuration / CycleWidth;
    int    frames_per_cycle  = CycleWidth * Fs / 1000;

    #ifdef WRITE_LOGS

    // log files
    std::ofstream xt;
    std::ofstream yt;
    std::ofstream dt;

    // open files
    xt.open( LOGS_DIRECTORY + OUTPUT_STREAM_INPUT);
    yt.open( LOGS_DIRECTORY + OUTPUT_STREAM_OUTPUT);
    dt.open( LOGS_DIRECTORY + OUTPUT_STREAM_TIME);

    #endif

    // main driver code
    generator<__type> gen;
    digital_clock<double> time_provider;
    time_provider.init(Fs);

    // interpolator
    signal_lagrange_interpolator interpolator;
    interpolator.init(1, 8);

    // decimator
    signal_decimator decimator;
    decimator.init(5);

    // auxiliary lambda

    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time = time_provider.tick())
        {
            __type signal = gen.sine( 1, 45, 30.0, time );
            interpolator.filt( &signal );

            // logginig
            #ifdef WRITE_LOGS
            xt << signal << "\n";

            // write interpolation result
            for( int k = 0 ; k < interpolator.get_rate_buffer().get_buff_size() ; k++ )
            {
                yt << interpolator.get_rate_buffer()[k] << "\n";

                if( decimator.decimate() )
                    dt << interpolator.get_rate_buffer()[k] << "\n";
            }

            #endif
        }
    }

    // close files
    #ifdef WRITE_LOGS
    xt.close();
    yt.close();
    dt.close();
    #endif

    return 0;
}

#endif // EXAMPLE_MATH_SPECIAL_FUNCTIONS_INTERPOLATION_H
