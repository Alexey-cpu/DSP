#ifndef EXAMPLE_CUSTOM_FILTERS_HMF_H
#define EXAMPLE_CUSTOM_FILTERS_HMF_H

#ifndef WRITE_LOGS
//#define WRITE_LOGS
#endif

#include "include/utils.h"
#include "include/sgen.h"
#include "include/filters_hmf.h"

// Recursive Fourier filter based harmonic filter
int filtes_hmf_example()
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
    Debugger::Log("filter initialization \n");
    sgen<__type> gen;

    harmonic_filter<__type> hmf;
    hmf.init(Fs, Fn, frames_per_cycle, 2);

    // buffer
    __type *buffer = __alloc__<__type>(frames_per_cycle);

    Debugger::Log("emulation \n");
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            buffer[j] = gen.sine( 1, Fn, 30, Fs );
        }

        // filtering
        hmf.filt(1, buffer);

        // logginig
        #ifdef WRITE_LOGS
        Km << hmf.module() << "\n";
        for( int j = 0 ; j < frames_per_cycle ; j++, time += 1 / Fs )
        {
            yt << buffer[j] << "\n";
            re << hmf.real(j) << "\n";
            im << hmf.imag(j) << "\n";
            am << __cabsf__( fcomplex<__type>(hmf.real(j), hmf.imag(j)) ) << "\n";
            dt << time << "\n";
        }
        #endif
    }

    Debugger::Log("memory clean \n");

    buffer = __mfree__(buffer);

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


#endif // EXAMPLE_CUSTOM_FILTERS_HMF_H
