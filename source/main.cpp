// examples:
#include "examples/examples.h"

int main()
{
    /*
    // s / ( Td * s + 1 )
    int N = 2;
    int P = 2;
    double Hs[2][2] = { { 0 , 1 } , {1 , (double)1/(double)4000} };
    double Rz[2][2] = { {1, -1}, {1, 1} };
    tuple_x3<void**, int, int> tuple = __fraction_numeric_substitution__<double>(Hs[0], Hs[1], Rz[0], Rz[1], N, P);

    for( int i = 0 ; i < tuple.item1; i++ )
    {
        for( int j = 0 ; j < tuple.item2; j++ )
        {
            cout << ((double*)tuple.item0[i])[j] << "\t";
        }
        cout << "\n";
    }

    tuple = __mfree__(tuple);
    */

    /*
    // w * w / ( s^2 + 2 * Kd * w * s + w * w )
    double Ts    = 1.0 / 4000.0;
    double omega = 2.0 * 3.14 * 120.0;
    double Kd    = 0.7;
    omega        = tan( omega * Ts / 2.0 );

    double a  = 1;
    double b  = omega / Kd;
    double c  = omega * omega;
    double k1 = a + b + c;
    double k2 = 2 * c - 2 * a;
    double k3 = a - b + c;

    double Gain = omega * omega;

    cout << Gain << "\t" << 2 * Gain << "\t" << Gain << "\n";
    cout << "\n";
    cout << k1 << "\t" << k2 << "\t" << k3 << "\n";

    int N = 3;
    int P = 2;
    double Hs[2][3] = { { omega * omega , 0, 0 } , { omega * omega , 2 * Kd * omega, 1 } };
    double Rz[2][2] = { {1, -1}, {1, 1} };
    tuple_x3<void**, int, int> tuple = fraction_numeric_substitution<double>(Hs[0], Hs[1], Rz[0], Rz[1], N, P);


    for( int i = 0 ; i < tuple.item1; i++ )
    {
        for( int j = 0 ; j < tuple.item2; j++ )
        {
            cout << ((double*)tuple.item0[i])[j] << "\t";
        }
        cout << "\n";
    }

    tuple = __mfree__(tuple);
    */

    /*
    double *res = nullptr;
    int    Nres = 0;

    __convf__<double>( Hs[0], Rz[0], 5, 2, &res, Nres);

    for( int i = 0 ; i < Nres ; i++ ) cout << res[i] << "\t";
    cout << "\n";

    __mfree__(res);
    */

    //complex_numbers_example();
    //filters_rff_example();
    //filtes_hmf_example();
    //filters_butt_example();
    //filters_cheb1_example();
    //filters_cheb2_example();
    //filters_ellip_example();
    //filters_fir_example();

    //filters_differentiator_example();
    //filters_aperiodic_example();
    //filters_integrator_example();
    //filters_leadlag_example();

    filters_filter_2nd_order_example();

    return 0;
}
