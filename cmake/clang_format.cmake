cmake_minimum_required(VERSION 3.12)

set(CLANG_VERSION 7)

find_program(
    CLANG_FORMAT_EXE
    NAMES "clang-format-${CLANG_VERSION}"
    DOC "Path to clang-format executable"
)

if(NOT CLANG_FORMAT_EXE)
    message(STATUS "clang-format not found on your system. Bailing out...")
    return()
else()
    message(STATUS "clang-format found: ${CLANG_FORMAT_EXE}")
    set(DO_CLANG_FORMAT "${CLANG_FORMAT_EXE}" "-i -style=file")
endif()


# get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h *.cc)

list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "3rd-party") 
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "gtest")
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "build")

#message("ALL_SOURCE_FILES " ${ALL_SOURCE_FILES})

add_custom_target(
        clang-format-${CLANG_VERSION}
        COMMAND ${CLANG_FORMAT_EXE}
        -style=LLVM
        -i
        ${ALL_SOURCE_FILES}
)
