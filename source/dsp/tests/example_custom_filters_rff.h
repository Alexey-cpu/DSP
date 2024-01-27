#ifndef EXAMPLE_CUSTOM_FILTERS_RFF_H
#define EXAMPLE_CUSTOM_FILTERS_RFF_H

#include <Comtrade.h>
#include "generators.h"
#include "filters_recursive_fourier_filter.h"

// Recursive Fourier filter
int filters_rff_example(string _LogsDirectory)
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

    // create COMTARDE registrator
    Comtrade registrator
            (
                "filters_ellip_example",
                Fn,
                Fs,
                EmulationDuration,
                {
                    new ComtradeAnalogChannel("xt"),
                    new ComtradeAnalogChannel("re"),
                    new ComtradeAnalogChannel("im"),
                    new ComtradeAnalogChannel("am")
                }
            );

    // retrieve channels
    ComtradeAnalogChannel* xt = registrator.find_analog_channel("xt");
    ComtradeAnalogChannel* re = registrator.find_analog_channel("re");
    ComtradeAnalogChannel* im = registrator.find_analog_channel("im");
    ComtradeAnalogChannel* am = registrator.find_analog_channel("am");

    // main driver code
    generator<__type> gen;
    digital_clock<double> time_provider;
    time_provider.init(Fs);

    standalone_recursive_fourier<__type> rff;
    rff.init(Fs, Fn, 2);

    int k = 0;
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time = time_provider.tick(), k++)
        {
            __type signal = gen.sine( 1, 2*Fn, 0, time ) + 0.2;
            Complex<__type> output = rff( &signal );

            xt->set_sample(k, signal);
            re->set_sample(k, __realf__(output) );
            im->set_sample(k, __imagf__(output) );
            am->set_sample(k, __cabsf__(output) );
        }
    }

    registrator.to_file(_LogsDirectory, STRINGIFY(filters_rff_example));

    return 0;
}

#endif // EXAMPLE_CUSTOM_FILTERS_RFF_H