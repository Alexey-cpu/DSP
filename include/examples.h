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
#include "include/fir.h"
#include "include/sgen.h"
#include "include/iir.h"
#include "include/recursive_fourier.h"
#include "include/quad_mltpx.h"
#include "include/transfer_functions.h"
#include "include/logical.h"

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

/*! \brief real-time quadrature demodulator utilization example and test  */
int example5()
{
    printf( " ...real-time quadrature demodulator utilization example and test... \n " );

    // define filter and it's input signal data types:
    typedef float  __flt_type;
    typedef float __gen_type;

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
    std::ofstream re;
    std::ofstream im;
    std::ofstream abs;
    std::ofstream phs;
    std::ofstream df;
    std::ofstream ff;
    std::ofstream tt;
    yt .open( directory + "\\yt.txt"  );
    re .open( directory + "\\re.txt"  );
    im .open( directory + "\\im.txt"  );
    abs.open( directory + "\\abs.txt" );
    phs.open( directory + "\\phs.txt" );
    df .open( directory + "\\df.txt"  );
    ff .open( directory + "\\ff.txt"   );
    tt .open( directory + "\\tt.txt"   );

    // signal generator:
    sgen< __gen_type > gen;
    __gen_type *signal = ( __gen_type* )calloc( frames_per_cycle , sizeof ( __gen_type ) );

    // filter initialization and memory allocation:
    quad_mltpx_rt<__flt_type> mltpx;
    mltpx.init( Fs , Fn , CycleWidth );
    mltpx.allocate();

    for( int i = 0 ; i < cycles_num ; i++ )
    {
        // generating signal buffer:
        for( int j = 0 ; j < frames_per_cycle ; j++ ) signal[j] = gen( 1 , Fn , 30 , Fs );

        // signal parameters computation:
        mltpx( signal , 0 , 0 );

        // generating outout:
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            yt  << signal[j]     << "\n";
            re  << mltpx.m_Re[j] << "\n";
            im  << mltpx.m_Im[j] << "\n";
            abs << sqrt( mltpx.m_Re[j] * mltpx.m_Re[j] + mltpx.m_Im[j] *  mltpx.m_Im[j] ) << "\n";
            phs << atan2( mltpx.m_Im[j] , mltpx.m_Re[j] ) * 180 / 3.14 << "\n";
            df  << mltpx.m_dF << "\n";
            ff  << mltpx.m_F  << "\n";
            tt  << time  << "\n";
            time += 1/ Fs;
        }
    }

    // close files:
    yt .close();
    re .close();
    im .close();
    abs.close();
    phs.close();
    df .close();
    ff .close();
    tt .close();

    // memory deallocation:
    mltpx.deallocate();
    free( signal );

    return 0;
}

/*! \brief quadrature demodulator utilization example and test  */
int example6()
{
    printf( " ... quadrature demodulator utilization example and test... \n " );

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

    // logs directory:
    std::string directory = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // files:
    std::ofstream yt;
    std::ofstream re;
    std::ofstream im;
    std::ofstream abs;
    std::ofstream phs;
    std::ofstream df;
    std::ofstream ff;
    std::ofstream tt;
    yt .open( directory + "\\yt.txt"  );
    re .open( directory + "\\re.txt"  );
    im .open( directory + "\\im.txt"  );
    abs.open( directory + "\\abs.txt" );
    phs.open( directory + "\\phs.txt" );
    df .open( directory + "\\df.txt"  );
    ff .open( directory + "\\ff.txt"  );
    tt .open( directory + "\\tt.txt"  );

    // signal generator:
    sgen< __gen_type > gen;
    __gen_type *signal = ( __gen_type* )calloc( frames_per_cycle , sizeof ( __gen_type ) );

    // filter initialization and memory allocation:
    quad_mltpx<__flt_type> mltpx;
    mltpx.init( Fs , Fn , 60 , 95 , CycleWidth );
    mltpx.allocate();

    for( int i = 0 ; i < cycles_num ; i++ )
    {
        // generating signal buffer:
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            // generating output:
            signal[j] = gen( 1 , Fn , 30 , Fs );
        }

        // signal parameters computation:
        mltpx( signal , 0 );

        // generating output:
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            yt  << signal[j] << "\n";
            re  << mltpx.m_Re << "\n";
            im  << mltpx.m_Im << "\n";
            abs << sqrt( mltpx.m_Re * mltpx.m_Re + mltpx.m_Im *  mltpx.m_Im ) << "\n";
            phs << atan2( mltpx.m_Im , mltpx.m_Re ) * 180 / 3.14 << "\n";
            df  << mltpx.m_dF << "\n";
            ff  << mltpx.m_F  << "\n";
            tt  << time  << "\n";
            time += 1/ Fs;
        }
    }

    // close files:
    yt .close();
    re .close();
    im .close();
    abs.close();
    phs.close();
    df .close();
    ff .close();
    tt .close();

    // memory deallocation:
    mltpx.deallocate();
    free( signal );

    return 0;
}

