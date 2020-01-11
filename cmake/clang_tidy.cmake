#*****************************************************************
#
#    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
#
#    mpeg2ts - mpeg2ts cmake
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
cmake_minimum_required(VERSION 3.10)

option(USE_CLANG_TIDY "Use clang-tidy for static code analysis" OFF)

if(NOT USE_CLANG_TIDY)
    message(STATUS "    Not using clang-tidy!")
    return()
endif()

message(STATUS "    Using clang-tidy")

set(CLANG_VERSION 7)

find_program(
    CLANG_TIDY_EXE 
    NAMES "clang-tidy-${CLANG_VERSION}"
)

if(NOT CLANG_TIDY_EXE)
    message(WARNING "    clang-tidy not found bailing out...")
    return()
else()
    message(STATUS "    clang-tidy found: ${CLANG_TIDY_EXE}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
message(STATUS "    clang-tidy    : ON")
message(STATUS "    Program exe   : ${CLANG_TIDY_EXE}")
message(STATUS "    Extra flags   : ${CXX_CLANG_TIDY_EXTRA_FLAGS}")
if(NOT CXX_CLANG_TIDY_EXTRA_FLAGS)
  set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}")
else()
  set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "${CXX_CLANG_TIDY_EXTRA_FLAGS}")
endif()
message(STATUS "    Command         : ${DO_CLANG_TIDY}")


# get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h *.cc)

list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "3rd-party") 
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "gtest")
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "build")
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "Release")
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "Debug")

# src/*.cc -checks=* -- -std=c++11 -I/usr/include/c++/5/ -I./include

add_custom_target(
    clang-tidy
    COMMAND ${CLANG_TIDY_EXE}
    ${ALL_SOURCE_FILES}
    -checks=*
    -- -std=c++11
    -I/usr/include/c++/7 -I./include
    }
)
