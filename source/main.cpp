#include <iostream>
#include <fstream>
#include <QElapsedTimer>
#include "include/special_functions.h"
#include "include/buffer.h"
#include "include/FIR.h"
#include "include/sgen.h"
#include "include/iir.h"

// special functions check:
int test0()
{
    // timer:
    QElapsedTimer timer;

    // Matlab elliptic functions use M = k * k parameter. Be carefull !!!

    double M = 0.7;
    double k = sqrt(M);
    int N = 100;
    double step = 1.0 / (double)N;
    double upper = +5;
    double lower = -5;
    int NN = (upper - lower) / step+2;
    double *u = ( double* ) calloc( NN , sizeof ( double ) );
    for( int i = 0 ; i < NN ; i++ )
    {
        u[i] = lower;
        lower += step;
    }

    timer.start();
    for( int i = 0 ; i < NN ; i++ )
    {
        __sn__( u[i] , k );
        __cn__( u[i] , k );
        __dn__( u[i] , k );
    }
    std::cout << "dt = " << timer.nsecsElapsed()/1e9 << "\n";

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

// complex arithmetic check:
int test1()
{
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

// buffer class example:
int test2()
{
    typedef double __type;
    int N0 = 15 , N1 = 10;

    __type *a = (__type*)calloc( N0 , sizeof ( __type ) );
    mirror_ring_buffer<__type> buff;
    buff.allocate( N1 );

    printf(  "a: \n" );
    for( int i = 0 ; i < N0 ; i++ ) { printf( "%f \t " , ( a[i] = i ) ); }
    for( int i = 0 ; i < N0 ; i++ ) { buff.fill_buff( &a[i] ); }
    printf(  "\n" );


    printf(  "buffer: \n" );
    for( int i = 0 ; i < N1 ; i++ ) { printf("%.4f \t" , buff[i] ); }

    buff.deallocate();
    free(a);
    a = nullptr;

    return 0;
}

// fir filters test:
int test3()
{
    typedef float __type ;

    // log directory:
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

    __type Fs = 4000 , Fn = 50;

    // signal generator:
    sgen<__type> gen;

    // filter configuration:
    fir<__type> FLP;
    // FLP.lp_init( Fs , 50 , 100 , 80 , 1 );
    // FLP.hp_init( Fs , 50 , 100 , 80 , 1 );
    // FLP.bp_init( Fs , 50 , 100 , 100 , 80 , 1 );
     FLP.bs_init( Fs , 50 , 100 , 100 , 80 , 1 );

    // window construction:
    FLP.m_wind.Chebyshev(60);
    FLP.allocate();

    // emulation:
    int cycles_num       = 10;
    int frames_per_cycle = 20;

    double time = 0;

    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            // signal generation and filtering:
            gen( 1 , Fn , 0 , Fs );
            FLP( &gen.m_out );

            // store signal and filter output:
            yt << gen.m_out << "\n";
            ft << FLP.m_out << "\n";
            tt << ( time += 1 / Fs ) << "\n";
        }
    }

    // filter frequency response computation:
    for( int i = 0 ; i < Fs / 2 ; i++ )
    {
        fir_fr< double > fr = FLP.freq_resp( i );
        Km << fr.Km << "\n";
        pH << fr.pH << "\n";
        FF << i << "\n";
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
    FLP.deallocate();
    return 0;
}

// iir filters test:
int test4()
{
    double Fs = 4000;

    // digital Butterworth or Chebyshev type I filters computation:
    iir_cf< double > coeffs{ 0 , 0 , 0 , -1 , -1 , -1 };
    // iir coefficients computation:
     coeffs = __butt_cheb1_digital_lp__< double >( Fs , 100 , 5 , 0 , 1 ); // it works !!!
    // coeffs = __butt_cheb1_digital_hp__( Fs , 800 , 6 , 1 , 1 ); // it works !!!
    // coeffs = __butt_cheb1_digital_bp__( Fs , 100 , 500 , 8 , 1 , 1 ); // it works !!!
    // coeffs = __butt_cheb1_digital_bs__( Fs , 100 , 700 , 6 , 1 , 1 );

     // digital Chebyshev type II or Elliptic filters computation:
     // coeffs = __cheb2_ellip_digital_lp__( Fs , 100 , 5 , 0 , 1 , 80 );
     // coeffs = __cheb2_ellip_digital_hp__( Fs , 100 , 6 , 0 , 1 , 80 );
     // coeffs = __cheb2_ellip_digital_bp__( Fs , 100 , 700 , 6 , 1 , 1 , 80 );
     // coeffs = __cheb2_ellip_digital_bs__( Fs , 100 , 700 , 6 , 0 , 1 , 80 );

    // show iir coefficients:
    // __show_coeffs__( coeffs );

    // filter input and output buffers:
    mirror_ring_buffer<double> *buff_sx = ( mirror_ring_buffer<double>* )calloc( coeffs.N , sizeof ( mirror_ring_buffer<double> ) );
    mirror_ring_buffer<double> *buff_sy = ( mirror_ring_buffer<double>* )calloc( coeffs.N , sizeof ( mirror_ring_buffer<double> ) );
    for( int i = 0 ; i < coeffs.N ; i++ )
    {
        buff_sx[i].allocate(4);
        buff_sy[i].allocate(3);
    }

    // signal generator:
    sgen<double> gen;

    // emulation:
    int cycles_num       = 10;
    int frames_per_cycle = 20;

    double _yt = 0;
    double _ft = 0;
    double time = 0;

    std::ofstream Km;
    std::ofstream pH;
    std::ofstream FF;
    std::ofstream yt;
    std::ofstream ft;
    std::ofstream tt;

    Km.open("C:\\Qt_projects\\DigitalFilters_x32\\logs\\Km.txt");
    pH.open("C:\\Qt_projects\\DigitalFilters_x32\\logs\\pH.txt");
    FF.open("C:\\Qt_projects\\DigitalFilters_x32\\logs\\FF.txt");
    yt.open("C:\\Qt_projects\\DigitalFilters_x32\\logs\\yt.txt");
    ft.open("C:\\Qt_projects\\DigitalFilters_x32\\logs\\ft.txt");
    tt.open("C:\\Qt_projects\\DigitalFilters_x32\\logs\\tt.txt");


    butterworth< double > butt;
    butt.lp_init( Fs , 50 , 100 , 5 );
    butt.allocate();
    butt.show_properties();

    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++ )
        {
            // signal generation and filtering:
            _yt = gen( 1 , 50 , 0 , 4000 );

            _ft = butt( &_yt );

            //butt_x64( &_yt );

            //_ft = __filt__( &_yt  , coeffs.cfnum , coeffs.cfden , coeffs.gains , coeffs.N , buff_sx , buff_sy );

            yt << _yt << "\n";
            ft << _ft << "\n";
            tt <<  (time+=1/Fs)  << "\n";
        }
    }

    // frequency response computation :
    for( int i = 0 ; i < Fs / 2 ; i++ )
    {
        iir_fr< double > fr = __iir_freq_resp__( coeffs.cfnum , coeffs.cfden , coeffs.gains , coeffs.N , Fs , i );
        Km << fr.Km << "\n";
        pH << fr.pH << "\n";
        FF << i     << "\n";
    }

    // emulation:

    // close files:
    Km.close();
    pH.close();
    FF.close();
    yt.close();
    ft.close();
    tt.close();

    // memory deallocation:
    if( coeffs.cfnum != 0 ) free( coeffs.cfnum );
    if( coeffs.cfden != 0 ) free( coeffs.cfden );
    if( coeffs.gains != 0 ) free( coeffs.gains );

    for( int i = 0 ; i < coeffs.N ; i++ )
    {
        buff_sx[i].deallocate();
        buff_sy[i].deallocate();
    }
    free( buff_sx );
    free( buff_sy );

    return 0;
}

int main()
{
    // test0(); // special functions check
    // test1(); // comlex arithmetic test
    // test2(); // buffer class example
    // test3(); // fir filters test
    // test4(); // iir filters test

    return 0;
}
