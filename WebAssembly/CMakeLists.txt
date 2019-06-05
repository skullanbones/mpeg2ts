cmake_minimum_required(VERSION 3.12)

project(
    a.out
    VERSION 0.0.1
    DESCRIPTION "Webassembly main"
    LANGUAGES CXX
)

add_definitions(-std=c++11 -O3)

add_executable(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s WASM=1 -s BINARYEN_METHOD='native-wasm' -s EXPORTED_FUNCTIONS='[_webAsmEntryPoint]'")
#target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cc
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        mpeg2ts
        mpeg2codec
        h264codec
)
