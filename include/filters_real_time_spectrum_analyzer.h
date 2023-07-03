#ifndef FILTERS_REAL_TIME_SPECTRUM_ANALYZER_H
#define FILTERS_REAL_TIME_SPECTRUM_ANALYZER_H

#include "filters_recursive_fourier_filter.h"

class real_time_spectrum_analyzer : public transfer_function_model
{
protected:

    // info
    delay<double>             m_Buffer;
    shared_recursive_fourier* m_Filters        = nullptr;
    int                       m_SpectrumWidth  = 5;

public:

    // constructors
    real_time_spectrum_analyzer(){}

    // virtual destructor
    virtual ~real_time_spectrum_analyzer()
    {
        if( m_Filters != nullptr )
            delete [] m_Filters;
    }

    // public methods
    void init( double _Fs, double _Fn, int _SpectrumWidth = 5 )
    {
        // call base initializer
        transfer_function_model::init( _Fs, _Fn );

        // initialize variables
        m_SpectrumWidth = _SpectrumWidth;

        // allocate and initialize recursive Fourier filters
        m_Filters = new shared_recursive_fourier[ m_SpectrumWidth ];

        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
            m_Filters[i].init( _Fs, _Fn, i );

        // allocate common buffer
        m_Buffer.allocate( m_Filters[0].get_buffer_size() );
    }

    Complex<double> filt( double* _Input, int _HarmonicNumber, bool _ConvolutedOutput )
    {
        // fill buffer
        m_Buffer.fill_buff(_Input);

        // filt signal
        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
            m_Filters[i].filt<double,double>( _Input, &m_Buffer );

        return _HarmonicNumber < m_SpectrumWidth ?
                    m_Filters[_HarmonicNumber].get_vector( _ConvolutedOutput ) :
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

#endif // FILTERS_REAL_TIME_SPECTRUM_ANALYZER_H
