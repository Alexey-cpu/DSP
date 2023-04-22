#ifndef FILTERS_TSF_H
#define FILTERS_TSF_H

// custom includes
#include "kernel_iir.h"
#include "kernel_dsp.h"
using namespace DSP_KERNEL;

#ifndef __ALG_PLATFORM
#define TSF_FILTERS_DEBUG // debugging is not available if the algorithm is running on a device !!!
#endif

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

/*! \defgroup <TRANSFER_FUNCTIONS> ( Transfer functions )
 *  \ingroup FILTERS
 *  \brief The module contains transfer function model and the range of the simple filters transfer functions
    @{
*/

template<typename __type>
class transfer_function : public model_base
{
    private:

    __type m_Gain = 1;

    delay<__type> m_buff_sx;
    delay<__type> m_buff_sy;

    // transfer function fraction
    tuple_x3<__type**,
            __ix32,
            __ix32 > m_Wz;

    // input fraction to transform
    tuple_x3<__type**,
            __ix32,
            __ix32 > m_Ws;

    //
    tuple_x3<__type**,
            __ix32,
            __ix32 > m_Rz;


    // memory allocation function
    __ix32 allocate()
    {
        #ifdef TSF_FILTERS_DEBUG
        Debugger::Log("filters_tsf","allocate()","Transfer function memory allocation");
        #endif

        // substitute billinear transform fraction into transfer function fraction
        m_Wz = __fraction_numeric_substitution__( &m_Ws.item0[0][0], &m_Ws.item0[1][0], &m_Rz.item0[0][0], &m_Rz.item0[1][0], m_Ws.item2, m_Rz.item2);

        // compute Gain
        m_Gain = (__type)1 / ((__type*)m_Wz.item0[1])[0];
        for( __ix32 j = 0 ; j < m_Wz.item2; j++ )
        {
            ((__type*)m_Wz.item0[1])[j] *= m_Gain;
        }

        m_buff_sx.allocate( m_Wz.item2 + 1 );
        m_buff_sy.allocate( m_Wz.item2 + 1 );

        return 1;
    }


    __ix32 deallocate()
    {
        #ifdef TSF_FILTERS_DEBUG
        Debugger::Log("filters_tsf","deallocate()","Transfer function memory deallocation");
        #endif

        m_Wz = __mfree__(m_Wz);
        m_Ws = __mfree__(m_Ws);
        m_Rz = __mfree__(m_Rz);
        return 1;
    }

    template<typename T> T filt(T *_input)
    {
        return __filt__(_input, &m_Wz.item0[0][0], &m_Wz.item0[1][0], &m_Gain, m_Wz.item2, m_Wz.item2, 1, &m_buff_sx, &m_buff_sy );
    }


    public:

    /*!
     *  \class initialization function
     *  \param[Fs] sampling frequency, Hz
     *  \param[fraction] transfer function fraction
     *  \details Transfer function fraction is represented by the tuple_x4 that contains:
     *           tuple_x3.item0 - pointer to the numerator/denominator of the transfer function
     *           tuple_x3.item1 - number of fraction polynons - always equals 2 for fraction
     *           tuple_x3.item2 - number of numerator/denominator elements ( numerator size always equals to denominator size )
    */
    void init(__fx64 Fs, tuple_x3< __type**, __ix32, __ix32 > Ws, tuple_x3< __type**, __ix32, __ix32 > Rz )
    {
        // check the input
        if( !Ws.item0 || Ws.item1 <= 0 || Ws.item2 <= 0 )
        {
            #ifdef TSF_FILTERS_DEBUG
            Debugger::Log("filters_tsf","init()","Wrong input");
            #endif

            return;
        }

        for( __ix32 i = 0 ; i < Ws.item1; i++ )
        {
            if( !Ws.item0[i] )
            {
                #ifdef TSF_FILTERS_DEBUG
                Debugger::Log("filters_tsf","init()","The input fraction is empty");
                #endif

                return;
            }
        }

        m_Ws = Ws;
        m_Rz = Rz;
        model_base::init(Ws.item2, Fs);
        allocate();
    }

