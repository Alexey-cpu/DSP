/*!
 * \file
 * \brief   Recursive Fourier
 * \authors A.Tykvinskiy
 * \date    21.01.2022
 * \version 1.0
 *
 * The header declares template recursive discrete Fourier transformation filter
*/

#ifndef RECURSIVE_FOURIER_H
#define RECURSIVE_FOURIER_H


#ifndef __ALG_PLATFORM // identify if the compilation is for ProsoftSystems IDE
#include "cmath"
#endif

/*! \defgroup <RECURSIVE_FIR> ( Recursive FIR )
 *  \brief the module contains reursive FIR filters template classes
    @{
*/

#include "buffer.h"

/*! \brief defines 16-bit integer type */
#ifndef __ix16
#define __ix16 short
#endif

/*! \brief defines 32-bit integer type */
#ifndef __ix32
#define __ix32 int
#endif

/*! \brief defines 64-bit integer type */
#ifndef __ix64
#define __ix64 long long
#endif

/*! \brief defines 16-bit unsigned integer type */
#ifndef __uix16
#define __uix16 unsigned short
#endif

/*! \brief defines 32-bit unsigned integer type */
#ifndef __uix32
#define __uix32 unsigned int
#endif

/*! \brief defines 64-bit unsigned integer type */
#ifndef __uix64
#define __uix64 unsigned long long
#endif

/*! \brief defines 32-bit floating point type */
#ifndef __fx32
#define __fx32 float
#endif

/*! \brief defines 64-bit floating point type */
#ifndef __fx64
#define __fx64 double
#endif

/*! \brief defines Pi */
#ifndef PI0 // PI
#define PI0 3.1415926535897932384626433832795
#endif

/*! \brief defines  2*Pi */
#ifndef PI2 // 2*PI
#define PI2 6.283185307179586476925286766559
#endif

/*! \brief recursive discrete Fourier transformation template class */
template< typename T > class recursive_fourier;

