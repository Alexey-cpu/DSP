#ifndef FILTERS_QDM_H
#define FILTERS_QDM_H

#include "filters_tsf.h"
#include "filters_rff.h"

class quadrature_demodulator : public transfer_function_model
{
protected:

    // info
    standalone_recursive_fourier<double>* m_Filters        = nullptr;
    int                        m_SpectrumWidth  = 5;

public:

    // constructors
    quadrature_demodulator(){}

    // virtual destructor
    virtual ~quadrature_demodulator()
    {
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

        // initialize recursive Fourier filters
        m_Filters = new standalone_recursive_fourier<double>[ m_SpectrumWidth ];

        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
        {
            m_Filters[i].init( _Fs, _Fn, i );
        }
    }

    Complex<double> filt( double* _Input, int _HarmonicNumber, bool _ConvolutedOutput )
    {
        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
            m_Filters[i](_Input);

        return _HarmonicNumber < m_SpectrumWidth ?
                    m_Filters[_HarmonicNumber].vector( _ConvolutedOutput ) :
                    Complex<double>::zero();
    }

    // virtual functions override
    virtual Complex<double> frequency_response( double _F ) override
    {
        return m_Filters != nullptr && m_SpectrumWidth > 0 ?
                    m_Filters[0].frequency_response(_F) :
                Complex<double>::zero();
    }

    virtual Complex<double> frequency_response( double _F, int _HarmonicNumber )
    {
        return m_Filters != nullptr && m_SpectrumWidth > 0 && _HarmonicNumber < m_SpectrumWidth ?
                    m_Filters[_HarmonicNumber].frequency_response(_F) :
                Complex<double>::zero();
    }
};

#endif // FILTERS_QDM_H
