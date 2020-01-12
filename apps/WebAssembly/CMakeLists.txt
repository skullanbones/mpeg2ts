cmake_minimum_required(VERSION 3.10)

project(
    a.out
    VERSION 0.0.1
    DESCRIPTION "Webassembly main"
    LANGUAGES CXX
)

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
