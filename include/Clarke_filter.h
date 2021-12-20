
// Template class Clarke transform implements Clarke toward and backward transformation for the input signal of the float 32 and 64 bit

#ifndef CLARKE_FILTER_H
#define CLARKE_FILTER_H

// customized C data types:
#ifndef __ix16
#define __ix16 short
#endif

#ifndef __ix32
#define __ix32 int
#endif

#ifndef __ix64
#define __ix64 long long
#endif

#ifndef __uix16
#define __uix16 unsigned short
#endif

#ifndef __uix32
#define __uix32 unsigned int
#endif

#ifndef __uix64
#define __uix64 unsigned long long
#endif

#ifndef __fx32
#define __fx32 float
#endif

#ifndef __fx64
#define __fx64 double
#endif

// Clarke's transformation filter:
template< typename T > class Clarke_filter;

// float x32:
template<> class Clarke_filter< __fx32 >
{
    typedef __fx32 __type;
public:
    __type m_a , m_b , m_c;

    // toward transformation function:

    // float x32 inputs:
    inline void toward_transformation( __type *A , __type *B , __type *C )
    {
        m_a = (*A) * 0.6666666666666666666666666666667 - (*B) * 0.3333333333333333333333333333333 - (*C) * 0.3333333333333333333333333333333;
        m_b = (*B) * 0.577350269189625764509148780502  - (*C) * 0.577350269189625764509148780502;
        m_c = (*A) * 0.3333333333333333333333333333333 + (*B) * 0.3333333333333333333333333333333 + (*C) * 0.3333333333333333333333333333333;
    }

    // float x64 inputs ( CAUTION !!! ROUNDING ERROR OCCURS !!! ) :
    inline void toward_transformation( __fx64 *A , __fx64 *B , __fx64 *C )
    {
        m_a = (*A) * 0.6666666666666666666666666666667 - (*B) * 0.3333333333333333333333333333333 - (*C) * 0.3333333333333333333333333333333;
        m_b = (*B) * 0.577350269189625764509148780502  - (*C) * 0.577350269189625764509148780502;
        m_c = (*A) * 0.3333333333333333333333333333333 + (*B) * 0.3333333333333333333333333333333 + (*C) * 0.3333333333333333333333333333333;
    }


    // backward transformation function:

    // float x32 inputs:
    inline void backward_transformation( __type *A , __type *B , __type *C )
    {
        m_a = (*A) + (*C);
        m_b = -(*A) * 0.5 + (*B) * 0.866025403784439 + (*C);
        m_c = -(*A) * 0.5 - (*B) * 0.866025403784439 + (*C);
    }

    // float x64 inputs ( CAUTION !!! ROUNDING ERROR OCCURS !!! )
    inline void backward_transformation( __fx64 *A , __fx64 *B , __fx64 *C )
    {
        m_a = (*A) + (*C);
        m_b = -(*A) * 0.5 + (*B) * 0.866025403784439 + (*C);
        m_c = -(*A) * 0.5 - (*B) * 0.866025403784439 + (*C);
    }

    // operators:

    // float x32 inputs:
    inline void operator () ( __type *A , __type *B , __type *C , __ix32 type )
    {
        if( !type ) toward_transformation  ( A , B , C );
        else        backward_transformation( A , B , C );
    }

    // float x64 inputs ( CAUTION !!! ROUNDING ERROR OCCURS !!! )
    inline void operator () ( __fx64 *A , __fx64 *B , __fx64 *C  , __ix32 type )
    {
        if( !type ) toward_transformation  ( A , B , C );
        else        backward_transformation( A , B , C );
    }
};

// float x64:
template<> class Clarke_filter< __fx64 >
{
    typedef __fx64 __type;
public:
    __type m_a , m_b , m_c;

    // toward transformation function:
    inline void toward_transformation( __type *A , __type *B , __type *C )
    {
        m_a = (*A) * 0.6666666666666666666666666666667 - (*B) * 0.3333333333333333333333333333333 - (*C) * 0.3333333333333333333333333333333;
        m_b = (*B) * 0.577350269189625764509148780502  - (*C) * 0.577350269189625764509148780502;
        m_c = (*A) * 0.3333333333333333333333333333333 + (*B) * 0.3333333333333333333333333333333 + (*C) * 0.3333333333333333333333333333333;
    }

    // backward transformation function:
    inline void backward_transformation( __type *A , __type *B , __type *C )
    {
        m_a = (*A) + (*C);
        m_b = -(*A) * 0.5 + (*B) * 0.866025403784439 + (*C);
        m_c = -(*A) * 0.5 - (*B) * 0.866025403784439 + (*C);
    }

    // operators:
    inline void operator () ( __type *A , __type *B , __type *C , int type )
    {
        if( !type ) toward_transformation  ( A , B , C );
        else        backward_transformation( A , B , C );
    }
};

// customized types names exclusion to avoid aliases during compilation:
#undef __ix16
#undef __ix32
#undef __ix64
#undef __uix16
#undef __uix32
#undef __uix64
#undef __fx32
#undef __fx64

#endif // CLARKE_FILTER_H
