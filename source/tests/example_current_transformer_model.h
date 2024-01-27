#ifndef EXAMPLE_CURRENT_TRANSFORMER_MODEL_H
#define EXAMPLE_CURRENT_TRANSFORMER_MODEL_H

#include "Utils.h"
#include "Complex.h"

#include "generators.h"
#include "Comtrade.h"
#include "filters_transfer_functions.h"

// Magnetization curve approximators
class IMagnetizationCurve
{
public:

    // constructors
    IMagnetizationCurve(){}

    // virtual destructor
    virtual ~IMagnetizationCurve(){}

    // pure virtual methods
    virtual double compute( double _MagneticInduction ) const = 0;
};

class MagnetizationCurveHyperbolicApproximator : public IMagnetizationCurve
{
    // info
    double m_Beta  = 0;
    double m_Alpha = 0;

public:

    // constructors
    MagnetizationCurveHyperbolicApproximator(
            Vector2D<double> _KneePoint,
            Vector2D<double> _SaturationPoint)
    {
        double B1 = _KneePoint.x;
        double H1 = _KneePoint.y;
        double B2 = _SaturationPoint.x;
        double H2 = _SaturationPoint.y;

        m_Beta  = log( H2 / H1 ) / ( B2 - B1 );
        m_Alpha = H2 / __realf__( __sinhf__( Complex<double>( m_Beta * B2 ) ) );
    }

    // virtual destructor
    virtual ~MagnetizationCurveHyperbolicApproximator(){}

    // getters
    double get_beta() const
    {
        return m_Beta;
    }

    double get_alpha() const
    {
        return m_Alpha;
    }

    virtual double compute( double _MagneticInduction ) const override
    {
        return __realf__( Complex<double>( m_Alpha ) * __sinhf__( Complex<double>( m_Beta, 0.0 ) * _MagneticInduction ) );
    }
};

class MagnetizationCurvePiecewiseApproximator : public IMagnetizationCurve
{
    double* m_B = nullptr;
    double* m_H = nullptr;
    double  m_K = tan( __to_radians__( 89.0 ) );
    int     m_N = 0;

public:

    // constructors
    MagnetizationCurvePiecewiseApproximator(
            Vector2D<double> _MagnetizationCurve[],
            int              _Size )
    {
        m_N = 2 * _Size - 1;
        m_B = new double[m_N];
        m_H = new double[m_N];

        int j = 0;

        for( int i = _Size - 1 ; i > 0 ; i--, j++ )
        {
            m_B[j] = -_MagnetizationCurve[ i ].x;
            m_H[j] = -_MagnetizationCurve[ i ].y;
        }

        for( int i = 0 ; i < _Size ; i++, j++ )
        {
            m_B[j] = +_MagnetizationCurve[ i ].x;
            m_H[j] = +_MagnetizationCurve[ i ].y;
        }
    }

    // virtual destructor
    virtual ~MagnetizationCurvePiecewiseApproximator()
    {
        if( m_B != nullptr )
            delete[] m_B;

        if( m_H != nullptr )
            delete[] m_H;
    }

    // public methods
    bool out_of_bounds( double _MagneticInduction ) const
    {
        return m_B != nullptr &&
                m_N >= 2 &&
                ( _MagneticInduction < m_B[0] || _MagneticInduction > m_B[ m_N - 1 ] );
    }

    // virtual methods override
    virtual double compute( double _MagneticInduction ) const override
    {
        // check minimum length constraint
        if( m_N < 2 )
        {
            return 0.0;
        }

        // linear extrapolation in case of out-of-range
        if( out_of_bounds( _MagneticInduction ) )
        {
            int    n  = _MagneticInduction < m_B[0] ? 0 :  m_N - 2;
            double dH = m_H[n+1] - m_H[n];
            double dB = m_B[n+1] - m_B[n];

            return m_H[n] + ( _MagneticInduction - m_B[n] ) * dH / dB;
        }

        int j = 0;

        for( int i = 1 ; i < m_N ; i++, j = i - 1 )
        {
            if( _MagneticInduction < m_B[i]  )
                break;
        }

        double B[2]{ 0, 0.0 };
        double H[2]{ 0, 0.0 };

        for( int k = 0 ; k < 2 && j < m_N ; k++, j++ )
        {
            B[k] = m_B[j];
            H[k] = m_H[j];
        }

        return __lagrange_polynom_interpolation__( B, H, _MagneticInduction, 2 );
    }
};

class MagnetizationCurveSmartApproximator : public IMagnetizationCurve
{
    MagnetizationCurvePiecewiseApproximator*  m_PiecewiseApproximator  = nullptr;
    MagnetizationCurveHyperbolicApproximator* m_HyperbolicApproximator = nullptr;

public:

    // constructors
    MagnetizationCurveSmartApproximator(
            Vector2D<double> _MagnetizationCurve[],
            int  _Size )
    {
        // approximate low saturation region
        m_PiecewiseApproximator =
                new MagnetizationCurvePiecewiseApproximator(_MagnetizationCurve, _Size);

        // approximate deep saturation region
        m_HyperbolicApproximator =
                new MagnetizationCurveHyperbolicApproximator
                (
                    _MagnetizationCurve[ _Size - 1 ],
                    _MagnetizationCurve[ _Size - 2 ]
                );
    }

    // virtual destructor
    virtual ~MagnetizationCurveSmartApproximator(){}

