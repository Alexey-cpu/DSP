#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include "Utils.h"

#ifndef __ALG_PLATFORM
#include "cmath"
#include "math.h"
#endif

/*! \defgroup <THREE_PHASE_TRANSFORMATIONS> ( Three phase ortohonal transformations )
 *  \brief the module describes complex numbers functions
    @{
*/

/*! \brief Clarke's forward / backward template filter */
template< typename __type > class Clarke_filter
{
public:
    /*! \brief alpha component of Clarke's transformation */
    __type m_a;
    /*! \brief betta component of Clarke's transformation */
    __type m_b;
    /*! \brief zero component of Clarke's transformation */
    __type m_c;

    /*!
     *  \brief Clarke's toward 32-bit transformation function
     *  \param[A] phase A input signal
     *  \param[B] phase B input signal
     *  \param[C] phase C input signal
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
     *  \param[A] phase A input signal
     *  \param[B] phase B input signal
     *  \param[C] phase C input signal
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
    inline void toward_transformation( double *A , double *B , double *C )
    {
        m_a = (*A) * 0.6666666666666666666666666666667 - (*B) * 0.3333333333333333333333333333333 - (*C) * 0.3333333333333333333333333333333;
        m_b = (*B) * 0.577350269189625764509148780502  - (*C) * 0.577350269189625764509148780502;
        m_c = (*A) * 0.3333333333333333333333333333333 + (*B) * 0.3333333333333333333333333333333 + (*C) * 0.3333333333333333333333333333333;
    }

    /*!
     *  \brief Clarke's backward 32-bit transformation function
     *  \param[A] pointer to alpha component
     *  \param[B] pointer to beta component
     *  \param[C] pointer to 0 component
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
     *  \param[A] pointer to alpha component
     *  \param[B] pointer to beta component
     *  \param[C] pointer to 0 component
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
    inline void backward_transformation( double *A , double *B , double *C )
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
    inline void operator () ( __type *A , __type *B , __type *C , int type )
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
    inline void operator () ( double *A , double *B , double *C  , int type )
    {
        if( !type ) toward_transformation  ( A , B , C );
        else        backward_transformation( A , B , C );
    }
};

/*! \brief Parkes's forward / backward template filter */
template< typename __type > class Park_filter
{
private:
    __type m_cnt , m_k1 , m_k2;
public:
    /*! \brief alpha component of Clarke's transformation */
    __type m_a;
    /*! \brief betta component of Clarke's transformation */
    __type m_b;
    /*! \brief zero component of Clarke's transformation */
    __type m_c;

    /*! \brief default constructor */
    Park_filter() { m_a = m_b = m_c = 0; m_cnt = 0; m_k1 = 2.0 / 3.0; m_k2 = 1.0 / 3.0; }

    /*! \brief default destructor */
    ~Park_filter(){}


    /*!
     *  \brief toward Park transformation
     *  \param[A] - pointer to the phase A samples
     *  \param[B] - pointer to the phase B samples
     *  \param[C] - pointer to the phase C samples
     *  \param[Fn] - nominal frequency               , Hz
     *  \param[Fs] - input signal sampling frequency , Hz
     *  \param[a_axis_alignment] - phase A alignment
     *  \return if a_axis_alignment = 0 the function fills Park's filter template class filelds as follows:
     *          \f[
     *
     *              \begin{pmatrix}
     *              d
     *              \\
     *              q
     *              \\
     *              0
     *              \end{pmatrix} = \frac{2}{3} *
     *
     *              \begin{pmatrix}
     *
     *              \sin{ \theta } & \sin{ \left( \theta - \frac{ 2 * \pi }{ 3 } \right) } & \sin{ \left( \theta + \frac{ 2 * \pi }{ 3 } \right) }
     *
     *              \\
     *
     *              \cos{ \theta } & \cos{ \left( \theta - \frac{ 2 * \pi }{ 3 } \right) } & \cos{ \left( \theta + \frac{ 2 * \pi }{ 3 } \right) }
     *
     *              \\
     *
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
     *
     *          if a_axis_alignment = 1 the function fills Park's filter template class filelds as follows:
     *          \f[
     *              \begin{pmatrix}
     *              d
     *              \\
     *              q
     *              \\
     *              0
     *              \end{pmatrix} = \frac{2}{3} *
     *
     *              \begin{pmatrix}
     *
     *              \sin{ \theta } & \sin{ \left( \theta - \frac{ 2 * \pi }{ 3 } \right) } & \sin{ \left( \theta + \frac{ 2 * \pi }{ 3 } \right) }
     *
     *              \\
     *
     *              \cos{ \theta } & \cos{ \left( \theta - \frac{ 2 * \pi }{ 3 } \right) } & \cos{ \left( \theta + \frac{ 2 * \pi }{ 3 } \right) }
     *
     *              \\
     *
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
     *          \f]
    */
    inline void toward_transformation( __type *A, __type *B, __type *C, double Fn , double Fs, bool a_axis_alignment = 1 )
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
            m_b = m_k1 * ( (*A) * cos( phi0 ) + (*B) * cos( phi1 ) + (*C) * cos( phi2 ) );
            m_c = m_k2 * ( (*A) + (*B) + (*C) );
        }

        // reference frame counter update:
        if (m_cnt < ceil( Fs / Fn ) ) m_cnt++;
        else m_cnt = 0;
    }

    /*!
     *  \brief backward Park transformation
     *  \param[D] - pointer to the D component samples
     *  \param[Q] - pointer to the Q component samples
     *  \param[Z] - pointer to the Z component samples
     *  \param[Fn] - nominal frequency               , Hz
     *  \param[Fs] - input signal sampling frequency , Hz
     *  \param[a_axis_alignment] - phase A alignment
     *  \return if a_axis_alignment = 0 the function fills Park's filter template class filelds as follows:
     *          \f[
     *
     *              \begin{pmatrix}
     *              a
     *              \\
     *              b
     *              \\
     *              c
     *              \end{pmatrix} = \frac{2}{3} *
     *
     *              \begin{pmatrix}
     *
     *              \sin{ \theta } & \cos{ \left( \theta \right) } & 1
     *
     *              \\
     *
     *              \sin{ \left( \theta - \frac{ 2 * \pi }{ 3 } \right) } & \cos{ \left( \theta - \frac{ 2 * \pi }{ 3 } \right) } & 1
     *
     *              \\
     *
     *              \sin{ \left( \theta + \frac{ 2 * \pi }{ 3 } \right) } & \cos{ \left( \theta + \frac{ 2 * \pi }{ 3 } \right) } & 1
     *
     *              \end{pmatrix} *
     *
     *              \begin{pmatrix}
     *              d
     *              \\
     *              q
     *              \\
     *              0
     *              \end{pmatrix}
     *
     *          \f]
     *
     *          if a_axis_alignment = 1 the function fills Park's filter template class filelds as follows:
     *
     *          \f[
     *
     *              \begin{pmatrix}
     *              a
     *              \\
     *              b
     *              \\
     *              c
     *              \end{pmatrix} = \frac{2}{3} *
     *
     *              \begin{pmatrix}
     *
     *              \cos{ \theta } & -\sin{ \left( \theta \right) } & 1
     *
     *              \\
     *
     *              \cos{ \left( \theta - \frac{ 2 * \pi }{ 3 } \right) } & -\sin{ \left( \theta - \frac{ 2 * \pi }{ 3 } \right) } & 1
     *
     *              \\
     *
     *              \cos{ \left( \theta + \frac{ 2 * \pi }{ 3 } \right) } & -\sin{ \left( \theta + \frac{ 2 * \pi }{ 3 } \right) } & 1
     *
     *              \end{pmatrix} *
     *
     *              \begin{pmatrix}
     *              d
     *              \\
     *              q
     *              \\
     *              0
     *              \end{pmatrix}
     *
     *          \f]
    */
    inline void backward_transformation( __type *D , __type *Q , __type *Z , double Fn , double Fs , bool a_axis_alignment = 1 )
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

    inline void operator () ( __type *A, __type *B, __type *C, double Fn , double Fs, bool a_axis_alignment , bool type = 0 )
    {
        if( !type ) toward_transformation  ( A , B ,  C,  Fn ,  Fs, a_axis_alignment );
        else backward_transformation( A , B ,  C,  Fn ,  Fs, a_axis_alignment );
    }
};

