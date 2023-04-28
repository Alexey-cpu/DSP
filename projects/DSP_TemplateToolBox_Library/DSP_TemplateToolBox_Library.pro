QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../source/main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../examples/config.h \
    ../../examples/example_classic_fir.h \
    ../../examples/example_classic_iir_butt.h \
    ../../examples/example_classic_iir_cheb1.h \
    ../../examples/example_classic_iir_cheb2.h \
    ../../examples/example_classic_iir_ellip.h \
    ../../examples/example_custom_filters_hmf.h \
    ../../examples/example_custom_filters_rff.h \
    ../../examples/example_fft.h \
    ../../examples/example_math_complex_numbers.h \
    ../../examples/example_math_special_functions.h \
    ../../examples/example_transfer_functions_aperiodic.h \
    ../../examples/example_transfer_functions_differentiator.h \
    ../../examples/example_transfer_functions_filter_2nd_order.h \
    ../../examples/example_transfer_functions_integrator.h \
    ../../examples/example_transfer_functions_leadlag.h \
    ../../examples/examples.h \
    ../../include/Complex.h \
    ../../include/filters_fir.h \
    ../../include/filters_hmf.h \
    ../../include/filters_iir.h \
    ../../include/filters_rff.h \
    ../../include/filters_tsf.h \
    ../../include/generators.h \
    ../../include/kernel_dsp.h \
    ../../include/kernel_fir.h \
    ../../include/kernel_iir.h \
    ../../include/logical.h \
    ../../include/special_functions.h \
    ../../include/transformations.h \
    ../../include/utils.h
