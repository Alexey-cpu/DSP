#include "examples/examples.h"

#define CONCATENATE_TOKENS( A, B ) (A)##(B)

int main()
{
    // driver examples
    //filters_fir_example();
    //complex_numbers_example();
    //filters_rff_example();
    //filtes_hmf_example();
    //filters_butt_example();
    //filters_cheb1_example();
    //filters_cheb2_example();
    //filters_ellip_example();
    //filters_differentiator_example();
    //filters_aperiodic_example();
    //filters_integrator_example();
    //filters_leadlag_example();
    //filters_filter_2nd_order_example();
    //fft_example();
    //interpolation_example();
    //interpolator_example();
    //fft_based_filter_example();
    //filtes_fft_example();
    //filters_harmonic_extracter_example();

    //filters_comtrade_write_example();

    //filters_comtrade_emulation_example();

    /*
    ComtradeFormat comtradeStation;
    QElapsedTimer timer;
    timer.start();
    comtradeStation.from_file( "C:/Users/Alexey/Desktop/script_1", "script_1" );
    std::cout << "reading file has taken " << timer.nsecsElapsed() / 1e9 << " seconds \n";

    ComtradeFormatAnalogChannel* Ua = comtradeStation.find_analog_channel("Ua");

    if( Ua != nullptr )
        std::cout << Ua->get_signal_maximum_level() << "\t" << Ua->get_signal_minimum_level() << "\n";

    comtradeStation.to_file( "C:/Users/Alexey/Desktop/script_1/copy", "script_1" );
    */

    return 0;
}
