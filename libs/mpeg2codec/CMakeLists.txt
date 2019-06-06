cmake_minimum_required(VERSION 3.12)

project(
    mpeg2codec
    VERSION 0.0.1
    DESCRIPTION "MPEG2 Codec parser lib"
    LANGUAGES CXX
)

add_library(${PROJECT_NAME})

# shared libraries need PIC
if (BUILD_SHARED_LIBS)
    set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE 1)
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/.>  # For CMake generated files: mpeg2ts_version.h , mpeg2ts_export.h
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

set_target_properties(
    ${PROJECT_NAME}
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
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Mpeg2Codec.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Mpeg2VideoParser.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Mpeg2VideoParser.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Mpeg2Codec.cc
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        mpeg2ts # only for EsParser.h include
)

add_dependencies(${PROJECT_NAME} plog)
add_dependencies(${PROJECT_NAME} json)