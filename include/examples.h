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
#include "include/sgen.h"
#include "include/logical.h"
#include "dsp.h"

// brief special functions utilization example and check
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

// complex arithmetics utilization example and test
int example1()
{
    printf( " ...complex arithmetics utilization example and test... \n " );
    fcomplex<double> z0( 1.0 , 2.0 ) , z1( 2.0 , 0.5 ) , z2( 3.5 , 4.5 );
    printf( "complex math functions check: \n" );
    printf( "z0 = %.4f \t %.4f \n" , __realf__(z0) , __imagf__(z0) );
    printf( "z1 = %.4f \t %.4f \n" , __realf__(z1) , __imagf__(z1) );
    printf( "z2 = %.4f \t %.4f \n" , __realf__(z2) , __imagf__(z2) );
    printf( "\n\n");

    printf( "arithmetics: \n" );

    printf( "z0 * z1          = %.4f \t %.4f \n" , __realf__( z0 * z1 )                   , __imagf__( z0 * z1 )                   );
    printf( "z0 / z1          = %.4f \t %.4f \n" , __realf__( z0 / z1 )                   , __imagf__( z0 / z1 )                   );
    printf( "sqrt( z0 )       = %.4f \t %.4f \n" , __realf__( __csqrtf__(z0))              , __imagf__( __csqrtf__(z0) )             );
    printf( "z0 + z1 + z2     = %.4f \t %.4f \n" , __realf__( z0 + z1 + z2 )              , __imagf__( z0 + z1 + z2  )             );
    printf( "z0 - z1 - z2     = %.4f \t %.4f \n" , __realf__( z0 - z1 - z2 )              , __imagf__( z0 - z1 - z2  )             );
    printf( "z0 * z1 / z2     = %.4f \t %.4f \n" , __realf__( z0 * z1 / z2 )              , __imagf__( z0 * z1 / z2  )             );
    printf( "(z1-z2)/(z1+z2)  = %.4f \t %.4f \n" , __realf__( ( z1 - z2 ) / ( z1 + z2 ) ) , __imagf__( ( z1 - z2 ) / ( z1 + z2 ) ) );
    z0 += z2; printf( "( z0 = z0 + z2 ) = %.4f \t %.4f \n" , __realf__(z0)                , __imagf__(z0)                          );
    z1 *= z2; printf( "( z1 = z1 * z2 ) = %.4f \t %.4f \n" , __realf__(z1)                , __imagf__(z1)                          );
    z2 /= z1; printf( "( z2 = z2 / z1 ) = %.4f \t %.4f \n" , __realf__(z2)                , __imagf__(z2)                          );
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

// Classic IIR test
int example2()
{
    printf( "...IIR filters utilization example and test ...\n" );

    // define filter and it's input signal data types:
    typedef float __flt_type;
    typedef float __gen_type;

    // emulation parameters:
    double Fs                = 4000;
    double Fn                = 50;
    double time              = 0;
    double EmulationDuration = 0.08;
    int    CycleWidth        = 5;
    int    cycles_num        = 1000 * EmulationDuration / CycleWidth;
    int    frames_per_cycle  = CycleWidth * Fs / 1000;
    int    flt_type          = 0; // ( 0 - lowpass 1 , 1 - highpass 2 , 2 - bandpass , 3 - bandstop )

    // logs directory:
    std::string directory = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // files:
    std::ofstream yt;
    std::ofstream cheb1_y;
    std::ofstream cheb2_y;
    std::ofstream buttf_y;
    std::ofstream ellip_y;

    std::ofstream cheb1_km;
    std::ofstream cheb2_km;
    std::ofstream buttf_km;
    std::ofstream ellip_km;
    std::ofstream cheb1_ph;
    std::ofstream cheb2_ph;
    std::ofstream buttf_ph;
    std::ofstream ellip_ph;
    std::ofstream ff;
    std::ofstream tt;
    yt      .open( directory + "\\yt.txt"      );
    cheb1_y .open( directory + "\\cheb1_y.txt" );
    cheb2_y .open( directory + "\\cheb2_y.txt" );
    buttf_y .open( directory + "\\buttf_y.txt" );
    ellip_y .open( directory + "\\ellip_y.txt" );
    cheb1_km.open( directory + "\\cheb1_km.txt" );
    cheb2_km.open( directory + "\\cheb2_km.txt" );
    buttf_km.open( directory + "\\buttf_km.txt" );
    ellip_km.open( directory + "\\ellip_km.txt" );
    cheb1_ph.open( directory + "\\cheb1_ph.txt" );
    cheb2_ph.open( directory + "\\cheb2_ph.txt" );
    buttf_ph.open( directory + "\\buttf_ph.txt" );
    ellip_ph.open( directory + "\\ellip_ph.txt" );
    ff.open( directory + "\\ff.txt" );
    tt     .open( directory + "\\tt.txt"      );

    // signal generator:
    sgen< __gen_type > gen;

    // IIR filters:
    DSP::chebyshev_1< __flt_type > cheb1;
    DSP::chebyshev_2< __flt_type > cheb2;
    DSP::butterworth< __flt_type > buttf;
    DSP::elliptic   < __flt_type > ellip;

    switch ( flt_type )
    {
        case 0: // lowpass
            cheb1.init( Fs , 8 , DSP::filter_type::lowpass , { 100 , 100 } , { 1  , 1 } );
            cheb2.init( Fs , 8 , DSP::filter_type::lowpass , { 100 , 100 } , { 80 , 1 } );
            buttf.init( Fs , 8 , DSP::filter_type::lowpass , { 100 , 100 } , { 1  , 1 } );
            ellip.init( Fs , 8 , DSP::filter_type::lowpass , { 100 , 100 } , { 80 , 1 } );
        break;

        case 1: // highpass
            cheb1.init( Fs , 8 , DSP::filter_type::highpass , { 100 , 100 } , { 1  , 1 } );
            cheb2.init( Fs , 8 , DSP::filter_type::highpass , { 100 , 100 } , { 80 , 1 } );
            buttf.init( Fs , 8 , DSP::filter_type::highpass , { 100 , 100 } , { 1  , 1 } );
            ellip.init( Fs , 8 , DSP::filter_type::highpass , { 100 , 100 } , { 80 , 1 } );
        break;

        case 2: // bandpass
            cheb1.init( Fs , 8 , DSP::filter_type::bandpass , { 100 , 100 } , { 1  , 1 } );
            cheb2.init( Fs , 8 , DSP::filter_type::bandpass , { 100 , 100 } , { 80 , 1 } );
            buttf.init( Fs , 8 , DSP::filter_type::bandpass , { 100 , 100 } , { 1  , 1 } );
            ellip.init( Fs , 8 , DSP::filter_type::bandpass , { 100 , 100 } , { 80 , 1 } );
        break;

        case 3: // bandstop
            cheb1.init( Fs , 8 , DSP::filter_type::bandstop , { 100 , 100 } , { 1  , 1 } );
            cheb2.init( Fs , 8 , DSP::filter_type::bandstop , { 100 , 100 } , { 80 , 1 } );
            buttf.init( Fs , 8 , DSP::filter_type::bandstop , { 100 , 100 } , { 1  , 1 } );
            ellip.init( Fs , 8 , DSP::filter_type::bandstop , { 100 , 100 } , { 80 , 1 } );
        break;

    }

    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            gen.sine( 1 , Fn , 0 , Fs );

            // filtering:
            double y1 = cheb1 ( &gen.m_out );
            double y2 = cheb2 ( &gen.m_out );
            double y3 = buttf ( &gen.m_out );
            double y4 = ellip ( &gen.m_out );

            // generating output:
            yt      << gen.m_out  << "\n";
            cheb1_y << y1 << "\n";
            cheb2_y << y2 << "\n";
            buttf_y << y3 << "\n";
            ellip_y << y4 << "\n";
            tt      << time       << "\n";
            time += 1 / Fs;
        }
    }

    // frequency response computation:
    DSP::fr fr1;
    DSP::fr fr2;
    DSP::fr fr3;
    DSP::fr fr4;

    for( int i = 0 ; i < Fs / 2 ; i++ )
    {
        fr1 = cheb1.frequency_response( i );
        fr2 = cheb2.frequency_response( i );
        fr3 = buttf.frequency_response( i );
        fr4 = ellip.frequency_response( i );
        cheb1_km << fr1.Km << "\n";
        cheb1_ph << fr1.pH << "\n";
        cheb2_km << fr2.Km << "\n";
        cheb2_ph << fr2.pH << "\n";
        buttf_km << fr3.Km << "\n";
        buttf_ph << fr3.pH << "\n";
        ellip_km << fr4.Km << "\n";
        ellip_ph << fr4.pH << "\n";
        ff << (double)i << "\n";
    }

    // close files:
    yt     .close();
    cheb1_y.close();
    cheb2_y.close();
    buttf_y.close();
    ellip_y.close();
    cheb1_km.close();
    cheb2_km.close();
    buttf_km.close();
    ellip_km.close();
    cheb1_ph.close();
    cheb2_ph.close();
    buttf_ph.close();
    ellip_ph.close();
    ff      .close();
    tt      .close();

    return 0;
}

