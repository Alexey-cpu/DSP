#ifndef FILTERS_QDM_H
#define FILTERS_QDM_H

#include "filters_tsf.h"
#include "filters_rff.h"

#ifndef PI0
#define PI0 3.1415926535897932384626433832795
#endif

#ifndef PI2
#define PI2 6.283185307179586476925286766559
#endif

class quadrature_demodulator : public transfer_function_model
{
protected:

    // info
    Complex<double>*           m_UnitVectors      = nullptr;
    Complex<double>*           m_OutputVectors    = nullptr;
    Complex<double>*           m_ReferenceVectors = nullptr;
    recursive_fourier<double>* m_Filters          = nullptr;
    int                        m_SpectrumWidth    = 5;

public:

    // constructors
    quadrature_demodulator(){}

    // virtual destructor
    virtual ~quadrature_demodulator()
    {
        if( m_UnitVectors != nullptr )
            delete [] m_UnitVectors;

        if( m_OutputVectors != nullptr )
            delete [] m_OutputVectors;

        if( m_ReferenceVectors != nullptr )
            delete [] m_ReferenceVectors;

        if( m_Filters != nullptr )
            delete [] m_Filters;
    }

    // public methods
    void init( double _Fs, double _Fn, int _SpectrumWidth )
    {
        // call base initializer
        transfer_function_model::init( _Fs, _Fn );

        // initialize variables
        m_SpectrumWidth = _SpectrumWidth;

        // initialize unit vectors for each harmonic
        m_UnitVectors = new Complex<double>[ m_SpectrumWidth ];

        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
        {
            m_UnitVectors[i] =
                    Complex<double>
                    (
                        +cos( PI2 * (double)i * _Fn / _Fs ),
                        +sin( PI2 * (double)i * _Fn / _Fs )
                    );
        }

        // initialize output vectors
        m_OutputVectors = new Complex<double>[ m_SpectrumWidth ];

        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
        {
            m_OutputVectors[i] = 0.0;
        }

        // initialize referece vectors
        m_ReferenceVectors = new Complex<double>[ m_SpectrumWidth ];

        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
        {
            m_ReferenceVectors[i] = Complex<double>( 1.0, 0.0 );
        }

        // initialize recursive Fourier filters
        m_Filters = new recursive_fourier<double>[ m_SpectrumWidth ];

        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
        {
            m_Filters[i].init( _Fs, _Fn, i );
        }
    }

    Complex<double> filt( double* _Input, int _HarmonicNumber )
    {
        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
        {
            m_OutputVectors[i] = m_Filters[i](_Input) * __conjf__( m_ReferenceVectors[i] );

            m_ReferenceVectors[i] *= m_UnitVectors[i];
        }

        return _HarmonicNumber < m_SpectrumWidth ? m_OutputVectors[_HarmonicNumber] : Complex<double>::zero();
    }

    // virtual functions override
    virtual Complex<double> frequency_response( double _F ) override
    {
        return Complex<double>::zero();
    }
};

// macro undefenition to avoid aliases
#undef PI0
#undef PI2

#endif // FILTERS_QDM_H
