cmake_minimum_required(VERSION 3.12)
set(PROJECT_NAME mpeg2ts)
set(EXPORT_NAME mpeg2ts)
set(STATIC_LIB ${PROJECT_NAME}_static)
set(SHARED_LIB ${PROJECT_NAME})
project(${PROJECT_NAME}
        VERSION 0.2.1
        DESCRIPTION "MPEG-2 Parsing of Video streams"
        LANGUAGES CXX)

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
# sub modules
#-------------------
set(BENCHMARK_ENABLE_TESTING OFF)
add_subdirectory (tests)
add_subdirectory (bench)

set(CMAKE_CXX_STANDARD 11)
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
                        -Wuseless-cast # warn if you perform a cast to the same type
                        -Wdouble-promotion # warn if float is implicit promoted to double
                        -Wformat=2 # warn on security issues around functions that format output
                                   # (ie printf)
#                        -Winline # Warn if a function that is declared as inline cannot be inlined.
                        -Werror # Make the specified warning into an error.
        )
endif()


#-------------------
# 3rd-party
#-------------------
set(PLOG_VERSION 1.1.4)
set(NLOHMANN_VERSION 3.5.0)

include_directories(3rd-party/nlohmann-${NLOHMANN_VERSION}/include)
include_directories(3rd-party/plog-${PLOG_VERSION}/include)

#-------------------
# static lib
#-------------------
add_library(${STATIC_LIB} STATIC)

#-------------------
# sources
#-------------------
target_sources(
        ${STATIC_LIB}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/h264/H264Parser.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mpeg2vid/Mpeg2VideoParser.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GetBits.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JsonSettings.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PesPacket.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PsiTables.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TsDemuxer.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TsPacketInfo.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TsParser.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TsStatistics.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TsUtilities.cc)

set(    3rdPartySources
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/nlohmann-${NLOHMANN_VERSION}/include/nlohmann/json.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Appenders/AndroidAppender.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Appenders/ColorConsoleAppender.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Appenders/ConsoleAppender.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Appenders/DebugOutputAppender.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Appenders/EventLogAppender.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Appenders/IAppender.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Appenders/RollingFileAppender.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Converters/NativeEOLConverter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Converters/UTF8Converter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Formatters/CsvFormatter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Formatters/FuncMessageFormatter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Formatters/MessageOnlyFormatter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Formatters/TxtFormatter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Init.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Log.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Logger.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Record.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Severity.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/Util.h
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/plog-${PLOG_VERSION}/include/plog/WinApi.h)


#-------------------
# includes & sources
#-------------------
## project
target_include_directories(${STATIC_LIB}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(${STATIC_LIB}
    PROPERTIES
        DEBUG_POSTFIX "-d"
)


#-------------------
# tsparser executable
#-------------------
add_executable(tsparser
        src/main.cc)

target_include_directories(tsparser
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(tsparser ${STATIC_LIB})

set_target_properties(tsparser
    PROPERTIES
    VERSION ${PROJECT_VERSION}
        DEBUG_POSTFIX "-d"
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/mpeg2ts_version.h.in mpeg2ts_version.h @ONLY)
target_include_directories(tsparser PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")


#-------------------
# shared lib
#-------------------
add_library(${SHARED_LIB} SHARED)
# shared libraries need PIC
set_property(TARGET ${SHARED_LIB} PROPERTY POSITION_INDEPENDENT_CODE 1)

target_include_directories(${SHARED_LIB}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(${SHARED_LIB}
  PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME ${EXPORT_NAME}
    CXX_LINK_WHAT_YOU_USE TRUE
    DEBUG_POSTFIX "-d"
)

target_sources(
        ${SHARED_LIB}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/public/mpeg2ts.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/public/Ts_IEC13818-1.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/public/TsUtilities.h
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/h264/H264Parser.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mpeg2vid/Mpeg2VideoParser.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/EsParser.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/GetBits.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/JsonSettings.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Logging.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/TsParser.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/TsStatistics.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/h264/H264Parser.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mpeg2vid/Mpeg2VideoParser.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GetBits.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JsonSettings.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PesPacket.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PsiTables.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TsDemuxer.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TsPacketInfo.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TsParser.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TsStatistics.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TsUtilities.cc)

#-------------------
# add custom targets
#-------------------
add_custom_target(unit-tests
     COMMAND ./tests/run_gtests
)

add_custom_target(benchmark-tests
     COMMAND ./bench/Parser_Benchmark
)