// Classic FIR test
int example3()
{
    printf( " ...FIR filters utilization example and test... \n " );

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
    std::ofstream lp_yt;
    std::ofstream hp_yt;
    std::ofstream bp_yt;
    std::ofstream bs_yt;
    std::ofstream yt;
    std::ofstream lp_km;
    std::ofstream lp_ph;
    std::ofstream hp_km;
    std::ofstream hp_ph;
    std::ofstream bp_km;
    std::ofstream bp_ph;
    std::ofstream bs_km;
    std::ofstream bs_ph;
    std::ofstream rdft_re;
    std::ofstream rdft_im;
    std::ofstream rdft_km;
    std::ofstream rdft_ph;
    std::ofstream rdft_abs;
    std::ofstream ff;
    std::ofstream tt;

    // opening files:
    yt   .open  ( directory + "\\yt.txt"      );
    lp_yt.open  ( directory + "\\lp_yt.txt"   );
    hp_yt.open  ( directory + "\\hp_yt.txt"   );
    bp_yt.open  ( directory + "\\bp_yt.txt"   );
    bs_yt.open  ( directory + "\\bs_yt.txt"   );
    lp_km.open  ( directory + "\\lp_km.txt"   );
    lp_ph.open  ( directory + "\\lp_ph.txt"   );
    hp_km.open  ( directory + "\\hp_km.txt"   );
    hp_ph.open  ( directory + "\\hp_ph.txt"   );
    bp_km.open  ( directory + "\\bp_km.txt"   );
    bp_ph.open  ( directory + "\\bp_ph.txt"   );
    bs_km.open  ( directory + "\\bs_km.txt"   );
    bs_ph.open  ( directory + "\\bs_ph.txt"   );
    rdft_km.open( directory + "\\rdft_km.txt" );
    rdft_ph.open( directory + "\\rdft_ph.txt" );
    rdft_re.open( directory + "\\rdft_re.txt" );
    rdft_im.open( directory + "\\rdft_im.txt" );
    rdft_abs.open( directory + "\\rdft_abs.txt" );
    tt   .open  ( directory + "\\tt.txt"      );
    ff   .open  ( directory + "\\ff.txt"      );

    // signal generator:
    sgen< __gen_type > gen;

    // filter configuration:
    DSP::fir              < __flt_type > lp_flt;
    DSP::fir              < __flt_type > hp_flt;
    DSP::fir              < __flt_type > bp_flt;
    DSP::fir              < __flt_type > bs_flt;
    DSP::recursive_fourier< __flt_type > rdft;

    // allocating window function:
    int order = 80;
    __fx64 *window = Chebyshev(60, order);

    // allocating filters:
    lp_flt.init(Fs , order , DSP::filter_type::lowpass  , { 100 , 500 } , window , true );
    hp_flt.init(Fs , order , DSP::filter_type::highpass , { 100 , 500 } , window , true );
    bp_flt.init(Fs , order , DSP::filter_type::bandpass , { 100 , 500 } , window , true );
    bs_flt.init(Fs , order , DSP::filter_type::bandstop , { 100 , 500 } , window , true );

    // recursive Fourier filter allocation:
    rdft.init(Fs, 50 , 1 , 1);

    // emulation:
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            // signal generation and filtering:
            gen.sine( 1 , Fn , 0 , Fs );

            // filtering:
            double yt1 = lp_flt( &gen.m_out );
            double yt2 = hp_flt( &gen.m_out );
            double yt3 = bp_flt( &gen.m_out );
            double yt4 = bs_flt( &gen.m_out );
            DSP::harmonic v = rdft( &gen.m_out );

            // store signal and filter output:
            lp_yt   << yt1       << "\n";
            hp_yt   << yt2       << "\n";
            bp_yt   << yt3       << "\n";
            bs_yt   << yt4       << "\n";
            rdft_re << v.re      << "\n";
            rdft_im << v.im      << "\n";
            rdft_abs << sqrt( v.re*v.re + v.im*v.im )      << "\n";
            yt      << gen.m_out << "\n";
            tt      << time      << "\n";

            // time increment:
            time += 1 / Fs;
        }
    }

    // filter frequency response computation:
    for( int i = 0 ; i < Fs / 2 ; i++ )
    {
        // frequency response computation:
        DSP::fr fr1 = lp_flt.frequency_response(i);
        DSP::fr fr2 = hp_flt.frequency_response(i);
        DSP::fr fr3 = bp_flt.frequency_response(i);
        DSP::fr fr4 = bs_flt.frequency_response(i);
        DSP::fr fr5 = rdft  .frequency_response(i);

        // frequency response recording:
        lp_km   << fr1.Km << "\n";
        lp_ph   << fr1.pH << "\n";
        hp_km   << fr2.Km << "\n";
        hp_ph   << fr2.pH << "\n";
        bp_km   << fr3.Km << "\n";
        bp_ph   << fr3.pH << "\n";
        bs_km   << fr4.Km << "\n";
        bs_ph   << fr4.pH << "\n";
        rdft_km << fr5.Km << "\n";
        rdft_ph << fr5.pH << "\n";

        ff << i << "\n";
    }

    // close files:
    yt     .close();
    lp_yt  .close();
    hp_yt  .close();
    bp_yt  .close();
    bs_yt  .close();
    lp_km  .close();
    lp_ph  .close();
    hp_km  .close();
    hp_ph  .close();
    bp_km  .close();
    bp_ph  .close();
    bs_km  .close();
    bs_ph  .close();
    rdft_re.close();
    rdft_im.close();
    rdft_km.close();
    rdft_ph.close();
    tt     .close();
    rdft_abs.close();

    // memory deallocation:
    free(window);

    return 0;
}

