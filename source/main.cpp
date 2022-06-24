#include "include/special_functions.h"

// examples:
#include "iostream"
#include "include/fcomplex.h"
#include "include/filters_iir.h"
#include "include/utils.h"

#include "include/examples.h"

int main()
{

    //filter_data<float> data = __butt_cheb1_digital_hp__<float>( 4000, 100, 11, 0, 1 );
    //__show__(data, data.type);
    //__dsp_clear_filter__(data);

    //example0();
    //example1();
    filters_butt_example();

    return 0;
}
