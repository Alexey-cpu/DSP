#ifndef EXAMPLE_CUSTOM_FILTERS_HMF_H
#define EXAMPLE_CUSTOM_FILTERS_HMF_H

#ifndef WRITE_LOGS
#define WRITE_LOGS
#endif

#include "config.h"

#include "generators.h"
#include "filters_harmonic_filter.h"
#include "filters_fast_fourier_transform_spectrum_analyzer.h"

/*
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

int filtes_fft_example()
{
    typedef double __type;

    // emulation parameters:
    double Fs                = 4000;
    double Fn                = 50;
    double time              = 0;
    double EmulationDuration = 0.12;
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

    fast_fourier_transform_spectrum_analyzer hmf;
    hmf.init(Fs, Fn, frames_per_cycle);

    // buffer
    __type *buffer = __alloc__<__type>(frames_per_cycle);

    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time = time_provider.tick() )
        {
            if( time > 0.04 )
            {
                buffer[j] = gen.sine( 2, Fn, 0.0, time ) +
                        2 * 0.2 * gen.sine( 1, 2 * Fn, 0.0, time ) +
                        2 * 0.3 * gen.sine( 1, 3 * Fn, 0.0, time );
            }
            else
            {
                buffer[j] = gen.sine( 1, Fn, 0.0, time ) +
                        0.2 * gen.sine( 1, 2 * Fn, 0.0, time ) +
                        0.3 * gen.sine( 1, 3 * Fn, 0.0, time );
            }


            // filtering
            //Complex<double> a = hmf.filt( &buffer[j], 1);

            //re << __realf__(a) << "\n";
            //im << __imagf__(a) << "\n";
            //am << __cabsf__( Complex<__type>( __realf__(a), __imagf__(a) ) ) << "\n";

        }

        // filtering
        Complex<double> a = hmf.filt( buffer, 3);

        re << __realf__(a) << "\n";
        im << __imagf__(a) << "\n";
        am << __cabsf__( Complex<__type>( __realf__(a), __imagf__(a) ) ) << "\n";

        // logginig
        #ifdef WRITE_LOGS
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            yt << buffer[j] << "\n";
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
*/

#endif // EXAMPLE_CUSTOM_FILTERS_HMF_H
