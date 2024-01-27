#ifndef CURRENT_TRANSFORMER_H
#define CURRENT_TRANSFORMER_H

#include "filters_transfer_functions.h"
#include "Complex.h"
#include "Utils.h"

// Magnetization curve
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
    // info
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
    virtual ~MagnetizationCurveSmartApproximator()
    {
        // clean
        if( m_PiecewiseApproximator != nullptr )
            delete m_PiecewiseApproximator;

        if( m_HyperbolicApproximator != nullptr )
            delete m_HyperbolicApproximator;
    }

    virtual double compute( double _MagneticInduction ) const override
    {
        return !m_PiecewiseApproximator->out_of_bounds( _MagneticInduction ) ?
                    m_PiecewiseApproximator->compute( _MagneticInduction) :
                    m_HyperbolicApproximator->compute( _MagneticInduction );
    }
};

// Current transformer model
class CurrentTransformer
{
protected:

    // info
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
    double process_primary_current( double* _PrimaryCurrentSample )
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

    double process_secondary_current( double* )
    {
        // Type here your amazing code =)
        return 0.0;
    }
};

#endif // CURRENT_TRANSFORMER_H
