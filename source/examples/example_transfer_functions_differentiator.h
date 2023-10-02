#ifndef EXAMPLE_TRANSFER_FUNCTIONS_DIFFERENTIATOR_H
#define EXAMPLE_TRANSFER_FUNCTIONS_DIFFERENTIATOR_H

#include "../../DSP/source/generators.h"
#include <Comtrade.h>
#include "../../DSP/source/filters_transfer_functions.h"
#include "config.h"

// Butterworth filter
int filters_differentiator_example()
{
    typedef float __type;

    // emulation parameters:
    double Fs                = 4000;
    double Ts                = (double)1 / Fs;
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
    differentiator<__type> filter;
    filter.init(Fs, Ts);

    // emulation
    int k = 0;
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time = time_provider.tick(), k++ )
        {
            __type signal = gen.sine( 1, Fn, 0, time );
            __type output = filter(&signal);

            xt->set_sample( k, signal );
            yt->set_sample( k, output );
        }
    }

    registrator.to_file( LOGS_DIRECTORY, STRINGIFY(filters_differentiator_example) );

    return 0;
}

#endif // EXAMPLE_TRANSFER_FUNCTIONS_DIFFERENTIATOR_H
