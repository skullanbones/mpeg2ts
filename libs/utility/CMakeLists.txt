
cmake_minimum_required(VERSION 3.5)

if(CMAKE_MINOR_VERSION GREATER 9)
project(
    utility
    VERSION 0.6.0
    DESCRIPTION "Common utility lib"
    LANGUAGES CXX
)
else()
project(
    utility
    VERSION 0.6.0
    LANGUAGES CXX
)
endif()

add_library(${PROJECT_NAME})

# shared libraries need PIC
# if (BUILD_SHARED_LIBS)
#     set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE 1)
# endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
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

if(CMAKE_MINOR_VERSION GREATER 7)
    target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
endif()

target_sources(
    ${PROJECT_NAME}
    # PUBLIC
    #     ${CMAKE_CURRENT_SOURCE_DIR}/include/GetBits.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GetBits.cc
)

install(TARGETS ${PROJECT_NAME} EXPORT mpeg2tsTargets)
