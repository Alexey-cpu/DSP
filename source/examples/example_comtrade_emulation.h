#ifndef EXAMPLE_COMTRADE_EMULATION_H
#define EXAMPLE_COMTRADE_EMULATION_H

#include "config.h"
#include "../../DSP/source/generators.h"
#include "../../DSP/source/filters_recursive_fourier_filter.h"
#include "../../ComtradeFileParser/ComtradeFileParser.h"

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
    ComtradeFileParser comtrade( oscDirectory, oscFileName );

    // retrieve sample frequency and reference frame frequency
    double Fs = comtrade.RateFrequency();
    double Fn = comtrade.NominalFrequency();
    size_t Ns = comtrade.SamplesNumber();

    // retrieve channel
    IComtradeFormatChannel* channel =
    comtrade.get_channel
            (
                [](IComtradeFormatChannel* _Channel)->bool
                {
                    ComtradeFormatAnalogChannel* channel = dynamic_cast<ComtradeFormatAnalogChannel*>(_Channel);
                    return channel != nullptr && channel->getId() == "Uf";
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

    //for( int i = 0 ; i < cycles_num ; i++ ){}

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


#endif // EXAMPLE_COMTRADE_EMULATION_H