    /*!
     *  \brief frequency responce computation function
     *  \param[F] input frequency, Hz
     *  \details The function computes the filter complex transfer function value for the given frequency
    */
    Complex<__fx64> frequency_response( __fx64 F ) override
    {
        return __freq_resp__( &((__type*)m_Wz.item0[0])[0], &((__type*)m_Wz.item0[1])[0], &m_Gain, m_Wz.item2, m_Wz.item2, 1, m_Fs, F );
    }


    /*!  \class default constructor */
    transfer_function()
    {
        #ifdef TSF_FILTERS_DEBUG
        Debugger::Log("filters_tsf","transfer_function()","Constructor call");
        #endif
    }

    /*!  \class default destructor */
    virtual ~transfer_function()
    {
        #ifdef TSF_FILTERS_DEBUG
        Debugger::Log("filters_tsf","~transfer_function()","Destructor call");
        #endif

        deallocate();
    }

    #ifdef TSF_FILTERS_DEBUG
    void show()
    {
        if( !m_Ws.item0 || !m_Wz.item0 )
        {
            #ifdef TSF_FILTERS_DEBUG
            Debugger::Log("filters_tsf","show()","Transfer function has not been instantiated yet");
            #endif
        }

        cout << "Ws = \n";
        for( __ix32 i = 0 ; i < m_Ws.item1; i++ )
        {
            for( __ix32 j = 0 ; j < m_Ws.item2; j++ )
            {
                cout << ((__type*)m_Ws.item0[i])[j] << "\t";
            }
            cout << "\n";
        }
        cout << "Gain(Ws) = " << 1 << "\n\n";

        cout << "Wz = \n";
        for( __ix32 i = 0 ; i < m_Wz.item1; i++ )
        {
            for( __ix32 j = 0 ; j < m_Wz.item2; j++ )
            {
                cout << ((__type*)m_Wz.item0[i])[j] << "\t";
            }
            cout << "\n";
        }
        cout << "Gain(Wz) = " << m_Gain << "\n\n";

    }
    #endif

    /*!
     *  \brief filtering operator
     *  \param[_input] input pointer
    */
    inline __type operator()(__type *input)
    {
        return filt(input);
    }
};

template<typename __type>
class leadlag final : public model_base
{
    transfer_function<__type> m_transfer_function;
public:

    /*!
     *  \brief initialization function
     *  \param[Fs] sampling frequency, Hz
     *  \param[T1] leadlag transfer function numerator time constant, s
     *  \param[T1] leadlag transfer function denominator time constant, s
     *  \details The function allocates filter resources and computes coefficients
     *           of the leadlag filter:
     *     \f[
     *          Ws = \frac{ s * T1 + 1 }{ s * T2 + 1 }
     *     \f]
    */
    void init(__fx64 Fs, __fx64 T1, __fx64 T2)
    {
        // Ws
        tuple_x3<__type**, __ix32, __ix32> Ws;
        Ws.item1    = 2;
        Ws.item2    = 2;
        Ws.item0    = __alloc__<__type*>(Ws.item1);
        Ws.item0[0] = __alloc__<__type>(Ws.item2);
        Ws.item0[1] = __alloc__<__type>(Ws.item2);

        // initialize numerator
        Ws.item0[0][0] = 1;
        Ws.item0[0][1] = T1;

        // initialize denominator
        Ws.item0[1][0] = 1;
        Ws.item0[1][1] = T2;

        // Rz
        tuple_x3<__type**, __ix32, __ix32> Rz;
        Rz.item1    = 2;
        Rz.item2    = 2;
        Rz.item0    = __alloc__<__type*>(Rz.item1);
        Rz.item0[0] = __alloc__<__type>(Rz.item2);
        Rz.item0[1] = __alloc__<__type>(Rz.item2);

        // initialize numerator
        Rz.item0[0][0] = +1;
        Rz.item0[0][1] = -1;

        // initialize denominator
        Rz.item0[1][0] = +1/Fs;
        Rz.item0[1][1] = +1/Fs;

        m_transfer_function.init(Fs, Ws, Rz);
        model_base::init(2, Fs);
    }

