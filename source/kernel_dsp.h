#ifndef KERNEL_DSP_H
#define KERNEL_DSP_H

#ifndef __ALG_PLATFORM
#include "cmath"
#include "math.h"
#include <iostream>
#include <cstring>
#include <stdlib.h>
#endif

#include "utils.h"
#include "Complex.h"

#define DEBUG

/*! \defgroup <FILTERS> ( Filters )
 *  \brief The module contains different filters implementations description
    @{
*/

/*! @} */

/*! \defgroup <SPECIAL_FILTERS> ( Special filters )
 *  \brief The module contains some specific filters
    @{
*/

/*! @} */

/*! \defgroup <KERNELS> ( Kernels )
 *  \brief The module contains low level DSP kernels
    @{
*/

/*! @} */

/*!  \namespace DSP_KERNEL */
namespace DSP_KERNEL
{

    /*! \defgroup <DSP_MAIN_KERNEL> ( Main kernel )
     *  \ingroup KERNELS
     *  \brief   The module contains the main DSP low level kernel utilized to generate other DSP kernels and models
     *  \details The module contains the main DSP low level kernel utilized to generate other DSP kernels and models.
     *           The main kernel defines main data structures, ennumerations and base classes.
        @{
    */

    /*!
     *  \struct bandwidth
     *  \brief  filter passband/stopband frequency and bandwidth
    */
    struct bandwidth
    {
        double Fc; ///< filter passband/stopband frequency, Hz
        double BW; ///< filter passband/stopband width, Hz
    };

    /*!
     *  \struct attenuation
     *  \brief filter passband/stopband attenuation data structure
    */
    struct attenuation
    {
        double G1; ///< filter passband attenuation, Db
        double G2; ///< filter stopband attenuation, Db
    };

    /*!
     *  \enum filter_type
     *  \brief filter type enumeration
    */
    enum filter_type
    {
        lowpass,  ///< lowpass  type filter
        highpass, ///< highpass type filter
        bandpass, ///< bandpass type filter
        bandstop, ///< bandstop type filter
        other     ///< other    type filter
    };

    /*!
     *  \brief filter data structure
     *  \details Contains all the filter information used for the analysis and modelling.
     *           The filter data structure supposes that the filter is modeled by the
     *           number of quadratic sections that have the same numerator and
     *           denominator order
    */
    template<typename __type>
    struct filter_data
    {
        filter_type         type = lowpass; ///< filter type
        Complex< double > *poles = nullptr; ///< lowpass normalized analogue prototype complex conjugate poles pairs
        Complex< double > *zeros = nullptr; ///< lowpass normalized analogue prototype complex conjugate zeros pairs
        Complex< double > *ratio = nullptr; ///< lowpass normalized analogue prototype zero frequency gains vector
        __type *cfnum            = nullptr; ///< filter numerator quadratic sections coefficients matrix
        __type *cfden            = nullptr; ///< filter denominator quadratic sections coefficients matrix
        __type *gains            = nullptr; ///< filter quadratic sections gains vector
        int64_t L                = -1;      ///< filter complex conjugate poles/zeros pairs number
        int64_t R                = -1;      ///< filter real odd pole existance flag
        int64_t N                = -1;      ///< filter quadratic sections number
        int64_t Nx               = -1;      ///< single section numerator order
        int64_t Ny               = -1;      ///< single section denominator order
        int64_t order            = -1;      ///< filter order
    };

    /*!
     *  \brief empty filter generation function
     *  \details generates empty filter data structure
    */
    template<typename __type>
    filter_data<__type> __dsp_gen_empty_filter__()
    {
        filter_data<__type> output;
        output.type  = lowpass;
        output.poles = nullptr;
        output.zeros = nullptr;
        output.ratio = nullptr;
        output.cfnum = nullptr;
        output.cfden = nullptr;
        output.gains = nullptr;
        output.L     = -1;
        output.R     = -1;
        output.N     = -1;
        output.Nx    = -1;
        output.Ny    = -1;
        output.order = -1;
        return output;
    }

    /*!
     *  \brief filter clear function
     *  \param[data] input filter data structure
     *  \details clears filter data structure
    */
    template<typename __type>
    filter_data<__type> __dsp_clear_filter__(filter_data<__type> data)
    {
        data.type  = lowpass;
        data.poles = __mfree__(data.poles);
        data.zeros = __mfree__(data.zeros);
        data.ratio = __mfree__(data.ratio);
        data.cfnum = __mfree__(data.cfnum);
        data.cfden = __mfree__(data.cfden);
        data.gains = __mfree__(data.gains);
        data.L     = -1;
        data.R     = -1;
        data.N     = -1;
        data.Nx    = -1;
        data.Ny    = -1;
        data.order = -1;
        return data;
    }

