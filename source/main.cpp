#include <iostream>
#include <fstream>
#include <QElapsedTimer>

#include <iostream>
#include <complex>


// examples:
#include "include/examples.h"

int main()
{

    DSP::fir< double > lp_flt;
    lp_flt.init(4000 , 80 , DSP::filter_type::lowpass  , { 100 , 500 } , Chebyshev(60, 80) , true );
    lp_flt.show();


    // run examples code:
    // example0();
    // example1();
    // example2();
    // example3();

    //std::cout << DSP::filter_type::bandpass << "\n";

    /*
    QElapsedTimer timer;
    fcomplex<double> a(1,2);
    fcomplex<double> b(3,4);
    fcomplex<double> c(5,6);

    std::complex<double> aa(1,2);
    std::complex<double> bb(3,4);
    std::complex<double> cc(5,6);

    timer.start();
    for( int i = 0 ; i < 1e6 ; i++ ) c = a + b;
    std::cout << "dt1 = " << timer.nsecsElapsed() / 1e9 << "\n";

    timer.start();
    for( int i = 0 ; i < 1e6 ; i++ ) cc = aa + bb;
    std::cout << "dt2 = " << timer.nsecsElapsed() / 1e9 << "\n";
    */


    return 0;
}
