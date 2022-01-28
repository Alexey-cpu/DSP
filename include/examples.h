/*!
 * \file
 * \brief   Examples
 * \authors A.Tykvinskiy
 * \date    28.01.2022
 * \version 1.0
 *
 * The examples of DSP template ToolBox utilization
*/

#ifndef EXAMPLES_H
#define EXAMPLES_H

// system headers:
#include <iostream>
#include <fstream>
#include "string"
#include <QElapsedTimer>

// DSP template library headers:
#include "include/special_functions.h"
#include "include/buffer.h"
#include "include/FIR.h"
#include "include/sgen.h"
#include "include/iir.h"
#include "include/recursive_fourier.h"

/*! \brief special functions utilization example and check */
int example0()
{
    printf( " ...special functions utilization example and test... \n " );

    // timer:
    QElapsedTimer timer;

    // Matlab elliptic functions use M = k * k parameter. Be carefull !!!

    double M     = 0.7;
    double k     = sqrt(M);
    int    N     = 100;
    double step  = 1.0 / (double)N;
    double upper = +5;
    double lower = -5;
    int     NN   = (upper - lower) / step+2;
    double  *u   = ( double* ) calloc( NN , sizeof ( double ) );

    // inout vector filling:
    for( int i = 0 ; i < NN ; i++ )
    {
        u[i] = lower;
        lower += step;
    }

    // logs directory:
    std::string logs = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // files:
    std::ofstream sn;
    std::ofstream cn;
    std::ofstream dn;
    std::ofstream cd;
    std::ofstream sd;
    std::ofstream nd;
    std::ofstream dc;
    std::ofstream nc;
    std::ofstream sc;
    std::ofstream ns;
    std::ofstream ds;
    std::ofstream cs;
    std::ofstream uu;

    // open files:
    sn.open( logs + "\\sn.txt" );
    cn.open( logs + "\\cn.txt" );
    dn.open( logs + "\\dn.txt" );
    cd.open( logs + "\\cd.txt" );
    sd.open( logs + "\\sd.txt" );
    nd.open( logs + "\\nd.txt" );
    dc.open( logs + "\\dc.txt" );
    nc.open( logs + "\\nc.txt" );
    sc.open( logs + "\\sc.txt" );
    ns.open( logs + "\\ns.txt" );
    ds.open( logs + "\\ds.txt" );
    cs.open( logs + "\\cs.txt" );
    uu.open( logs + "\\uu.txt" );

    // write files:
    for( int i = 0 ; i < NN ; i++ )
    {
        sn << __sn__( u[i] , k ) << "\n";
        cn << __cn__( u[i] , k ) << "\n";
        dn << __dn__( u[i] , k ) << "\n";
        cd << __cd__( u[i] , k ) << "\n";
        sd << __sd__( u[i] , k ) << "\n";
        nd << __nd__( u[i] , k ) << "\n";
        dc << __dc__( u[i] , k ) << "\n";
        nc << __nc__( u[i] , k ) << "\n";
        sc << __sc__( u[i] , k ) << "\n";
        ns << __ns__( u[i] , k ) << "\n";
        ds << __ds__( u[i] , k ) << "\n";
        cs << __cs__( u[i] , k ) << "\n";
        uu << u[i] << "\n";
    }

    // main elliptic functions execution time estimation:
    timer.start();
    for( int i = 0 ; i < NN ; i++ )
    {
        __sn__( u[i] , k );
        __cn__( u[i] , k );
        __dn__( u[i] , k );
    }
    std::cout << "dt = " << timer.nsecsElapsed()/1e9 << "\n";

    // close files:
    sn.close();
    cn.close();
    dn.close();
    cd.close();
    sd.close();
    nd.close();
    dc.close();
    nc.close();
    sc.close();
    ns.close();
    ds.close();
    cs.close();
    uu.close();

    // memory deallocation:
    free(u);

    return 0;
}

