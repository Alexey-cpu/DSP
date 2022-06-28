#ifndef EXAMPLES_H
#define EXAMPLES_H

#define WRITE_LOGS

// system headers:
#include <iostream>
#include <fstream>
#include "string"
#include <QElapsedTimer>

// DSP template library headers:
#include "include/special_functions.h"
#include "include/buffer.h"
#include "include/sgen.h"
#include "include/logical.h"
#include "include/filters_rff.h"
#include "include/filters_hmf.h"
#include "include/filters_iir.h"
#include "include/filters_fir.h"


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
    Debugger::Log("filter initialization \n");
    sgen<__type> gen;
    recursive_fourier<__type> rff;
    rff.init(Fs, Fn, 2);

    Debugger::Log("emulation \n");
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time += 1 / Fs )
        {
            __type signal = gen.sine( 1, 2*Fn, 0, Fs ) + 0.2;
            fcomplex<__type> output = rff( &signal );

            // logginig

            #ifdef WRITE_LOGS
            yt << gen.m_out << "\n";
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

// Butterworth filter
int filters_butt_example()
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

    #ifdef WRITE_LOGS

    // logs directory:
    std::string directory = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // log files
    std::ofstream xt;
    std::ofstream yt;
    std::ofstream pH;
    std::ofstream Km;
    std::ofstream dt;

    // open files
    yt .open(directory + "\\yt.txt");
    xt .open(directory + "\\xt.txt");
    pH .open(directory + "\\pH.txt");
    Km .open(directory + "\\Km.txt");
    dt .open(directory + "\\dt.txt");

    #endif

    // generator initialization
    sgen<__type> gen;

    // filter initialization
    butterworth<__type> filter;
    filter.init(Fs, 11, filter_type::bandstop, {100 , 400} );
    filter.show();

    // emulation
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time += 1 / Fs )
        {
            __type signal = gen.sine( 1, Fn, 0, Fs );
            __type output = filter(&signal);

            #ifdef WRITE_LOGS
            xt << signal << "\n";
            yt << output << "\n";
            dt << time   << "\n";
            #endif
        }
    }

    for( int i = 0 ; i < Fs / 2 ; i++ )
    {
        fcomplex<__type> output = filter.frequency_response( (double)i );

        #ifdef WRITE_LOGS
        pH << __cargf__(output) << "\n";
        Km << __cabsf__(output) << "\n";
        #endif
    }

    // close files

    #ifdef WRITE_LOGS
    xt.close();
    yt.close();
    Km.close();
    pH.close();
    dt.close();
    #endif

    return 0;
}

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

    #ifdef WRITE_LOGS

    // logs directory:
    std::string directory = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // log files
    std::ofstream xt;
    std::ofstream yt;
    std::ofstream pH;
    std::ofstream Km;
    std::ofstream dt;

    // open files
    yt .open(directory + "\\yt.txt");
    xt .open(directory + "\\xt.txt");
    pH .open(directory + "\\pH.txt");
    Km .open(directory + "\\Km.txt");
    dt .open(directory + "\\dt.txt");

    #endif

    // generator initialization
    sgen<__type> gen;

    // filter initialization
    chebyshev_1<__type> filter;
    filter.init(Fs, 12, filter_type::bandpass, {100 , 400}, 1 );
    filter.show();

    // emulation
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time += 1 / Fs )
        {
            __type signal = gen.sine( 1, Fn, 0, Fs );
            __type output = filter(&signal);

            #ifdef WRITE_LOGS
            xt << signal << "\n";
            yt << output << "\n";
            dt << time   << "\n";
            #endif
        }
    }

    for( int i = 0 ; i < Fs / 2 ; i++ )
    {
        fcomplex<__type> output = filter.frequency_response( (double)i );

        #ifdef WRITE_LOGS
        pH << __cargf__(output) << "\n";
        Km << __cabsf__(output) << "\n";
        #endif
    }

    // close files

    #ifdef WRITE_LOGS
    xt.close();
    yt.close();
    Km.close();
    pH.close();
    dt.close();
    #endif

    return 0;
}

