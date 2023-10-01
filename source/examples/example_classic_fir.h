#ifndef EXAMPLE_CLASSIC_FIR_H
#define EXAMPLE_CLASSIC_FIR_H

#include "config.h"
#include "../../COMTRADE/Comtrade.h"
#include "../../DSP/source/generators.h"
#include "../../DSP/source/filters_fir.h"

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

    // create COMTARDE registrator
    Comtrade registrator
            (
                "filters_ellip_example",
                Fn,
                Fs,
                {
                    new ComtradeFormatAnalogChannel("xt"),
                    new ComtradeFormatAnalogChannel("yt")
                }
            );

    registrator.set_samples_number( frames_per_cycle * cycles_num );

    // retrieve channels
    ComtradeFormatAnalogChannel* xt = registrator.find_analog_channel("xt");
    ComtradeFormatAnalogChannel* yt = registrator.find_analog_channel("yt");

    // generator initialization
    generator<__type> gen;
    digital_clock<double> time_provider;
    time_provider.init(Fs);

    // filter initialization
    window_function window;
    window.Hamming(80);

    // filter initialization
    fir<__type> filter;
    filter.init(Fs, filter_type::bandstop, {50, 500}, window, 1);

    // emulation
    int k = 0;
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time = time_provider.tick(), k++ )
        {
            __type signal = gen.distortion( 1, Fn, 30, 0.4, 5, time );
            __type output = filter(&signal);

            xt->set_sample( k, signal );
            yt->set_sample( k, output );
        }
    }

    registrator.to_file( LOGS_DIRECTORY, STRINGIFY(filters_fir_example) );

    return 0;
}


#endif // EXAMPLE_CLASSIC_FIR_H
