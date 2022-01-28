/*!
 * \file
 * \brief   Recursive root mean square
 * \authors A.Tykvinskiy
 * \date    28.01.2022
 * \version 1.0
 *
 * The header declares template recursive root mean square filter
*/

#ifndef RECURSIVE_RMS_H
#define RECURSIVE_RMS_H

#ifndef __ALG_PLATFORM // identify if the compilation is for ProsoftSystems IDE
#include "cmath"
#endif

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

/*! \defgroup <RECURSIVE_FIR> ( Recursive FIR )
 *  \brief the module contains reursive FIR filters template classes
    @{
*/

/*! \brief recursive root mean square template class */
template< typename T > class recursive_rms;


/*!
 *  \brief recursive root mean square template class 32-bit floating point implementation
 *  \details 32-bit floating point recursive root mean square is not recommended for utilization
 *           with 32-bit floating point inout as it results in a sufficient rounding error that causes
 *           signal parameters computation error !!!
*/
template<> class recursive_rms<__fx32>
{
    typedef __fx32 __type;
private:
    /*! \brief nominal input signal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief recursive root mean square filter gain */
    __fx64 m_Gain;
     /*! \brief recursive root mean square filter buffer size */
    __fx64 m_Ns;
     /*! \brief auxiliary variable */
    __fx64 m_auxv;
    /*! \brief auxiliary variable */
    __fx64 m_y;
    /*! \brief recursive root mean square filter order */
    __ix32 m_order;

    /*! \brief recursive root mean square filter buffer */
    mirror_ring_buffer<__type> m_buffer_sx;

public:
    /*! \brief recursive root mean square filter output */
    __fx64 m_out;
    /*! \brief recursive root mean square filter frequency amplitude response */
    __fx64 m_Km ;
    /*! \brief recursive root mean square filter frequency amplitude response */
    __fx64 m_pH ;

    /*! \brief  recursive root mean square filter memory allocation function
     *  \return the function allocates memory for the recursive rott mean square filter buffer
   */
    __ix32 allocate() { return ( m_buffer_sx.allocate( m_order + 2 ) ); }

    /*! \brief  recursive root mean square filter memory deallocation function
     *  \return the function deallocates memory for the recursive root mean square filter buffer
   */
    void deallocate() { m_buffer_sx.deallocate(); }

    /*! \brief  recursive root mean square filter initialization function
     *  \param[Fn  ]  - input signal nominal frequency  , Hz
     *  \param[Fs  ]  - input signal sampling frequency , hz
     *  \param[order] - recursive mean filter order
     *  \return the function initializes recursive root mean square filter
   */
    void init( __fx64 Fn , __fx64 Fs , __ix32 order )
    {
        m_Fs      = Fs;
        m_Fn      = Fn;
        m_order   = order;
        m_Ns      = m_order;
        m_Ts      = 1 / m_Fs;
        m_Gain    = 1 / m_Ns;
		m_y		  = 0;
		m_out     = 0;
    }

    /*! \brief  recursive root mean square filter default constructor */
    recursive_rms()
    {
        m_Fs      = 4000;
        m_Fn      = 50;
        m_order   = m_Fs / m_Fn;
        m_Ns      = m_order;
        m_Ts      = 1 / m_Fs;
        m_Gain    = 1 / m_Ns;
		m_y		  = 0;
		m_out     = 0;
    };

    /*! \brief  recursive root mean square filter initializing constructor
     *  \param[Fn  ]  - input signal nominal frequency  , Hz
     *  \param[Fs  ]  - input signal sampling frequency , hz
     *  \param[order] - recursive mean filter order
     *  \return This constructor calls initialization function
    */
    recursive_rms( __fx64 Fn , __fx64 Fs , __ix32 order ) { init( Fn , Fs , order ); }

    /*! \brief  recursive root mean square filter default destructor */
    ~recursive_rms(){ deallocate(); }

    /*! \brief recursive root mean square filter frequency response computation function
     *  \param[F] - input signal frequency , Hz
     *  \return The function computes phase and amplitude frequency response for the signal having frequency F
    */
    __ix32 FreqCharacteristics( __fx64 F )
    {
        __type Re_nom = 0 , Im_nom = 0 , Re_den = 0 , Im_den = 0 , Re = 0 , Im = 0;
        Re_nom = 1 - cos(-PI2 * F * m_Ns * m_Ts);
        Im_nom = 0 - sin(-PI2 * F * m_Ns * m_Ts);
        Re_den = 1 - cos(-PI2 * F * m_Ts);
        Im_den = 0 - sin(-PI2 * F * m_Ts);
        Re   = (Re_nom * Re_den + Im_nom * Im_den) / (Re_den * Re_den + Im_den * Im_den);
        Im   = (Re_den * Im_nom - Re_nom * Im_den) / (Re_den * Re_den + Im_den * Im_den);
        m_Km = m_Gain * sqrt(Re * Re + Im * Im);
        m_pH = atan2(Im , Re);
        return 0;
    }


    /*! \brief  32-bit recursive root mean square filter filtering function
     *  \param[input] - pointer to the nput signal frames
     *  \return The function computes recursive mean fiter output
    */
    inline void filt ( __type *input )
    {
        m_auxv = (__fx64)( *input ) * (__fx64)( *input );
        m_buffer_sx.fill_buff( &m_auxv );
        m_y = m_Gain * ( m_auxv - (__fx64)m_buffer_sx[ m_order ] ) + m_y;
        m_out =  ( m_y <= 0 ) ? 0 : sqrtf( m_y );
    }

    /*! \brief  64-bit recursive root mean square filter filtering function
     *  \param[input] - pointer to the nput signal frames
     *  \return The function computes recursive mean fiter output
    */
    inline void filt ( __fx64 *input )
    {
        m_auxv = (__fx64)( *input ) * (__fx64)( *input );
        m_buffer_sx.fill_buff( &m_auxv );
        m_y = m_Gain * ( m_auxv - (__fx64)m_buffer_sx[ m_order ] ) + m_y;
        m_out =  ( m_y <= 0 ) ? 0 : sqrtf( m_y );
    }

    /*! \brief  32-bit recursive root mean square filter filtering operator
     *  \param[input] - pointer to the nput signal frames
     *  \return The operator calls the function that computes recursive mean fiter output
    */
    inline void operator () ( __type *input ){ filt ( input ); }

    /*! \brief  64-bit recursive root mean square filter filtering operator
     *  \param[input] - pointer to the nput signal frames
     *  \return The operator calls the function that computes recursive mean fiter output
    */
    inline void operator () ( __fx64 *input ){ filt ( input ); }
};

