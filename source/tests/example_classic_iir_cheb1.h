#ifndef EXAMPLE_CLASSIC_IIR_CHEB1_H
#define EXAMPLE_CLASSIC_IIR_CHEB1_H

#include "config.h"
#include "Comtrade.h"

#include "generators.h"
#include "filters_iir.h"

// Checbyshev type I filter
int filters_cheb1_example()
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
                    new ComtradeAnalogChannel("xt"),
                    new ComtradeAnalogChannel("yt")
                }
            );

    registrator.set_samples_number( frames_per_cycle * cycles_num );

    // retrieve channels
    ComtradeAnalogChannel* xt = registrator.find_analog_channel("xt");
    ComtradeAnalogChannel* yt = registrator.find_analog_channel("yt");

    // generator initialization
    generator<__type> gen;
    digital_clock<double> time_provider;
    time_provider.init(Fs);

    // filter initialization
    chebyshev_1<__type> filter;
    filter.init(Fs, 12, filter_type::bandpass, {100 , 400}, 1 );
    filter.show();

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

    registrator.to_file( LOGS_DIRECTORY, STRINGIFY(filters_cheb1_example) );

    return 0;
}


#endif // EXAMPLE_CLASSIC_IIR_CHEB1_H