QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        include/math_const.cpp \
        include/special_functions.cpp \
        source/main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    include/Clarke_filter.h \
    include/buffer.h \
    include/complex.h \
    include/math_const.hpp \
    include/recursive_fourier.h \
    include/recursive_mean.h \
    include/recursive_rms.h \
    include/special_fcn.h \
    include/special_functions.hpp

DISTFILES += \
    .gitignore
