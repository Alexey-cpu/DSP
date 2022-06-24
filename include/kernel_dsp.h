/*!
 *
 * brief   DSP Kernel
 * authors A.Tykvinskiy
 * date    21.12.2021
 * version 1.0
 *
 * The header declares DSP kernel classes, data structures and functions
*/

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
#include "fcomplex.h"
#include "special_functions.h"

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
 *  \brief the module contains different filters implementations description
    @{
*/

/*! @} */

namespace DSP_KERNEL
{
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
     *  \brief base filter class
     *  \details Defines the building base for the filtres creation.
     *           Use inheritance mechnism to create the deriving filter class
    */
    class filter_abstract
    {
    protected:

        // system variables
        __ix32 m_order;
        __fx64 m_Fs;
        __fx64 m_Ts;

        // default constructor
        filter_abstract(){}

        // virtual destructor
        virtual ~filter_abstract(){};

    public:

        /*!
         *  \brief memory allocation function
         *  \details the functions must allocate the filter resources.
         *           Do not call this function from the derived classes
         *           constructors. The functions is
         *           supposed to be called explicitly by user.
        */
        virtual __ix32 allocate()   = 0;

        /*!
         *  \brief memory deallocation function
         *  \details the functions must clear the filter resources.
         *           Do not call this function from the derived classes
         *           destructors. The function is
         *           supposed to be called explicitly by user.
        */
        virtual __ix32 deallocate() = 0;

        /*!
         *  \brief filter frequency responce computation function
         *  \details The function must compute filter frequency responce
         *           and return the complex transfer function value
         *           for the given frequency F
        */
        virtual fcomplex<__fx64> frequency_response( __fx64 F ) = 0;
    };

    /*!
     *  \brief filter data structure
     *  \details contains all the filter information
     *           used for the analysis and modelling
    */
    template<typename __type>
    struct filter_data
    {
        filter_type         type  = lowpass; ///< filter type
        fcomplex< __fx64 > *poles = nullptr; ///< lowpass normalized analogue prototype complex conjugate poles pairs
        fcomplex< __fx64 > *zeros = nullptr; ///< lowpass normalized analogue prototype complex conjugate zeros pairs
        fcomplex< __fx64 > *ratio = nullptr; ///< lowpass normalized analogue prototype zero frequency gains vector
        __type *cfnum             = nullptr; ///< IIR filter numerator quadratic sections coefficients matrix
        __type *cfden             = nullptr; ///< IIR filter denominator quadratic sections coefficients matrix
        __type *gains             = nullptr; ///< IIR filter quadratic sections gains vector
        __ix32 L                  = -1;      ///< IIR filter complex conjugate poles/zeros pairs number
        __ix32 R                  = -1;      ///< IIR filter real odd pole existance flag
        __ix32 N                  = -1;      ///< IIR filter quadratic sections number
        __ix32 Nx                 = -1;      ///< IIR single section numerator order
        __ix32 Ny                 = -1;      ///< IIR single section denominator order
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
        output.poles   = nullptr;
        output.zeros   = nullptr;
        output.ratio   = nullptr;
        output.cfnum = nullptr;
        output.cfden = nullptr;
        output.gains = nullptr;
        output.L     = -1;
        output.R     = -1;
        output.N     = -1;
        output.Nx    = -1;
        output.Ny    = -1;
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
        return data;
    }
}

/*! @} */

#undef __fx32
#undef __fx64
#undef __fxx64
#undef __ix32
#undef PI0
#undef PI2

#endif // KERNEL_DSP_H