    /*!
     *  \brief frequency responce computation interface
     *  \details Defines the building base for the DSP components creation.
     *           Use inheritance mechnism to create the deriving filter class
    */
    class frequency_responce_computation_interface
    {
    public:

        /*! \brief default constructor */
        frequency_responce_computation_interface(){}

        /*! \brief default virtual destructor */
        virtual ~frequency_responce_computation_interface(){}

        /*!
         *  \brief filter frequency responce computation function
         *  \details The function must compute filter frequency responce
         *           and return the complex transfer function value
         *           for the given frequency F
        */
        virtual Complex<double> frequency_response( double F ) = 0;
    };

    /*!
     *  \brief classic filter coefficients computation interface
     *  \details Defines the IIR/FIR classic filter coefficients computation interface
    */
    class classic_filter_coefficients_computation_interface
    {
    public:

        // constructors
        classic_filter_coefficients_computation_interface(){}

        // virtual destructor
        virtual ~classic_filter_coefficients_computation_interface(){}

        /*!  \brief lowpass filter computation function */
        virtual filter_data<double> compute_lowpass()  = 0;

        /*!  \brief highpass filter computation function */
        virtual filter_data<double> compute_highpass() = 0;

        /*!  \brief bandpass filter computation function */
        virtual filter_data<double> compute_bandpass() = 0;

        /*!  \brief bandstop filter computation function */
        virtual filter_data<double> compute_bandstop() = 0;

        /*!
         *   \brief coefficients computation function
         *   \param[type] filter type
        */
        filter_data<double> compute_filter_data(filter_type _Type)
        {
            switch (_Type)
            {
                case filter_type::lowpass :
                    return compute_lowpass();
                break;

                case filter_type::highpass :
                    return compute_highpass();
                break;

                case filter_type::bandpass :
                    return compute_bandpass();
                break;

                case filter_type::bandstop :
                    return compute_bandstop();
                break;

                case filter_type::other :
                    return compute_lowpass();
                break;
            }

             return compute_lowpass();
        }

        /*!
         *   \brief coefficients round function
         *   \param[type] filter type
         *   \details the function rounds transforms filter coefficietns from double precision to any other type exept integer.
        */
        template< typename __type > filter_data< __type > round_coefficients(filter_type type)
        {
            // compute filter data
            filter_data<double> data = compute_filter_data(type);

            // generate rouned filter data version
            filter_data<__type> matrix;

            // round numerator coefficients
            if( data.cfnum != nullptr )
            {
                matrix.cfnum = __alloc__<__type>( data.Nx * data.N );

                for( int64_t j = 0 ; j < data.N ; j++ )
                {
                    for( int64_t i = 0 ; i < data.Nx ; i++ )
                    {
                        matrix.cfnum[ j * data.Nx + i ] = data.cfnum[ j * data.Nx + i ];
                    }
                }
            }

            // round denominator coefficients
            if( data.cfden != nullptr )
            {
                matrix.cfden = __alloc__<__type>( data.Ny * data.N );

                for( int64_t j = 0 ; j < data.N ; j++ )
                {
                    for( int64_t i = 0 ; i < data.Ny ; i++ )
                    {
                        matrix.cfden[ j * data.Ny + i ] = data.cfden[ j * data.Ny + i ];
                    }
                }
            }

            // round gains
            if( data.gains != nullptr )
            {
                matrix.gains = __alloc__<__type>( data.N + 1 );

                for( int64_t j = 0 ; j < data.N ; j++ )
                {
                    matrix.gains[j] = data.gains[j];
                }

                //matrix.gains[data.N] = data.gains[data.N];
            }

            // overwrite zeros
            if( data.zeros != nullptr )
            {
                matrix.zeros = __alloc__< Complex<double> >( data.N );

                for( int64_t j = 0 ; j < data.N ; j++ )
                {
                    matrix.zeros[j] = data.zeros[j];
                }
            }

            // overwrite poles
            if( data.poles != nullptr )
            {
                matrix.poles = __alloc__< Complex<double> >( data.N );

                for( int64_t j = 0 ; j < data.N ; j++ )
                {
                    matrix.poles[j] = data.poles[j];
                }
            }

            // overwrite ratios
            if( data.ratio != nullptr )
            {
                matrix.ratio = __alloc__< Complex<double> >( data.N + 1 );

                for( int64_t j = 0 ; j < data.N ; j++ )
                {
                    matrix.ratio[j] = data.ratio[j];
                }

                matrix.ratio[data.N] = data.ratio[data.N];
            }

            // assign other fields
            matrix.L  = data.L;
            matrix.R  = data.R;
            matrix.Nx = data.Nx;
            matrix.Ny = data.Ny;
            matrix.N  = data.N;

            // clear filter data
            __dsp_clear_filter__(data);

            return matrix;
        }
    };

