#ifndef FILTERS_INTERPOLATORS_H
#define FILTERS_INTERPOLATORS_H

#include "kernel_dsp.h"
#include "special_functions.h"
using namespace DSP_KERNEL;

/*! \defgroup <INTERPOLATION_FIR_FILTER> ( interpolation FIR filter )
 *  \ingroup CLASSIC_FIR_FILTERS
 *  \brief the module contains signal interpolation filter
    @{
*/
class signal_lagrange_interpolator
{
protected:

    // info
    int           m_InterpolationRatio = 0;
    int           m_Order = 1;
    delay<double> m_Buffer;
    delay<double> m_Delay;

public:

    // constructors
    signal_lagrange_interpolator(){}

    // virtual destructor
    virtual ~signal_lagrange_interpolator(){}

    // public functions
    void init( int _Order, int _InterpolationRatio )
    {
        // initialize info
        m_InterpolationRatio = _InterpolationRatio;
        m_Order              = _Order;
        m_Buffer.allocate(m_InterpolationRatio);
        m_Delay.allocate(m_Order + 1);
    }

    void filt( double* _Input )
    {
        if( _Input == nullptr )
            return;

        m_Delay.fill_buff(_Input);
        double X[m_Order];
        double Y[m_Order];

        for( int i = 0 ; i < m_Order ; i++ )
        {
            X[i] = m_Order - i - 1;
            Y[i] = m_Delay[ i ];
        }

        for( int j = 0 ; j < m_InterpolationRatio ; j++ )
        {
            double interpolatedSample = lagrange_polynom_interpolation( X, Y, (double)j / (double)m_InterpolationRatio, m_Order );
            m_Buffer.fill_buff( &interpolatedSample );
        }
    }

    delay<double>& get_rate_buffer()
    {
        return m_Buffer;
    }
};

class signal_decimator
{
protected:

    int    m_DecimationRatio = 0;
    int    m_SamplesCounter  = 0;
    double m_Sample;

public:

    // constructors
    signal_decimator(){}

    // virtual destructor
    virtual ~signal_decimator(){}

    // public methods
    void init( int _DecimationRatio )
    {
        m_DecimationRatio = _DecimationRatio;
        m_SamplesCounter  = m_DecimationRatio;
    }

    bool decimate()
    {
        if( m_SamplesCounter >= m_DecimationRatio - 1 )
        {
            m_SamplesCounter = 0;
        }
        else
        {
            m_SamplesCounter++;
        }

        return m_SamplesCounter == 0;
    }
};

/*! @} */

#endif // FILTERS_INTERPOLATORS_H