// Checbyshev type II filter
int filters_cheb2_example()
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

    #ifdef WRITE_LOGS

    // logs directory:
    std::string directory = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // log files
    std::ofstream xt;
    std::ofstream yt;
    std::ofstream pH;
    std::ofstream Km;
    std::ofstream dt;

    // open files
    yt .open(directory + "\\yt.txt");
    xt .open(directory + "\\xt.txt");
    pH .open(directory + "\\pH.txt");
    Km .open(directory + "\\Km.txt");
    dt .open(directory + "\\dt.txt");

    #endif

    // generator initialization
    sgen<__type> gen;

    // filter initialization
    chebyshev_2<__type> filter;
    filter.init(Fs, 11, filter_type::lowpass, {100 , 400}, 80 );
    filter.show();

    // emulation
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time += 1 / Fs )
        {
            __type signal = gen.sine( 1, Fn, 0, Fs );
            __type output = filter(&signal);

            #ifdef WRITE_LOGS
            xt << signal << "\n";
            yt << output << "\n";
            dt << time   << "\n";
            #endif
        }
    }

    for( int i = 0 ; i < Fs / 2 ; i++ )
    {
        fcomplex<__type> output = filter.frequency_response( (double)i );

        #ifdef WRITE_LOGS
        pH << __cargf__(output) << "\n";
        Km << __cabsf__(output) << "\n";
        #endif
    }

    // close files

    #ifdef WRITE_LOGS
    xt.close();
    yt.close();
    Km.close();
    pH.close();
    dt.close();
    #endif

    return 0;
}

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

    #ifdef WRITE_LOGS

    // logs directory:
    std::string directory = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // log files
    std::ofstream xt;
    std::ofstream yt;
    std::ofstream pH;
    std::ofstream Km;
    std::ofstream dt;

    // open files
    yt .open(directory + "\\yt.txt");
    xt .open(directory + "\\xt.txt");
    pH .open(directory + "\\pH.txt");
    Km .open(directory + "\\Km.txt");
    dt .open(directory + "\\dt.txt");

    #endif

    // generator initialization
    sgen<__type> gen;

    // filter initialization
    elliptic<__type> filter;
    filter.init(Fs, 11, filter_type::bandstop, {100 , 400}, 80, 1 );
    filter.show();

    // emulation
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time += 1 / Fs )
        {
            __type signal = gen.sine( 1, Fn, 0, Fs );
            __type output = filter(&signal);

            #ifdef WRITE_LOGS
            xt << signal << "\n";
            yt << output << "\n";
            dt << time   << "\n";
            #endif
        }
    }

    for( int i = 0 ; i < Fs / 2 ; i++ )
    {
        fcomplex<__type> output = filter.frequency_response( (double)i );

        #ifdef WRITE_LOGS
        pH << __cargf__(output) << "\n";
        Km << __cabsf__(output) << "\n";
        #endif
    }

    // close files
    #ifdef WRITE_LOGS
    xt.close();
    yt.close();
    Km.close();
    pH.close();
    dt.close();
    #endif

    return 0;
}

// classic FIR filter example
int filters_fir_example()
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

    #ifdef WRITE_LOGS

    // logs directory:
    std::string directory = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // log files
    std::ofstream xt;
    std::ofstream yt;
    std::ofstream pH;
    std::ofstream Km;
    std::ofstream dt;

    // open files
    yt .open(directory + "\\yt.txt");
    xt .open(directory + "\\xt.txt");
    pH .open(directory + "\\pH.txt");
    Km .open(directory + "\\Km.txt");
    dt .open(directory + "\\dt.txt");

    #endif

    // generator initialization
    sgen<__type> gen;

    // filter initialization
    window_function window;
    window.Chebyshev(60, 80);

    //
    fir<__type> filter;
    filter.init(Fs, filter_type::bandstop, {100, 500}, window, 1);
    filter.show();

    // emulation
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time += 1 / Fs )
        {
            __type signal = gen.sine( 1, Fn, 0, Fs );
            __type output = filter(&signal);

            #ifdef WRITE_LOGS
            xt << signal << "\n";
            yt << output << "\n";
            dt << time   << "\n";
            #endif
        }
    }

    for( int i = 0 ; i < Fs / 2 ; i++ )
    {
        fcomplex<__type> output = filter.frequency_response( (double)i );

        #ifdef WRITE_LOGS
        pH << __cargf__(output) << "\n";
        Km << __cabsf__(output) << "\n";
        #endif
    }

    // close files

    #ifdef WRITE_LOGS
    xt.close();
    yt.close();
    Km.close();
    pH.close();
    dt.close();
    #endif

    return 0;
}

