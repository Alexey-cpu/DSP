#ifndef EXAMPLE_MATH_COMPLEX_NUMBERS_H
#define EXAMPLE_MATH_COMPLEX_NUMBERS_H

#ifndef WRITE_LOGS
//#define WRITE_LOGS
#endif

#include "../../DSP/include/Complex.h"

// complex numbers example
int complex_numbers_example()
{
    printf( " ...complex arithmetics utilization example and test... \n " );
    Complex<double> z0( 1.0 , 2.0 ) , z1( 2.0 , 0.5 ) , z2( 3.5 , 4.5 );
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


#endif // EXAMPLE_MATH_COMPLEX_NUMBERS_H