/*! \brief simple filters utilization example and test  */
int example7()
{
    printf( "...simple filters utilization example and test...\n" );

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

    // logs directory:
    std::string directory = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // files:
    std::ofstream yt;
    std::ofstream dif_y;
    std::ofstream app_y;
    std::ofstream leg_y;
    std::ofstream int_y;
    std::ofstream lpf_y;
    std::ofstream hpf_y;
    std::ofstream bpf_y;
    std::ofstream bsf1_y;
    std::ofstream bsf2_y;
    std::ofstream tt;
    yt   .open( directory + "\\yt.txt"    );
    dif_y.open( directory + "\\dif_y.txt" );
    app_y.open( directory + "\\app_y.txt" );
    leg_y.open( directory + "\\leg_y.txt" );
    int_y.open( directory + "\\int_y.txt" );
    lpf_y.open( directory + "\\lpf_y.txt" );
    hpf_y.open( directory + "\\hpf_y.txt" );
    bpf_y.open( directory + "\\bpf_y.txt" );
    bsf1_y.open( directory + "\\bsf1_y.txt" );
    bsf2_y.open( directory + "\\bsf2_y.txt" );
    tt   .open( directory + "\\tt.txt"    );

    // signal generator:
    sgen< __gen_type > gen;

    // transfer functions:
    differentiator        < __flt_type > dif;
    aperiodic             < __flt_type > app;
    leadlag               < __flt_type > leg;
    integrator            < __flt_type > itg;
    lowpass2_filter       < __flt_type > lpf;
    highpass2_filter      < __flt_type > hpf;
    bandpass2_filter      < __flt_type > bpf;
    bandstop2_filter_type1< __flt_type > bsf1;
    bandstop2_filter_type2< __flt_type > bsf2;

    dif .init( Fs , Fn , 0.01 );
    app .init( Fs , Fn , 0.01 );
    leg .init( Fs , Fn , 0.01 , 0.02 );
    itg .init( Fs , Fn );
    lpf .init( Fs , Fn , 0.707 , 120 );
    hpf .init( Fs , Fn , 0.707 , 120 );
    bpf .init( Fs , Fn , 0.707 , 120 );
    bsf1.init( Fs , Fn , 0.707 , 120 );
    bsf2.init( Fs , Fn , 120 , 110 );

    dif .allocate();
    app .allocate();
    leg .allocate();
    itg .allocate();
    lpf .allocate();
    hpf .allocate();
    bpf .allocate();
    bsf1.allocate();
    bsf2.allocate();

    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            gen( 1 , Fn , 0 , Fs );

            // filtering:
            dif ( &gen.m_out );
            app ( &gen.m_out );
            leg ( &gen.m_out );
            itg ( &gen.m_out );
            lpf ( &gen.m_out );
            hpf ( &gen.m_out );
            bpf ( &gen.m_out );
            bsf1( &gen.m_out );
            bsf2( &gen.m_out );

            // generating output:
            yt     << gen.m_out  << "\n";
            dif_y  << dif.m_out  << "\n";
            app_y  << app.m_out  << "\n";
            leg_y  << leg.m_out  << "\n";
            int_y  << itg.m_out  << "\n";
            lpf_y  << lpf.m_out  << "\n";
            hpf_y  << hpf.m_out  << "\n";
            bpf_y  << bpf.m_out  << "\n";
            bsf1_y << bsf1.m_out << "\n";
            bsf2_y << bsf2.m_out << "\n";
            tt    << time       << "\n";
            time += 1 / Fs;
        }
    }

    // close files:
    yt    .close();
    dif_y .close();
    app_y .close();
    leg_y .close();
    int_y .close();
    lpf_y .close();
    hpf_y .close();
    bpf_y .close();
    bsf1_y.close();
    bsf2_y.close();
    tt    .close();

    // memory deallocation:
    dif .deallocate();
    app .deallocate();
    leg .deallocate();
    itg .deallocate();
    lpf .deallocate();
    hpf .deallocate();
    bpf .deallocate();
    bsf1.deallocate();
    bsf2.deallocate();

    return 0;
}