    /*!
     *  \brief frequency responce computation function
     *  \param[F] input frequency, Hz
     *  \details The function computes the filter complex transfer function value for the given frequency
    */
    Complex<__fx64> frequency_response( __fx64 F ) override
    {
        return m_transfer_function.frequency_response(F);
    }

    /*! \brief default constructor */
    leadlag(){}

    /*! \brief default destructor */
   ~leadlag(){}

    /*!
     *  \brief filtering operator
     *  \param[_input] input pointer
    */
    inline __type operator()(__type *input)
    {
        return m_transfer_function(input);
    }
};

template<typename __type>
class aperiodic final : public model_base
{
    transfer_function<__type> m_transfer_function;
public:

    /*!
     *  \brief initialization function
     *  \param[Fs] sampling frequency, Hz
     *  \param[Ta] aperiodic time constant, s
     *  \details The function allocates filter resources and computes coefficients
     *           of the aperiodic filter:
     *     \f[
     *          Ws = \frac{ 1 }{ s * Ta + 1 }
     *     \f]
    */
    void init(__fx64 Fs, __fx64 Ta)
    {
        tuple_x3<__type**, __ix32, __ix32> Ws;
        Ws.item1    = 2;
        Ws.item2    = 2;
        Ws.item0    = __alloc__<__type*>(Ws.item1);
        Ws.item0[0] = __alloc__<__type>(Ws.item2);
        Ws.item0[1] = __alloc__<__type>(Ws.item2);

        // initialize numerator
        Ws.item0[0][0] = 1;
        Ws.item0[0][1] = 0;

        // initialize denominator
        Ws.item0[1][0] = 1;
        Ws.item0[1][1] = Ta;

        // Rz
        tuple_x3<__type**, __ix32, __ix32> Rz;
        Rz.item1    = 2;
        Rz.item2    = 2;
        Rz.item0    = __alloc__<__type*>(Rz.item1);
        Rz.item0[0] = __alloc__<__type>(Rz.item2);
        Rz.item0[1] = __alloc__<__type>(Rz.item2);

        // initialize numerator
        Rz.item0[0][0] = +1;
        Rz.item0[0][1] = -1;

        // initialize denominator
        Rz.item0[1][0] = +1/Fs;
        Rz.item0[1][1] = +1/Fs;

        m_transfer_function.init(Fs, Ws, Rz);
        model_base::init(2, Fs);
    }

    /*!
     *  \brief frequency responce computation function
     *  \param[F] input frequency, Hz
     *  \details The function computes the filter complex transfer function value for the given frequency
    */
    Complex<__fx64> frequency_response( __fx64 F ) override
    {
        return m_transfer_function.frequency_response(F);
    }

    /*! \brief default constructor */
    aperiodic(){}

    /*! \brief default destructor */
   ~aperiodic(){}

    /*!
     *  \brief filtering operator
     *  \param[_input] input pointer
    */
    inline __type operator()(__type *input)
    {
        return m_transfer_function(input);
    }
};

template<typename __type>
class integrator final : public model_base
{
    transfer_function<__type> m_transfer_function;
public:

    /*!
     *  \brief initialization function
     *  \param[Fs] sampling frequency, Hz
     *  \details The function allocates filter resources and computes coefficients
     *           of the integrator filter:
     *     \f[
     *          Ws = \frac{ 1 }{ s }
     *     \f]
    */
    void init(__fx64 Fs)
    {
        tuple_x3<__type**, __ix32, __ix32> Ws;
        Ws.item1    = 2;
        Ws.item2    = 2;
        Ws.item0    = __alloc__<__type*>(Ws.item1);
        Ws.item0[0] = __alloc__<__type>(Ws.item2);
        Ws.item0[1] = __alloc__<__type>(Ws.item2);

        // initialize numerator
        Ws.item0[0][0] = 1;
        Ws.item0[0][1] = 0;

        // initialize denominator
        Ws.item0[1][0] = 0;
        Ws.item0[1][1] = 1;

        // Rz
        tuple_x3<__type**, __ix32, __ix32> Rz;
        Rz.item1    = 2;
        Rz.item2    = 2;
        Rz.item0    = __alloc__<__type*>(Rz.item1);
        Rz.item0[0] = __alloc__<__type>(Rz.item2);
        Rz.item0[1] = __alloc__<__type>(Rz.item2);

        // initialize numerator
        Rz.item0[0][0] = +1;
        Rz.item0[0][1] = -1;

        // initialize denominator
        Rz.item0[1][0] = +1/Fs;
        Rz.item0[1][1] = +1/Fs;

        m_transfer_function.init(Fs, Ws, Rz);
        model_base::init(2, Fs);
    }

