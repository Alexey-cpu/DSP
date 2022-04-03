#ifndef SBLAS_H
#define SBLAS_H

#include "malloc.h"

/*! \brief defines 32-bit floating point type */
#ifndef __fx32
#define __fx32 float
#endif

/*! \brief defines 64-bit floating point type */
#ifndef __fx64
#define __fx64 double
#endif

/*! \brief defines extended 64-bit floating point type */
#ifndef __fxx64
#define __fxx64 long double
#endif

/*! \brief defines 32-bit integer type */
#ifndef __ix32
#define __ix32 int
#endif

namespace SBLAS
{
    enum compression
    {
        by_cols,
        by_rows,
        triplet
    };

    // sparse matrix template data structure
    template<typename __type> struct smatrix
    {
        __type *data;
        __ix32 *cols;
        __ix32 *rows;
        __ix32 ncols;
        __ix32 nrows;
    };


    // memory allocation:
    template<typename __type > smatrix<__type> __smx_allocate__(__ix32 nrows, __ix32 ncols, __ix32 nelem)
    {
        // generating output:
        smatrix<__type> output{ nullptr, nullptr, nullptr, -1, -1, -1 };

        // allocating output:
        output.cols = (__ix32*)calloc(nelem, sizeof (__ix32));
        output.rows = (__ix32*)calloc(nelem, sizeof (__ix32));
        output.data = (__type*)calloc(nelem, sizeof (__type));

        // returning output:
        if( output.cols && output.rows && output.data )
        {
            output.ncols = ncols;
            output.nrows = nrows;
            return output;
        }
        else
        {
            if(output.cols) free(output.cols);
            if(output.rows) free(output.rows);
            if(output.data) free(output.data);
            output.cols = nullptr;
            output.rows = nullptr;
            output.data = nullptr;
            return output;
        }
    }

    // memory deallocation:
    template<typename __type > smatrix<__type> __smx_deallocate__( smatrix<__type> input )
    {
        if(input.cols) free(input.cols);
        if(input.rows) free(input.rows);
        if(input.data) free(input.data);
        input.cols  = nullptr;
        input.rows  = nullptr;
        input.data  = nullptr;
        input.ncols = -1;
        input.nrows = -1;
        return input;
    }

    // sparse matrix memory allocation check:
    template<typename __type > smatrix<__type> __smx_is_not_empty__( smatrix<__type> input )
    {
        return input.cols && input.rows && input.data;
    }
}



// undefining macros to avoid aliases during compilation
#undef __fx32
#undef __fx64
#undef __fxx64
#undef __ix32

#endif // SBLAS_H
