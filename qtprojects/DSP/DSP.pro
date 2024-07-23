QT -= gui
CONFIG += c++14
QMAKE_CXXFLAGS += -O2

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix {
    INCLUDEPATH += /usr/lib/gcc/x86_64-linux-gnu/9/include
}

# main project paths paths
HEADERS_PATH = \
    ../../source/dsp \
    ../../source/comtrade \
    ../../source/utils \

SOURCE_PATH = \
    ../../source/dsp \
    ../../source/comtrade \
    ../../source/utils \

# retrieve project sorce code files
for(path,HEADERS_PATH):HEADERS += $$files($${path}/*.h,true)

# retrieve project header files
for(path,SOURCE_PATH):SOURCES += $$files($${path}/*.cpp,true)

# attach main.cpp
SOURCES += main.cpp

# attach external/internal resources
for(path,HEADERS):INCLUDEPATH += $$dirname(path)

# attach libraries

#COMTRADELIB
#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../libs/ -lCOMTRADELIB
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libs/ -lCOMTRADELIB
#else:unix: LIBS += -L$$PWD/../../libs/ -lCOMTRADELIB

#collect dependecies for libraries
for(path,HEADERS):DEPENDPATH += $$dirname(path)
