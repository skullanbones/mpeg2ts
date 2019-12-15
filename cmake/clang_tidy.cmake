cmake_minimum_required(VERSION 3.12)

option(USE_CLANG_TIDY "Use clang-tidy for static code analysis" ON)

if(NOT USE_CLANG_TIDY)
    message(STATUS "Not using clang-tidy!")
    return()
endif()

message(STATUS "Using cppcheck")

find_program(
    CLANG_TIDY_EXE 
    NAMES "clang-tidy-7"
)

if(NOT CLANG_TIDY_EXE)
    message(WARNING "clang-tidy not found bailing out...")
    return()
else()
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
message(STATUS "clang-tidy    : ON")
message(STATUS "Program exe   : ${CLANG_TIDY_EXE}")
message(STATUS "Extra flags   : ${CXX_CLANG_TIDY_EXTRA_FLAGS}")
if(NOT CXX_CLANG_TIDY_EXTRA_FLAGS)
  set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}")
else()
  set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "${CXX_CLANG_TIDY_EXTRA_FLAGS}")
endif()
message(STATUS "Command         : ${DO_CLANG_TIDY}")


# get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h *.cc)

list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "3rd-party") 
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "gtest")
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "build")

# src/*.cc -checks=* -- -std=c++11 -I/usr/include/c++/5/ -I./include

add_custom_target(
        clang-tidy
        COMMAND ${CLANG_TIDY_EXE}
        ${ALL_SOURCE_FILES}
        -checks=*
        -- -std=c++11
        -I/usr/include/c++/7 -I./include
        }
)
