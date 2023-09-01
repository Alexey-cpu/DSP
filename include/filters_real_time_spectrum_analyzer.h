#ifndef FILTERS_REAL_TIME_SPECTRUM_ANALYZER_H
#define FILTERS_REAL_TIME_SPECTRUM_ANALYZER_H

#include "filters_recursive_fourier_filter.h"

#ifndef PI0
#define PI0 3.1415926535897932384626433832795
#endif

#ifndef PI2
#define PI2 6.283185307179586476925286766559
#endif

class real_time_spectrum_analyzer final : public transfer_function_model
{
protected:

    // info
    delay<double>             m_Buffer;
    shared_recursive_fourier* m_Filters       = nullptr;
    int                       m_SpectrumWidth = 5;
    double                    m_Gain          = 1.0 / sqrt(2.0);

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

    Complex<double> filt( double* _Input, int _HarmonicNumber )
    {
        if( _Input == nullptr )
            return Complex<double>::zero();

        // fill buffer
        m_Buffer.fill_buff(_Input);

        // filt signal
        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
            m_Filters[i].filt<double,double>( _Input, &m_Buffer );

        return _HarmonicNumber < m_SpectrumWidth ?
                    m_Filters[_HarmonicNumber].get_vector( true ) * m_Gain:
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

class real_time_frequency_invariant_spectrum_analyzer final : public transfer_function_model
{
protected:

    // info
    int                m_SpectrumWidth   = 5;
    int                m_N1              = 0;
    int                m_N2              = 0;
    int                m_N3              = 0;
    double             m_Gain            = 1.0;
    double             m_Fn              = 50.0;
    Complex< double >* m_UnitVectors     = nullptr;
    Complex< double >* m_ReferenceFrames = nullptr;
    delay<double>*     m_InputDelays1    = nullptr;
    delay<double>*     m_InputDelays2    = nullptr;
    delay<double>*     m_OutputDelays1   = nullptr;
    delay<double>*     m_OutputDelays2   = nullptr;

public:

    // constructors
    real_time_frequency_invariant_spectrum_analyzer(){}

    // virtual destructor
    virtual ~real_time_frequency_invariant_spectrum_analyzer()
    {
        if( m_InputDelays1 != nullptr )
            delete [] m_InputDelays1;

        if( m_InputDelays2 != nullptr )
            delete [] m_InputDelays2;

        if( m_OutputDelays1 != nullptr )
            delete [] m_OutputDelays1;

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
        // initialize filter info
        this->m_Fs            = _Fs;
        this->m_Fn            = _Fn;
        this->m_Ts            = 1.0 / _Fs;
        this->m_Order         = 1.5 * _Fs / _Fn;
        this->m_SpectrumWidth = _SpectrumWidth;

        // setup unit vectors and reference frames
        m_UnitVectors     = new Complex<double>[ m_SpectrumWidth ];
        m_ReferenceFrames = new Complex<double>[ m_SpectrumWidth ];

        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
        {
            m_UnitVectors[i] =
                    Complex<double>
                    (
                        +cos( PI2 * (double)i * _Fn / _Fs ),
                        +sin( PI2 * (double)i * _Fn / _Fs )
                    );

            m_ReferenceFrames[i] = Complex<double>( 1.0, 0.0 );
        }

        // initialize input delays
        int N1 = _Fs / _Fn;
        int N2 = _Fs / _Fn / 2.0;
        m_InputDelays1 = new delay<double>[ m_SpectrumWidth ];
        m_InputDelays2 = new delay<double>[ m_SpectrumWidth ];

        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
        {
            m_InputDelays1[i].allocate(N1+N2+1);
            m_InputDelays2[i].allocate(N1+N2+1);
        }

        // initialize output delays
        m_OutputDelays1 = new delay<double>[ m_SpectrumWidth ];
        m_OutputDelays2 = new delay<double>[ m_SpectrumWidth ];

        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
        {
            m_OutputDelays1[i].allocate(2);
            m_OutputDelays2[i].allocate(2);
        }

        // compute filter transfer function delays
        m_N1   = N1;
        m_N2   = N2;
        m_N3   = N1 + N2;
        m_Gain = 4.0 / (double)N1 / (double)N1 / sqrt(2.0);
    }

    Complex<double> filt( double* _Input, int _HarmonicNumber )
    {
        if( _Input == nullptr )
            return Complex<double>::zero();

        for( int i = 0 ; i < m_SpectrumWidth ; i++ )
        {
            // multiply by reference frames
            double a = *_Input * __imagf__( m_ReferenceFrames[i] );
            double b = *_Input * __realf__( m_ReferenceFrames[i] );

            // fill direct / quadrature filters delays
            m_InputDelays1[i].fill_buff( &a );
            m_InputDelays2[i].fill_buff( &b );

            // direct component filter
            a = ( 2 * m_OutputDelays1[i][0] - m_OutputDelays1[i][1] );
            b = ( m_InputDelays1[i][0] - m_InputDelays1[i][ m_N1 ] - m_InputDelays1[i][ m_N2 ] + m_InputDelays1[i][ m_N1 + m_N2 ] ) * m_Gain;
            a = a + b;
            m_OutputDelays1[i].fill_buff(&a);

            // quadrature component filter
            a = ( 2 * m_OutputDelays2[i][0] - m_OutputDelays2[i][1] );
            b = ( m_InputDelays2[i][0] - m_InputDelays2[i][ m_N1 ] - m_InputDelays2[i][ m_N2 ] + m_InputDelays2[i][ m_N1 + m_N2 ] ) * m_Gain;
            b = a + b;
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

class real_time_frequency_invariant_harmonic_extracter final : public transfer_function_model
{
private:

    // constants
    const double m_Epsilon    = 1e-4;
    const double m_UpperLimit = +sqrt( 3.4028235E+38 / 2 );
    const double m_LowerLimit = -sqrt( 3.4028235E+38 / 2 );

    // info
    delay<double>                        m_InputBuffer;
    standalone_recursive_fourier<double> m_RecursiveMeanRe;
    standalone_recursive_fourier<double> m_RecursiveMeanIm;
    shared_recursive_fourier            *m_RecursiveFourierFilters = nullptr;
    Complex< double >*                   m_ReferenceFrames         = nullptr;
    Complex< double >*                   m_UnitVectors             = nullptr;
    double                               m_Fs                      = 4000;
    double                               m_Fn                      = 50;
    int64_t                              m_SpectrumWidth           = 5;
    int64_t                              m_SamplesPerPeriod        = m_Fs / m_Fn;

    // service methods
    int64_t allocate()
    {
        #ifdef HMF_DEBUG
        Debugger::Log("harmonic_filter","allocate()","Memory allocation");
        #endif

        if( m_SpectrumWidth <= 0 )
            return false;

        // allocate and initialize recursive Fourier filter
        m_RecursiveFourierFilters = new shared_recursive_fourier[m_SpectrumWidth];
        for( int64_t i = 0 ; i < m_SpectrumWidth ; i++ )
            m_RecursiveFourierFilters[i].init( m_Fs, m_Fn, i );

        // allocate and initialize reference frames and reference frames for each harmonic
        m_ReferenceFrames = new Complex<double>[ m_SpectrumWidth ];
        m_UnitVectors     = new Complex<double>[ m_SpectrumWidth ];

        for( int64_t i = 0 ; i < m_SpectrumWidth ; i++ )
        {
            m_UnitVectors[i] =
                    Complex<double>
                    (
                        +cos( PI2 * (double)i * m_Fn / m_Fs ),
                        +sin( PI2 * (double)i * m_Fn / m_Fs )
                    );

            m_ReferenceFrames[i] = Complex<double>( 1.0, 0.0 );
        }

        // initialize and allocate recursive mean filters
        m_RecursiveMeanRe.init( m_Fs, 2.0 * m_Fn, 0);
        m_RecursiveMeanIm.init( m_Fs, 2.0 * m_Fn, 0);

        // allocate buffers
        m_InputBuffer.allocate( m_RecursiveFourierFilters[0].get_buffer_size() );

        return true;
    }

    int64_t deallocate()
    {
        #ifdef HMF_DEBUG
        Debugger::Log("harmonic_filter","deallocate()","Memory free");
        #endif

        // remove recursive Fourier filters
        if( m_RecursiveFourierFilters != nullptr )
        {
            delete [] m_RecursiveFourierFilters; // delete operator calls object destructor which frees the object resources
            m_RecursiveFourierFilters = nullptr;
        }

        // remove reference frames vector
        if( m_ReferenceFrames != nullptr )
        {
            delete [] m_ReferenceFrames;
            m_ReferenceFrames = nullptr;
        }

        // remove reference units vectors vector
        if( m_UnitVectors != nullptr )
        {
            delete [] m_UnitVectors;
            m_UnitVectors = nullptr;
        }

        return true;
    }

public:

    // constructors
    real_time_frequency_invariant_harmonic_extracter()
    {
        #ifdef HMF_DEBUG
        Debugger::Log("harmonic_filter","harmonic_filter()","Filter constructor call");
        #endif

        m_Fs                      = 4000;
        m_Fn                      = 50;
        m_SpectrumWidth           = 5;
        m_SamplesPerPeriod        = m_Fs / m_Fn;
        m_RecursiveFourierFilters = nullptr;
    }

    // virtual destructors
    virtual ~real_time_frequency_invariant_harmonic_extracter()
    {
        #ifdef HMF_DEBUG
        Debugger::Log("harmonic_filter","~harmonic_filter()","Filter destructor call");
        #endif

        deallocate();
    }

    // public methods
    void init(double _Fs, double _Fn, int64_t _SpectrumWidth)
    {
        // call base implementation
        transfer_function_model::init( _Fn > 0 ? _Fs / _Fn : 1, _Fs );

        // initialize system variables
        m_Fs                      = _Fs;
        m_Fn                      = _Fn;
        m_SpectrumWidth           = _SpectrumWidth;
        m_SamplesPerPeriod        = m_Fs / m_Fn;
        m_RecursiveFourierFilters = nullptr;

        // memory allocation
        allocate();

        #ifdef HMF_DEBUG
        Debugger::Log("harmonic_filter","init()","Filter initialization");
        Debugger::Log("Fs               = " + to_string(m_Fs));
        Debugger::Log("Fn               = " + to_string(m_Fn));
        Debugger::Log("SpectrumWidth    = " + to_string(m_SpectrumWidth));
        Debugger::Log("SamplesPerPeriod = " + to_string(m_SamplesPerPeriod));
        #endif
    }

    Complex<double> filt( double* _Input, int64_t _HarmonicNumber )
    {
        // check input
        if( _Input == nullptr || _HarmonicNumber >= m_SpectrumWidth )
            return Complex<double>::zero();

        // bound signal between upper and lower limits
        double boundedInput = __saturation__( *_Input, m_UpperLimit, m_LowerLimit);

        // fill shared buffer
        m_InputBuffer.fill_buff(&boundedInput);

        // filter bounded signal
        m_RecursiveFourierFilters[_HarmonicNumber].filt( &boundedInput, &m_InputBuffer );

        // retrieve vector
        Complex<double> signalVector = m_RecursiveFourierFilters[_HarmonicNumber].get_vector();

        // compute vector amplitude
        double a = __realf__(signalVector) * __realf__(signalVector);
        double b = __imagf__(signalVector) * __imagf__(signalVector);
        double c = 0.0;
        m_RecursiveMeanRe(&a);
        m_RecursiveMeanIm(&b);

        if( _HarmonicNumber == 0 )
        {
            a = __abs__( __realf__( m_RecursiveMeanRe.get_vector() ) );
            c = ( a < m_Epsilon ) ? (double)0 : sqrt( a );
        }
        else
        {
            a = __abs__( __realf__( m_RecursiveMeanRe.get_vector() ) );
            b = __abs__( __realf__( m_RecursiveMeanIm.get_vector() ) );
            c = ( ( a < m_Epsilon ) && ( b < m_Epsilon ) ) ? (double)0 : sqrt( ( a + b ) * (double)0.5 );
        }

        // compute input isgnal phase unit vector relatively to the reference frame
        Complex<double> signalPhaseUnitVector = Complex<double>
                (
                    cos( __cargf__( signalVector ) ),
                    sin( __cargf__( signalVector ) )
                ) * __conjf__( m_ReferenceFrames[_HarmonicNumber] ) * Complex<double>( 0.0, +1.0 );

        // update reference frame
        m_ReferenceFrames[_HarmonicNumber] *= m_UnitVectors[_HarmonicNumber];

        // compute vector
        return signalPhaseUnitVector * c;
    }

    // virtual methods override
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
