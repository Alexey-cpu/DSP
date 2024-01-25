#ifndef EXAMPLE_CURRENT_TRANSFORMER_MODEL_H
#define EXAMPLE_CURRENT_TRANSFORMER_MODEL_H

#include "Utils.h"
#include "Complex.h"
#include "config.h"

#include "generators.h"
#include "Comtrade.h"
#include "filters_transfer_functions.h"

class MagneticInductionFluxDensityComputer
{
public:

    // constructors
    MagneticInductionFluxDensityComputer(){}

    // virtual destructor
    virtual ~MagneticInductionFluxDensityComputer(){}

    // pure virtual methods
    virtual double compute( double _MagneticFluxDensity ) const = 0;
};

class MagneticInductionFluxDensityHyperbolicApproximator : public MagneticInductionFluxDensityComputer
{
    // info
    double m_Beta  = 0;
    double m_Alpha = 0;

public:

    // constructors
    MagneticInductionFluxDensityHyperbolicApproximator(
            int    _SecondaryWindingTurnsNumber,
            double _CrossSectionArea,
            double _PathLength,
            double _NominalFrequency,
            double _CurrentVoltageCurveKneeVoltage,
            double _CurrentVoltageCurveKneeCurrent,
            double _CurrentVoltageCurveDeepSaturationVoltage,
            double _CurrentVoltageCurveDeepSaturationCurrent)
    {
        double K1 = PI2 * _NominalFrequency * (double)_SecondaryWindingTurnsNumber * _CrossSectionArea / sqrt(2.0);
        double K2 = _PathLength / (double)_SecondaryWindingTurnsNumber;

        double B1 = _CurrentVoltageCurveKneeVoltage / K1;
        double H1 = _CurrentVoltageCurveKneeCurrent / K2;

        double B2 = _CurrentVoltageCurveDeepSaturationVoltage / K1;
        double H2 = _CurrentVoltageCurveDeepSaturationCurrent / K2;

        m_Beta  = log( H2 / H1 ) / ( B2 - B1 );
        m_Alpha = H2 / __realf__( __sinhf__( Complex<double>( m_Beta * B2 ) ) );

        cout << m_Alpha << "\t" << m_Beta << "\n";

    }

    MagneticInductionFluxDensityHyperbolicApproximator(
            double _SaturationCurveKneeMagneticFluxDensity,
            double _SaturationCurveKneeMagneticInductionFluxDensity,
            double _SaturationCurveSaturatedMagneticFluxDensity,
            double _SaturationCurveSaturatedMagneticInductionFluxDensity)
    {
        m_Beta  = log( _SaturationCurveSaturatedMagneticInductionFluxDensity / _SaturationCurveKneeMagneticInductionFluxDensity ) / ( _SaturationCurveSaturatedMagneticFluxDensity - _SaturationCurveKneeMagneticFluxDensity );
        m_Alpha = _SaturationCurveSaturatedMagneticInductionFluxDensity / __realf__( __sinhf__( Complex<double>( m_Beta * _SaturationCurveSaturatedMagneticFluxDensity ) ) );

        cout << m_Alpha << "\t" << m_Beta << "\n";
    }

    // virtual destructor
    virtual ~MagneticInductionFluxDensityHyperbolicApproximator(){}

    // getters
    double get_beta() const
    {
        return m_Beta;
    }

    double get_alpha() const
    {
        return m_Alpha;
    }

    virtual double compute( double _MagneticFluxDensity ) const override
    {
        return __realf__( Complex<double>( m_Alpha ) * __sinhf__( Complex<double>( m_Beta, 0.0 ) * _MagneticFluxDensity ) );
    }
};

class CurrentTransformer
{
protected:

    // info
    const MagneticInductionFluxDensityComputer* m_MagneticInductionFluxDensityComputer = nullptr;

    int    m_PrimaryWindingTurnsNumber   = 0;
    int    m_SecondaryWindingTurnsNumber = 0;
    double m_CrossSection                = 0;
    double m_PathLength                  = 0;
    double m_Resistance                  = 0;
    double m_Inductance                  = 0;
    double m_NominalFrequency            = 0;
    double m_ADCRateFrequency            = 0;
    double m_ADCSamplingPeriod           = 0;

