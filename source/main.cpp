#include <iostream>
#include <fstream>
#include <QElapsedTimer>

// examples:
#include "include/examples.h"

int main()
{
    // run examples code:
    // example0();
    // example1();
    // example2();
    // example3();
    // example4();
    // example5();
    // example6();
    // example7();
    // example8();
    // example9();
    // example10();
    // example11();
    // example12();

    /*
    int bsize = 10;
    int asize = 15;
    mirror_ring_buffer< complex< float > > bf;
    bf.allocate( bsize );
    for( int i = 0 ; i < asize ; i++ ) { complex< float > a = complex< float >( i , i ); bf( &a ); }
    for( int i = 0 ; i < bsize ; i++ ) { std::cout << bf[i].m_re << "\t" << bf[i].m_im << "\n"; }
    */

    /*
    complex< float > a(1,2);
    complex< float > b(3,4);
    complex< float > c;
    QElapsedTimer timer;
    timer.start();
    for( int i = 0 ; i < 1e6 ; i++ ){ c = a + b; }
    std::cout << timer.nsecsElapsed() / 1e9 << "\n";
    */

    return 0;
}
