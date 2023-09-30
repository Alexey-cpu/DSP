#ifndef EXAMPLE_COMTRADE_EMULATION_H
#define EXAMPLE_COMTRADE_EMULATION_H

#include "config.h"
#include "../../DSP/source/generators.h"
#include "../../DSP/source/filters_recursive_fourier_filter.h"
#include "../../ComtradeFileParser/ComtradeFormat.h"

// Recursive Fourier filter
int filters_comtrade_emulation_example()
{
    typedef double __type;

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

    // read COMTRADE file
    string oscDirectory = "C:/Qt_projects/ComtradeFileParser/sample_osc";
    string oscFileName  = "O9_50";
    ComtradeFormat comtrade;
    comtrade.from_file( oscDirectory, oscFileName );

    // retrieve sample frequency and reference frame frequency
    double Fs = comtrade.get_rate_frequency();
    double Fn = comtrade.get_nominal_frequency();
    size_t Ns = comtrade.get_samples_number();

    // retrieve channel
    AbstractComtradeFormatChannel* channel =
    comtrade.find_channel
            (
                [](AbstractComtradeFormatChannel* _Channel)->bool
                {
                    ComtradeFormatAnalogChannel* channel = dynamic_cast<ComtradeFormatAnalogChannel*>(_Channel);
                    return channel != nullptr && channel->get_name() == "Uf";
                }
            );

    // stand alone recursive Fourier filter
    standalone_recursive_fourier<__type> rff;
    rff.init(Fs, Fn, 0);

    if( channel != nullptr )
    {
        for( size_t i = 0 ; i < Ns ; i++ )
        {
            double sample = channel->get_data_sample(i);

            Complex<double> vector = rff.filt( &sample );

            xt << sample << "\n";
            yt << __realf__( vector ) << "\n";
        }
    }

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

int filters_comtrade_write_example()
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

    // create COMTRADE file
    string oscDirectory = "C:/Qt_projects/DSP/logs";
    string oscFileName  = "osc";
    ComtradeFormat comtrade("DSP", 50, 4000);
    comtrade.set_samples_number( cycles_num * frames_per_cycle );

    ComtradeFormatAnalogChannel* xt = comtrade.add_analog_channel( STRINGIFY(xt) );
    ComtradeFormatAnalogChannel* re = comtrade.add_analog_channel( STRINGIFY(re) );
    ComtradeFormatAnalogChannel* im = comtrade.add_analog_channel( STRINGIFY(im) );
    ComtradeFormatAnalogChannel* am = comtrade.add_analog_channel( STRINGIFY(am) );

    // main driver code
    generator<__type> gen;
    digital_clock<double> time_provider;
    time_provider.init(Fs);

    // stand alone recursive Fourier filter
    standalone_recursive_fourier<__type> rff;
    rff.init(Fs, Fn, 0);

    // emulation
    int k = 0;
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time = time_provider.tick(), k++ )
        {
            __type signal = gen.sine( 1, 2*Fn, 0, time ) + 0.2;
            Complex<__type> output = rff( &signal );

            // write COMTRADE
            xt->set_data_sample(k, signal);
            re->set_data_sample(k, __realf__( output ) );
            im->set_data_sample(k, __imagf__( output ) );
            am->set_data_sample(k, __cabsf__( output ) );
        }
    }

    comtrade.to_file(oscDirectory, oscFileName);

    return 0;
}

#endif // EXAMPLE_COMTRADE_EMULATION_H