    virtual double compute( double _MagneticInduction ) const override
    {
        return !m_PiecewiseApproximator->out_of_bounds( _MagneticInduction ) ?
                    m_PiecewiseApproximator->compute( _MagneticInduction) :
                    m_HyperbolicApproximator->compute( _MagneticInduction );
    }
};

// current transformer model
class CurrentTransformer
{
protected:

    // model info
    const int    m_PrimaryWindingTurnsNumber   = 0;
    const int    m_SecondaryWindingTurnsNumber = 0;
    const double m_CrossSectionArea            = 0;
    const double m_PathLength                  = 0;
    const double m_Resistance                  = 0;
    const double m_Inductance                  = 0;
    const double m_InrushCurrentDecayTime      = 0;
    const double m_NominalFrequency            = 0;
    const double m_ADCRateFrequency            = 0;

    const IMagnetizationCurve*
    m_MagneticInductionFluxDensityComputer = nullptr;

    // computed parameters
    double m_SecondaryCurrent             = 0; // I2(t)
    double m_MagneticInductionFluxDensity = 0; // H(t)
    double m_MagneticInduction            = 0; // B(t)
    double m_MagnetizingCurrent           = 0; // B(t)

    // auxiliary coefficients
    double m_K1 = 1.0;
    double m_K2 = 1.0;
    double m_K3 = 1.0;
    double m_K4 = 1.0;
    double m_K5 = 1.0;

    // internal filters
    integrator<double>     m_Integrator;
    differentiator<double> m_Differentiator;

public:

    // constructors
    CurrentTransformer(
            int    _PrimaryTurnsNumber,
            int    _SecondaryTurnsNumber,
            double _CrossSectionArea,
            double _PathLength,
            double _Resistance,
            double _Inductance,
            double _InrushCurrentDecayTime,
            double _NominalFrequency,
            double _ADCRateFrequency,
            IMagnetizationCurve* _MagnetizationCurve ) :
        m_PrimaryWindingTurnsNumber(_PrimaryTurnsNumber),
        m_SecondaryWindingTurnsNumber(_SecondaryTurnsNumber),
        m_CrossSectionArea(_CrossSectionArea),
        m_PathLength(_PathLength),
        m_Resistance(_Resistance),
        m_Inductance(_Inductance),
        m_InrushCurrentDecayTime(_InrushCurrentDecayTime),
        m_NominalFrequency(_NominalFrequency),
        m_ADCRateFrequency(_ADCRateFrequency),
        m_MagneticInductionFluxDensityComputer(_MagnetizationCurve)
    {
        // initialization
        m_K1 = (double)m_PrimaryWindingTurnsNumber / (double)m_SecondaryWindingTurnsNumber;
        m_K2 = m_PathLength / (double)m_SecondaryWindingTurnsNumber;
        m_K3 = m_Resistance / (double)m_SecondaryWindingTurnsNumber / m_CrossSectionArea;
        m_K4 = m_Inductance / (double)m_SecondaryWindingTurnsNumber / m_CrossSectionArea;
        m_K5 = __abs__( m_InrushCurrentDecayTime ) > 0.0 ? 0.5 / m_InrushCurrentDecayTime : 0.0;

        // allocate integrator
        m_Integrator.init( m_ADCRateFrequency );
        m_Differentiator.init( m_ADCRateFrequency, 1.0 / m_ADCRateFrequency );
    }

    // virtual destructor
    virtual ~CurrentTransformer()
    {
        // clean memory
        if( m_MagneticInductionFluxDensityComputer != nullptr )
            delete m_MagneticInductionFluxDensityComputer;
    }

    // getters
    double get_secondary_current() const
    {
        return m_SecondaryCurrent;
    }

    double get_magnetic_induction_flux_density() const
    {
        return m_MagneticInductionFluxDensity;
    }

    double get_magnetic_induction() const
    {
        return m_MagneticInduction;
    }

    double get_magnetizing_current() const
    {
        return m_MagnetizingCurrent;
    }

    // public methods
    double process( double* _PrimaryCurrentSample )
    {
        if( m_MagneticInductionFluxDensityComputer == nullptr || _PrimaryCurrentSample == nullptr )
            return 0.0;

        double magneticInductionDerivative =
                m_SecondaryCurrent * m_K3 + m_Differentiator(&m_SecondaryCurrent) * m_K4 - m_MagneticInduction * m_K5;

        m_MagneticInduction            = m_Integrator(&magneticInductionDerivative);
        m_MagneticInductionFluxDensity = m_MagneticInductionFluxDensityComputer->compute(m_MagneticInduction);
        m_MagnetizingCurrent           = m_MagneticInductionFluxDensity * m_K2;
        m_SecondaryCurrent             = *_PrimaryCurrentSample * m_K1 - m_MagnetizingCurrent;

        return m_SecondaryCurrent;
    }
};

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
                        8e3 * ( sin( PI2 * Fn * time ) + exp( -( time - faultTime ) / 0.08 ) );


            currentTransformer.process( &primaryCurrent );

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
                    new ComtradeAnalogChannel("yt")
                }
            );

    ComtradeAnalogChannel* yt = registrationStation.find_analog_channel("yt");

    // emulation
    if( channel != nullptr )
    {
        for( size_t i = 0 ; i < Ns ; i++ )
        {
            double sample     = channel->get_sample( i );
            double multiplier = channel->get_calibration_multiplier_coefficient();
            double addition   = channel->get_calibration_addition_coefficient();

            yt->set_sample( i, sample * multiplier + addition ); // y' = kx + b
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
