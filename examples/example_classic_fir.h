#ifndef EXAMPLE_CLASSIC_FIR_H
#define EXAMPLE_CLASSIC_FIR_H

#ifndef WRITE_LOGS
#define WRITE_LOGS
#endif

#include "include/generators.h"
#include "include/filters_fir.h"

// classic FIR filter example
int filters_fir_example()
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

    // logs directory:
    std::string directory = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // log files
    std::ofstream xt;
    std::ofstream yt;
    std::ofstream pH;
    std::ofstream Km;
    std::ofstream dt;

    // open files
    yt .open(directory + "\\yt.txt");
    xt .open(directory + "\\xt.txt");
    pH .open(directory + "\\pH.txt");
    Km .open(directory + "\\Km.txt");
    dt .open(directory + "\\dt.txt");

    #endif

    // generator initialization
    generator<__type> gen;
    digital_clock<double> time_provider;
    time_provider.init(Fs);

    // filter initialization
    window_function window;
    window.Hamming(80);

    // filter initialization
    fir<__type> filter;
    filter.init(Fs, filter_type::lowpass, {50, 500}, window, 1);

    // emulation
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time = time_provider.tick() )
        {
            __type signal = gen.distortion( 1, Fn, 30, 0.4, 5, time );
            //__type signal = gen.sine( 1, Fn, 0, time );
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
        fcomplex<__type> output = filter.frequency_response( (double)i );

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


#endif // EXAMPLE_CLASSIC_FIR_H
