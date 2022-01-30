/*!
 * \file
 * \brief   Template forward / backward Clarke transform class
 * \authors A.Tykvinskiy
 * \date    21.12.2021
 * \version 1.0
 *
 * The header declares ring buffer and mirror ring buffer template classes
*/

/*! \defgroup <THREE_PHASE_TRANSFORMATIONS> ( Three phase ortohonal transformations )
 *  \brief the module describes complex numbers functions
    @{
*/

#ifndef СLARKE_FILTER_H
#define СLARKE_FILTER_H

#include "math.h"

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

/*! \brief defines 32-bit floating point type */
#ifndef __fx32
#define __fx32 float
#endif

/*! \brief defines 64-bit floating point type */
#ifndef __fx64
#define __fx64 double
#endif

/*! \brief defines pi */
#ifndef PI0
#define PI0 3.1415926535897932384626433832795
#endif

/*! \brief defines 2*pi */
#ifndef PI2
#define PI2 6.283185307179586476925286766559
#endif

/*! \brief defines pi / 2 */
#ifndef PI_2
#define PI_2 1.5707963267948966192313216916398
#endif

/*! \brief defines pi / 4 */
#ifndef PI_4
#define PI_4 0.78539816339744830961566084581988
#endif

/*! \brief Clarke's forward / backward transformation template class */
template< typename T > class Clarke_filter;

/*! \brief Parkes's forward / backward transformation template class */
template< typename T > class Park_filter;

