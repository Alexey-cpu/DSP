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
     *  \brief classic filter interface
     *  \details Defines the IIR/FIR classic filter interface
    */
    class classic_filter_interface
    {
    public:

        // constructors
        classic_filter_interface(){}

        // virtual destructor
        virtual ~classic_filter_interface(){}

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
        filter_data<double> compute_filter_data(filter_type type)
        {
            switch (type)
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
            if(data.cfnum)
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
            if(data.cfden)
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
            if(data.gains)
            {
                matrix.gains = __alloc__<__type>( data.N + 1 );

                for( int64_t j = 0 ; j < data.N ; j++ )
                {
                    matrix.gains[j] = data.gains[j];
                }

                matrix.gains[data.N] = data.gains[data.N];
            }

            // overwrite zeros
            if( data.zeros )
            {
                matrix.zeros = __alloc__< Complex<double> >( data.N );

                for( int64_t j = 0 ; j < data.N ; j++ )
                {
                    matrix.zeros[j] = data.zeros[j];
                }
            }

            // overwrite poles
            if( data.zeros )
            {
                matrix.poles = __alloc__< Complex<double> >( data.N );

                for( int64_t j = 0 ; j < data.N ; j++ )
                {
                    matrix.poles[j] = data.poles[j];
                }
            }

            // overwrite ratios
            if( data.ratio )
            {
                matrix.ratio = __alloc__< Complex<double> >( data.N );

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

        void init(int64_t order, double Fs)
        {
            m_Order = order;
            m_Fs    = Fs;
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
     *           The class models delay for DSP
    */
    template< typename __type >
    class delay final
    {
    protected:
        __type *m_upper;   ///< delay buffer upper part pointer
        __type *m_lower;   ///< delay buffer lower part pointer
        __type *m_data;    ///< delay buffer data samples buffer
        int     m_nelem;   ///< delay buffer single ( upper/lower ) part size
        int     m_buffpos; ///< delay buffer current upper/lower part pointer position

        /*!
         *  \brief The function fills delay buffer
         *  \param[input] input sample pointer
        */
        template< typename T > inline void fill_buff( T *input )
        {
            *m_lower = *input;
            *m_upper = *input;
            if( ++m_buffpos >= m_nelem )
            {
                m_lower = &m_data[0];
                m_upper = &m_data[m_nelem];
                m_buffpos = 0;
            }
            else
            {
                m_lower++;
                m_upper++;
            }
        }

    public:

        /*! \brief default constructor */
        delay()
        {
            #ifdef BUFFER_DEBUG
            Debugger::Log("delay_base","delay_base()","Constructor call");
            #endif

            m_upper   = nullptr;
            m_lower   = nullptr;
            m_data    = nullptr;
            m_nelem   = 0;
            m_buffpos = 0;
        }

        /*!
         *  \brief default constructor
         *  \details The functions clears delay resources at the end of it's lifetime
        */
        virtual ~delay()
        {
            #ifdef BUFFER_DEBUG
            Debugger::Log("delay_base","~delay_base()","Destructor call");
            #endif

            deallocate();
        }

        /*!
         *  \brief The function allocates delay buffer
         *  \param[nelem] delay buffer upper/lower part size
         *  \details The function is supposed to be called explicitly by the user
        */
        int allocate( int nelem )
        {

            #ifdef BUFFER_DEBUG
            Debugger::Log("delay_base","allocate()","Memory allocation");
            #endif

            if( ( nelem > 0 ) && !m_data )
            {
                m_nelem = nelem;
                m_data = __alloc__<__type>( 2 * m_nelem );

                if( m_data )
                {
                   m_lower = &m_data[0];
                   m_upper = &m_data[m_nelem];
                }

                return ( m_data != 0 );
            }
            else return 0;
        }

        /*! \brief The function frees delay resources */
        void deallocate()
        {
            #ifdef BUFFER_DEBUG
            Debugger::Log("delay_base","deallocate()","Memory deallocation");
            #endif

            m_data = __mfree__(m_data);
        }

        /*! \brief the function returns current lower/upper part pointer position */
        inline int getBuffPos()
        {
            return m_buffpos;
        }

        /*! \brief the function returns lower/upper part delay buffer size */
        inline int getBuffSize()
        {
            return m_nelem;
        }


        /*!
         *  \brief the operator invokation results in return of an n-th sample from the past values
         *  \param[n] sample number
        */
        inline __type operator [] ( int n )
        {
            return *( m_upper - n - 1 );
        }

        /*! \brief The operator invokation supposes to result in delay buffer fill */
        template< typename T > inline void operator () ( T *input )
        {
            this->fill_buff<T>(input);
        }
    };

    /*! @} */

    /*!
     *  \brief buffer model
     *  \details Defines buffer model
     *           The class models circular buffer
    */
    template< typename __type >
    class buffer final
    {
    protected:

        __type *m_lower;   ///< buffer pointer
        __type *m_data;    ///< data samples buffer
        int     m_nelem;   ///< buffer size
        int     m_buffpos; ///< current buffer pointer position

        /*!
         *  \brief The function fills buffer
         *  \param[input] input sample pointer
        */
        template< typename T > inline void fill_buff( T *input )
        {
            *m_lower = *input;
            if( ++m_buffpos >= m_nelem )
            {
                m_lower   = &m_data[0];
                m_buffpos = 0;
            }
            else m_lower++;
        }

    public:

        /*! \brief default constructor */
        buffer()
        {
            #ifdef BUFFER_DEBUG
            Debugger::Log("buffer_base","buffer_base()","Constructor call");
            #endif

            m_lower   = nullptr;
            m_data    = nullptr;
            m_nelem   = 0;
            m_buffpos = 0;
        }

        /*!
         *  \brief default destructor
         *  \details The function clears buffer resources
        */
        virtual ~buffer()
        {
            #ifdef BUFFER_DEBUG
            Debugger::Log("buffer_base","~buffer_base()","Destructor call");
            #endif

            deallocate();
        }

        /*!
         *  \brief The function allocates buffer
         *  \param[nelem] delay buffer upper/lower part size
         *  \details The function is supposed to be called explicitly by the user
        */
        int allocate( int nelem )
        {
            #ifdef BUFFER_DEBUG
            Debugger::Log("buffer_base","allocate()","Memory allocation");
            #endif

            if( ( nelem > 0 ) && !m_data )
            {
                m_nelem = nelem;
                m_data  = __alloc__<__type>( m_nelem );
                m_lower = ( m_data ) ? &m_data[0] : 0;
                return ( m_data != 0 );
            }
            else return 0;
        }

        /*! \brief The function frees delay resources */
        void deallocate()
        {
            #ifdef BUFFER_DEBUG
            Debugger::Log("buffer_base","deallocate()","Memory deallocation");
            #endif

            m_data = __mfree__(m_data);
        }

        /*! \brief the function returns current lower/upper part pointer position */
        inline int getBuffPos()
        {
            return m_buffpos;
        }

        /*! \brief the function returns lower/upper part delay buffer size */
        inline int getBuffSize()
        {
            return m_nelem;
        }


        /*!
         *  \brief the operator invokation results in return of an n-th sample from the past values
         *  \param[n] sample number
        */
        inline __type operator [] ( int n )
        {
            return m_data[n];
        }

        /*! \brief The operator invokation supposes to result in delay buffer fill */
        template< typename T > inline void operator () ( T *input )
        {
            this->fill_buff<T>(input);
        }
    };

    /*! @} */
}

#endif // KERNEL_DSP_H