/*! \brief recursive root mean square template class 64-bit floating point implementation */
template<> class recursive_rms<__fx64>
{
    typedef __fx64 __type;
private:
    /*! \brief nominal input signal frequency , Hz */
    __fx64 m_Fn;
    /*! \brief input signal sampling frequency , Hz */
    __fx64 m_Fs;
    /*! \brief input signal sampling period , s */
    __fx64 m_Ts;
    /*! \brief recursive root mean square filter gain */
    __fx64 m_Gain;
     /*! \brief recursive root mean square filter buffer size */
    __fx64 m_Ns;
     /*! \brief auxiliary variable */
    __fx64 m_auxv;
    /*! \brief auxiliary variable */
    __fx64 m_y;
    /*! \brief recursive root mean square filter order */
    __ix32 m_order;

    /*! \brief recursive root mean square filter buffer */
    mirror_ring_buffer<__type> m_buffer_sx;

public:
    /*! \brief recursive root mean square filter output */
    __fx64 m_out;
    /*! \brief recursive root mean square filter frequency amplitude response */
    __fx64 m_Km ;
    /*! \brief recursive root mean square filter frequency amplitude response */
    __fx64 m_pH ;

    /*! \brief  recursive root mean square filter memory allocation function
     *  \return the function allocates memory for the recursive rott mean square filter buffer
   */
    __ix32 allocate() { return ( m_buffer_sx.allocate( m_order + 2 ) ); }

    /*! \brief  recursive root mean square filter memory deallocation function
     *  \return the function deallocates memory for the recursive root mean square filter buffer
   */
    void deallocate() { m_buffer_sx.deallocate(); }

    /*! \brief  recursive root mean square filter initialization function
     *  \param[Fn  ]  - input signal nominal frequency  , Hz
     *  \param[Fs  ]  - input signal sampling frequency , hz
     *  \param[order] - recursive mean filter order
     *  \return the function initializes recursive root mean square filter
   */
    void init( __fx64 Fn , __fx64 Fs , __ix32 order )
    {
        m_Fs      = Fs;
        m_Fn      = Fn;
        m_order   = order;
        m_Ns      = m_order;
        m_Ts      = 1 / m_Fs;
        m_Gain    = 1 / m_Ns;
        m_y		  = 0;
        m_out     = 0;
    }

    /*! \brief  recursive root mean square filter default constructor */
    recursive_rms()
    {
        m_Fs      = 4000;
        m_Fn      = 50;
        m_order   = m_Fs / m_Fn;
        m_Ns      = m_order;
        m_Ts      = 1 / m_Fs;
        m_Gain    = 1 / m_Ns;
        m_y		  = 0;
        m_out     = 0;
    };

    /*! \brief  recursive root mean square filter initializing constructor
     *  \param[Fn  ]  - input signal nominal frequency  , Hz
     *  \param[Fs  ]  - input signal sampling frequency , hz
     *  \param[order] - recursive mean filter order
     *  \return This constructor calls initialization function
    */
    recursive_rms( __fx64 Fn , __fx64 Fs , __ix32 order ) { init( Fn , Fs , order ); }

    /*! \brief  recursive root mean square filter default destructor */
    ~recursive_rms(){ deallocate(); }

    /*! \brief recursive root mean square filter frequency response computation function
     *  \param[F] - input signal frequency , Hz
     *  \return The function computes phase and amplitude frequency response for the signal having frequency F
    */
    __ix32 FreqCharacteristics( __fx64 F )
    {
        __type Re_nom = 0 , Im_nom = 0 , Re_den = 0 , Im_den = 0 , Re = 0 , Im = 0;
        Re_nom = 1 - cos(-PI2 * F * m_Ns * m_Ts);
        Im_nom = 0 - sin(-PI2 * F * m_Ns * m_Ts);
        Re_den = 1 - cos(-PI2 * F * m_Ts);
        Im_den = 0 - sin(-PI2 * F * m_Ts);
        Re   = (Re_nom * Re_den + Im_nom * Im_den) / (Re_den * Re_den + Im_den * Im_den);
        Im   = (Re_den * Im_nom - Re_nom * Im_den) / (Re_den * Re_den + Im_den * Im_den);
        m_Km = m_Gain * sqrt(Re * Re + Im * Im);
        m_pH = atan2(Im , Re);
        return 0;
    }

    /*! \brief  64-bit recursive root mean square filter filtering function
     *  \param[input] - pointer to the nput signal frames
     *  \return The function computes recursive mean fiter output
    */
    inline void filt ( __type *input )
    {
        m_auxv = ( *input ) * ( *input );
        m_buffer_sx.fill_buff( &m_auxv );
        m_y = m_Gain * ( m_auxv - m_buffer_sx[ m_order ] ) + m_y;
        m_out =  ( m_y <= 0 ) ? 0 : sqrtf( m_y );
    }

    /*! \brief  64-bit recursive root mean square filter filtering operator
     *  \param[input] - pointer to the nput signal frames
     *  \return The operator calls the function that computes recursive mean fiter output
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

#endif // RECURSIVE_RMS_H
