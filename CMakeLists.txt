cmake_minimum_required(VERSION 3.10)
project(ts_lib)

set(CMAKE_CXX_STANDARD 11)
if (MSVC)
    add_compile_options(/W4)
else()    
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -Winline)
endif()

## project
include_directories(include)
include_directories(include/h264)
include_directories(include/mpeg2vid)
include_directories(include/public)
include_directories(tests)

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
        include/Types.h
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

# shared lib
add_library(mpeg2ts SHARED
        ${TsLibSourcesList})
# shared libraries need PIC
set_property(TARGET mpeg2ts PROPERTY POSITION_INDEPENDENT_CODE 1)

# static lib
add_library(mpeg2ts_static STATIC
        ${TsLibSourcesList})

# tsparser executable
add_executable(tsparser
        src/main.cc
        ${3rdPartySources})

target_link_libraries(tsparser mpeg2ts_static)