/*!
 *  \brief recursive discrete Fourier transformation 32-bit floating point implementation
 *  \details 32-bit floating point recursive Fourier transformation is not recommended for utilization
 *           with 32-bit floating point inout as it results in a sufficient rounding error that causes
 *           signal parameters computation error !!!
*/
template<> class recursive_fourier<__fx32>
{
    typedef __fx32 __type;
private:
    /*! \brief nominal input signal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief recursive Fourier filter gain */
    __fx64 m_Gain;
    /*! \brief recursive Fourier filter buffer size */
    __fx64 m_Ns;
    /*! \brief computed harmonic number */
    __fx64 m_hnum;
    /*! \brief auxiliary variable */
    __fx64 m_a0;
    /*! \brief auxiliary variable */
    __fx64 m_Ks;
    /*! \brief auxiliary variable */
    __fx64 m_Kc;
    /*! \brief recursive Fourier filter order */
    __ix32 m_order;

    /*! \brief recursive Fourier filter buffer */
    mirror_ring_buffer<__type> m_buffer_sx;

public:
     /*! \brief harmonic real output component */
    __fx64 m_a;
    /*! \brief harmonic imaginary output component */
    __fx64 m_b;
    /*! \brief recursive Fourier filter frequency amplitude response */
    __fx64 m_Km;
    /*! \brief recursive Fourier filter frequency phase response */
    __fx64 m_pH;

     /*! \brief  recursive Fourier filter memory allocation function
      *  \return the function allocates memory for the recursive Fourier filter buffer
    */
    __ix32  allocate() { return ( m_buffer_sx.allocate( m_order + 2 ) ); }

    /*! \brief  recursive Fourier filter memory deallocation function
     *  \return the function deallocates memory of the recursive Fourier buffer
   */
    void deallocate() { m_buffer_sx.deallocate(); }

    /*! \brief  recursive Fourier filter initialization function
     *  \param[Fn  ] - input signal nominal frequency  , Hz
     *  \param[Fs  ] - input signal sampling frequency , hz
     *  \param[hnum] - number of computed harmonic
     *  \return the function initializes recursive Fourier filter
   */
    void init( __type Fn , __type Fs , __ix32 hnum )
    {
        // system variables initialization:
        m_Fn      = Fn;
        m_Fs      = Fs;
        m_Ts      = 1 / m_Fs;
        m_Ns      = ceil(m_Fs / m_Fn);
        m_order   = m_Ns;
        m_Gain    = ( hnum == 0 ) ? ( 1.0 / m_Ns ) : ( 2.0 / m_Ns );
        m_hnum    = hnum;
        m_Ks      = sin( PI2 * (__fx64)m_hnum / m_Ns );
        m_Kc      = cos( PI2 * (__fx64)m_hnum / m_Ns );

        // filter output initialization:
        m_a0 = 0;
        m_a  = 0;
        m_b  = 0;
        m_Km = 0;
        m_pH = 0;
    }

    /*! \brief  recursive Fourier filter default constructor */
    recursive_fourier()
    {
        // system variables initialization:
        m_Fn       = 50;
        m_Fs       = 4000;
        m_Ts       = 1 / m_Fs;
        m_Ns       = ceil(m_Fs / m_Fn);
        m_order    = m_Ns;
        m_Gain     = 2.0 / m_Ns;
        m_hnum     = 1;
        m_Ks      = sin( PI2 * (__fx64)m_hnum / m_Ns );
        m_Kc      = cos( PI2 * (__fx64)m_hnum / m_Ns );

        // filter output initialization:
        m_a0 = 0;
        m_a  = 0;
        m_b  = 0;
        m_Km = 0;
        m_pH = 0;
    };

    /*! \brief  recursive Fourier filter initializing constructor
     *  \param[Fn  ] - input signal nominal frequency  , Hz
     *  \param[Fs  ] - input signal sampling frequency , hz
     *  \param[hnum] - number of computed harmonic
     *  \return This constructor calls initialization function
    */
    recursive_fourier( __type Fn , __type Fs , __ix32 hnum ) { init( Fn , Fs , hnum ); }

    /*! \brief  recursive Fourier filter default destructor */
    ~recursive_fourier(){ deallocate(); }

    /*! \brief  32-bit recursive Fourier filter filtering function
     *  \param[input] - pointer to the nput signal frames
     *  \return The function computes real and imaginary harmonic component
    */
    inline void filt ( __type *input )
    {
        m_buffer_sx.fill_buff( input );
        m_a0 = m_a + ( ( __fx64 )*input - ( __fx64 )m_buffer_sx[ m_order ] ) * m_Gain;
        m_a  = m_a0 * m_Kc - m_b * m_Ks;
        m_b  = m_a0 * m_Ks + m_b * m_Kc;
    }

    /*! \brief  64-bit recursive Fourier filter filtering function
     *  \param[input] - pointer to the nput signal frames
     *  \return The function computes real and imaginary harmonic component
    */
    inline void filt ( __fx64 *input )
    {
        m_buffer_sx.fill_buff( input );
        m_a0 = m_a + ( *input - m_buffer_sx[ m_order ] ) * m_Gain;
        m_a  = m_a0 * m_Kc - m_b * m_Ks;
        m_b  = m_a0 * m_Ks + m_b * m_Kc;
    }

    /*! \brief recursive Fourier filter frequency response computation function
     *  \param[F] - input signal frequency , Hz
     *  \return The function computes phase and amplitude frequency response for the signal having frequency F
    */
    __ix32 FreqCharacteristics( __type F )
    {
        // complex frequency coeffs:
        __type Re1 = 0 , Im1 = 0 , Re2 = 0 , Im2 = 0 , K = 1 / m_Ns;

        // transfer function:
        Re1 = 1 - cos( -PI2 * F * m_Ns * m_Ts );
        Im1 = 0 - sin( -PI2 * F * m_Ns * m_Ts );
        Re2 = 1 - ( cos(-PI2 * F * m_Ts) * m_Kc - sin(-PI2 * F * m_Ts) * m_Ks );
        Im2 = 0 - ( cos(-PI2 * F * m_Ts) * m_Ks + sin(-PI2 * F * m_Ts) * m_Kc );

        // amplitude and phase response:
        if( F == m_Fn * m_hnum )
        {
            m_Km = 1;
            m_pH = 0;
        }
        else
        {
            m_Km = sqrt( Re1 * Re1 + Im1 * Im1 ) / sqrt( Re2 * Re2 + Im2 * Im2 ) * K;
            m_pH = atan2( Im1 , Re1 ) - atan2( Im2 , Re2 );
        }

        return 0;
    }

    /*! \brief  32-bit recursive Fourier filter filtering operator
     *  \param[input] - pointer to the nput signal frames
     *  \return The operator calls the function that computes real and imaginary harmonic component
    */
    inline void operator () ( __type *input ){ filt ( input ); }

    /*! \brief  64-bit recursive Fourier filter filtering operator
     *  \param[input] - pointer to the nput signal frames
     *  \return The operator calls the function that computes real and imaginary harmonic component
    */
    inline void operator () ( __fx64 *input ){ filt ( input ); }
};