/*! \brief complex arithmetics utilization example and test */
int example1()
{
    printf( " ...complex arithmetics utilization example and test... \n " );

    complex<double> z0( 1.0 , 2.0 ) , z1( 2.0 , 0.5 ) , z2( 3.5 , 4.5 );
    printf( "complex math functions check: \n" );
    printf( "z0 = %.4f \t %.4f \n" , z0.m_re , z0.m_im );
    printf( "z1 = %.4f \t %.4f \n" , z1.m_re , z1.m_im );
    printf( "z2 = %.4f \t %.4f \n" , z2.m_re , z2.m_im );
    printf( "\n\n");

    printf( "arithmetics: \n" );
    printf( "sqrt( z0 )       = %.4f \t %.4f \n" , __sqrtf__(z0).m_re , __sqrtf__(z0).m_im );
    printf( "z0 + z1 + z2     = %.4f \t %.4f \n" , ( z0 + z1 + z2 ).m_re , ( z0 + z1 + z2 ).m_im );
    printf( "z0 - z1 - z2     = %.4f \t %.4f \n" , ( z0 - z1 - z2 ).m_re , ( z0 - z1 - z2 ).m_im );
    printf( "z0 * z1 / z2     = %.4f \t %.4f \n" , ( z0 * z1 / z2 ).m_re , ( z0 * z1 / z2 ).m_im );
    printf( "(z1-z2)/(z1+z2)  = %.4f \t %.4f \n" , ( ( z1 - z2 ) / ( z1 + z2 ) ).m_re , ( ( z1 - z2 ) / ( z1 + z2 ) ).m_im );
    printf( "( z0 = z0 + z2 ) = %.4f \t %.4f \n" , ( z0 += z2 ).m_re , ( z0 += z2 ).m_im );
    printf( "( z1 = z1 * z2 ) = %.4f \t %.4f \n" , ( z1 *= z2 ).m_re , ( z1 *= z2 ).m_im );
    printf( "( z2 = z2 / z1 ) = %.4f \t %.4f \n" , ( z2 /= z1 ).m_re , ( z2 /= z1 ).m_im );
    printf( "\n\n");

    printf( " complex functions : \n" );
    printf( " sinh( z0 )       = %.4f \t %.4f \n" , __sinhf__(z0).m_re , __sinhf__(z0).m_im );
    printf( " cosh( z0 )       = %.4f \t %.4f \n" , __coshf__(z0).m_re , __coshf__(z0).m_im );
    printf( " tanh( z0 )       = %.4f \t %.4f \n" , __tanhf__(z0).m_re , __tanhf__(z0).m_im );
    printf( " ctnh( z0 )       = %.4f \t %.4f \n" , __ctnhf__(z0).m_re , __ctnhf__(z0).m_im );
    printf( " conj( z0 )       = %.4f \t %.4f \n" , __conjf__(z0).m_re , __conjf__(z0).m_im );
    printf( " norm( z0 )       = %.4f \t %.4f \n" , __normf__(z0).m_re , __normf__(z0).m_im );
    printf( " z0*rot( 60 , 1 ) = %.4f \t %.4f \n" , ( z0 *=__rotf__( 60.0 , 1) ).m_re , ( z0 *=__rotf__( 60.0 , 1) ).m_im );
    printf( "\n\n");

    printf( " scalar functions : \n" );
    printf( " abs( z0 )  = %.4f \n" , __absf__(z0) );
    printf( " arg( z0 )  = %.4f \n" , __argf__(z0) * 180 / 3.14 );

    return 0;
}

/*! \brief FIR filters utilization example and test */
int example2()
{
    printf( " ...FIR filters utilization example and test... \n " );

    // define filter and it's input signal data types:
    typedef float  __flt_type;
    typedef double __gen_type;

    // emulation parameters:
    double Fs                = 4000;
    double Fn                = 50;
    double time              = 0;
    double EmulationDuration = 1;
    int    CycleWidth        = 5;
    int    cycles_num        = 1000 * EmulationDuration / CycleWidth;
    int    frames_per_cycle  = CycleWidth * Fs / 1000;

    // logs directory:
    std::string directory = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // files:
    std::ofstream yt;
    std::ofstream ft;
    std::ofstream Km;
    std::ofstream pH;
    std::ofstream FF;
    std::ofstream tt;
    std::ofstream cf;
    yt.open( directory + "\\yt.txt" );
    ft.open( directory + "\\ft.txt" );
    Km.open( directory + "\\Km.txt" );
    pH.open( directory + "\\pH.txt" );
    FF.open( directory + "\\FF.txt" );
    cf.open( directory + "\\cf.txt" );
    tt.open( directory + "\\tt.txt" );

    // signal generator:
    sgen< __gen_type > gen;

    // filter configuration:
    fir< __flt_type > flt;
    // flt.lp_init( Fs , 50 , 100 , 80 , 1 );
    // flt.hp_init( Fs , 50 , 100 , 80 , 1 );
    // flt.bp_init( Fs , 50 , 100 , 100 , 80 , 1 );
     flt.bs_init( Fs , 50 , 100 , 100 , 80 , 1 );

    // window construction:
    flt.m_wind.Chebyshev(60);
    flt.allocate();

    // emulation:

    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            // signal generation and filtering:
            gen( 1 , Fn , 0 , Fs );

            // filtering:
            flt( &gen.m_out );

            // store signal and filter output:
            yt << gen.m_out << "\n";
            ft << flt.m_out << "\n";
            tt << time      << "\n";

            // time increment:
            time += 1 / Fs;
        }
    }

    // filter frequency response computation:
    for( int i = 0 ; i < Fs / 2 ; i++ )
    {
        fir_fr< double > fr = flt.freq_resp( i );
        Km << fr.Km << "\n";
        pH << fr.pH << "\n";
        FF << i     << "\n";
    }

    // close files:
    yt.close();
    ft.close();
    Km.close();
    pH.close();
    FF.close();
    cf.close();
    tt.close();

    // memory deallocation:
    flt.deallocate();

    return 0;
}

