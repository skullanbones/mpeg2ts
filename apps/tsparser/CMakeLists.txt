cmake_minimum_required(VERSION 3.12)

project(
    tsparser
    VERSION 0.4.0
    DESCRIPTION "MPEG-2 CLI Parsing of transport streams"
    LANGUAGES CXX
)

add_executable(tsparser)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
        src/main.cc
 )

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include/h264
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include/mpeg2vid
        ${CMAKE_CURRENT_BINARY_DIR}/.  # For CMake generated files like mpeg2ts_version.h
)

target_link_libraries(
    ${PROJECT_NAME} 
    PRIVATE 
        mpeg2ts
        mpeg2codec
        h264codec
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    DEBUG_POSTFIX "-d"
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