    /*! @} */

    /*!
     *  \brief base DSP model class
     *  \details Defines the building base for the DSP components creation.
     *           Use inheritance mechnism to create the deriving filter class
    */
    class transfer_function_model : public frequency_responce_computation_interface
    {
    protected:

        // system variables
        int64_t m_Order; ///< filter order
        double  m_Fs;    ///< filter sampling frequency
        double  m_Ts;    ///< filter sampling period

        /*!
         *  \brief initialazation function
         *  \param[order] filter order
         *  \param[Fs] filter sampling frequency
        */

        void init( int64_t _Order, double _Fs )
        {
            m_Order = _Order;
            m_Fs    = _Fs;
            m_Ts    = 1.0 / m_Fs;
        }

    public:

        /*! \brief default constructor */
        transfer_function_model(){}

        /*! \brief default virtual destructor */
        virtual ~transfer_function_model(){}
    };

    /*! @} */

    /*!
     *  \brief delay model
     *  \details Defines delay model
    */
    template< typename __type >
    class delay final
    {
    protected:

        __type *m_CurrentSamplePointer;
        __type *m_data;
        int     m_SamplesNumber;
        int     m_BuffPos;

    public:

        /*! \brief default constructor */
        delay()
        {
            #ifdef DEBUG
            Debugger::Log("delay","delay()","Constructor call");
            #endif

            m_data    = nullptr;
            m_SamplesNumber   = 0;
            m_BuffPos = 0;
        }

        /*!
         *  \brief default destructor
         *  \details The function clears buffer resources
        */
        virtual ~delay()
        {
            #ifdef DEBUG
            Debugger::Log("delay","~delay()","Destructor call");
            #endif

            deallocate();
        }

        /*!
         *  \brief The function allocates buffer
         *  \param[nelem] delay buffer upper/lower part size
         *  \details The function is supposed to be called explicitly by the user
        */
        int allocate( int _Nelem )
        {
            #ifdef DEBUG
            Debugger::Log("delay","allocate()","Memory allocation");
            #endif

            if( m_data != nullptr )
                deallocate();

            if( ( _Nelem > 0 ) && !m_data )
            {
                m_SamplesNumber = _Nelem;
                m_data  = __alloc__<__type>( m_SamplesNumber );
                return ( m_data != nullptr );
            }

            return 0;
        }

        /*! \brief The function frees delay resources */
        void deallocate()
        {
            #ifdef DEBUG
            Debugger::Log("delay","deallocate()","Memory deallocation");
            #endif

            m_data    = __mfree__(m_data);
            m_CurrentSamplePointer   = nullptr;
            m_SamplesNumber   = 0;
            m_BuffPos = 0;
        }

        /*! \brief the function returns current lower/upper part pointer position */
        inline int get_buff_pos()
        {
            return m_BuffPos;
        }

        /*! \brief the function returns lower/upper part delay buffer size */
        inline int get_buff_size()
        {
            return m_SamplesNumber;
        }

        /*! \brief the function returns pointer to local buffer */
        inline __type get_data( int _N ) const
        {
            int index = m_BuffPos - 1 - _N;

            if( index < 0 )
                index += m_SamplesNumber;

            return m_data[ index ];
        }

        /*!
         *  \brief The function fills buffer
         *  \param[input] input sample pointer
        */
        template< typename T > inline void fill_buff( T* _Input )
        {
            if( _Input == nullptr )
                return;

            m_data[m_BuffPos] = *_Input;

            if( ++m_BuffPos >= m_SamplesNumber )
                m_BuffPos = 0;
        }

        /*!
         *  \brief the operator invokation results in return of an n-th sample from the past values
         *  \param[n] sample number
        */
        inline __type operator [] ( int _N )
        {
            return get_data( _N );
        }

        /*! \brief The operator invokation supposes to result in delay buffer fill */
        template< typename T > inline void operator () ( T *input )
        {
            this->fill_buff<T>(input);
        }
    };

    /*! @} */

    /*! @} */
}

#endif // KERNEL_DSP_H
