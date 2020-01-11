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

option(USE_CLANG_FORMAT "Use clang-format for formatting c++ files" ON)

if(NOT USE_CLANG_FORMAT)
    message(STATUS "Not using clang-tidy!")
    return()
endif()

message(STATUS "Using clang-format")

set(CLANG_VERSION 7)

find_program(
    CLANG_FORMAT_EXE
    NAMES "clang-format-${CLANG_VERSION}"
    DOC "Path to clang-format executable"
)

if(NOT CLANG_FORMAT_EXE)
    message(STATUS "    clang-format not found on your system. Bailing out...")
    return()
else()
    message(STATUS "    clang-format found: ${CLANG_FORMAT_EXE}")
    set(DO_CLANG_FORMAT "${CLANG_FORMAT_EXE}" "-i -style=file")
endif()


# get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h *.cc)

list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "3rd-party") 
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "gtest")
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "build")

add_custom_target(
    clang-format
    COMMAND ${CLANG_FORMAT_EXE}
    -style=file
    -i
    ${ALL_SOURCE_FILES}
)
