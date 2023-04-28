#ifndef EXAMPLE_TRANSFER_FUNCTIONS_FILTER_2ND_ORDER_H
#define EXAMPLE_TRANSFER_FUNCTIONS_FILTER_2ND_ORDER_H


#include "config.h"
#include "../../DSP/include/generators.h"
#include "../../DSP/include/filters_tsf.h"

// Butterworth filter
int filters_filter_2nd_order_example()
{
    typedef float __type;

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
    std::ofstream pH;
    std::ofstream Km;
    std::ofstream dt;

    // open files
    xt.open( LOGS_DIRECTORY + OUTPUT_STREAM_INPUT);
    yt.open( LOGS_DIRECTORY + OUTPUT_STREAM_OUTPUT);
    pH.open( LOGS_DIRECTORY + OUTPUT_STREAM_PHASE_RESPONSE);
    Km.open( LOGS_DIRECTORY + OUTPUT_STREAM_AMPLITUDE_RESPONSE);
    dt.open( LOGS_DIRECTORY + OUTPUT_STREAM_TIME);

    #endif

    // generator initialization
    generator<__type> gen;
    digital_clock<double> time_provider;
    time_provider.init(Fs);

    // filter initialization
    filter_2nd_order<__type> filter;
    filter.init(Fs, 120, 0.7, filter_type::highpass);
    filter.show();

    // emulation
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time = time_provider.tick() )
        {
            __type signal = gen.sine( 1, Fn, 0, time );
            __type output = filter(&signal);

            #ifdef WRITE_LOGS
            xt << signal << "\n";
            yt << output << "\n";
            dt << time   << "\n";
            #endif
        }
    }

    for( int i = 0 ; i < Fs / 2 ; i++ )
    {
        Complex<__type> output = filter.frequency_response( (double)i );

        #ifdef WRITE_LOGS
        pH << __cargf__(output) << "\n";
        Km << __cabsf__(output) << "\n";
        #endif
    }

    // close files

    #ifdef WRITE_LOGS
    xt.close();
    yt.close();
    Km.close();
    pH.close();
    dt.close();
    #endif

    return 0;
}

#endif // EXAMPLE_TRANSFER_FUNCTIONS_FILTER_2ND_ORDER_H