/*! \brief recursive discrete Fourier transformation 64-bit floating point implementation */
template<> class recursive_fourier<__fx64>
{
    typedef __fx64 __type;
private:
    /*! \brief nominal input signal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief recursive Fourier filter gain */
    __fx64 m_Gain;
    /*! \brief recursive Fourier filter buffer size */
    __fx64 m_Ns;
    /*! \brief computed harmonic number */
    __fx64 m_hnum;
    /*! \brief auxiliary variable */
    __fx64 m_a0;
    /*! \brief auxiliary variable */
    __fx64 m_Ks;
    /*! \brief auxiliary variable */
    __fx64 m_Kc;
    /*! \brief recursive Fourier filter order */
    __ix32 m_order;

    /*! \brief recursive Fourier filter buffer */
    mirror_ring_buffer<__type> m_buffer_sx;

public:
    /*! \brief harmonic real output component */
   __fx64 m_a;
   /*! \brief harmonic imaginary output component */
   __fx64 m_b;
   /*! \brief recursive Fourier filter frequency amplitude response */
   __fx64 m_Km;
   /*! \brief recursive Fourier filter frequency phase response */
   __fx64 m_pH;

   /*! \brief  recursive Fourier filter memory allocation function
    *  \return the function allocates memory for the recursive Fourier filter buffer
  */
    int  allocate() { return ( m_buffer_sx.allocate( m_order + 2 ) ); }

    /*! \brief  recursive Fourier filter memory deallocation function
     *  \return the function deallocates memory of the recursive Fourier buffer
   */
    void deallocate() { m_buffer_sx.deallocate(); }

    /*! \brief  recursive Fourier filter initialization function
     *  \param[Fn  ] - input signal nominal frequency  , Hz
     *  \param[Fs  ] - input signal sampling frequency , hz
     *  \param[hnum] - number of computed harmonic
     *  \return the function initializes recursive Fourier filter
   */
    void init( __fx64 Fn , __fx64 Fs , __ix32 hnum )
    {
        // system variables initialization:
        m_Fn      = Fn;
        m_Fs      = Fs;
        m_Ts      = 1 / m_Fs;
        m_Ns      = ceil(m_Fs / m_Fn);
        m_order   = m_Ns;
        m_Gain    = ( hnum == 0 ) ? ( 1.0 / m_Ns ) : ( 2.0 / m_Ns );
        m_hnum    = hnum;
        m_Ks      = sin( PI2 * (__type)m_hnum / m_Ns );
        m_Kc      = cos( PI2 * (__type)m_hnum / m_Ns );

        // filter output initialization:
        m_a0 = 0;
        m_a  = 0;
        m_b  = 0;
        m_Km = 0;
        m_pH = 0;
    }

    /*! \brief  recursive Fourier filter default constructor */
    recursive_fourier()
    {
        // system variables initialization:
        m_Fn       = 50;
        m_Fs       = 4000;
        m_Ts       = 1 / m_Fs;
        m_Ns       = ceil(m_Fs / m_Fn);
        m_order    = m_Ns;
        m_Gain     = 2.0 / m_Ns;
        m_hnum     = 1;
        m_Ks       = sin( PI2 * (__fx64)m_hnum / m_Ns );
        m_Kc       = cos( PI2 * (__fx64)m_hnum / m_Ns );

        // filter output initialization:
        m_a0 = 0;
        m_a  = 0;
        m_b  = 0;
        m_Km = 0;
        m_pH = 0;
    };

    /*! \brief  recursive Fourier filter initializing constructor
     *  \param[Fn  ] - input signal nominal frequency  , Hz
     *  \param[Fs  ] - input signal sampling frequency , hz
     *  \param[hnum] - number of computed harmonic
     *  \return This constructor calls initialization function
    */
    recursive_fourier( __fx64 Fn , __fx64 Fs , __ix32 hnum ) { init( Fn , Fs , hnum ); }

    /*! \brief  recursive Fourier filter default destructor */
    ~recursive_fourier(){ deallocate(); }

    /*! \brief  32-bit recursive Fourier filter filtering function
     *  \param[input] - pointer to the nput signal frames
     *  \return The function computes real and imaginary harmonic component
    */
    __ix32 FreqCharacteristics( __fx64 F )
    {
        // complex frequency coeffs:
        __fx64 Re1 = 0 , Im1 = 0 , Re2 = 0 , Im2 = 0 , K = 1 / m_Ns;

        // transfer function:
        Re1 = 1 - cos( -PI2 * F * m_Ns * m_Ts );
        Im1 = 0 - sin( -PI2 * F * m_Ns * m_Ts );
        Re2 = 1 - ( cos(-PI2 * F * m_Ts) * m_Kc - sin(-PI2 * F * m_Ts) * m_Ks );
        Im2 = 0 - ( cos(-PI2 * F * m_Ts) * m_Ks + sin(-PI2 * F * m_Ts) * m_Kc );

        // amplitude and phase response:
        if( F == m_Fn * m_hnum )
        {
            m_Km = 1;
            m_pH = 0;
        }
        else
        {
            m_Km = sqrt( Re1 * Re1 + Im1 * Im1 ) / sqrt( Re2 * Re2 + Im2 * Im2 ) * K;
            m_pH = atan2( Im1 , Re1 ) - atan2( Im2 , Re2 );
        }

        return 0;
    }

    /*! \brief  64-bit recursive Fourier filter filtering function
     *  \param[input] - pointer to the nput signal frames
     *  \return The function computes real and imaginary harmonic component
    */
    inline void filt ( __type *input )
    {
        m_buffer_sx.fill_buff( input );
        m_a0 = m_a + ( *input - m_buffer_sx[ m_order ] ) * m_Gain;
        m_a  = m_a0 * m_Kc - m_b * m_Ks;
        m_b  = m_a0 * m_Ks + m_b * m_Kc;
    }

    /*! \brief  64-bit recursive Fourier filter filtering operator
     *  \param[input] - pointer to the nput signal frames
     *  \return The operator calls the function that computes real and imaginary harmonic component
    */
    inline void operator () ( __type *input ){ filt ( input ); }
};

/*! @} */

// customized data types exclusion to avoid aliases:
#undef __ix16
#undef __ix32
#undef __ix64
#undef __uix16
#undef __uix32
#undef __uix64
#undef __fx32
#undef __fx64

// customized math constants exclusion to avoid aliases:
#undef PI0
#undef PI2
#undef PI_2
#undef PI_4
#undef inv_PI0
#undef inv_PI2
#undef inv_PI_2
#undef inv_PI_4

#endif // RECURSIVE_FOURIER_H
