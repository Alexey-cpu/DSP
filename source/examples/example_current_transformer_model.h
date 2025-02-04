#ifndef EXAMPLE_CURRENT_TRANSFORMER_MODEL_H
#define EXAMPLE_CURRENT_TRANSFORMER_MODEL_H

#include "current_transformer.h"
#include "generators.h"
#include "Comtrade.h"

// Magnetization curve approximators

void magnetization_curve_hyperbolic_approximator_test(string _LogsDirectory)
{
    printf("running test %s \n\n", STRINGIFY(magnetization_curve_hyperbolic_approximator_test));

    // control points of B-H characteristic
    Vector2D<double> magnetizationCurve[11]
    {
        { 0.00, 0.0 },
        { 0.50, 10  },
        { 1.00, 25  },
        { 1.20, 30  },
        { 1.40, 45  },
        { 1.60, 100 },
        { 1.65, 150 },
        { 1.70, 250 },
        { 1.75, 400 },
        { 1.76, 500 },
        { 1.77, 600 }
    };

    // open files
    std::ofstream bt;
    std::ofstream ht;
    bt.open( _LogsDirectory + "\\B.txt");
    ht.open( _LogsDirectory + "\\H.txt");

    MagnetizationCurveHyperbolicApproximator
            magneticInductionFluxDensityHyperbolicApproximator
            (
                { magnetizationCurve[5].x, magnetizationCurve[5].y },
                { magnetizationCurve[10].x, magnetizationCurve[10].y }
            );

    for( int i = 0 ; i < 5.0 ; i++ )
    {
        ht << magneticInductionFluxDensityHyperbolicApproximator.compute( (double)i ) << "\n";
        bt << i << "\n";
    }

    cout << magneticInductionFluxDensityHyperbolicApproximator.get_beta() << "\t" << magneticInductionFluxDensityHyperbolicApproximator.get_alpha();

    // close files
    bt.close();
    ht.close();
}

void magnetization_curve_piecewise_approximator_test(string _LogsDirectory)
{
    printf("running test %s \n\n", STRINGIFY(magnetization_curve_piecewise_approximator_test));

    Vector2D<double> curve[11]
    {
        { 0.00, 0.0 },
        { 0.50, 10  },
        { 1.00, 25  },
        { 1.20, 30  },
        { 1.40, 45  },
        { 1.60, 100 },
        { 1.65, 150 },
        { 1.70, 250 },
        { 1.75, 400 },
        { 1.76, 500 },
        { 1.77, 600 }
    };

    MagnetizationCurvePiecewiseApproximator
            approximator
            (
                curve,
                11
            );

    // open files
    std::ofstream bt;
    std::ofstream ht;
    bt.open( _LogsDirectory + "\\B.txt");
    ht.open( _LogsDirectory + "\\H.txt");

    for( double i = 0 ; i < 5.0 ; i += 0.01 )
    {
        bt << i << "\n";
        ht << approximator.compute( i ) << "\n";
    }

    // close files
    bt.close();
    ht.close();
}

void magnetization_curve_smart_approximator_test(string _LogsDirectory)
{
    printf("running test %s \n\n", STRINGIFY(magnetization_curve_piecewise_approximator_test));

    Vector2D<double> curve[11]
    {
        { 0.00, 0.0 },
        { 0.50, 10  },
        { 1.00, 25  },
        { 1.20, 30  },
        { 1.40, 45  },
        { 1.60, 100 },
        { 1.65, 150 },
        { 1.70, 250 },
        { 1.75, 400 },
        { 1.76, 500 },
        { 1.77, 600 }
    };

    MagnetizationCurveSmartApproximator
            approximator
            (
                curve,
                11
            );

    // open files
    std::ofstream bt;
    std::ofstream ht;
    bt.open( _LogsDirectory + "\\B.txt");
    ht.open( _LogsDirectory + "\\H.txt");

    for( double i = 0 ; i < 5.0 ; i += 0.01 )
    {
        bt << i << "\n";
        ht << approximator.compute( i ) << "\n";
    }

    // close files
    bt.close();
    ht.close();
}

