#include "include/special_functions.h"

// examples:
#include "include/examples.h"

#include <list>


#include "iostream"
#include "include/dsp.h"
using namespace DSP;

int example_function()
{
    // emulation parameters:
    double Fs                = 4000;
    double Fn                = 50;
    double EmulationDuration = 0.08;
    int    CycleWidth        = 5;
    int    cycles_num        = 1000 * EmulationDuration / CycleWidth;
    int    frames_per_cycle  = CycleWidth * Fs / 1000;

    // test filters
    butterworth< double > buttf_lp;
    butterworth< double > buttf_hp;
    butterworth< double > buttf_bp;
    butterworth< double > buttf_bs;

    // test signal generator
    sgen< double > gen;

    // filters parameters
    double Fc        = 100;
    double BandWidth = 100;
    int    order     = 8;

    // filters initialization
    buttf_lp.init( Fs , order , filter_type::lowpass  , { Fc , BandWidth } );
    buttf_hp.init( Fs , order , filter_type::highpass , { Fc , BandWidth } );
    buttf_bp.init( Fs , order , filter_type::bandpass , { Fc , BandWidth } );
    buttf_bs.init( Fs , order , filter_type::bandstop , { Fc , BandWidth } );

    // emulation:
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            // signal generation
            gen.sine( 1 , Fn , 0 , Fs );

            // filtering
            double yt1 = buttf_lp( &gen.m_out );
            double yt2 = buttf_hp( &gen.m_out );
            double yt3 = buttf_bp( &gen.m_out );
            double yt4 = buttf_bs( &gen.m_out );
        }
    }

    // frequency response computation
    for( int i = 0 ; i < Fs / 2 ; i++ )
    {
        fr fr1 = buttf_lp.frequency_response(i);
        fr fr2 = buttf_hp.frequency_response(i);
        fr fr3 = buttf_bp.frequency_response(i);
        fr fr4 = buttf_bs.frequency_response(i);
    }

    return 0;
}

int example()
{
    // define filter and it's input signal data types:
    typedef double  __type;

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
    DSP::chebyshev_1< __type > cheb1;

    // filters initialization:
    double Fc =  100;
    double Gp =  1;
    double NU = -1; // not used parameter
    cheb1.init( Fs , 8 , DSP::filter_type::lowpass , { Fc , NU } , Gp );

    // emulation:
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            gen.sine( 1 , Fn , 0 , Fs );

            // filtering:
            double y1 = cheb1 ( &gen.m_out );

            // generating output:
            yt      << gen.m_out  << "\n";
            iir_y   << y1 << "\n";
            tt      << time       << "\n";
            time += 1 / Fs;
        }
    }

    // frequency response computation
    for( int f = 0 ; f < Fs / 2 ; f++ )
    {
        fr fr1 = cheb1.frequency_response( f );
        iir_km << fr1.Km << "\n";
        iir_ph << fr1.pH << "\n";
        ff << (double)f << "\n";
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

void window_fcn_examples_generator()
{
    std::list< std::string > windows;

    std::string directory = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // window function names:
    windows.push_back("Barlett");
    windows.push_back("BartlettHanning");
    windows.push_back("Blackman");
    windows.push_back("BlackmanHarris");
    windows.push_back("Bohman");
    windows.push_back("Chebyshev");
    windows.push_back("FlatTop");
    windows.push_back("Gaussian");
    windows.push_back("Hamming");
    windows.push_back("Hann");
    windows.push_back("Kaiser");
    windows.push_back("Nutall");
    windows.push_back("Parzen");
    windows.push_back("Rectangular");
    windows.push_back("Triangular");
    windows.push_back("Tukey");

    for( auto it = windows.begin() ; it != windows.end() ; it++ )
    {
        std::ofstream file;
        std::string   name = *it;
        file.open( directory + "\\" + name + "_example.cpp" );

        if( *it != "Chebyshev" )
        {
            file << "int main()" << "\n";
            file << "{" << "\n";
            file << "\t" << "int order  = 10;" << "\n";
            file << "\t" << "double " << "*" << name << " = " << name << "(order);" << "\n";
            file << "\t" << "free(" << name << ")\n";
            file << "\t" << "return 0;" << "\n";
            file << "}" << "\n";
        }

        std::cout << *it << "\n";
    }

}

#include "iostream"
#include "include/fcomplex.h"

int main()
{
    fcomplex<double> z(1,2);
    fcomplex<double> f = __sinhf__(z);
    std::cout << "real(f) = " << __realf__(f) << "\n";
    std::cout << "imag(f) = " << __imagf__(f) << "\n";
    return 0;
}