/*! \brief Clarke's forward / backward 32-bit filter */
template<> class Clarke_filter< __fx32 >
{
    typedef __fx32 __type;
public:
    /*!
     *  \brief Clarke's forward / backward 32-bit filter
     *  \param[m_a] alpha component of Clarke's transformation
     *  \param[m_b] betta component of Clarke's transformation
     *  \param[m_c] zero component of Clarke's transformation
    */
    __type m_a , m_b , m_c;

    /*!
     *  \brief Clarke's toward 32-bit transformation function
     *  \param[ A ] phase A input signal
     *  \param[ B ] phase B input signal
     *  \param[ C ] phase C input signal
     *  \return the function fills Clarke's filter template class filelds as follows:
     *          \f[
     *
     *              \begin{pmatrix}
     *              \alpha
     *              \\
     *              \beta
     *              \\
     *              0
     *              \end{pmatrix} = \sqrt{ \frac{2}{3} } *
     *
     *              \begin{pmatrix}
     *
     *              1 & -\frac{1}{2} & -\frac{1}{2}
     *              \\
     *              0 &  \frac{ \sqrt{3} }{ 2 } & -\frac{ \sqrt{3} }{ 2 }
     *              \\
     *              \frac{1}{2} & \frac{1}{2} & \frac{1}{2}
     *
     *              \end{pmatrix} *
     *
     *              \begin{pmatrix}
     *              a
     *              \\
     *              b
     *              \\
     *              c
     *              \end{pmatrix}
     *
     *          \f]
    */
    inline void toward_transformation( __type *A , __type *B , __type *C )
    {
        m_a = (*A) * 0.6666666666666666666666666666667 - (*B) * 0.3333333333333333333333333333333 - (*C) * 0.3333333333333333333333333333333;
        m_b = (*B) * 0.577350269189625764509148780502  - (*C) * 0.577350269189625764509148780502;
        m_c = (*A) * 0.3333333333333333333333333333333 + (*B) * 0.3333333333333333333333333333333 + (*C) * 0.3333333333333333333333333333333;
    }

    /*!
     *  \brief Clarke's toward 64-bit transformation function
     *  \param[ A ] phase A input signal
     *  \param[ B ] phase B input signal
     *  \param[ C ] phase C input signal
     *  \return the function fills Clarke's filter template class filelds as follows:
     *          \f[
     *
     *              \begin{pmatrix}
     *              \alpha
     *              \\
     *              \beta
     *              \\
     *              0
     *              \end{pmatrix} = \sqrt{ \frac{2}{3} } *
     *
     *              \begin{pmatrix}
     *
     *              1 & -\frac{1}{2} & -\frac{1}{2}
     *              \\
     *              0 &  \frac{ \sqrt{3} }{ 2 } & -\frac{ \sqrt{3} }{ 2 }
     *              \\
     *              \frac{1}{2} & \frac{1}{2} & \frac{1}{2}
     *
     *              \end{pmatrix} *
     *
     *              \begin{pmatrix}
     *              a
     *              \\
     *              b
     *              \\
     *              c
     *              \end{pmatrix}
     *
     *          \f]
    */
    inline void toward_transformation( __fx64 *A , __fx64 *B , __fx64 *C )
    {
        m_a = (*A) * 0.6666666666666666666666666666667 - (*B) * 0.3333333333333333333333333333333 - (*C) * 0.3333333333333333333333333333333;
        m_b = (*B) * 0.577350269189625764509148780502  - (*C) * 0.577350269189625764509148780502;
        m_c = (*A) * 0.3333333333333333333333333333333 + (*B) * 0.3333333333333333333333333333333 + (*C) * 0.3333333333333333333333333333333;
    }

    /*!
     *  \brief Clarke's backward 32-bit transformation function
     *  \param[ A ] pointer to alpha component
     *  \param[ B ] pointer to beta component
     *  \param[ C ] pointer to 0 component
     *  \return the function fills Clarke's filter template class filelds as follows:
     *          \f[
     *
     *              \begin{pmatrix}
     *              a
     *              \\
     *              b
     *              \\
     *              c
     *              \end{pmatrix} = \sqrt{ \frac{2}{3} } *
     *
     *              \begin{pmatrix}
     *
     *              1 & 0 & \frac{ 1 }{ \sqrt{2} }
     *              \\
     *              \frac{ -1 }{ 2 } &  \frac{ \sqrt{3} }{ 2 } & \frac{ 1 }{ \sqrt{ 2 } }
     *              \\
     *              -\frac{1}{2} & -\frac{ \sqrt{3} }{ 2 } & \frac{ 1 }{ \sqrt{2} }
     *
     *              \end{pmatrix} *
     *
     *              \begin{pmatrix}
     *              \alpha
     *              \\
     *              \beta
     *              \\
     *              0
     *              \end{pmatrix}
     *
     *          \f]
    */
    inline void backward_transformation( __type *A , __type *B , __type *C )
    {
        m_a = (*A) + (*C);
        m_b = -(*A) * 0.5 + (*B) * 0.866025403784439 + (*C);
        m_c = -(*A) * 0.5 - (*B) * 0.866025403784439 + (*C);
    }

    /*!
     *  \brief Clarke's backward 32-bit transformation function
     *  \param[ A ] pointer to alpha component
     *  \param[ B ] pointer to beta component
     *  \param[ C ] pointer to 0 component
     *  \return the function fills Clarke's filter template class filelds as follows:
     *          \f[
     *
     *              \begin{pmatrix}
     *              a
     *              \\
     *              b
     *              \\
     *              c
     *              \end{pmatrix} = \sqrt{ \frac{2}{3} } *
     *
     *              \begin{pmatrix}
     *
     *              1 & 0 & \frac{ 1 }{ \sqrt{2} }
     *              \\
     *              \frac{ -1 }{ 2 } &  \frac{ \sqrt{3} }{ 2 } & \frac{ 1 }{ \sqrt{ 2 } }
     *              \\
     *              -\frac{1}{2} & -\frac{ \sqrt{3} }{ 2 } & \frac{ 1 }{ \sqrt{2} }
     *
     *              \end{pmatrix} *
     *
     *              \begin{pmatrix}
     *              \alpha
     *              \\
     *              \beta
     *              \\
     *              0
     *              \end{pmatrix}
     *
     *          \f]
    */
    inline void backward_transformation( __fx64 *A , __fx64 *B , __fx64 *C )
    {
        m_a = (*A) + (*C);
        m_b = -(*A) * 0.5 + (*B) * 0.866025403784439 + (*C);
        m_c = -(*A) * 0.5 - (*B) * 0.866025403784439 + (*C);
    }


    /*!
      *\brief Clarke's backward / toward transformation template class 32-bit () operator
      *\param[A]    pointer to the phase A signal or to alpha component
      *\param[B]    pointer to the phase B signal or to beta component
      *\param[C]    pointer to the phase C signal or to 0 component
      *\param[type] Clarke's transformation type flag ( 0 - toward , 1 - backward )
    */
    inline void operator () ( __type *A , __type *B , __type *C , __ix32 type )
    {
        if( !type ) toward_transformation  ( A , B , C );
        else        backward_transformation( A , B , C );
    }

    /*!
      *\brief Clarke's backward / toward transformation template class 64-bit () operator
      *\param[A]    pointer to the phase A signal or to alpha component
      *\param[B]    pointer to the phase B signal or to beta component
      *\param[C]    pointer to the phase C signal or to 0 component
      *\param[type] Clarke's transformation type flag ( 0 - toward , 1 - backward )
    */
    inline void operator () ( __fx64 *A , __fx64 *B , __fx64 *C  , __ix32 type )
    {
        if( !type ) toward_transformation  ( A , B , C );
        else        backward_transformation( A , B , C );
    }
};

