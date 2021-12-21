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
    include/butterworth.h \
    include/complex.h \
    include/fir.h \
    include/logical.h \
    include/recursive_fourier.h \
    include/recursive_mean.h \
    include/recursive_rms.h \
    include/sgen.h \
    include/special_functions.h \
    include/clarke_filter.h \

DISTFILES += \
    .gitignore
