#*****************************************************************
#
#    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
#
#    mpeg2ts - mpeg2ts tests
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
#*******************************************************************/
cmake_minimum_required(VERSION 3.5)

if(CMAKE_MINOR_VERSION GREATER 9)
include(GoogleTest) # Introduced in CMake 3.10
project(
    run_gtests
    VERSION 0.6.0
    DESCRIPTION "GTests for mpeg2ts lib"
    LANGUAGES CXX
)
else()
project(
    run_gtests
    VERSION 0.6.0
    LANGUAGES CXX
)
endif()


#-------------------
# Locate GTest
enable_testing()
find_package(GTest MODULE)
find_library(GMOCK_LIBRARY
             NAMES gmock
                   libgmock
                   libgmock.a
             PATHS "${GTEST_DIR}"
             PATH_SUFFIXES lib
)

include_directories(${GTEST_INCLUDE_DIRS})

if (NOT GTest_FOUND)
    message(WARNING "    Could NOT Find GTest")
endif()

#-------------------
# tests
#-------------------
# Now simply link against gtest, gmock, gtest_main or gmock_main as needed. Eg
add_executable(
    ${PROJECT_NAME}
    gtest_main.cc
)

target_compile_definitions(${PROJECT_NAME} PRIVATE MPEG2TS_DLL_EXPORTS)

if(CMAKE_MINOR_VERSION GREATER 7)
    target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
endif()

target_sources(${PROJECT_NAME}
    PRIVATE
        GetBits_Tests.cc
        Mpeg2VideoParser_Tests.cc
        PsiTables_Tests.cc
        TsDemuxer_Tests.cc
        TsPacketTestData.h
        TsParser_Tests.cc
        TsStatistics_Tests.cc
        TsUtilities_Tests.cc
        H264Parser_Tests.cc
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/.
        ${CMAKE_CURRENT_BINARY_DIR}/..
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/mpeg2ts/src># Testing private src/headers
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/h264codec/src># Testing private src/headers
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/mpeg2codec/src># Testing private src/headers
)

if(USE_DOCKER)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GMOCK_LIBRARY} ${GTEST_LIBRARY} pthread)
else(USE_DOCKER)
    find_package(Threads REQUIRED)
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
            Threads::Threads
            gmock
    )
endif(USE_DOCKER)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        mpeg2ts
        mpeg2codec
        h264codec
)


if(GTEST_HAVE_DISCOVERY)
  gtest_discover_tests(${PROJECT_NAME} TEST_PREFIX ${exportname}. TEST_LIST
                       ${PROJECT_NAME}_targets)
else()
    if(CMAKE_MINOR_VERSION GREATER 8)
    # For CMake 3.9 onwards
        gtest_add_tests(
            TARGET
            ${PROJECT_NAME}
            SOURCES
            ${TARGET_SOURCES}
            TEST_PREFIX
            ${exportname}.
            TEST_LIST
            ${PROJECT_NAME}_targets
        )
    endif()
endif()
