cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(DSP VERSION 1.0.1 LANGUAGES CXX C)
set(ADS_VERSION 3.8.1)

# setup common project options
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

# add CMake options
option(DSP_BUILD_EXAMPLES "Build examples" OFF)

# add compile definitions
add_definitions(-DDSP_EXAMPLES_LOGS_DIRECTORY="${CMAKE_CURRENT_LIST_DIR}/logs")
add_definitions(-DDSP_EXAMPLES_OSCS_DIRECTORY="${CMAKE_CURRENT_LIST_DIR}/osc")

# include CMake macro
include("cmake/collect_source_code_and_resources.cmake")

# collec source code
list(APPEND LIBRARY
    "source/comtrade/"
    "source/dsp/"
    "source/models/"
)

set(PATHS "")
list(APPEND PATHS ${LIBRARY})
cmake_language(CALL COLLECT_SOURCE_CODE_AND_RESOURCES PATHS)

# add library
add_library(DSP ${HEADERS} ${SOURCES})
add_library(DSP::DSP ALIAS DSP)
target_include_directories(DSP PUBLIC ${DIRECTORIES})

if(DSP_BUILD_EXAMPLES)
include(DSP_examples.cmake)
endif()