    differentiator<double> m_Differentiator;
    integrator<double> m_Integrator;

    double m_SecondaryCurrent             = 0; // i2(t)
    double m_MagneticInductionFluxDensity = 0; // H
    double m_MagneticFluxDensity          = 0; // B

    double m_K1 = 1.0;
    double m_K2 = 1.0;
    double m_K3 = 1.0;
    double m_K4 = 1.0;

public:

    // constructors
    CurrentTransformer(
            MagneticInductionFluxDensityComputer* _MagneticInductionFluxDensityComputer,
            int    _PrimaryWindingTurnsNumber,
            int    _SecondaryWindingTurnsNumber,
            double _CrossSection,
            double _PathLength,
            double _Resistance,
            double _Inductance,
            double _NominalFrequency,
            double _ADCRateFrequency ) :
        m_MagneticInductionFluxDensityComputer(_MagneticInductionFluxDensityComputer),
        m_PrimaryWindingTurnsNumber(_PrimaryWindingTurnsNumber),
        m_SecondaryWindingTurnsNumber(_SecondaryWindingTurnsNumber),
        m_CrossSection(_CrossSection),
        m_PathLength(_PathLength),
        m_Resistance(_Resistance),
        m_Inductance(_Inductance),
        m_NominalFrequency(_NominalFrequency),
        m_ADCRateFrequency(_ADCRateFrequency)
    {
        // initialization
        m_K1                = (double)m_PrimaryWindingTurnsNumber / (double)m_SecondaryWindingTurnsNumber;
        m_K2                = m_PathLength / (double)m_SecondaryWindingTurnsNumber;
        m_K3                = m_Resistance / (double)m_SecondaryWindingTurnsNumber / m_CrossSection;
        m_K4                = m_Inductance / (double)m_SecondaryWindingTurnsNumber / m_CrossSection;
        m_ADCSamplingPeriod = 1.0 / _ADCRateFrequency;

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

    double get_magnetic_flux_density() const
    {
        return m_MagneticFluxDensity;
    }

    // public methods
    double process( double* _PrimaryCurrentSample )
    {
        if( m_MagneticInductionFluxDensityComputer == nullptr || _PrimaryCurrentSample == nullptr )
            return 0.0;

        m_SecondaryCurrent             = *_PrimaryCurrentSample * m_K1 - m_MagneticInductionFluxDensity * m_K2;
        m_MagneticFluxDensity          += (m_SecondaryCurrent * m_K3 + m_Differentiator(&m_SecondaryCurrent) * m_K4) * m_ADCSamplingPeriod;
        m_MagneticInductionFluxDensity = m_MagneticInductionFluxDensityComputer->compute(m_MagneticFluxDensity);

        //m_SecondaryCurrent             = *_PrimaryCurrentSample * m_K1 - m_MagneticInductionFluxDensity * m_K2;
        //m_MagneticFluxDensity          = m_Integrator(&m_SecondaryCurrent) * m_K3 + m_SecondaryCurrent * m_K4;
        //m_MagneticInductionFluxDensity = m_MagneticInductionFluxDensityComputer->compute(m_MagneticFluxDensity);

        return m_SecondaryCurrent;
    }
};

void magnetic_induction_flux_density_hyperbolic_approximator_test()
{
    printf("running test %s \n\n", STRINGIFY(magnetic_induction_flux_density_hyperbolic_approximator_test));

    // control points of B-H characteristic
    double H1 = 0.2;
    double B1 = 392.0;
    double H2 = 1.5;
    double B2 = 426.0;

    // open files
    std::ofstream bt;
    std::ofstream ht;
    bt.open( LOGS_DIRECTORY + "\\bt.txt");
    ht.open( LOGS_DIRECTORY + "\\ht.txt");

    MagneticInductionFluxDensityHyperbolicApproximator
            magneticInductionFluxDensityHyperbolicApproximator( B1, H1, B2, H2 );

    for( int i = 0 ; i < 435 ; i++ )
    {
        ht << i << "\n";
        bt << magneticInductionFluxDensityHyperbolicApproximator.compute( (double)i ) << "\n";
    }

    cout << magneticInductionFluxDensityHyperbolicApproximator.get_beta() << "\t" << magneticInductionFluxDensityHyperbolicApproximator.get_alpha();

    // close files
    bt.close();
    ht.close();
}

void current_transformer_test()
{
    printf("running test %s \n\n", STRINGIFY(current_transformer_test));

    // emulation parameters:
    double Fs                = 4000;
    double Fn                = 50;
    double time              = 0;
    double EmulationDuration = 0.08;
    int    CycleWidth        = 5;
    int    cycles_num        = 1000 * EmulationDuration / CycleWidth;
    int    frames_per_cycle  = CycleWidth * Fs / 1000;

    // create COMTARDE registrator
    Comtrade registrator
            (
                STRINGIFY(current_transformer_test),
                Fn,
                Fs,
                {
                    new ComtradeAnalogChannel("I1"),
                    new ComtradeAnalogChannel("I2"),
                    new ComtradeAnalogChannel("B"),
                    new ComtradeAnalogChannel("H")
                }
            );

    registrator.set_samples_number( frames_per_cycle * cycles_num );

    // generator initialization
    generator<double>     generator;
    digital_clock<double> timeProvider;
    timeProvider.init(Fs);

    // retrieve channels
    ComtradeAnalogChannel* I1 = registrator.find_analog_channel("I1");
    ComtradeAnalogChannel* I2 = registrator.find_analog_channel("I2");
    ComtradeAnalogChannel* B = registrator.find_analog_channel("B");
    ComtradeAnalogChannel* H = registrator.find_analog_channel("H");

    // current transformer model initialization
    double H1 = 0.2;
    double B1 = 392.0;
    double H2 = 1.5;
    double B2 = 426.0;

    double primaryWindingTurns   = 1;
    double secondaryWindingTurns = 200;
    double crossSectionArea      = 0.0007;
    double pathLength            = 0.37;
    double resistance            = (0.31 + 0.48);
    double inductance            = ( 3.644*10e-5 + 0.001146 );

    /*
    MagneticInductionFluxDensityHyperbolicApproximator(
            int    _SecondaryWindingTurnsNumber,
            double _CrossSectionArea,
            double _PathLength,
            double _NominalFrequency,
            double _CurrentVoltageCurveKneeVoltage,
            double _CurrentVoltageCurveKneeCurrent,
            double _CurrentVoltageCurveDeepSaturationVoltage,
            double _CurrentVoltageCurveDeepSaturationCurrent)*/

    CurrentTransformer currentTransformer
            (
                new MagneticInductionFluxDensityHyperbolicApproximator
                (
                    1.507,
                    65,
                    1.81,
                    970
                ),
                primaryWindingTurns,
                secondaryWindingTurns,
                crossSectionArea,
                pathLength,
                resistance,
                inductance,
                Fn,
                Fs
            );

    // emulation
    int k = 0;
    for( int i = 0 ; i < cycles_num ; i++ )
    {
        for( int j = 0 ; j < frames_per_cycle ; j++, time = timeProvider.tick(), k++ )
        {
            double primaryCurrent = 0;

            /*if( time < EmulationDuration * 0.5 )
            {
                primaryCurrent  = generator.sine( 1e3, Fn, 0, time );
            }
            else
            {
                primaryCurrent  = generator.sine( 100e3, Fn, 0, time );
            }*/

            primaryCurrent  = generator.sine( 20e3, Fn, 0, time );

            currentTransformer.process( &primaryCurrent );

            I1->set_sample( k, primaryCurrent );
            I2->set_sample( k, currentTransformer.get_secondary_current() );
            B->set_sample( k, currentTransformer.get_magnetic_flux_density() );
            H->set_sample( k, currentTransformer.get_magnetic_induction_flux_density() );
        }
    }

    // write COMTRADE file
    registrator.to_file( LOGS_DIRECTORY, STRINGIFY(current_transformer_test) );
}

#endif // EXAMPLE_CURRENT_TRANSFORMER_MODEL_H
