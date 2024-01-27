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
    // setup directories
    std::string logsDirectory = "C:/Qt_projects/DSP/logs";
    std::string oscDirectory  = "C:/Qt_projects/DSP/osc";
    std::string oscName       = "test";

    // run tests / examples
    //filters_fir_example(logsDirectory);
    //filters_butt_example(logsDirectory);
    //filters_cheb1_example(logsDirectory);
    //filters_cheb2_example(logsDirectory);
    //filters_ellip_example(logsDirectory);
    //filters_aperiodic_example(logsDirectory);
    //filters_integrator_differentiator_example(logsDirectory);
    //filters_leadlag_example(logsDirectory);
    //filters_filter_2nd_order_example(logsDirectory);
    //filters_rff_example(logsDirectory);
    //filters_harmonic_extracter_example(logsDirectory);

    // current transformer modelling
    //magnetization_curve_hyperbolic_approximator_test(logsDirectory);
    //magnetization_curve_piecewise_approximator_test(logsDirectory);
    //magnetization_curve_smart_approximator_test(logsDirectory);
    //current_transformer_model_test(logsDirectory);

    current_transformer_model_oscillogram_test(
                logsDirectory,
                oscDirectory,
                oscName);

    return 0;
}
