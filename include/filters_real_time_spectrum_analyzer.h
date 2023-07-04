#ifndef FILTERS_REAL_TIME_SPECTRUM_ANALYZER_H
#define FILTERS_REAL_TIME_SPECTRUM_ANALYZER_H

#include "filters_recursive_fourier_filter.h"

#ifndef PI0
#define PI0 3.1415926535897932384626433832795
#endif

#ifndef PI2
#define PI2 6.283185307179586476925286766559
#endif

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

class real_time_frequency_invariant_spectrum_analyzer : public transfer_function_model
{
protected:

    // info
    int                m_SpectrumWidth   = 5;
    Complex< double >* m_UnitVectors     = nullptr;
    Complex< double >* m_ReferenceFrames = nullptr;
    delay<double>*     m_InputDelays1    = nullptr;
    delay<double>*     m_OutputDelays1   = nullptr;
    delay<double>*     m_InputDelays2    = nullptr;
    delay<double>*     m_OutputDelays2   = nullptr;

    int m_N1 = 0;
    int m_N2 = 0;
    double m_Gain;

public:

    // constructors
    real_time_frequency_invariant_spectrum_analyzer(){}

    // virtual destructor
    virtual ~real_time_frequency_invariant_spectrum_analyzer()
    {
        if( m_InputDelays1 != nullptr )
            delete [] m_InputDelays1;

        if( m_OutputDelays1 != nullptr )
            delete [] m_OutputDelays1;

        if( m_InputDelays2 != nullptr )
            delete [] m_InputDelays2;

        if( m_OutputDelays2 != nullptr )
            delete [] m_OutputDelays2;

        if( m_UnitVectors != nullptr )
            delete [] m_UnitVectors;

        if( m_ReferenceFrames != nullptr )
            delete [] m_ReferenceFrames;
    }

    // public methods
    void init( double _Fs, double _Fn, int _SpectrumWidth = 5 )
    {
        // intialize transfer function model and setup spectrum width
        this->m_SpectrumWidth = _SpectrumWidth;

        // setup unit vectors and reference frames
        m_UnitVectors     = new Complex<double>[ m_SpectrumWidth ];
        m_ReferenceFrames = new Complex<double>[ m_SpectrumWidth ];

        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
        {
            m_UnitVectors[i] =
                    Complex<double>
                    (
                        cos( PI2 * (double)i * _Fn / _Fs ),
                        sin( PI2 * (double)i * _Fn / _Fs )
                    );

            m_ReferenceFrames[i] = Complex<double>( 1.0, 0.0 );
        }

        // initialize input delays
        int N = 2 * _Fs / _Fn;
        m_InputDelays1 = new delay<double>[ m_SpectrumWidth ];
        m_InputDelays2 = new delay<double>[ m_SpectrumWidth ];

        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
        {
            m_InputDelays1[i].allocate(N+1);
            m_InputDelays2[i].allocate(N+1);
        }

        //
        m_N1   = (double)N / 2.0;
        m_N2   = N;
        m_Gain = 1 / (double)m_N1 / (double)m_N1;

        // initialize output delays
        m_OutputDelays1 = new delay<double>[ m_SpectrumWidth ];
        m_OutputDelays2 = new delay<double>[ m_SpectrumWidth ];

        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
        {
            m_OutputDelays1[i].allocate(4);
            m_OutputDelays2[i].allocate(4);
        }
    }

    Complex<double> filt( double* _Input, int _HarmonicNumber, bool _A = true )
    {
        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
        {
            // multiply by reference frames
            double a = *_Input * __realf__( m_ReferenceFrames[i] );
            double b = *_Input * __imagf__( m_ReferenceFrames[i] );

            // fill input delays
            m_InputDelays1[i].fill_buff( &a );
            m_InputDelays2[i].fill_buff( &b );

            // filt
            a = ( 2 * m_OutputDelays1[i][0] - m_OutputDelays1[i][1] ) +
                    ( m_InputDelays1[i][0] - 2 * m_InputDelays1[i][ m_N1 ] + m_InputDelays1[i][ m_N2 ] ) * m_Gain;

            b = ( 2 * m_OutputDelays2[i][0] - m_OutputDelays2[i][1] ) +
                    ( m_InputDelays2[i][0] - 2 * m_InputDelays2[i][ m_N1 ] + m_InputDelays2[i][ m_N2 ] ) * m_Gain;

            // fill output delays
            m_OutputDelays1[i].fill_buff(&a);
            m_OutputDelays2[i].fill_buff(&b);

            // update reference frame
            m_ReferenceFrames[i] *= m_UnitVectors[i];
        }

        return _HarmonicNumber < m_SpectrumWidth ?
                    Complex<double>( m_OutputDelays1[_HarmonicNumber][0], m_OutputDelays2[_HarmonicNumber][0] ) :
                    Complex<double>::zero();
    }

    // virtual functions override
    virtual Complex<double> frequency_response( double _F ) override
    {
        (void)_F;
        return Complex<double>::zero();
    }
};

// macro undefenition to avoid aliases
#undef PI0
#undef PI2

#endif // FILTERS_REAL_TIME_SPECTRUM_ANALYZER_H
