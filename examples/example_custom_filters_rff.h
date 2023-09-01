#ifndef EXAMPLE_CUSTOM_FILTERS_RFF_H
#define EXAMPLE_CUSTOM_FILTERS_RFF_H

#include "config.h"
#include "../../DSP/include/generators.h"
#include "../../DSP/include/filters_recursive_fourier_filter.h"

// Recursive Fourier filter
int filters_rff_example()
{
    typedef double __type;

    // emulation parameters:
    double Fs                = 4000;
    double Fn                = 50;
    double time              = 0;
    double EmulationDuration = 0.08;
    int    CycleWidth        = 5;
    int    cycles_num        = 1000 * EmulationDuration / CycleWidth;
    int    frames_per_cycle  = CycleWidth * Fs / 1000;

    #ifdef WRITE_LOGS

    // log files
    std::ofstream xt;
    std::ofstream yt;
    std::ofstream re;
    std::ofstream im;
    std::ofstream am;
    std::ofstream pH;
    std::ofstream Km;
    std::ofstream dt;

    // open files
    xt.open( LOGS_DIRECTORY + OUTPUT_STREAM_INPUT);
    yt.open( LOGS_DIRECTORY + OUTPUT_STREAM_OUTPUT);
    pH.open( LOGS_DIRECTORY + OUTPUT_STREAM_PHASE_RESPONSE);
    Km.open( LOGS_DIRECTORY + OUTPUT_STREAM_AMPLITUDE_RESPONSE);
    dt.open( LOGS_DIRECTORY + OUTPUT_STREAM_TIME);
    re .open(LOGS_DIRECTORY + OUTPUT_STREAM_REAL_COMPONENT);
    im .open(LOGS_DIRECTORY + OUTPUT_STREAM_IMAG_COMPONENT);
    am .open(LOGS_DIRECTORY + OUTPUT_STREAM_SIGNAL_AMPLITUDE);

    #endif

    // main driver code
    generator<__type> gen;
    digital_clock<double> time_provider;
    time_provider.init(Fs);

    standalone_recursive_fourier<__type> rff;
    rff.init(Fs, Fn, 2);

    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time = time_provider.tick())
        {
            __type signal = gen.sine( 1, 2*Fn, 0, time ) + 0.2;
            Complex<__type> output = rff( &signal );

            // logginig
            #ifdef WRITE_LOGS
            yt << signal << "\n";
            re << __realf__(output) << "\n";
            im << __imagf__(output) << "\n";
            am << __cabsf__(output) << "\n";
            dt << time << "\n";
            #endif
        }
    }

    for( int i = 0 ; i < Fs / 2 ; i++ )
    {
        Complex<__type> output = rff.frequency_response( (double)i );

        #ifdef WRITE_LOGS
        pH << __cargf__(output) << "\n";
        Km << __cabsf__(output) << "\n";
        #endif
    }

    // close files
    #ifdef WRITE_LOGS
    xt.close();
    yt.close();
    re.close();
    im.close();
    am.close();
    pH.close();
    Km.close();
    dt.close();
    #endif

    return 0;
}

#endif // EXAMPLE_CUSTOM_FILTERS_RFF_H
