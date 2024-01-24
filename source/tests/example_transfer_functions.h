#ifndef EXAMPLE_TRANSFER_FUNCTIONS_APERIODIC_H
#define EXAMPLE_TRANSFER_FUNCTIONS_APERIODIC_H

#include "config.h"
#include "Comtrade.h"

#include "generators.h"
#include "filters_transfer_functions.h"

int filters_aperiodic_example()
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
    aperiodic<__type> filter;
    filter.init(Fs, 0.01);

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

    registrator.to_file( LOGS_DIRECTORY, STRINGIFY(filters_aperiodic_example) );

    return 0;
}

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
    filter_2nd_order<__type> filter;
    filter.init(Fs, 120, 0.7, filter_type::bandstop);
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

    registrator.to_file( LOGS_DIRECTORY, STRINGIFY(filters_filter_2nd_order_example) );

    return 0;
}

int filters_integrator_example()
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
    integrator<__type> filter;
    filter.init(Fs);

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

    registrator.to_file( LOGS_DIRECTORY, STRINGIFY( filters_integrator_example ) );

    return 0;
}

int filters_leadlag_example()
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
    leadlag<__type> filter;
    filter.init(Fs, 0.01, 0.02);

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

    registrator.to_file( LOGS_DIRECTORY, STRINGIFY(filters_leadlag_example) );

    return 0;
}

#endif // EXAMPLE_TRANSFER_FUNCTIONS_APERIODIC_H