/*! \brief comb FIR filter utilization example and test */
int example3()
{
    printf( " ...comb FIR filter utilization example and test... \n " );

    // define filter and it's input signal data types:
    typedef float  __flt_type;
    typedef double __gen_type;

    // emulation parameters:
    double Fs                = 4000;
    double Fn                = 50;
    double time              = 0;
    double EmulationDuration = 0.08;
    int    CycleWidth        = 5;
    int    cycles_num        = 1000 * EmulationDuration / CycleWidth;
    int    frames_per_cycle  = CycleWidth * Fs / 1000;

    // logs directory:
    std::string directory = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // files:
    std::ofstream yt;
    std::ofstream ft;
    std::ofstream Km;
    std::ofstream pH;
    std::ofstream FF;
    std::ofstream tt;
    std::ofstream cf;
    yt.open( directory + "\\yt.txt" );
    ft.open( directory + "\\ft.txt" );
    Km.open( directory + "\\Km.txt" );
    pH.open( directory + "\\pH.txt" );
    FF.open( directory + "\\FF.txt" );
    cf.open( directory + "\\cf.txt" );
    tt.open( directory + "\\tt.txt" );

    // signal generator:
    sgen< __gen_type > gen;

    // filter configuration:
    fcomb< __flt_type > flt;
    flt.init( Fs , Fn );
    flt.allocate();

    // emulation:
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            // signal generation and filtering:
            gen( 1 , Fn , 0 , Fs );

            // filtering:
            flt( &gen.m_out );

            // store signal and filter output:
            yt << gen.m_out << "\n";
            ft << flt.m_out << "\n";
            tt << time      << "\n";

            // time increment:
            time += 1 / Fs;
        }
    }

    // filter frequency response computation:
    for( int i = 0 ; i < Fs / 2 ; i++ )
    {
        fir_fr< double > fr = flt.freq_resp( i );
        Km << fr.Km << "\n";
        pH << fr.pH << "\n";
        FF << i     << "\n";
    }

    // close files:
    yt.close();
    ft.close();
    Km.close();
    pH.close();
    FF.close();
    cf.close();
    tt.close();

    // memory deallocation:
    flt.deallocate();

    return 0;
}

/*! \brief equalized comb FIR filter utilization example and test */
int example4()
{
    printf( " ...equalized comb FIR filter utilization example and test... \n " );

    // define filter and it's input signal data types:
    typedef float  __flt_type;
    typedef double __gen_type;

    // emulation parameters:
    double Fs                = 4000;
    double Fn                = 50;
    double time              = 0;
    double EmulationDuration = 0.08;
    int    CycleWidth        = 5;
    int    cycles_num        = 1000 * EmulationDuration / CycleWidth;
    int    frames_per_cycle  = CycleWidth * Fs / 1000;

    // logs directory:
    std::string directory = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // files:
    std::ofstream yt;
    std::ofstream ft;
    std::ofstream Km;
    std::ofstream pH;
    std::ofstream FF;
    std::ofstream tt;
    std::ofstream cf;
    yt.open( directory + "\\yt.txt" );
    ft.open( directory + "\\ft.txt" );
    Km.open( directory + "\\Km.txt" );
    pH.open( directory + "\\pH.txt" );
    FF.open( directory + "\\FF.txt" );
    cf.open( directory + "\\cf.txt" );
    tt.open( directory + "\\tt.txt" );

    // signal generator:
    sgen< __gen_type > gen;

    // filter configuration:
    fcombeq< __flt_type > flt;
    flt.init( Fs , Fn , 1 , 0.01 );
    flt.allocate();

    // emulation:
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            // signal generation and filtering:
            gen( 1 , Fn , 0 , Fs );

            // filtering:
            flt( &gen.m_out );

            // store signal and filter output:
            yt << gen.m_out << "\n";
            ft << flt.m_out << "\n";
            tt << time      << "\n";

            // time increment:
            time += 1 / Fs;
        }
    }

    // filter frequency response computation:
    for( int i = 0 ; i < Fs / 2 ; i++ )
    {
        fir_fr< double > fr = flt.freq_resp( i );
        Km << fr.Km << "\n";
        pH << fr.pH << "\n";
        FF << i     << "\n";
    }

    // close files:
    yt.close();
    ft.close();
    Km.close();
    pH.close();
    FF.close();
    cf.close();
    tt.close();

    // memory deallocation:
    flt.deallocate();

    return 0;
}


#endif // EXAMPLES_H
