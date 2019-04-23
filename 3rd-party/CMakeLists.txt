cmake_minimum_required(VERSION 3.0)
project(3rd-party)
set(PLOG_VERSION 1.1.4)
set(NLOHMANN_VERSION 3.5.0)


# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

include(ExternalProject)

############# plog

ExternalProject_Add(
    plog
    URL ${CMAKE_CURRENT_SOURCE_DIR}/plog-${PLOG_VERSION}.tar.gz
    INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/plog-${PLOG_VERSION}
    CMAKE_ARGS  -D CMAKE_BUILD_TYPE=Release
                -D CMAKE_INSTALL_PREFIX=<INSTALL_DIR>
                -D BUILD_SAMPLES=OFF
)

############# nlohmann/json

ExternalProject_Add(
    json
    URL ${CMAKE_CURRENT_SOURCE_DIR}/json-${NLOHMANN_VERSION}.tar.gz
    INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/json-${NLOHMANN_VERSION}
    CMAKE_ARGS  -D CMAKE_BUILD_TYPE=Release
                -D BUILD_TESTING=OFF
                -D CMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)


############ google test

#-------------------------------------------------------------
# Taken from https://github.com/google/googletest/tree/master/googletest#incorporating-into-an-existing-cmake-project
#-------------------------------------------------------------

if(NOT USE_DOCKER)
message("NOT using Docker! Downloading & configuring google test...")

  # Download and unpack googletest at configure time
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists_googletest.txt.in ${CMAKE_CURRENT_SOURCE_DIR}/googletest-download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/googletest-download )
  if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/googletest-download )
  if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
  endif()

  # Prevent overriding the parent project's compiler/linker
  # settings on Windows
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Add googletest directly to our build. This defines
  # the gtest and gtest_main targets.
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/googletest-src
                   ${CMAKE_CURRENT_SOURCE_DIR}/googletest-build
                   EXCLUDE_FROM_ALL)

  # The gtest/gtest_main targets carry header search path
  # dependencies automatically when using CMake 2.8.11 or
  # later. Otherwise we have to add them here ourselves.
  if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
  endif()
endif(NOT USE_DOCKER)


############ google benchmark


if(NOT USE_DOCKER)
  message("NOT using Docker! Downloading & configuring google benchmark...")

  set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Disable GTest")
  # Build release version
  set(DCMAKE_BUILD_TYPE RELEASE)
  # If you want to self-test benchmark lib too, turn me ON
  #
  set(BENCHMARK_ENABLE_TESTING OFF)

  # Download and unpack benchmark at configure time
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists_benchmark.txt.in ${CMAKE_CURRENT_SOURCE_DIR}/benchmark-download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/benchmark-download )
  if(result)
    message(FATAL_ERROR "CMake step for benchmark failed: ${result}")
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/benchmark-download )
  if(result)
    message(FATAL_ERROR "Build step for benchmark failed: ${result}")
  endif()


  # Prevent overriding the parent project's compiler/linker
  # settings on Windows
  #set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Add benchmark directly to our build. This defines
  # the gtest and gtest_main targets.
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/benchmark-src
                   ${CMAKE_CURRENT_SOURCE_DIR}/benchmark-build
                   EXCLUDE_FROM_ALL)

  # The gtest/gtest_main targets carry header search path
  # dependencies automatically when using CMake 2.8.11 or
  # later. Otherwise we have to add them here ourselves.
  if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
  endif()
endif(NOT USE_DOCKER)
