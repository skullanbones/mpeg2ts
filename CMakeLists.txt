cmake_minimum_required(VERSION 3.10)
project(ts_lib)

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
                        -Winline # Warn if a function that is declared as inline cannot be inlined.
                        -Werror # Make the specified warning into an error.
        )
endif()

#-------------------
# includes & sources
#-------------------
## project
include_directories(include)

## 3rd-party
include_directories(3rd-party/nlohmann/include)
include_directories(3rd-party/plog-1.1.4/include)

set(    TsLibSourcesList
        include/public/mpeg2ts.h
        include/public/Ts_IEC13818-1.h
        include/public/TsUtilities.h
        include/h264/H264Parser.h
        include/mpeg2vid/Mpeg2VideoParser.h
        include/EsParser.h
        include/GetBits.h
        include/JsonSettings.h
        include/Logging.h
        include/TsParser.h
        include/TsStatistics.h
        src/h264/H264Parser.cc
        src/mpeg2vid/Mpeg2VideoParser.cc
        src/GetBits.cc
        src/JsonSettings.cc
        src/PesPacket.cc
        src/PsiTables.cc
        src/TsDemuxer.cc
        src/TsPacketInfo.cc
        src/TsParser.cc
        src/TsStatistics.cc
        src/TsUtilities.cc)

set(    3rdPartySources
        3rd-party/nlohmann/include/json.hpp
        3rd-party/plog-1.1.4/include/plog/Appenders/AndroidAppender.h
        3rd-party/plog-1.1.4/include/plog/Appenders/ColorConsoleAppender.h
        3rd-party/plog-1.1.4/include/plog/Appenders/ConsoleAppender.h
        3rd-party/plog-1.1.4/include/plog/Appenders/DebugOutputAppender.h
        3rd-party/plog-1.1.4/include/plog/Appenders/EventLogAppender.h
        3rd-party/plog-1.1.4/include/plog/Appenders/IAppender.h
        3rd-party/plog-1.1.4/include/plog/Appenders/RollingFileAppender.h
        3rd-party/plog-1.1.4/include/plog/Converters/NativeEOLConverter.h
        3rd-party/plog-1.1.4/include/plog/Converters/UTF8Converter.h
        3rd-party/plog-1.1.4/include/plog/Formatters/CsvFormatter.h
        3rd-party/plog-1.1.4/include/plog/Formatters/FuncMessageFormatter.h
        3rd-party/plog-1.1.4/include/plog/Formatters/MessageOnlyFormatter.h
        3rd-party/plog-1.1.4/include/plog/Formatters/TxtFormatter.h
        3rd-party/plog-1.1.4/include/plog/Init.h
        3rd-party/plog-1.1.4/include/plog/Log.h
        3rd-party/plog-1.1.4/include/plog/Logger.h
        3rd-party/plog-1.1.4/include/plog/Record.h
        3rd-party/plog-1.1.4/include/plog/Severity.h
        3rd-party/plog-1.1.4/include/plog/Util.h
        3rd-party/plog-1.1.4/include/plog/WinApi.h)

#-------------------
# static lib
#-------------------
add_library(mpeg2ts_static STATIC
        ${3rdPartySources}
        ${TsLibSourcesList})

#-------------------
# tsparser executable
#-------------------
add_executable(tsparser
        src/main.cc
        ${3rdPartySources})

target_link_libraries(tsparser mpeg2ts_static)        

#-------------------
# shared lib
#-------------------
add_library(mpeg2ts SHARED
        ${TsLibSourcesList}
        ${3rdPartySources})
# shared libraries need PIC
set_property(TARGET mpeg2ts PROPERTY POSITION_INDEPENDENT_CODE 1)