    /*!
     *  \brief frequency responce computation function
     *  \param[F] input frequency, Hz
     *  \details The function computes the filter complex transfer function value for the given frequency
    */
    Complex<__fx64> frequency_response( __fx64 F ) override
    {
        return m_transfer_function.frequency_response(F);
    }

    /*! \brief default constructor */
    integrator(){}

    /*! \brief default destructor */
   ~integrator(){}

    /*!
     *  \brief filtering operator
     *  \param[_input] input pointer
    */
    inline __type operator()(__type *input)
    {
        return m_transfer_function(input);
    }
};

template<typename __type>
class differentiator final : public model_base
{
    transfer_function<__type> m_transfer_function;
public:

    /*!
     *  \brief initialization function
     *  \param[Fs] sampling frequency, Hz
     *  \param[Td] differentiator time constant, s
     *  \details The function allocates filter resources and computes coefficients of
     *           differentiator filter:
     *     \f[
     *          Ws = \frac{ s }{ s * Td + 1 }
     *     \f]
    */
    void init(__fx64 Fs, __fx64 Td)
    {
        tuple_x3<__type**, __ix32, __ix32> Ws;
        Ws.item1 = 2;
        Ws.item2 = 2;
        Ws.item0 = __alloc__<__type*>(Ws.item1);
        Ws.item0[0] = __alloc__<__type>(Ws.item2);
        Ws.item0[1] = __alloc__<__type>(Ws.item2);

        // initialize numerator
        Ws.item0[0][0] = 0;
        Ws.item0[0][1] = 1;

        // initialize denominator
        Ws.item0[1][0] = 1;
        Ws.item0[1][1] = Td;

        // Rz
        tuple_x3<__type**, __ix32, __ix32> Rz;
        Rz.item1    = 2;
        Rz.item2    = 2;
        Rz.item0    = __alloc__<__type*>(Rz.item1);
        Rz.item0[0] = __alloc__<__type>(Rz.item2);
        Rz.item0[1] = __alloc__<__type>(Rz.item2);

        // initialize numerator
        Rz.item0[0][0] = +1;
        Rz.item0[0][1] = -1;

        // initialize denominator
        Rz.item0[1][0] = +1/Fs;
        Rz.item0[1][1] = +1/Fs;

        m_transfer_function.init(Fs, Ws, Rz);
        model_base::init(2, Fs);
    }

    /*!
     *  \brief frequency responce computation function
     *  \param[F] input frequency, Hz
     *  \details The function computes the filter complex transfer function value for the given frequency
    */
    Complex<__fx64> frequency_response( __fx64 F ) override
    {
        return m_transfer_function.frequency_response(F);
    }

    /*! \brief default constructor */
    differentiator(){}

    /*! \brief default destructor */
   ~differentiator(){}

    /*!
     *  \brief filtering operator
     *  \param[_input] input pointer
    */
    inline __type operator()(__type *input)
    {
        return m_transfer_function(input);
    }
};

template<typename __type>
class filter_2nd_order final : public model_base
{
    transfer_function<__type> m_transfer_function;
public:

