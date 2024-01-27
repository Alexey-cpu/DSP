#include "example_classic_iir.h"
#include "example_classic_fir.h"
#include "example_custom_filters_hmf.h"
#include "example_custom_filters_rff.h"
#include "example_custom_filters_harmonic_extracter.h"
#include "example_math_special_functions.h"
#include "example_math_complex_numbers.h"
#include "example_transfer_functions.h"
#include "example_math_special_functions_fft.h"
#include "example_math_special_functions_interpolation.h"
#include "example_comtrade_emulation.h"
#include "example_current_transformer_model.h"

int main()
{
    //filters_fir_example();
    //filters_butt_example();
    //filters_cheb1_example();
    //filters_cheb2_example();
    //filters_ellip_example();
    //filters_aperiodic_example();
    //filters_integrator_differentiator_example();
    //filters_leadlag_example();
    //filters_filter_2nd_order_example();
    //filters_rff_example();
    //filters_harmonic_extracter_example();

    //magnetization_curve_hyperbolic_approximator_test();
    //magnetization_curve_piecewise_approximator_test();
    current_transformer_test();

    return 0;
}
