#include "iostream"
#include "include/dsp.h"
using namespace DSP;

int example()
{
    // define filter and it's input signal data types:
    typedef float __type;

    // emulation parameters:
    double Fs                = 4000;
    double Fn                = 50;
    double time              = 0;
    double EmulationDuration = 0.08;
    int    CycleWidth        = 5;
    int    cycles_num        = 1000 * EmulationDuration / CycleWidth;
    int    frames_per_cycle  = CycleWidth * Fs / 1000;

    // logs directory (input here your log directory):
    std::string directory = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // files:
    std::ofstream yt;
    std::ofstream iir_y;
    std::ofstream tt;
    std::ofstream ff;
    std::ofstream iir_km;
    std::ofstream iir_ph;

    yt    .open( directory + "\\yt.txt"    );
    iir_y .open( directory + "\\iir_y.txt" );
    tt    .open( directory + "\\tt.txt"    );
    iir_km.open( directory + "\\iir_km.txt");
    iir_ph.open( directory + "\\iir_ph.txt");
    ff    .open( directory + "\\ff.txt"    );

    // signal generator:
    sgen<__type> gen;

    // IIR filters:
    DSP::chebyshev_2< __type > cheb2;

    // filters initialization:
    double Fc =  100;
	double BW =  500;
    double Gs =  80;
    double NU = -1; // not used parameter
    cheb2.init( Fs , 8 , DSP::filter_type::bandpass , { Fc , BW } , Gs );
	

    // emulation:
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            gen.sine( 1 , Fn , 0 , Fs );

            // filtering:
            __type y1 = cheb2 ( &gen.m_out );

            // generating output:
            yt      << gen.m_out  << "\n";
            iir_y   << y1         << "\n";
            tt      << time       << "\n";
            time += 1 / Fs;
        }
    }

    // frequency response computation
    for( int f = 0 ; f < Fs / 2 ; f++ )
    {
        fr fr1 = cheb2.frequency_response( f );
        iir_km << fr1.Km    << "\n";
        iir_ph << fr1.pH    << "\n";
        ff     << (double)f << "\n";
    }

    // closing files:
    yt    .close();
    iir_y .close();
    tt    .close();
    iir_km.close();
    iir_ph.close();
    ff    .close();

    return 0;
}