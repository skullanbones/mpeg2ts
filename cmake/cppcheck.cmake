cmake_minimum_required(VERSION 3.12)

option(USE_CPPCHECK "Use cppcheck for static code analysis" ON)

if(NOT USE_CPPCHECK)
    message(STATUS "Not using cppcheck!")
    return()
endif()

message(STATUS "Using cppcheck")

find_program(
    CPPCHECK_EXE
    NAMES "cppcheck"
    DOC "Path to cppcheck executable"
)

if(NOT CPPCHECK_EXE)
    message(WARNING "cppcheck not found on your system. Bailing out...")
    return()
else()
    message(STATUS "    cppcheck found: ${CPPCHECK_EXE}")
endif()


# get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h *.cc)

list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "3rd-party") 
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "gtest")
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "build")

add_custom_target(
    cppcheck
    COMMAND ${CPPCHECK_EXE}
    --enable=all
    --std=c++11
    --verbose
    --quiet
    ${ALL_SOURCE_FILES}
)
