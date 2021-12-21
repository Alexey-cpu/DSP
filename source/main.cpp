#include <iostream>
#include <fstream>
#include <QElapsedTimer>
#include "include/special_functions.h"
#include "include/buffer.h"
#include "include/FIR.h"

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
    complex<double> z0( 1.0 , 2.0 );
    complex<double> z1( 2.0 , 0.5 );
    complex<double> z2( 3.5 , 4.5 );

    std::cout << " complex math functions check: " << "\n\n";
    std::cout << "z0 = " << z0.m_re << "\t" << z0.m_im << "\n";
    std::cout << "z1 = " << z1.m_re << "\t" << z1.m_im << "\n";
    std::cout << "z2 = " << z2.m_re << "\t" << z2.m_im << "\n\n";

    std::cout << "arithmetics: " << "\n\n";
    std::cout << " z0 + z1 + z2    =  " << ( z0 + z1 + z2 ).m_re << "\t\t" << ( z0 + z1 + z2 ).m_im << "\n";
    std::cout << " z0 - z1 - z2    =  " << ( z0 - z1 - z2 ).m_re << "\t\t" << ( z0 - z1 - z2 ).m_im << "\n";
    std::cout << " z0 * z1 / z2    =  " << ( z0 * z1 / z2 ).m_re << "\t\t" << ( z0 * z1 / z2 ).m_im << "\n";
    std::cout << " (z1-z2)/(z1+z2) =  " << ( ( z1 - z2 ) / ( z1 + z2 ) ).m_re << "\t" << ( ( z1 - z2 ) / ( z1 + z2 ) ).m_im << "\n";
    std::cout << " z0 = z0 + z2    =  " << ( z0 += z2 ).m_re << "\t\t" << z0.m_im << "\n";
    std::cout << " z1 = z1 * z2    =  " << ( z1 *= z2 ).m_re << "\t\t" << z1.m_im << "\n";
    std::cout << " z2 = z2 / z1    =  " << ( z2 /= z1 ).m_re << "\t\t" << z2.m_im << "\n";

    std::cout << "functions: " << "\n\n";
    std::cout << " sinh( z0 )    =  " << __sinhf__(z0).m_re << "\t" << __sinhf__(z0).m_im  << "\n";
    std::cout << " cosh( z0 )    =  " << __coshf__(z0).m_re << "\t" << __coshf__(z0).m_im  << "\n";
    std::cout << " tanh( z0 )    =  " << __tanhf__(z0).m_re << "\t" << __tanhf__(z0).m_im  << "\n";
    std::cout << " ctnh( z0 )    =  " << __ctnhf__(z0).m_re << "\t" << __ctnhf__(z0).m_im  << "\n";
    std::cout << " abs( z0 )    =  " << __absf__(z0) << "\n";
    std::cout << " arg( z0 )    =  " << __argf__(z0) * 180 / 3.14 << "\n";
    std::cout << " conj( z0 )    =  " << __conjf__(z0).m_re << "\t\t" << __conjf__(z0).m_im << "\n";
    std::cout << " norm( z0 )    =  " << __normf__(z0).m_re << "\t\t" << __normf__(z0).m_im << "\n";
    std::cout << " z0*rot( 60 , 1 )    =  " << ( z0 *=__rotf__( 60.0 , 1) ).m_re << "\t\t" << (  z0 ).m_im << "\n";
    return 0;
}

// buffer class example:
int test2()
{
    typedef double __type;
    int N0 = 15;
    int N1 = 10;
    __type *a = new __type [N0];

    mirror_ring_buffer<__type> buff;
    buff.allocate( N1 );

    std::cout << "a: " << "\n";
    for( int i = 0 ; i < N0 ; i++ ) std::cout << ( a[i] = i ) << "\t";
    std::cout << "\n";

    for( int i = 0 ; i < N0 ; i++ ) { buff.fill_buff( &a[i] ); }

    std::cout << "buffer: " << "\n";
    for( int i = 0 ; i < N1 ; i++ ) std::cout <<  buff[i] << "\t";
    std::cout << "\n";

    buff.deallocate();
    delete [] a;
    a = nullptr;

    return 0;
}

// fir lowpass test:
int test3()
{
    fir_lp<float> FLP;
    FLP.init( 4000 , 50 , 100 , 80 , 1 );
    FLP.m_wind_fcn.Hamming();
    FLP.allocate();
    FLP.freq_rp( "C:\\Qt_projects\\DigitalFilters_x32\\logs" );
    FLP.deallocate();
    return 0;
}

// fir highpass test:
int test4()
{
    fir_hp<float> FHP;
    FHP.init( 4000 , 50 , 100 , 80 , 1 );
    FHP.m_wind_fcn.Hamming();
    FHP.allocate();
    FHP.freq_rp( "C:\\Qt_projects\\DigitalFilters_x32\\logs" );
    FHP.deallocate();
    return 0;
}

// fir bandpass test:
int test5()
{
    fir_bp<float> FBP;
    FBP.init( 4000 , 50 , 100 , 100 , 80 , 1 );
    FBP.m_wind_fcn.Hamming();
    FBP.allocate();
    FBP.freq_rp( "C:\\Qt_projects\\DigitalFilters_x32\\logs" );
    FBP.deallocate();
    return 0;
}

// fir bandstop test:
int test6()
{
    fir_bs<float> FBP;
    FBP.init( 4000 , 50 , 100 , 100 , 80 , 1 );
    FBP.m_wind_fcn.Hamming();
    FBP.allocate();
    FBP.freq_rp( "C:\\Qt_projects\\DigitalFilters_x32\\logs" );
    FBP.deallocate();
    return 0;
}

int main()
{
    // test3(); // fir lowpass test
    // test4(); // fir highpass test
    // test5(); // fir bandpass test
    // test6(); // fir bandstop test

    return 0;
}