QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17
CONFIG += lrelease
CONFIG += embed_translations
QMAKE_CXXFLAGS += -O2

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix {
    INCLUDEPATH += /usr/lib/gcc/x86_64-linux-gnu/9/include
}

# main project paths paths
BIN_DIR = ../../bin
DIST_DIR = ../../shared
LIB_DIR = ../../libs

TOOLS_PATH = \
           ../../tools/MATH/source \
           ../../tools/UTILS/source \
           ../../tools/COMTRADE/source \

PROJECT_PATH = ../../qtprojects
SOURCE_PATH = \
    ../../source/gui \
    ../../source/kernel \
    ../../source/tests

# retrieve project header files
for(path,SOURCE_PATH):SOURCES += $$files($${path}/*.cpp,true)
for(path,TOOLS_PATH):SOURCES += $$files($${path}/*.cpp,true)

# translations file
for(path,PROJECT_PATH):TRANSLATIONS += $$files($${path}/*.ts,true)

# retrieve project sorce code files
for(path,SOURCE_PATH):HEADERS += $$files($${path}/*.h,true)
for(path,TOOLS_PATH):HEADERS += $$files($${path}/*.h,true)

# retrieve project resource files
for(path,PROJECT_PATH):RESOURCES += $$files($${path}/*.qrc,true)

SOURCES += main.cpp

# attach external/internal resources
for(path,HEADERS):INCLUDEPATH += $$dirname(path)

# install guide
CONFIG(release, debug|release): shared.path = $${BIN_DIR}/release/shared
CONFIG(debug, debug|release): shared.path = $${BIN_DIR}/debug/shared
shared.files = $${DIST_DIR}/*.xml
INSTALLS += shared
