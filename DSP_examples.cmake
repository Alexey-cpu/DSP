# collec source code
list(APPEND EXAMPLES "source/examples/")

set(PATHS "")
list(APPEND PATHS ${EXAMPLES})
cmake_language(CALL COLLECT_SOURCE_CODE_AND_RESOURCES PATHS)

# add library
add_executable(DSP_examples ${HEADERS} ${SOURCES})
target_link_libraries(DSP_examples DSP::DSP)
target_include_directories(DSP_examples PRIVATE ${DIRECTORIES})