/*! \brief Fortescue template filter */
template< typename __type > class Fortescue_filter
{
public:
    /*! \brief zero sequence real component */
    __type m_re0;
    /*! \brief zero sequence imaginary component */
    __type m_im0;
    /*! \brief positive sequence real component */
    __type m_re1;
    /*! \brief positive sequence imaginary component */
    __type m_im1;
    /*! \brief negative sequence real component */
    __type m_re2;
    /*! \brief negative sequence imaginary component */
    __type m_im2;

    /*! \brief default constructor */
    Fortescue_filter()
    {
        m_re0 = m_re1 = m_re2 = 0;
        m_im0 = m_im1 = m_im2 = 0;
    }

    /*! \brief default destructor */
    ~Fortescue_filter()
    {
        m_re0 = m_re1 = m_re2 = 0;
        m_im0 = m_im1 = m_im2 = 0;
    }

    /*!
     *  \brief 32-bit Fortescue filtering function
     *  \param[re_a] - phase A real      component
     *  \param[im_a] - phase A imaginary component
     *  \param[re_b] - phase B real      component
     *  \param[im_b] - phase B imaginary component
     *  \param[re_c] - phase C real      component
     *  \param[im_c] - phase C imaginary component
     *  \return the function fills Fortescue filter template class filelds as follows:
     *          \f[
     *
     *              \begin{pmatrix}
     *              1
     *              \\
     *              2
     *              \\
     *              0
     *              \end{pmatrix} = \frac{ 1 }{ 3 } *
     *
     *              \begin{pmatrix}
     *
     *              1 & a & a^2
     *              \\
     *              0 &  a^2 & a
     *              \\
     *              1 & 1 & 1
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
    inline void filt( __type re_a, __type im_a , __type re_b, __type im_b , __type re_c , __type im_c )
    {
        // zero sequence
        m_re0 = (re_a + re_b + re_c) * 0.33333333333333333333333333333;
        m_im0 = (im_a + im_b + im_c) * 0.33333333333333333333333333333;
        // positive sequence
        m_re1 = (re_a - (re_b * 0.500 + im_b * 0.866) - (re_c * 0.500 - im_c * 0.866)) * 0.33333333333333333333333333333;
        m_im1 = (im_a + (re_b * 0.866 - im_b * 0.500) - (re_c * 0.866 + im_c * 0.500)) * 0.33333333333333333333333333333;
        // negative sequence
        m_re2 = (re_a - (re_b * 0.500 - im_b * 0.866) - (re_c * 0.500 + im_c * 0.866)) * 0.33333333333333333333333333333;
        m_im2 = (im_a - (re_b * 0.866 + im_b * 0.500) + (re_c * 0.866 - im_c * 0.500)) * 0.33333333333333333333333333333;
    }

    /*!
     *  \brief 64-bit Fortescue filtering function
     *  \param[re_a] - phase A real      component
     *  \param[im_a] - phase A imaginary component
     *  \param[re_b] - phase B real      component
     *  \param[im_b] - phase B imaginary component
     *  \param[re_c] - phase C real      component
     *  \param[im_c] - phase C imaginary component
     *  \return the function fills Fortescue filter template class filelds as follows:
     *          \f[
     *
     *              \begin{pmatrix}
     *              1
     *              \\
     *              2
     *              \\
     *              0
     *              \end{pmatrix} = \frac{ 1 }{ 3 } *
     *
     *              \begin{pmatrix}
     *
     *              1 & a & a^2
     *              \\
     *              0 &  a^2 & a
     *              \\
     *              1 & 1 & 1
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
    inline void filt( double re_a, double im_a , double re_b, double im_b , double re_c , double im_c )
    {
        // zero sequence
        m_re0 = (re_a + re_b + re_c) * 0.33333333333333333333333333333;
        m_im0 = (im_a + im_b + im_c) * 0.33333333333333333333333333333;
        // positive sequence
        m_re1 = (re_a - (re_b * 0.500 + im_b * 0.866) - (re_c * 0.500 - im_c * 0.866)) * 0.33333333333333333333333333333;
        m_im1 = (im_a + (re_b * 0.866 - im_b * 0.500) - (re_c * 0.866 + im_c * 0.500)) * 0.33333333333333333333333333333;
        // negative sequence
        m_re2 = (re_a - (re_b * 0.500 - im_b * 0.866) - (re_c * 0.500 + im_c * 0.866)) * 0.33333333333333333333333333333;
        m_im2 = (im_a - (re_b * 0.866 + im_b * 0.500) + (re_c * 0.866 - im_c * 0.500)) * 0.33333333333333333333333333333;
    }

    /*!
     *  \brief 32-bit Fortescue filtering function
     *  \param[re_a] - phase A real      component
     *  \param[im_a] - phase A imaginary component
     *  \param[re_b] - phase B real      component
     *  \param[im_b] - phase B imaginary component
     *  \param[re_c] - phase C real      component
     *  \param[im_c] - phase C imaginary component
     */
    inline void operator() ( __type re_a, __type im_a , __type re_b, __type im_b , __type re_c , __type im_c )
    {
        filt( re_a, im_a , re_b, im_b , re_c , im_c );
    }

    /*!
     *  \brief 64-bit Fortescue filtering function
     *  \param[re_a] - phase A real      component
     *  \param[im_a] - phase A imaginary component
     *  \param[re_b] - phase B real      component
     *  \param[im_b] - phase B imaginary component
     *  \param[re_c] - phase C real      component
     *  \param[im_c] - phase C imaginary component
     */
    inline void operator() ( double re_a, double im_a , double re_b, double im_b , double re_c , double im_c )
    {
        filt( re_a, im_a , re_b, im_b , re_c , im_c );
    }

};

/*! @} */

#endif // СLARKE_FILTER_H