// Elementary transfer functions tests:
int example4()
{
    printf( " ...FIR filters utilization example and test... \n " );

    // define filter and it's input signal data types:
    typedef double  __flt_type;
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

    // logs:
    std::ofstream yt;
    std::ofstream comb_yt;
    std::ofstream combeq_yt;
    std::ofstream diff_yt;
    std::ofstream intg_yt;
    std::ofstream lead_yt;
    std::ofstream flt2_yt;
    std::ofstream aper_yt;

    std::ofstream comb_km;
    std::ofstream combeq_km;
    std::ofstream diff_km;
    std::ofstream intg_km;
    std::ofstream lead_km;
    std::ofstream flt2_km;
    std::ofstream aper_km;

    std::ofstream comb_ph;
    std::ofstream combeq_ph;
    std::ofstream diff_ph;
    std::ofstream intg_ph;
    std::ofstream lead_ph;
    std::ofstream flt2_ph;
    std::ofstream aper_ph;

    std::ofstream tt;
    std::ofstream ff;

    yt       .open( directory + "\\yt.txt");
    comb_yt  .open( directory + "\\comb_yt.txt");
    combeq_yt.open( directory + "\\combeq_yt.txt");
    diff_yt  .open( directory + "\\diff_yt.txt");
    intg_yt  .open( directory + "\\intg_yt.txt");
    lead_yt  .open( directory + "\\lead_yt.txt");
    flt2_yt  .open( directory + "\\flt2_yt.txt");
    aper_yt  .open( directory + "\\aper_yt.txt");

    comb_km  .open( directory + "\\comb_km.txt");
    combeq_km.open( directory + "\\combeq_km.txt");
    diff_km  .open( directory + "\\diff_km.txt");
    intg_km  .open( directory + "\\intg_km.txt");
    lead_km  .open( directory + "\\lead_km.txt");
    flt2_km  .open( directory + "\\flt2_km.txt");
    aper_km  .open( directory + "\\aper_km.txt");

    comb_ph  .open( directory + "\\comb_ph.txt");
    combeq_ph.open( directory + "\\combeq_ph.txt");
    diff_ph  .open( directory + "\\diff_ph.txt");
    intg_ph  .open( directory + "\\intg_ph.txt");
    lead_ph  .open( directory + "\\lead_ph.txt");
    flt2_ph  .open( directory + "\\flt2_ph.txt");
    aper_ph  .open( directory + "\\aper_ph.txt");

    tt       .open( directory + "\\tt.txt");
    ff       .open( directory + "\\ff.txt");

    // signal generator:
    sgen< __gen_type > gen;

    // fitters:
    DSP::derivative         < __flt_type > diff;
    DSP::integrator         < __flt_type > intg;
    DSP::leadlag            < __flt_type > lead;
    DSP::second_order_filter< __flt_type > sof;
    DSP::fcomb              < __flt_type > comb;
    DSP::fcombeq            < __flt_type > combeq;
    DSP::derivative         < __flt_type > aper;

    // filters initialization:
    diff.init  (Fs, Fn, (__flt_type)1 / Fs );
    intg.init  (Fs, Fn );
    lead.init  (Fs, Fn, 0.01, 0.02 );
    sof .init  (Fs, Fn, 100, 0.707, DSP::lowpass);
    comb.init  (Fs, Fn, 0);
    combeq.init(Fs, Fn, 5, 0.05, 0);
    aper  .init(Fs, Fn, 0.01);

    // emulation:
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            // signal generation and filtering:
            gen.sine( 1 , Fn , 0 , Fs );

            // filtering:
            double yt1 = diff  (&gen.m_out);
            double yt2 = intg  (&gen.m_out);
            double yt3 = lead  (&gen.m_out);
            double yt4 = sof   (&gen.m_out);
            double yt5 = comb  (&gen.m_out);
            double yt6 = combeq(&gen.m_out);
            double yt7 = aper  (&gen.m_out);

            // logging:
            yt        << gen.m_out << "\n";
            diff_yt   << yt1       << "\n";
            intg_yt   << yt2       << "\n";
            lead_yt   << yt3       << "\n";
            flt2_yt   << yt4       << "\n";
            comb_yt   << yt5       << "\n";
            combeq_yt << yt6       << "\n";
            aper_yt   << yt7       << "\n";
            tt        << time << "\n";

            // time increment:
            time += 1 / Fs;
        }
    }

    // filter frequency response computation:
    for( int i = 0 ; i < Fs / 2 ; i++ )
    {
        DSP::fr fr1 = diff  .frequency_response(i);
        DSP::fr fr2 = intg  .frequency_response(i);
        DSP::fr fr3 = lead  .frequency_response(i);
        DSP::fr fr4 = sof   .frequency_response(i);
        DSP::fr fr5 = comb  .frequency_response(i);
        DSP::fr fr6 = combeq.frequency_response(i);
        DSP::fr fr7 = aper  .frequency_response(i);

        // logging:
        diff_km   << fr1.Km << "\n";
        intg_km   << fr2.Km << "\n";
        lead_km   << fr3.Km << "\n";
        flt2_km   << fr4.Km << "\n";
        comb_km   << fr5.Km << "\n";
        combeq_km << fr6.Km << "\n";
        aper_km   << fr7.Km << "\n";

        diff_ph   << fr1.pH << "\n";
        intg_ph   << fr2.pH << "\n";
        lead_ph   << fr3.pH << "\n";
        flt2_ph   << fr4.pH << "\n";
        comb_ph   << fr5.pH << "\n";
        combeq_ph << fr6.pH << "\n";
        aper_ph   << fr7.pH << "\n";

        ff << i << "\n";
    }

    // closing files:
    yt       .close();
    comb_yt  .close();
    combeq_yt.close();
    diff_yt  .close();
    intg_yt  .close();
    lead_yt  .close();
    flt2_yt  .close();
    aper_yt  .close();

    comb_km  .close();
    combeq_km.close();
    diff_km  .close();
    intg_km  .close();
    lead_km  .close();
    flt2_km  .close();
    aper_km  .close();

    comb_ph  .close();
    combeq_ph.close();
    diff_ph  .close();
    intg_ph  .close();
    lead_ph  .close();
    flt2_ph  .close();
    aper_ph  .close();

    ff       .close();
    tt       .close();

    return 0;
}

#endif // EXAMPLES_H
