# ---------------------------------------------------------------------------
# ufo-matlab – CMake build for the MATLAB MEX bindings
# ---------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.21)     # 3.21+ ships a modern FindMatlab
project(ufo-matlab LANGUAGES C)

# ---------------------------------------------------------------------------
# 1.  MATLAB
# ---------------------------------------------------------------------------
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)        # prefer config packages
find_package(Matlab REQUIRED COMPONENTS MX_LIBRARY MEX_LIBRARY
                                         MAT_LIBRARY ENG_LIBRARY)

# Locate the driver script “mex”
if (NOT Matlab_MEX_SCRIPT)
    find_program(Matlab_MEX_SCRIPT
                 NAMES mex
                 HINTS "${Matlab_ROOT_DIR}/bin")
    if (NOT Matlab_MEX_SCRIPT)
        message(FATAL_ERROR "MATLAB found at ${Matlab_ROOT_DIR} "
                            "but no 'mex' script detected. "
                            "Install the MATLAB MEX add-on or pass "
                            "-DMatlab_MEX_SCRIPT=/absolute/path/to/mex")
    endif ()
endif ()
message(STATUS "mex driver script found at ${Matlab_MEX_SCRIPT}")

# ---------------------------------------------------------------------------
# 2.  UFO-Core (via pkg-config)
# ---------------------------------------------------------------------------
find_package(PkgConfig REQUIRED)
pkg_check_modules(UFO REQUIRED ufo>=0.12)

# ---------------------------------------------------------------------------
# 3.  Source lists  ( **updated** )
# ---------------------------------------------------------------------------
set(MEX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/matlab/mex")   # <-- adjust here if needed

file(GLOB_RECURSE MEX_SRC   "${MEX_DIR}/*.c")
file(GLOB_RECURSE API_HDR   "${MEX_DIR}/include/*.h")

# Abort early with a clear message if we glob the wrong place
if (MEX_SRC STREQUAL "")
    message(FATAL_ERROR
        "No .c source files found in ${MEX_DIR}. "
        "Make sure the MATLAB bindings live there or update MEX_DIR.")
endif ()

# ---------------------------------------------------------------------------
# 4.  Build the MEX gateway (libufo_mex.${Matlab_MEX_EXTENSION})
# ---------------------------------------------------------------------------
add_library(ufo_mex SHARED ${MEX_SRC} ${API_HDR})

target_include_directories(ufo_mex PRIVATE
        ${Matlab_INCLUDE_DIRS}
        ${UFO_INCLUDE_DIRS}
        "${MEX_DIR}/include")

# Needed by every MEX source
target_compile_definitions(ufo_mex PRIVATE MATLAB_MEX_FILE)

# Link against MATLAB’s stub and UFO-Core (+transitive deps)
target_link_libraries(ufo_mex PRIVATE
        ${Matlab_MEX_LIBRARY}
        ${UFO_LIBRARIES})

# Produce “ufo_mex.mexa64” (no “lib” prefix)
set_target_properties(ufo_mex PROPERTIES
        PREFIX ""
        SUFFIX ${Matlab_MEX_EXTENSION})

# ---------------------------------------------------------------------------
# 5.  Installation layout
# ---------------------------------------------------------------------------
# (a)  Binary to   <prefix>/matlab/+ufo
install(TARGETS ufo_mex
        LIBRARY DESTINATION matlab/+ufo)

# (b)  Pure-MATLAB API (*.m)  – assumes they live in matlab/+ufo
install(DIRECTORY matlab/+ufo
        DESTINATION matlab)

# ---------------------------------------------------------------------------
# 6.  Convenience target  ‘make run-mex’
# ---------------------------------------------------------------------------
if (Matlab_EXECUTABLE)
    add_custom_target(run-mex
        COMMAND ${Matlab_EXECUTABLE}
                -batch "addpath('${CMAKE_INSTALL_PREFIX}/matlab'); \
                        ufo.test.selfcheck"
        COMMENT "Launching MATLAB self-test")
endif ()
