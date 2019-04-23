cmake_minimum_required(VERSION 3.12)

project(sample_tsutilities
    VERSION 0.0.1
    DESCRIPTION "Sample TsUtilities"
    LANGUAGES CXX
)

add_executable(${PROJECT_NAME})

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE mpeg2ts)