void current_transformer_model_test(string _LogsDirectory)
{
    printf("running test %s \n\n", STRINGIFY(current_transformer_model_test));

    // emulation parameters:
    double Fs                = 4000;
    double Fn                = 50;
    double time              = 0;
    double EmulationDuration = 1.5;
    int    CycleWidth        = 5;
    int    cycles_num        = 1000 * EmulationDuration / CycleWidth;
    int    frames_per_cycle  = CycleWidth * Fs / 1000;

    // create COMTARDE registrator
    Comtrade registrator
            (
                STRINGIFY(current_transformer_model_test),
                Fn,
                Fs,
                EmulationDuration,
                {
                    new ComtradeAnalogChannel("I1"),
                    new ComtradeAnalogChannel("I2"),
                    new ComtradeAnalogChannel("Im"),
                    new ComtradeAnalogChannel("B"),
                    new ComtradeAnalogChannel("H")
                }
            );

    registrator.set_samples_number( frames_per_cycle * cycles_num );

    // retrieve channels
    ComtradeAnalogChannel* I1  = registrator.find_analog_channel("I1");
    ComtradeAnalogChannel* I2  = registrator.find_analog_channel("I2");
    ComtradeAnalogChannel* B   = registrator.find_analog_channel("B");
    ComtradeAnalogChannel* H   = registrator.find_analog_channel("H");
    ComtradeAnalogChannel* Im  = registrator.find_analog_channel("Im");

    // time provider initialization
    digital_clock<double> timeProvider;
    timeProvider.init(Fs);

    // current transformer model initialization
    double primaryWindingTurns   = 1;
    double secondaryWindingTurns = 1000;
    double crossSectionArea      = 0.00945;
    double pathLength            = 2.5;
    double resistance            = 60 + 8.8;
    double inductance            = 2.58 / PI2 / Fn;

    Vector2D<double> magnetizationCurve[11]
    {
        { 0.00, 0.0 },
        { 0.50, 10  },
        { 1.00, 25  },
        { 1.20, 30  },
        { 1.40, 45  },
        { 1.60, 100 },
        { 1.65, 150 },
        { 1.70, 250 },
        { 1.75, 400 },
        { 1.76, 500 },
        { 1.77, 600 }
    };

    CurrentTransformer currentTransformer
            (
                primaryWindingTurns,
                secondaryWindingTurns,
                crossSectionArea,
                pathLength,
                resistance,
                inductance,
                0,
                Fn,
                Fs,
                new MagnetizationCurveSmartApproximator(
                    magnetizationCurve,
                    11 )
            );

    // emulation
    int k = 0;
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time = timeProvider.tick(), k++ )
        {
            double faultTime = 0.2;

            double primaryCurrent = time < faultTime ?
                        1e3 * sin( PI2 * Fn * time ) :
                        10e3 * ( sin( PI2 * Fn * time ) + exp( -( time - faultTime ) / 0.08 ) );


            currentTransformer.process_primary_current( &primaryCurrent );

            I1->set_sample( k, primaryCurrent );
            I2->set_sample( k, currentTransformer.get_secondary_current() );
            Im->set_sample( k, currentTransformer.get_magnetizing_current() );
            B->set_sample( k, currentTransformer.get_magnetic_induction() );
            H->set_sample( k, currentTransformer.get_magnetic_induction_flux_density() );
        }
    }

    // write COMTRADE file
    registrator.to_file( _LogsDirectory, STRINGIFY(current_transformer_model_test) );
}

void current_transformer_model_oscillogram_test(string _LogsDirectory, string _OscDirectory, string _OscName)
{
    // read COMTRADE file
    Comtrade emulationStation;
    emulationStation.from_file( _OscDirectory, _OscName );

    // retrieve COMTRADE station info
    double Fs = emulationStation.get_rate_frequency();
    double Fn = emulationStation.get_nominal_frequency();
    size_t Ns = emulationStation.get_samples_number();

    emulationStation.print_station_info();

    // retrieve channel
    ComtradeAnalogChannel* channel =
            emulationStation.find_analog_channel("Ic10 СВ2");

    // create registration station
    Comtrade registrationStation
            (
                STRINGIFY(current_transformer_model_oscillogram_test),
                Fn,
                Fs,
                Comtrade::compute_oscillogram_duration( Ns, Fs ),
                {
                    new ComtradeAnalogChannel("I2"),
                    new ComtradeAnalogChannel("Im"),
                    new ComtradeAnalogChannel("B"),
                    new ComtradeAnalogChannel("H")
                }
            );

    ComtradeAnalogChannel* I2 = registrationStation.find_analog_channel("I2");
    ComtradeAnalogChannel* Im = registrationStation.find_analog_channel("Im");
    ComtradeAnalogChannel* B  = registrationStation.find_analog_channel("B");
    ComtradeAnalogChannel* H  = registrationStation.find_analog_channel("H");

    // current transformer model initialization
    double primaryWindingTurns   = 1;
    double secondaryWindingTurns = 1000;
    double crossSectionArea      = 0.00945;
    double pathLength            = 2.5;
    double resistance            = 60 + 8.8;
    double inductance            = 2.58 / PI2 / Fn;

    Vector2D<double> magnetizationCurve[11]
    {
        { 0.00, 0.0 },
        { 0.50, 10  },
        { 1.00, 25  },
        { 1.20, 30  },
        { 1.40, 45  },
        { 1.60, 100 },
        { 1.65, 150 },
        { 1.70, 250 },
        { 1.75, 400 },
        { 1.76, 500 },
        { 1.77, 600 }
    };

    CurrentTransformer currentTransformer
            (
                primaryWindingTurns,
                secondaryWindingTurns,
                crossSectionArea,
                pathLength,
                resistance,
                inductance,
                10,
                Fn,
                Fs,
                new MagnetizationCurveSmartApproximator(
                    magnetizationCurve,
                    11 )
            );

    // emulation
    if( channel != nullptr )
    {
        for( size_t i = 0 ; i < Ns ; i++ )
        {
            double sample = channel->get_sample( i ) * channel->get_calibration_multiplier_coefficient() +
                    channel->get_calibration_addition_coefficient();

            currentTransformer.process_secondary_current( &sample );

            I2->set_sample( i, sample );
            Im->set_sample( i, currentTransformer.get_magnetizing_current() );
            B->set_sample( i, currentTransformer.get_magnetic_induction() );
            H->set_sample( i, currentTransformer.get_magnetic_induction_flux_density() );
        }
    }
    else
    {
    #ifdef DEBUGGER
            Debugger::LogError(
                        STRINGIFY(current_transformer_model_oscillogram_test),
                        STRINGIFY(current_transformer_model_oscillogram_test),
                        "Selected Channel does not exist" );
    #endif
    }

    registrationStation.to_file(
                _LogsDirectory,
                STRINGIFY(current_transformer_model_oscillogram_test) );
}

#endif // EXAMPLE_CURRENT_TRANSFORMER_MODEL_H
