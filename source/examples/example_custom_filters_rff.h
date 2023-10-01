#ifndef EXAMPLE_CUSTOM_FILTERS_RFF_H
#define EXAMPLE_CUSTOM_FILTERS_RFF_H

#include "config.h"
#include "../../DSP/source/generators.h"
#include "../../COMTRADE/Comtrade.h"
#include "../../DSP/source/filters_recursive_fourier_filter.h"

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

    // create COMTARDE registrator
    Comtrade registrator
            (
                "filters_ellip_example",
                Fn,
                Fs,
                {
                    new ComtradeFormatAnalogChannel("xt"),
                    new ComtradeFormatAnalogChannel("re"),
                    new ComtradeFormatAnalogChannel("im"),
                    new ComtradeFormatAnalogChannel("am")
                }
            );

    registrator.set_samples_number( frames_per_cycle * cycles_num );

    // retrieve channels
    ComtradeFormatAnalogChannel* xt = registrator.find_analog_channel("xt");
    ComtradeFormatAnalogChannel* re = registrator.find_analog_channel("re");
    ComtradeFormatAnalogChannel* im = registrator.find_analog_channel("im");
    ComtradeFormatAnalogChannel* am = registrator.find_analog_channel("am");

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

    registrator.to_file(LOGS_DIRECTORY, STRINGIFY(filters_rff_example));

    return 0;
}

#endif // EXAMPLE_CUSTOM_FILTERS_RFF_H
