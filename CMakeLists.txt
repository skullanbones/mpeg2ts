cmake_minimum_required(VERSION 3.12)

project(
    mpeg2ts
    VERSION 0.4.0
    DESCRIPTION "MPEG-2 Parsing of transport streams"
    LANGUAGES CXX
)

#-------------------
# global settings
#-------------------
if(EXISTS /.dockerenv)
    message("Detected running inside docker container. Using container dependencies instead.")
    set(USE_DOCKER ON) # To use dependencies from Docker instead of local build
else()
    set(USE_DOCKER OFF)
endif()

#-------------------
# modules
#-------------------
if(NOT "${CMAKE_CURRENT_LIST_DIR}/cmake" IN_LIST CMAKE_MODULE_PATH)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
endif()

include(clang_format)
include(cppcheck)
include(iwyu)
include(doxygen)


#-------------------
# sub modules
#-------------------
set(BENCHMARK_ENABLE_TESTING OFF)
option(ENABLE_TESTS "Build with tests" ON)
option(ENABLE_COMPONENT_TESTS "Build with component tests" ON)
option(BUILD_SAMPLES "Build samples" ON)
option(ENABLE_COVERAGE "Enable GCOV code coverage" OFF)

#-------------------
# gcov code coverage
#-------------------
if(UNIX AND ${ENABLE_COVERAGE} AND CMAKE_BUILD_TYPE MATCHES Debug)
    include(CodeCoverage)
    APPEND_COVERAGE_COMPILER_FLAGS()
    SETUP_TARGET_FOR_COVERAGE_LCOV(
        NAME coverage
        EXECUTABLE ./tests/run_gtests "${CMAKE_CURRENT_LIST_DIR}/assets/bbc_one.ts"
    )
endif()

if(ENABLE_TESTS)
    add_subdirectory(tests)
    add_subdirectory(bench)
endif()

if(MSVC OR APPLE)
    set(ENABLE_COMPONENT_TESTS OFF)
endif()
if(ENABLE_COMPONENT_TESTS)
    add_subdirectory(component_tests)
endif()

#-------------------
# third-party
#-------------------
add_subdirectory(3rd-party)


if(NOT CMAKE_INSTALL_BINDIR)
    include(GNUInstallDirs)
endif()


#-------------------
# compiler settings
#-------------------
if (UNIX AND NOT APPLE)
    add_compile_options(-Wuseless-cast # warn if you perform a cast to the same type
                        -Werror # Make the specified warning into an error.
    )
endif()

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall
                        -Wextra # reasonable and standard
                        -Wshadow # warn the user if a variable declaration shadows one from a
                                 # parent context
                        -Wnon-virtual-dtor # warn the user if a class with virtual functions has a
                                           # non-virtual destructor. This helps catch hard to
                                           # track down memory errors
                        -Wold-style-cast # warn for c-style casts
                        -Wcast-align # warn for potential performance problem casts
                        -Wunused # warn on anything being unused
                        -Woverloaded-virtual # warn if you overload (not override) a virtual
                                             # function
                        -Wpedantic # warn if non-standard C++ is used
                        -Wconversion # warn on type conversions that may lose data
#                        -Wsign-conversion # warn on sign conversions
#                        -Wmisleading-indentation # warn if identation implies blocks where blocks
                                                  # do not exist
#                        -Wduplicated-cond # warn if if / else chain has duplicated conditions
#                        -Wduplicated-branches # warn if if / else branches have duplicated code
#                        -Wlogical-op # warn about logical operations being used where bitwise were
                                     # probably wanted
#                        -Wnull-dereference # warn if a null dereference is detected
                        -Wdouble-promotion # warn if float is implicit promoted to double
                        -Wformat=2 # warn on security issues around functions that format output
                                   # (ie printf)
#                        -Winline # Warn if a function that is declared as inline cannot be inlined.
        )
endif()


#-------------------
# 3rd-party
#-------------------
set(PLOG_VERSION 1.1.4)
set(NLOHMANN_VERSION 3.5.0)

include_directories(3rd-party/json-${NLOHMANN_VERSION}/include)
include_directories(3rd-party/plog-${PLOG_VERSION}/include)

#-------------------
# CMake generated files
#-------------------
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/mpeg2ts_version.h.in mpeg2ts_version.h @ONLY)

#-------------------
# tsparser executable
#-------------------
# only build tsparser on UNIX (getopt only exist in Linux)
if (UNIX)
add_executable(tsparser)

target_sources(
    tsparser
    PRIVATE
        src/main.cc
 )

target_include_directories(
    tsparser
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/h264
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mpeg2vid
        ${CMAKE_CURRENT_BINARY_DIR}/.  # For CMake generated files like mpeg2ts_version.h
)

target_link_libraries(
    tsparser 
    PRIVATE 
        ${PROJECT_NAME}
        mpeg2codec
        h264codec
)

set_target_properties(
    tsparser
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    DEBUG_POSTFIX "-d"
)

target_compile_features(tsparser PRIVATE cxx_std_11)
endif(UNIX)

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

#-------------------
# add custom targets
#-------------------
add_custom_target(unit-tests
    COMMAND ./tests/run_gtests "${CMAKE_CURRENT_LIST_DIR}/assets/bbc_one.ts"
)

add_custom_target(benchmark-tests
    COMMAND ./bench/Parser_Benchmark
)

#-------------------
# installation
#-------------------
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mpeg2ts.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Ts_IEC13818-1.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/TsUtilities.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/settings.json
        ${CMAKE_CURRENT_BINARY_DIR}/mpeg2ts_export.h
        DESTINATION include/${PROJECT_NAME}
)

install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("mpeg2tsConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/mpeg2tsConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/mpeg2tsConfigVersion.cmake
        DESTINATION lib/cmake/${PROJECT_NAME}
)

#-------------------
# samples
#-------------------
if(BUILD_SAMPLES)
    add_subdirectory(samples/TsUtilities)
endif(BUILD_SAMPLES)




#-------------------
# CPack
#-------------------
include(CPack)
