QT -= gui
TEMPLATE = lib
CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# main project paths paths
BIN_DIR = ../../bin/COMTRADELIB
LIB_DIR = ../../libs

SOURCE_PATH = \
    ../../source/comtrade \
    ../../source/utils \

# retrieve project header files
for(path,SOURCE_PATH):SOURCES += $$files($${path}/*.cpp,true)
for(path,TOOLS_PATH):SOURCES += $$files($${path}/*.cpp,true)

# retrieve project sorce code files
for(path,SOURCE_PATH):HEADERS += $$files($${path}/*.h,true)
for(path,TOOLS_PATH):HEADERS += $$files($${path}/*.h,true)

# attach external/internal resources
for(path,HEADERS):INCLUDEPATH += $$dirname(path)

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

# install lib
CONFIG(release, debug|release): shared.path = $${BIN_DIR}/release
CONFIG(debug, debug|release): shared.path = $${BIN_DIR}/debug
shared.files += $${LIB_DIR}/*.dll
shared.files += $${LIB_DIR}/*.a
INSTALLS += shared
