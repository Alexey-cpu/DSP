#ifndef EXAMPLE_CLASSIC_IIR_ELLIP_H
#define EXAMPLE_CLASSIC_IIR_ELLIP_H

#include "config.h"
#include "../../COMTRADE/Comtrade.h"
#include "../../DSP/source/generators.h"
#include "../../DSP/source/filters_iir.h"

// Checbyshev type II filter
int filters_ellip_example()
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
    elliptic<__type> filter;
    filter.init(Fs, 11, filter_type::lowpass, {100 , 400}, 80, 1 );
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

    // write COMTRADE file
    registrator.to_file( LOGS_DIRECTORY, registrator.get_station_name() );

    return 0;
}


#endif // EXAMPLE_CLASSIC_IIR_ELLIP_H