/*! \brief DSP logical components utilization examples and test  */
int example8()
{
    printf( " ... DSP logical components utilization examples and test ... \n " );

    // emulation parameters:
    double Fs                = 4000;
    double time              = 0;
    double EmulationDuration = 1.0;
    int    CycleWidth        = 5;
    int    cycles_num        = 1000 * EmulationDuration / CycleWidth;
    int    frames_per_cycle  = CycleWidth * Fs / 1000;

    // logs directory:
    std::string directory = "C:\\Qt_projects\\DigitalFilters_x32\\logs";

    // files:
    std::ofstream yt;
    std::ofstream ton_y;
    std::ofstream tof_y;
    std::ofstream tp_y;
    std::ofstream rs_y;
    std::ofstream sr_y;
    std::ofstream rr_y;
    std::ofstream ff_y;
    std::ofstream tt;
    yt   .open( directory + "\\yt.txt");
    ton_y.open( directory + "\\ton_y.txt");
    tof_y.open( directory + "\\tof_y.txt");
    tp_y .open( directory + "\\tp_y.txt" );
    rs_y .open( directory + "\\rs_y.txt" );
    sr_y .open( directory + "\\sr_y.txt" );
    rr_y .open( directory + "\\rr_y.txt" );
    ff_y .open( directory + "\\ff_y.txt" );
    tt   .open( directory + "\\tt.txt"   );

    // timers:
    timers   ton; ton.init( Fs , 1 );
    timers   tof; tof.init( Fs , 1 );
    timers   tp ; tp .init( Fs , 1 );

    // triggers:
    triggers rs;
    triggers sr;
    triggers rr;
    triggers ff;

    // emulation:
    int input = 0;
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            input = ( (time > 0.1) && (time <= 0.5) ) ? 1 : 0;

            // timing:
            ton.ton( input , 0.2 );
            tof.tof( input , 0.2 );
            tp .tp ( input , 0.2 );

            // triggering:
            rs.rs_trig( input , (time >= 0.5) );
            sr.sr_trig( input , (time >= 0.5) );
            rr.rr_trig( input );
            ff.ff_trig( input );

            // generating output:
            yt    << input          << "\n";
            ton_y << ton.getState() << "\n";
            tof_y << tof.getState() << "\n";
            tp_y  << tp .getState() << "\n";
            rs_y  << rs .getState() << "\n";
            sr_y  << sr .getState() << "\n";
            rr_y  << rr .getState() << "\n";
            ff_y  << ff .getState() << "\n";
            tt    << time           << "\n";
            time += 1 / Fs;
        }
    }

    // close files:
    yt   .close();
    ton_y.close();
    tof_y.close();
    tp_y .close();
    rs_y .close();
    sr_y .close();
    rr_y .close();
    ff_y .close();
    tt   .close();

    return 0;
}

#endif // EXAMPLES_H
