#include "example_current_transformer_model.h"
#include "QDir"

int main()
{
    cout << QDir().currentPath().toStdString() << "\n";

    // setup directories
    std::string logsDirectory = "C:/Qt_projects/DSP/logs";
    std::string oscDirectory  = "C:/Qt_projects/DSP/osc";
    std::string oscName       = "test";

    // run tests / examples
    //magnetization_curve_hyperbolic_approximator_test(logsDirectory);
    //magnetization_curve_piecewise_approximator_test(logsDirectory);
    //magnetization_curve_smart_approximator_test(logsDirectory);
    current_transformer_model_test(logsDirectory);
    //current_transformer_model_oscillogram_test(logsDirectory, oscDirectory, oscName);

    return 0;
}
