CONFIG += c++17
QMAKE_CXXFLAGS += -O2

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix {
    INCLUDEPATH += /usr/lib/gcc/x86_64-linux-gnu/9/include
}

# main project paths paths
TOOLS_PATH = \
           ../../tools/UTILS/source \

SOURCE_PATH = \
    ../../source/dsp \
    ../../source/comtrade \
    ../../source/models \
    ../../source/utils \

# retrieve project header files
for(path,SOURCE_PATH):SOURCES += $$files($${path}/*.cpp,true)
for(path,TOOLS_PATH):SOURCES += $$files($${path}/*.cpp,true)

# retrieve project sorce code files
for(path,SOURCE_PATH):HEADERS += $$files($${path}/*.h,true)
for(path,TOOLS_PATH):HEADERS += $$files($${path}/*.h,true)

# attach main.cpp
SOURCES += main.cpp

# attach external/internal resources
for(path,HEADERS):INCLUDEPATH += $$dirname(path)

HEADERS += \
    ../../source/models/tests/current_transformer.h