// complex numbers example
int complex_numbers_example()
{
    printf( " ...complex arithmetics utilization example and test... \n " );
    fcomplex<double> z0( 1.0 , 2.0 ) , z1( 2.0 , 0.5 ) , z2( 3.5 , 4.5 );
    printf( "complex math functions check: \n" );
    printf( "z0 = %.4f \t %.4f \n" , __realf__(z0) , __imagf__(z0) );
    printf( "z1 = %.4f \t %.4f \n" , __realf__(z1) , __imagf__(z1) );
    printf( "z2 = %.4f \t %.4f \n" , __realf__(z2) , __imagf__(z2) );
    printf( "\n\n");

    printf( "arithmetics: \n" );

    printf( "z0 * z1          = %.4f \t %.4f \n" , __realf__( z0 * z1 ) , __imagf__( z0 * z1 ));
    printf( "z0 / z1          = %.4f \t %.4f \n" , __realf__( z0 / z1 ) , __imagf__( z0 / z1 ));
    printf( "sqrt( z0 )       = %.4f \t %.4f \n" , __realf__( __csqrtf__(z0)), __imagf__( __csqrtf__(z0)));
    printf( "z0 + z1 + z2     = %.4f \t %.4f \n" , __realf__( z0 + z1 + z2 ), __imagf__( z0 + z1 + z2  ));
    printf( "z0 - z1 - z2     = %.4f \t %.4f \n" , __realf__( z0 - z1 - z2 ), __imagf__( z0 - z1 - z2  ));
    printf( "z0 * z1 / z2     = %.4f \t %.4f \n" , __realf__( z0 * z1 / z2 ), __imagf__( z0 * z1 / z2  ));
    printf( "(z1-z2)/(z1+z2)  = %.4f \t %.4f \n" , __realf__( ( z1 - z2 ) / ( z1 + z2 ) ) , __imagf__( ( z1 - z2 ) / ( z1 + z2 ) ));
    z0 += z2; printf( "( z0 = z0 + z2 ) = %.4f \t %.4f \n" , __realf__(z0), __imagf__(z0));
    z1 *= z2; printf( "( z1 = z1 * z2 ) = %.4f \t %.4f \n" , __realf__(z1), __imagf__(z1));
    z2 /= z1; printf( "( z2 = z2 / z1 ) = %.4f \t %.4f \n" , __realf__(z2), __imagf__(z2));
    printf( "\n\n");

    printf( " complex functions : \n" );
    printf( " sinh( z0 )       = %.4f \t %.4f \n" , __realf__( __sinhf__(z0) ) , __imagf__( __sinhf__(z0) ) );
    printf( " cosh( z0 )       = %.4f \t %.4f \n" , __realf__( __coshf__(z0) ) , __imagf__( __coshf__(z0) ) );
    printf( " tanh( z0 )       = %.4f \t %.4f \n" , __realf__( __tanhf__(z0) ) , __imagf__( __tanhf__(z0) ) );
    printf( " ctnh( z0 )       = %.4f \t %.4f \n" , __realf__( __ctnhf__(z0) ) , __imagf__( __ctnhf__(z0) ) );
    printf( " conj( z0 )       = %.4f \t %.4f \n" , __realf__( __conjf__(z0) ) , __imagf__( __conjf__(z0) ) );
    printf( " norm( z0 )       = %.4f \t %.4f \n" , __realf__( __cnormf__(z0) ) , __imagf__( __cnormf__(z0) ) );
    z0 *=__crotf__<double>( 60.0 , 1); printf( " z0*rot( 60 , 1 ) = %.4f \t %.4f \n" , __realf__( z0 ) , __imagf__( z0 ) );
    printf( "\n\n");

    printf( " scalar functions : \n" );
    printf( " abs( z0 )  = %.4f \n" , __cabsf__<double>(z0) );
    printf( " arg( z0 )  = %.4f \n" , __cargf__<double>(z0) * 180.0 / 3.1415926535897932384626433832795);
    return 0;
}

// special functions examples
int special_functions_example()
{
    double M = 0.7;
    double k = sqrt(M);
    double u = 5;

    printf("__sn__ = %.4f", __sn__( u , k ) );
    printf("__cn__ = %.4f", __cn__( u , k ) );
    printf("__dn__ = %.4f", __dn__( u , k ) );
    printf("__cd__ = %.4f", __cd__( u , k ) );
    printf("__sd__ = %.4f", __sd__( u , k ) );
    printf("__dc__ = %.4f", __dc__( u , k ) );
    printf("__nc__ = %.4f", __nc__( u , k ) );
    printf("__sc__ = %.4f", __sc__( u , k ) );
    printf("__sc__ = %.4f", __sc__( u , k ) );
    printf("__ns__ = %.4f", __ns__( u , k ) );
    printf("__ds__ = %.4f", __ds__( u , k ) );
    printf("__cs__ = %.4f", __cs__( u , k ) );

    return 0;
}

#endif // EXAMPLES_H
