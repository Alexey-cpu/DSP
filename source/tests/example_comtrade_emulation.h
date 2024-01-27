#ifndef EXAMPLE_COMTRADE_EMULATION_H
#define EXAMPLE_COMTRADE_EMULATION_H

#include "config.h"
#include "Comtrade.h"

#include "generators.h"
#include "filters_recursive_fourier_filter.h"

// Recursive Fourier filter
int filters_comtrade_emulation_example()
{
    typedef double __type;

    // read COMTRADE file
    Comtrade emulationStation;
    string oscDirectory = "C:/Qt_projects/ComtradeFileParser/sample_osc";
    string oscFileName  = "O3_50";
    emulationStation.from_file( oscDirectory, oscFileName );

    // retrieve sample frequency and reference frame frequency
    double Fs = emulationStation.get_rate_frequency();
    double Fn = emulationStation.get_nominal_frequency();
    size_t Ns = emulationStation.get_samples_number();

    // retrieve channel
    ComtradeChannelBase* channel =
    emulationStation.find_channel
            (
                [](ComtradeChannelBase* _Channel)->bool
                {
                    ComtradeAnalogChannel* channel = dynamic_cast<ComtradeAnalogChannel*>(_Channel);
                    return channel != nullptr && channel->get_name() == "Uf";
                }
            );

    //
    Comtrade registrationStation
            (
                "filters_ellip_example",
                Fn,
                Fs,
                Ns * Fs,
                {
                    new ComtradeAnalogChannel("xt"),
                    new ComtradeAnalogChannel("yt")
                }
            );

    ComtradeAnalogChannel* xt = registrationStation.find_analog_channel("xt");
    ComtradeAnalogChannel* yt = registrationStation.find_analog_channel("yt");

    // stand alone recursive Fourier filter
    standalone_recursive_fourier<__type> rff;
    rff.init(Fs, Fn, 0);

    int k = 0;
    if( channel != nullptr )
    {
        for( size_t i = 0 ; i < Ns ; i++, k++ )
        {
            double sample = channel->get_sample(i);
            Complex<double> vector = rff.filt( &sample );

            xt->set_sample( k, sample );
            yt->set_sample( k, __realf__( vector ) );
        }
    }

    registrationStation.to_file( LOGS_DIRECTORY, STRINGIFY(filters_comtrade_emulation_example) );

    return 0;
}

#endif // EXAMPLE_COMTRADE_EMULATION_H
