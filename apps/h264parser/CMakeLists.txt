#*****************************************************************
#
#    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
#
#    mpeg2ts - mpeg2ts tsparser
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
    h264parser
    VERSION 0.5.0
    DESCRIPTION "CLI Parsing of h264 stream"
    LANGUAGES CXX
)
else()
project(
    h264parser
    VERSION 0.5.0
    LANGUAGES CXX
)
endif()

add_executable(
    ${PROJECT_NAME}
    src/main.cc
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/mpeg2ts/src># For TsParser.h
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/mpeg2codec/src># For Mpeg2VideoParser.h
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/h264codec/src># For H264Parser.h
)

target_link_libraries(
    ${PROJECT_NAME} 
    PRIVATE 
        h264codec
        mpeg2ts # For GetBits() Should actually be moved out to its own lib
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    DEBUG_POSTFIX "-d"
)

if(CMAKE_MINOR_VERSION GREATER 7)
    target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11) # Available in CMake 3.8
endif()
