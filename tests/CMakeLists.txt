#-------------------------------------------------------------
# Taken from https://github.com/google/googletest/tree/master/googletest#incorporating-into-an-existing-cmake-project
#-------------------------------------------------------------

if(NOT USE_DOCKER)
message("NOT using Docker! Downloading & configuring google test...")

  # Download and unpack googletest at configure time
  configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
  if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
  if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
  endif()

  # Prevent overriding the parent project's compiler/linker
  # settings on Windows
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Add googletest directly to our build. This defines
  # the gtest and gtest_main targets.
  add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                   ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                   EXCLUDE_FROM_ALL)

  # The gtest/gtest_main targets carry header search path
  # dependencies automatically when using CMake 2.8.11 or
  # later. Otherwise we have to add them here ourselves.
  if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
  endif()
endif(NOT USE_DOCKER)

#-------------------------------------------------------------
set(CMAKE_CXX_STANDARD 11)
enable_testing()

#-------------------
# includes & sources
#-------------------
set(TestSources
    GetBits_Tests.cc
    Mpeg2VideoParser_Tests.cc
    PsiTables_Tests.cc
    TsDemuxer_Tests.cc
    TsPacketTestData.h
    TsParser_Tests.cc
    TsStatistics_Tests.cc
    TsUtilities_Tests.cc
    H264Parser_Tests.cc
)

#-------------------
if(USE_DOCKER)
  # Locate GTest
  find_package(GTest REQUIRED)

  find_library(GMOCK_LIBRARY
                  NAMES gmock 
                        libgmock 
                        libgmock.a 
                  PATHS "${GTEST_DIR}"
                  PATH_SUFFIXES lib)

  include_directories(${GTEST_INCLUDE_DIRS})
endif(USE_DOCKER)

#-------------------
# tests
#-------------------
# Now simply link against gtest, gmock, gtest_main or gmock_main as needed. Eg
add_executable(run_gtests gtest_main.cc ${TestSources})
include_directories(../include)
if(USE_DOCKER)
  target_link_libraries(run_gtests ${GMOCK_LIBRARY} ${GTEST_LIBRARY} pthread)
else(USE_DOCKER)
  target_link_libraries(run_gtests gmock)
endif(USE_DOCKER)
target_link_libraries(run_gtests mpeg2ts_static)