/*! \brief Parkes's forward / backward 32-bit filter */
template<> class Park_filter< __fx32 >
{
    typedef __fx32 __type;
private:
    __type m_cnt , m_k1 , m_k2;
public:
    /*!
     *  \brief Parkes's forward / backward 32-bit filter
     *  \param[m_a] alpha component of Clarke's transformation
     *  \param[m_b] betta component of Clarke's transformation
     *  \param[m_c] zero component of Clarke's transformation
    */
    __type m_a , m_b , m_c;

    /*! \brief default constructor */
    Park_filter() { m_a = m_b = m_c = 0; m_cnt = 0; m_k1 = 2.0 / 3.0; m_k2 = 1.0 / 3.0; }

    /*! \brief default destructor */
    ~Park_filter(){}

    /*!
     *  \brief toward transformation
    */
    inline void toward_transformation( __type *A, __type *B, __type *C, __fx64 Fn , __fx64 Fs, bool a_axis_alignment = 1 )
    {
        // angles computation:
        __type phi0 = PI2 * Fn * (__type)m_cnt / Fs;
        __type phi1 = PI2 * Fn * (__type)m_cnt / Fs - 2 * PI0 / 3;
        __type phi2 = PI2 * Fn * (__type)m_cnt / Fs + 2 * PI0 / 3;

        // Parke's transformation components computation:
        if ( a_axis_alignment )
        {
            m_a = m_k1 * ( (*A) * cos( phi0 ) + (*B) * cos( phi1 ) + (*C) * cos( phi2 ) );
            m_b = m_k1 * (-(*A) * sin( phi0 ) - (*B) * sin( phi1 ) - (*C) * sin( phi2 ) );
            m_c = m_k2 * ( (*A) + (*B) + (*C) );
        }
        else
        {
            m_a = m_k1 * ( (*A) * sin( phi0 ) + (*B) * sin( phi1 ) + (*C) * sin( phi2 ) );
            m_b = m_k1 * (-(*A) * cos( phi0 ) - (*B) * cos( phi1 ) - (*C) * cos( phi2 ) );
            m_c = m_k2 * ( (*A) + (*B) + (*C) );
        }

        // reference frame counter update:
        if (m_cnt < ceil( Fs / Fn ) ) m_cnt++;
        else m_cnt = 0;
    }

    /*!
     *  \brief backward transformation
    */
    inline void backward_transformation( __type *D , __type *Q , __type *Z , __fx64 Fn , __fx64 Fs , bool a_axis_alignment = 1 )
    {
        // angles computation:
        __type phi0 = PI2 * Fn * (__type)m_cnt / Fs;
        __type phi1 = PI2 * Fn * (__type)m_cnt / Fs - 2 * PI0 / 3;
        __type phi2 = PI2 * Fn * (__type)m_cnt / Fs + 2 * PI0 / 3;

        if ( a_axis_alignment )
        {
            m_a = ( (*D) * cos( phi0 ) - (*Q) * sin( phi0 ) + (*Z) );
            m_b = ( (*D) * cos( phi1 ) - (*Q) * sin( phi1 ) + (*Z) );
            m_c = ( (*D) * cos( phi2 ) - (*Q) * sin( phi2 ) + (*Z) );
        }
        else
        {
            m_a = ( (*D) * sin( phi0 ) + (*Q) * cos( phi0 ) + (*Z) );
            m_b = ( (*D) * sin( phi1 ) + (*Q) * cos( phi1 ) + (*Z) );
            m_c = ( (*D) * sin( phi2 ) + (*Q) * cos( phi2 ) + (*Z) );
        }

        // reference frame counter update:
        if (m_cnt < ceil( Fs / Fn ) ) m_cnt++;
        else m_cnt = 0;
    }

    inline void operator () ( __type *A, __type *B, __type *C, __fx64 Fn , __fx64 Fs, bool a_axis_alignment , bool type = 0 )
    {
        if( !type ) toward_transformation  ( A , B ,  C,  Fn ,  Fs, a_axis_alignment );
        else backward_transformation( A , B ,  C,  Fn ,  Fs, a_axis_alignment );
    }

};

