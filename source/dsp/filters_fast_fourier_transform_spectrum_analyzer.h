#ifndef FILTERS_FAST_FOURIER_TRANSFORM_SPECTRUM_ANALYZER_H
#define FILTERS_FAST_FOURIER_TRANSFORM_SPECTRUM_ANALYZER_H

#include "filters_interpolators.h"

class fast_fourier_transform_spectrum_analyzer
{
protected:

    // info
    double m_Fn             = 50;
    double m_Fs             = 4000;
    int    m_FramesPerCycle = 20;
    int    m_FFTPeriod      = 128;

    delay<double>    m_Delay;

    double           m_Gain            = 1.0 / sqrt(2.0);
    Complex<double>* m_Spectrum        = nullptr;
    Complex<double>* m_ReferenceFrames = nullptr;
    Complex<double>* m_UnitVectors     = nullptr;

    signal_lagrange_interpolator m_Interpolator;
    signal_decimator             m_Decimator;

public:

    // constructors
    fast_fourier_transform_spectrum_analyzer(){}

    // virtual destructor
    virtual ~fast_fourier_transform_spectrum_analyzer()
    {
        if( m_Spectrum != nullptr )
            delete [] m_Spectrum;

        if( m_ReferenceFrames != nullptr )
            delete [] m_ReferenceFrames;

        if( m_UnitVectors != nullptr )
            delete [] m_UnitVectors;
    }

    // public functions
    void init( double _Fs, double _Fn, int _FramesPerCycle, int _InterpolationOrder = 1 )
    {
        m_FramesPerCycle = _FramesPerCycle;
        m_Fn             = _Fn;
        m_Fs             = _Fs;

        int framesPerPeriod    = _Fs / _Fn;
        int divider            = __euclide_algorithm__( m_FFTPeriod, framesPerPeriod );
        int interpolationRatio = __max__( m_FFTPeriod, framesPerPeriod ) / divider;
        int decimationRatio    = __min__( m_FFTPeriod, framesPerPeriod ) / divider;

        m_Interpolator.init( (_InterpolationOrder > 0 ? _InterpolationOrder : 1), interpolationRatio);
        m_Decimator.init(decimationRatio);

        m_Delay.allocate(m_FFTPeriod);
        m_Spectrum        = new Complex<double>[m_FFTPeriod];
        m_ReferenceFrames = new Complex<double>[m_FFTPeriod];
        m_UnitVectors     = new Complex<double>[m_FFTPeriod];

        for( int i = 0 ; i < m_FFTPeriod ; i++ )
        {
            m_Spectrum[i]        = 0.0;
            m_ReferenceFrames[i] = Complex<double>( 1.0, 0.0 );

            m_UnitVectors[i] =
                    Complex<double>
                    (
                        +cos( PI2 * (double)i / (double)m_FFTPeriod ),
                        +sin( PI2 * (double)i / (double)m_FFTPeriod )
                    );
        }
    }

    Complex<double> filt( double* _Input, int _HarmonicNumber )
    {
        // interpolation / decimation cycle
        for( int i = 0 ; i < m_FramesPerCycle ; i++ )
        {
            m_Interpolator.filt( &_Input[i] );

            for( int j = 0 ; j < m_Interpolator.get_rate_buffer().get_buff_size() ; j++ )
            {
                if( m_Decimator.decimate() )
                {
                    double data = m_Interpolator.get_rate_buffer().get_data(j);

                    m_Delay.fill_buff( &data );

                    // reference frame update
                    for( int k = 1 ; k < m_FFTPeriod ; k++ )
                    {
                        m_ReferenceFrames[k] *= m_UnitVectors[k];
                    }
                }
            }
        }

        // retrieve data
        for( int k = 0 ; k < m_FFTPeriod ; k++ )
        {
            m_Spectrum[k] = m_Delay.get_data(k);
        }

        // FFT cycle
        __fft0__<double>( m_Spectrum, m_FFTPeriod, 1);

        // convolution cycle
        for( int k = 1 ; k < m_FFTPeriod ; k++ )
        {
            m_Spectrum[k] *= m_ReferenceFrames[k];
        }

        return _HarmonicNumber < m_FFTPeriod ? m_Spectrum[_HarmonicNumber] * m_Gain : Complex<double>::zero();
    }
};

#endif // FILTERS_FAST_FOURIER_TRANSFORM_SPECTRUM_ANALYZER_H
