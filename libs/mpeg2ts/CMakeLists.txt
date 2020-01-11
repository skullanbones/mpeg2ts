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
#*******************************************************************/
cmake_minimum_required(VERSION 3.12)

project(
    mpeg2ts
    VERSION 0.5.0
    DESCRIPTION "MPEG-2 Parsing lib of transport streams"
    LANGUAGES CXX
)

#-------------------
# CMake generated files
#-------------------
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/mpeg2ts_version.h.in mpeg2ts_version.h @ONLY)

#-------------------
# shared / static lib
# use cmake -DBUILD_SHARED_LIBS=YES .. to build shared libs
#-------------------
add_library(${PROJECT_NAME})

# shared libraries need PIC
if (BUILD_SHARED_LIBS)
    set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE 1)
endif()

# Exporter for DLLs
include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/.> # For CMake generated files: mpeg2ts_version.h , mpeg2ts_export.h
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME ${PROJECT_NAME}
    CXX_LINK_WHAT_YOU_USE TRUE
    DEBUG_POSTFIX "-d"
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/EsParser.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GetBits.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GetBits.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JsonSettings.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JsonSettings.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Logging.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PesPacket.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PsiTables.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TsDemuxer.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TsPacketInfo.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TsParser.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TsParser.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TsStatistics.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TsStatistics.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TsUtilities.cc
)

add_dependencies(${PROJECT_NAME} plog)
add_dependencies(${PROJECT_NAME} json)