/*! \brief Clarke's forward / backward 64-bit filter */
template<> class Clarke_filter< __fx64 >
{
    typedef __fx64 __type;
public:
    /*!
     *  \brief Clarke's forward / backward 32-bit filter
     *  \param[m_a] alpha component of Clarke's transformation
     *  \param[m_b] betta component of Clarke's transformation
     *  \param[m_c] zero component of Clarke's transformation
    */
    __type m_a , m_b , m_c;

    /*!
     *  \brief Clarke's toward 32-bit transformation function
     *  \param[ A ] phase A input signal
     *  \param[ B ] phase B input signal
     *  \param[ C ] phase C input signal
     *  \return the function fills Clarke's filter template class filelds as follows:
     *          \f[
     *
     *              \begin{pmatrix}
     *              \alpha
     *              \\
     *              \beta
     *              \\
     *              0
     *              \end{pmatrix} = \sqrt{ \frac{2}{3} } *
     *
     *              \begin{pmatrix}
     *
     *              1 & -\frac{1}{2} & -\frac{1}{2}
     *              \\
     *              0 &  \frac{ \sqrt{3} }{ 2 } & -\frac{ \sqrt{3} }{ 2 }
     *              \\
     *              \frac{1}{2} & \frac{1}{2} & \frac{1}{2}
     *
     *              \end{pmatrix} *
     *
     *              \begin{pmatrix}
     *              a
     *              \\
     *              b
     *              \\
     *              c
     *              \end{pmatrix}
     *
     *          \f]
    */
    inline void toward_transformation( __type *A , __type *B , __type *C )
    {
        m_a = (*A) * 0.6666666666666666666666666666667 - (*B) * 0.3333333333333333333333333333333 - (*C) * 0.3333333333333333333333333333333;
        m_b = (*B) * 0.577350269189625764509148780502  - (*C) * 0.577350269189625764509148780502;
        m_c = (*A) * 0.3333333333333333333333333333333 + (*B) * 0.3333333333333333333333333333333 + (*C) * 0.3333333333333333333333333333333;
    }

    /*!
     *  \brief Clarke's backward 32-bit transformation function
     *  \param[ A ] pointer to alpha component
     *  \param[ B ] pointer to beta component
     *  \param[ C ] pointer to 0 component
     *  \return the function fills Clarke's filter template class filelds as follows:
     *          \f[
     *
     *              \begin{pmatrix}
     *              a
     *              \\
     *              b
     *              \\
     *              c
     *              \end{pmatrix} = \sqrt{ \frac{2}{3} } *
     *
     *              \begin{pmatrix}
     *
     *              1 & 0 & \frac{ 1 }{ \sqrt{2} }
     *              \\
     *              \frac{ -1 }{ 2 } &  \frac{ \sqrt{3} }{ 2 } & \frac{ 1 }{ \sqrt{ 2 } }
     *              \\
     *              -\frac{1}{2} & -\frac{ \sqrt{3} }{ 2 } & \frac{ 1 }{ \sqrt{2} }
     *
     *              \end{pmatrix} *
     *
     *              \begin{pmatrix}
     *              \alpha
     *              \\
     *              \beta
     *              \\
     *              0
     *              \end{pmatrix}
     *
     *          \f]
    */
    inline void backward_transformation( __type *A , __type *B , __type *C )
    {
        m_a = (*A) + (*C);
        m_b = -(*A) * 0.5 + (*B) * 0.866025403784439 + (*C);
        m_c = -(*A) * 0.5 - (*B) * 0.866025403784439 + (*C);
    }

    /*!
      *\brief Clarke's backward / toward transformation template class 32-bit () operator
      *\param[A]    pointer to the phase A signal or to alpha component
      *\param[B]    pointer to the phase B signal or to beta component
      *\param[C]    pointer to the phase C signal or to 0 component
      *\param[type] Clarke's transformation type flag ( 0 - toward , 1 - backward )
    */
    inline void operator () ( __type *A , __type *B , __type *C , __ix32 type )
    {
        if( !type ) toward_transformation  ( A , B , C );
        else        backward_transformation( A , B , C );
    }
};

/*! \brief Parkes's forward / backward 32-bit filter */
template<> class Park_filter< __fx64 >{};

/*! @} */

// customized types names exclusion to avoid aliases during compilation:
#undef __ix16
#undef __ix32
#undef __ix64
#undef __uix16
#undef __uix32
#undef __uix64
#undef __fx32
#undef __fx64

#endif // СLARKE_FILTER_H
