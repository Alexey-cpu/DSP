#ifndef KERNEL_DSP_H
#define KERNEL_DSP_H

#ifndef __ALG_PLATFORM
#include "cmath"
#include "math.h"
#include <iostream>
#include <cstring>
#endif

#include "utils.h"
#include "buffer.h"
#include "Complex.h"

#define DEBUG

#ifndef __fx32
#define __fx32 float
#endif

#ifndef __fx64
#define __fx64 double
#endif

#ifndef __fxx64
#define __fxx64 long double
#endif

#ifndef __ix32
#define __ix32 int
#endif

#ifndef PI0
#define PI0 3.1415926535897932384626433832795
#endif

#ifndef PI2
#define PI2 6.283185307179586476925286766559
#endif

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
     *           The main kernel defines DSP components models main data structure and base class.
     *           It alse contains the range of auxiliary data structures and enumerations.
     *           All the models implemented using this kernel must derive from the filter_abstract base class.
        @{
    */

    /*!
     *  \struct bandwidth
     *  \brief  filter passband/stopband frequency and bandwidth
    */
    struct bandwidth
    {
        __fx64 Fc; ///< filter passband/stopband frequency, Hz
        __fx64 BW; ///< filter passband/stopband width, Hz
    };

    /*!
     *  \struct attenuation
     *  \brief filter passband/stopband attenuation data structure
    */
    struct attenuation
    {
        __fx64 G1; ///< filter passband attenuation, Db
        __fx64 G2; ///< filter stopband attenuation, Db
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
        filter_type         type  = lowpass; ///< filter type
        Complex< __fx64 > *poles = nullptr; ///< lowpass normalized analogue prototype complex conjugate poles pairs
        Complex< __fx64 > *zeros = nullptr; ///< lowpass normalized analogue prototype complex conjugate zeros pairs
        Complex< __fx64 > *ratio = nullptr; ///< lowpass normalized analogue prototype zero frequency gains vector
        __type *cfnum             = nullptr; ///< filter numerator quadratic sections coefficients matrix
        __type *cfden             = nullptr; ///< filter denominator quadratic sections coefficients matrix
        __type *gains             = nullptr; ///< filter quadratic sections gains vector
        __ix32 L                  = -1;      ///< filter complex conjugate poles/zeros pairs number
        __ix32 R                  = -1;      ///< filter real odd pole existance flag
        __ix32 N                  = -1;      ///< filter quadratic sections number
        __ix32 Nx                 = -1;      ///< single section numerator order
        __ix32 Ny                 = -1;      ///< single section denominator order
        __ix32 order              = -1;      ///< filter order
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
     *  \brief base DSP model class
     *  \details Defines the building base for the DSP components creation.
     *           Use inheritance mechnism to create the deriving filter class
    */
    class model_base
    {
    protected:

        // system variables
        __ix32 m_order; ///< filter order
        __fx64 m_Fs;    ///< filter sampling frequency
        __fx64 m_Ts;    ///< filter sampling period

        /*! \brief default constructor */
        model_base(){}

        /*!
         *  \brief initialazation function
         *  \param[order] filter order
         *  \param[Fs] filter sampling frequency
        */

        void init(__ix32 order, __fx64 Fs)
        {
            m_order = order;
            m_Fs    = Fs;
            m_Ts    = (__fx64)1 / m_Fs;
        }

        /*! \brief default virtual destructor */
        virtual ~model_base(){}

    public:

        /*!
         *  \brief filter frequency responce computation function
         *  \details The function must compute filter frequency responce
         *           and return the complex transfer function value
         *           for the given frequency F
        */
        virtual Complex<__fx64> frequency_response( __fx64 F ) = 0;
    };

    /*!
     *  \brief classic filter interface
     *  \details Defines the IIR/FIR classic filter interface
    */
    class classic_filter_interface
    {
    public:

        /*!  \brief lowpass filter computation function */
        virtual filter_data<__fx64> compute_lowpass()  = 0;

        /*!  \brief highpass filter computation function */
        virtual filter_data<__fx64> compute_highpass() = 0;

        /*!  \brief bandpass filter computation function */
        virtual filter_data<__fx64> compute_bandpass() = 0;

        /*!  \brief bandstop filter computation function */
        virtual filter_data<__fx64> compute_bandstop() = 0;

        /*!
         *   \brief coefficients computation function
         *   \param[type] filter type
        */
        filter_data<__fx64> compute_filter_data(filter_type type)
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
            filter_data<__fx64> data = compute_filter_data(type);

            // generate rouned filter data version
            filter_data<__type> matrix;

            // round numerator coefficients
            if(data.cfnum)
            {
                matrix.cfnum = __alloc__<__type>( data.Nx * data.N );

                for( __ix32 j = 0 ; j < data.N ; j++ )
                {
                    for( __ix32 i = 0 ; i < data.Nx ; i++ )
                    {
                        matrix.cfnum[ j * data.Nx + i ] = data.cfnum[ j * data.Nx + i ];
                    }
                }
            }

            // round denominator coefficients
            if(data.cfden)
            {
                matrix.cfden = __alloc__<__type>( data.Ny * data.N );

                for( __ix32 j = 0 ; j < data.N ; j++ )
                {
                    for( __ix32 i = 0 ; i < data.Ny ; i++ )
                    {
                        matrix.cfden[ j * data.Ny + i ] = data.cfden[ j * data.Ny + i ];
                    }
                }
            }

            // round gains
            if(data.gains)
            {
                matrix.gains = __alloc__<__type>( data.N + 1 );

                for( __ix32 j = 0 ; j < data.N ; j++ )
                {
                    matrix.gains[j] = data.gains[j];
                }

                matrix.gains[data.N] = data.gains[data.N];
            }

            // overwrite zeros
            if( data.zeros )
            {
                matrix.zeros = __alloc__< Complex<__fx64> >( data.N );

                for( __ix32 j = 0 ; j < data.N ; j++ )
                {
                    matrix.zeros[j] = data.zeros[j];
                }
            }

            // overwrite poles
            if( data.zeros )
            {
                matrix.poles = __alloc__< Complex<__fx64> >( data.N );

                for( __ix32 j = 0 ; j < data.N ; j++ )
                {
                    matrix.poles[j] = data.poles[j];
                }
            }

            // overwrite ratios
            if( data.ratio )
            {
                matrix.ratio = __alloc__< Complex<__fx64> >( data.N );

                for( __ix32 j = 0 ; j < data.N ; j++ )
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

}

#undef __fx32
#undef __fx64
#undef __fxx64
#undef __ix32
#undef PI0
#undef PI2

#endif // KERNEL_DSP_H
