QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        files/Butterworth.cpp \
        files/Chebyshev_I.cpp \
        files/Chebyshev_II.cpp \
        files/Elliptic.cpp \
        files/FIR_filt.cpp \
        files/Source.cpp \
        files/aperiodic_filt.cpp \
        files/aperiodic_filt_eq.cpp \
        files/cplx_num.cpp \
        files/cplx_operations.cpp \
        files/integrators.cpp \
        files/math_const.cpp \
        files/mirror_ring_buff_array.cpp \
        files/mirror_ring_buff_x32.cpp \
        files/orto_transformations_1ph.cpp \
        files/orto_transformations_3ph.cpp \
        files/quad_mltpx.cpp \
        files/signal_gen.cpp \
        files/simple_IIR.cpp \
        files/special_functions.cpp \
        files/timers.cpp \
        files/triggers.cpp \
        files/wind_fcn.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    files/Butterworth.hpp \
    files/Chebyshev_I.hpp \
    files/Chebyshev_II.hpp \
    files/Elliptic.hpp \
    files/FIR_filt.hpp \
    files/aperiodic_filt.hpp \
    files/aperiodic_filt_eq.hpp \
    files/cplx_num.hpp \
    files/cplx_operations.hpp \
    files/integrators.hpp \
    files/math_const.hpp \
    files/mirror_ring_buff_array.hpp \
    files/mirror_ring_buff_x32.hpp \
    files/orto_transformations_1ph.hpp \
    files/orto_transformations_3ph.hpp \
    files/quad_mltpx.hpp \
    files/signal_gen.hpp \
    files/simple_IIR.hpp \
    files/special_functions.hpp \
    files/timers.hpp \
    files/triggers.hpp \
    files/wind_fcn.hpp

DISTFILES += \
    .gitignore
