#ifndef EXAMPLE_CUSTOM_FILTERS_HMF_H
#define EXAMPLE_CUSTOM_FILTERS_HMF_H

#ifndef WRITE_LOGS
#define WRITE_LOGS
#endif

#include "config.h"
#include "../../DSP/include/generators.h"
#include "../../DSP/include/filters_harmonic_filter.h"

// Recursive Fourier filter based harmonic filter
int filtes_hmf_example()
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

    harmonic_filter<__type> hmf;
    hmf.init(Fs, Fn, frames_per_cycle, 3);

    // buffer
    __type *buffer = __alloc__<__type>(frames_per_cycle);

    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time = time_provider.tick() )
        {
            buffer[j] = 0.5 + gen.sine( 1, 2*Fn, 30, time );
        }

        // filtering
        hmf.filt(2, buffer);

        // logginig
        #ifdef WRITE_LOGS
        Km << hmf.module() << "\n";
        for( int j = 0 ; j < frames_per_cycle ; j++, time += 1 / Fs )
        {
            yt << buffer[j] << "\n";
            re << hmf.real(j) << "\n";
            im << hmf.imag(j) << "\n";
            am << __cabsf__( Complex<__type>(hmf.real(j), hmf.imag(j)) ) << "\n";
            dt << time << "\n";
        }
        #endif
    }

    buffer = __mfree__(buffer);

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


#endif // EXAMPLE_CUSTOM_FILTERS_HMF_H