    /*!
     *  \brief initialization function
     *  \param[Fs] sampling frequency, Hz
     *  \param[Fc] cut-off frequency, Hz
     *  \param[Kd] damping ratio
     *  \param[type] filter type ( see kernel_dsp.h for the further details )
     *  \details The function allocates filter resources and computes coefficients
     *           of the simple second order lowpass/highpass/bandpass/bandstop filter depending upon the type.
    */
    void init(__fx64 Fs, __fx64 Fc, __fx64 Kd, filter_type type)
    {
        tuple_x3<__type**, __ix32, __ix32> Ws;
        Ws.item1    = 2;
        Ws.item2    = 3;
        Ws.item0    = __alloc__<__type*>(Ws.item1);
        Ws.item0[0] = __alloc__<__type>(Ws.item2);
        Ws.item0[1] = __alloc__<__type>(Ws.item2);

        __fx64 omega = 2 * PI0 * Fc;
        omega = tan( omega / Fs / 2.0 );

        switch (type)
        {
            case filter_type::lowpass:

                // initialize numerator
                Ws.item0[0][0] = omega * omega;
                Ws.item0[0][1] = 0;
                Ws.item0[0][2] = 0;

                // initialize denominator
                Ws.item0[1][0] = omega * omega;
                Ws.item0[1][1] = 2 * omega * Kd;
                Ws.item0[1][2] = 1;

            break;

            case filter_type::highpass:

                // initialize numerator
                Ws.item0[0][0] = 0;
                Ws.item0[0][1] = 0;
                Ws.item0[0][2] = 1;

                // initialize denominator
                Ws.item0[1][0] = omega * omega;
                Ws.item0[1][1] = 2 * omega * Kd;
                Ws.item0[1][2] = 1;

            break;

            case filter_type::bandpass:

                // initialize numerator
                Ws.item0[0][0] = 0;
                Ws.item0[0][1] = 2 * omega * Kd;
                Ws.item0[0][2] = 0;

                // initialize denominator
                Ws.item0[1][0] = omega * omega;
                Ws.item0[1][1] = 2 * omega * Kd;
                Ws.item0[1][2] = 1;

            break;

            case filter_type::bandstop:

                // initialize numerator
                Ws.item0[0][0] = omega * omega;
                Ws.item0[0][1] = 0;
                Ws.item0[0][2] = 1;

                // initialize denominator
                Ws.item0[1][0] = omega * omega;
                Ws.item0[1][1] = 2 * omega * Kd;
                Ws.item0[1][2] = 1;

            break;

            default:

                // initialize numerator
                Ws.item0[0][0] = omega * omega;
                Ws.item0[0][1] = 0;
                Ws.item0[0][2] = 0;

                // initialize denominator
                Ws.item0[1][0] = omega * omega;
                Ws.item0[1][1] = 2 * omega * Kd;
                Ws.item0[1][2] = 1;

            break;
        }

        // Rz
        tuple_x3<__type**, __ix32, __ix32> Rz;
        Rz.item1    = 2;
        Rz.item2    = 2;
        Rz.item0    = __alloc__<__type*>(Rz.item1);
        Rz.item0[0] = __alloc__<__type>(Rz.item2);
        Rz.item0[1] = __alloc__<__type>(Rz.item2);

        // initialize numerator
        Rz.item0[0][0] = +1;
        Rz.item0[0][1] = -1;

        // initialize denominator
        Rz.item0[1][0] = +1;
        Rz.item0[1][1] = +1;

        m_transfer_function.init(Fs, Ws, Rz);
        model_base::init(2, Fs);
    }

    /*!
     *  \brief frequency responce computation function
     *  \param[F] input frequency, Hz
     *  \details The function computes the filter complex transfer function value for the given frequency
    */
    Complex<__fx64> frequency_response( __fx64 F ) override
    {
        return m_transfer_function.frequency_response(F);
    }

    /*! \brief default constructor */
    filter_2nd_order(){}

    /*! \brief default destructor */
   ~filter_2nd_order(){}

    /*!
     *  \brief filtering operator
     *  \param[_input] input pointer
    */
    inline __type operator()(__type *input)
    {
        return m_transfer_function(input);
    }

    void show()
    {
        m_transfer_function.show();
    }
};

/*! @} */

// undefining the custom macro to avoid aliases
#undef __fx32
#undef __fx64
#undef __fxx64
#undef __ix32
#undef PI0
#undef PI2

#endif // FILTERS_TSF_H
