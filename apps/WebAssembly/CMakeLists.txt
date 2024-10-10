#*****************************************************************
#
#    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
#
#    mpeg2ts - mpeg2ts apps
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
project(
    a.out
    VERSION 1.0.0
    DESCRIPTION "Webassembly main"
    LANGUAGES CXX
)
else()
project(
    a.out
    VERSION 1.0.0
    LANGUAGES CXX
)
endif()

add_definitions(-std=c++11 -O3)

add_executable(
    ${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc
)

#if(JS_ONLY)
#    message(STATUS "Setting compilation target to native JavaScript")
#    set(CMAKE_EXECUTABLE_SUFFIX ".js")
#    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s EXPORTED_FUNCTIONS='[_webAsmEntryPoint]'")
#else(JS_ONLY)
#    message(STATUS "Setting compilation target to WASM")
#    set(CMAKE_EXECUTABLE_SUFFIX ".wasm")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-std=c++11 -O3 -s WASM=1 -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1 -s EXTRA_EXPORTED_RUNTIME_METHODS='[cwrap, ccall]'" )
#EXPORTED_FUNCTIONS='[_webAsmEntryPoint]'
#endif(JS_ONLY)


target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/mpeg2ts/src># For TsParser.h
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/mpeg2codec/src># For Mpeg2VideoParser.h
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/h264codec/src># For H264Parser.h
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        mpeg2ts
        mpeg2codec
        h264codec
)
