QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../ComtradeFileParser/ComtradeFileParser.h \
    ../source/Complex.h \
    ../source/examples/config.h \
    ../source/examples/example_classic_fir.h \
    ../source/examples/example_classic_iir_butt.h \
    ../source/examples/example_classic_iir_cheb1.h \
    ../source/examples/example_classic_iir_cheb2.h \
    ../source/examples/example_classic_iir_ellip.h \
    ../source/examples/example_comtrade_emulation.h \
    ../source/examples/example_custom_filters_harmonic_extracter.h \
    ../source/examples/example_custom_filters_hmf.h \
    ../source/examples/example_custom_filters_rff.h \
    ../source/examples/example_math_complex_numbers.h \
    ../source/examples/example_math_special_functions.h \
    ../source/examples/example_math_special_functions_fft.h \
    ../source/examples/example_math_special_functions_interpolation.h \
    ../source/examples/example_transfer_functions_aperiodic.h \
    ../source/examples/example_transfer_functions_differentiator.h \
    ../source/examples/example_transfer_functions_filter_2nd_order.h \
    ../source/examples/example_transfer_functions_integrator.h \
    ../source/examples/example_transfer_functions_leadlag.h \
    ../source/examples/examples.h \
    ../source/filters_fast_fourier_transform_spectrum_analyzer.h \
    ../source/filters_fir.h \
    ../source/filters_harmonic_filter.h \
    ../source/filters_iir.h \
    ../source/filters_interpolators.h \
    ../source/filters_real_time_spectrum_analyzer.h \
    ../source/filters_recursive_fourier_filter.h \
    ../source/filters_transfer_functions.h \
    ../source/generators.h \
    ../source/kernel_dsp.h \
    ../source/kernel_fir.h \
    ../source/kernel_iir.h \
    ../source/logical.h \
    ../source/special_functions.h \
    ../source/transformations.h \
    ../source/utils.h

SOURCES += \
    ../source/main.cpp
