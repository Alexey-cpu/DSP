QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        source/main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    include/buffer.h \
    include/examples.h \
    include/fcomplex.h \
    include/filters_fir.h \
    include/filters_hmf.h \
    include/filters_iir.h \
    include/filters_rff.h \
    include/kernel_dsp.h \
    include/kernel_fir.h \
    include/kernel_iir.h \
    include/logical.h \
    include/sgen.h \
    include/special_functions.h \
    include/transformations.h \
    include/utils.h

DISTFILES += \
    .gitignore
