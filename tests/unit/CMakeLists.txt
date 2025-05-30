cmake_minimum_required(VERSION 3.5)

set(TEST_SRCS
    test-suite.c
    test-buffer.c
    test-graph.c
    test-node.c
    test-profiler.c
    test-max-input-nodes.cpp
    test-handle-concurrency.c
    test-opencl.c
    test-mex-handle.c
    )

set(SUITE_BIN "test-suite")

add_executable(${SUITE_BIN} ${TEST_SRCS})
target_include_directories(${SUITE_BIN} PRIVATE ${CMAKE_SOURCE_DIR}/matlab/include)

set_property(TARGET ${SUITE_BIN} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${SUITE_BIN} PROPERTY CXX_STANDARD_REQUIRED ON)


target_link_libraries(${SUITE_BIN} ufo m ${UFOCORE_DEPS})

add_test(${SUITE_BIN} ${SUITE_BIN})

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/gtester.xsl"
               "${CMAKE_CURRENT_BINARY_DIR}/gtester.xsl"
               @ONLY IMMEDIATE)
