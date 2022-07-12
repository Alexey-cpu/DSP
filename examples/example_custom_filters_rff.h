#ifndef EXAMPLE_CUSTOM_FILTERS_RFF_H
#define EXAMPLE_CUSTOM_FILTERS_RFF_H

#ifndef WRITE_LOGS
//#define WRITE_LOGS
#endif

#include "include/generators.h"
#include "include/filters_rff.h"

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

    #ifdef WRITE_LOGS

    // logs directory:
    std::string directory = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // log files
    std::ofstream yt;
    std::ofstream re;
    std::ofstream im;
    std::ofstream am;
    std::ofstream pH;
    std::ofstream Km;
    std::ofstream dt;

    // open files
    yt .open(directory + "\\yt.txt");
    re .open(directory + "\\re.txt");
    im .open(directory + "\\im.txt");
    am .open(directory + "\\am.txt");
    pH .open(directory + "\\pH.txt");
    Km .open(directory + "\\Km.txt");
    dt .open(directory + "\\dt.txt");

    #endif

    // main driver code
    generator<__type> gen;
    digital_clock<double> time_provider;
    time_provider.init(Fs);

    recursive_fourier<__type> rff;
    rff.init(Fs, Fn, 2);

    Debugger::Log("emulation \n");
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time = time_provider.tick())
        {
            __type signal = gen.sine( 1, 2*Fn, 0, time ) + 0.2;
            fcomplex<__type> output = rff( &signal );

            // logginig
            #ifdef WRITE_LOGS
            yt << signal << "\n";
            re << __realf__(output) << "\n";
            im << __imagf__(output) << "\n";
            am << __cabsf__(output) << "\n";
            dt << time << "\n";
            #endif
        }
    }

    Debugger::Log("frequency response computation \n");
    for( int i = 0 ; i < Fs / 2 ; i++ )
    {
        fcomplex<__type> output = rff.frequency_response( (double)i );

        #ifdef WRITE_LOGS
        pH << __cargf__(output) << "\n";
        Km << __cabsf__(output) << "\n";
        #endif
    }

    Debugger::Log("memory clean \n");

    // close files

    #ifdef WRITE_LOGS
    yt .close();
    re .close();
    im .close();
    am .close();
    pH .close();
    Km .close();
    dt .close();
    #endif

    return 0;
}


#endif // EXAMPLE_CUSTOM_FILTERS_RFF_H
