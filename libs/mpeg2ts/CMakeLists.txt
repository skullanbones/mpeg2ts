cmake_minimum_required(VERSION 3.12)

project(
    mpeg2ts
    VERSION 0.4.0
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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/h264>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/mpeg2vid>
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

#-------------------
# mpeg2 codec lib
#-------------------
add_library(mpeg2codec)

# shared libraries need PIC
if (BUILD_SHARED_LIBS)
    set_property(TARGET mpeg2codec PROPERTY POSITION_INDEPENDENT_CODE 1)
endif()

target_include_directories(mpeg2codec
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/mpeg2codec>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/.>  # For CMake generated files: mpeg2ts_version.h , mpeg2ts_export.h
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/mpeg2vid>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

set_target_properties(
    mpeg2codec
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME mpeg2codec
    CXX_LINK_WHAT_YOU_USE TRUE
    DEBUG_POSTFIX "-d"
)

target_compile_features(mpeg2codec PRIVATE cxx_std_11)

target_sources(
    mpeg2codec
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mpeg2vid/Mpeg2Codec.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mpeg2vid/Mpeg2VideoParser.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mpeg2vid/Mpeg2VideoParser.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mpeg2vid/Mpeg2Codec.cc
)

add_dependencies(mpeg2codec plog)
add_dependencies(mpeg2codec json)

#-------------------
# h264 codec lib
#-------------------
add_library(h264codec)

# shared libraries need PIC
if (BUILD_SHARED_LIBS)
    set_property(TARGET h264codec PROPERTY POSITION_INDEPENDENT_CODE 1)
endif()

target_include_directories(h264codec
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/h264>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/.> # For CMake generated files: mpeg2ts_version.h , mpeg2ts_export.h
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/h264>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

set_target_properties(h264codec
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME h264codec
    CXX_LINK_WHAT_YOU_USE TRUE
    DEBUG_POSTFIX "-d"
)

target_compile_features(h264codec PRIVATE cxx_std_11)

target_sources(
    h264codec
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/h264/H264Codec.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/h264/H264Parser.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/h264/H264Parser.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/h264/H264Codec.cc
)

add_dependencies(h264codec plog)
add_dependencies(h264codec json)
