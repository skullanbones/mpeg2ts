cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

option(USE_VALGRIND "Use Valgrind for dynamic profiling / memcheck analysis" ON)

if(NOT USE_VALGRIND)
    message(STATUS "Not using Valgrind!")
    return()
endif()

message(STATUS "Using Valgrind")

find_program(MEMORYCHECK_COMMAND NAMES valgrind)
set(MEMORYCHECK_COMMAND valgrind)
set(CTEST_MEMORYCHECK_COMMAND valgrind )
set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--tool=callgrind -v" ) 
set(CTEST_MEMORYCHECK_TYPE "Valgrind")
set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")
set(MEMORYCHECK_SUPPRESSIONS_FILE
    "${PROJECT_SOURCE_DIR}/tests/valgrind_suppress.txt")
message(
        STATUS
        "MemCheck will use ${MEMORYCHECK_COMMAND} ${MEMORYCHECK_COMMAND_OPTIONS} ${MEMORYCHECK_SUPPRESSIONS_FILE}"
)
message(
        STATUS
        "Read the Documentation for the meaning of these flags: http://valgrind.org/docs/manual/mc-manual.html"
)
set(USE_MEM_CHECK ON)
