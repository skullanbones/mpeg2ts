#*****************************************************************
#
#    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
#
#    mpeg2ts - mpeg2ts lib
#
#    This file is part of mpeg2ts (Mpeg2 Transport Stream Library).
#
#    Unless you have obtained mpeg2ts under a different license,
#    this version of mpeg2ts is mpeg2ts|GPL.
#    Mpeg2ts|GPL is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License as
#    published by the Free Software Foundation; either version 2,
#    or (at your option) any later version.
#
#    Mpeg2ts|GPL is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with mpeg2ts|GPL; see the file COPYING. If not, write to
#    the Free Software Foundation, 59 Temple Place - Suite 330,
#    Boston, MA 02111-1307, USA.
#
#******************************************************************/
cmake_minimum_required(VERSION 3.5)

if(CMAKE_MINOR_VERSION GREATER 9)
project(
    mpeg2ts
    VERSION 0.6.0
    DESCRIPTION "Main project for MPEG-2 Parsing of transport streams"
    LANGUAGES CXX
)
else()
project(
    mpeg2ts
    VERSION 0.6.0
    LANGUAGES CXX
)
endif()

# Set Language Standard
# Require C++11 and disable extensions for all targets
if(CMAKE_MINOR_VERSION LESS 8)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()


include(CTest)

#-------------------
# global settings
#-------------------
if(EXISTS /.dockerenv)
    message("Detected running inside docker container. Using container dependencies instead.")
    set(USE_DOCKER ON) # To use dependencies from Docker instead of local build
else()
    set(USE_DOCKER OFF)
endif()

#-------------------
# modules
#-------------------
if(NOT "${CMAKE_CURRENT_LIST_DIR}/cmake" IN_LIST CMAKE_MODULE_PATH)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
endif()

#-------------------
# sub modules
#-------------------
set(BENCHMARK_ENABLE_TESTING OFF)
option(ENABLE_TESTS "Build with tests" OFF)
option(ENABLE_COMPONENT_TESTS "Build with component tests" OFF)
option(BUILD_SAMPLES "Build samples" ON)
option(ENABLE_COVERAGE "Enable GCOV code coverage" OFF)
option(ENABLE_WEBASSEMBLY "Enable Web assembly code" OFF)

#-------------------
# cmake code
#-------------------
include(clang_format)
include(cppcheck)
include(iwyu)
include(doxygen)
include(valgrind)
include(clang_tidy)
include(emscripten)

#-------------------
# third-party
#-------------------
set(PLOG_VERSION 1.1.5)
set(NLOHMANN_VERSION 3.9.1)
add_subdirectory(3rd-party)


#-------------------
# gcov code coverage
#-------------------
if(UNIX AND ${ENABLE_COVERAGE} AND CMAKE_BUILD_TYPE MATCHES Debug)
    include(CodeCoverage)
    APPEND_COVERAGE_COMPILER_FLAGS()
    SETUP_TARGET_FOR_COVERAGE_LCOV(
        NAME coverage
        EXECUTABLE ./tests/run_gtests "${CMAKE_CURRENT_LIST_DIR}/assets/bbc_one.ts"
    )
endif()

if(ENABLE_TESTS)
    add_subdirectory(tests)
    add_subdirectory(bench)
endif()

if(MSVC)
    set(ENABLE_COMPONENT_TESTS OFF)
endif()
if(ENABLE_COMPONENT_TESTS)
    add_subdirectory(component_tests)
endif()


if(NOT CMAKE_INSTALL_BINDIR)
    include(GNUInstallDirs)
endif()


#-------------------
# compiler settings
#-------------------
if(NOT ENABLE_WEBASSEMBLY)
    if (UNIX AND NOT APPLE)
        add_compile_options(-Wuseless-cast # warn if you perform a cast to the same type
                            -Werror # Make the specified warning into an error.
        )
    endif()
endif()

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall
                        -Wextra # reasonable and standard
                        -Wshadow # warn the user if a variable declaration shadows one from a
                                 # parent context
                        -Wnon-virtual-dtor # warn the user if a class with virtual functions has a
                                           # non-virtual destructor. This helps catch hard to
                                           # track down memory errors
                        -Wold-style-cast # warn for c-style casts
                        -Wcast-align # warn for potential performance problem casts
                        -Wunused # warn on anything being unused
                        -Woverloaded-virtual # warn if you overload (not override) a virtual
                                             # function
                        -Wpedantic # warn if non-standard C++ is used
                        -Wconversion # warn on type conversions that may lose data
#                        -Wsign-conversion # warn on sign conversions
#                        -Wmisleading-indentation # warn if identation implies blocks where blocks
                                                  # do not exist
#                        -Wduplicated-cond # warn if if / else chain has duplicated conditions
#                        -Wduplicated-branches # warn if if / else branches have duplicated code
#                        -Wlogical-op # warn about logical operations being used where bitwise were
                                     # probably wanted
#                        -Wnull-dereference # warn if a null dereference is detected
                        -Wdouble-promotion # warn if float is implicit promoted to double
                        -Wformat=2 # warn on security issues around functions that format output
                                   # (ie printf)
#                        -Winline # Warn if a function that is declared as inline cannot be inlined.
        )
endif()


#-------------------
# 3rd-party
#-------------------
include_directories(3rd-party/json-${NLOHMANN_VERSION}/include)
include_directories(3rd-party/plog-${PLOG_VERSION}/include)


#-------------------
# subdirectories
#-------------------
add_subdirectory(apps)
add_subdirectory(libs)



#-------------------
# add custom targets
#-------------------
if (ENABLE_TESTS)
    if (MSVC)
        add_custom_target(unit-tests
            COMMAND ./tests/${CMAKE_BUILD_TYPE}/run_gtests.exe "${CMAKE_CURRENT_LIST_DIR}/assets/bbc_one.ts"
        )
    else()
        add_custom_target(unit-tests
            COMMAND ./tests/run_gtests "${CMAKE_CURRENT_LIST_DIR}/assets/bbc_one.ts"
        )
    endif(MSVC)

    add_custom_target(benchmark-tests
        COMMAND ./bench/Parser_Benchmark
    )
endif(ENABLE_TESTS)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("mpeg2tsConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/mpeg2tsConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/mpeg2tsConfigVersion.cmake
        DESTINATION lib/cmake/${PROJECT_NAME}
)

#-------------------
# samples
#-------------------
if(BUILD_SAMPLES)
    add_subdirectory(samples/TsUtilities)
endif(BUILD_SAMPLES)

#-------------------
# CPack
#-------------------
include(CPack)
