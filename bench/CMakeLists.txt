cmake_minimum_required(VERSION 3.10)

if(NOT USE_DOCKER)
  message("NOT using Docker! Downloading & configuring google benchmark...")

  set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Disable GTest")
  # Build release version
  set(DCMAKE_BUILD_TYPE RELEASE)
  # If you want to self-test benchmark lib too, turn me ON
  #
  set(BENCHMARK_ENABLE_TESTING OFF)

  # Download and unpack benchmark at configure time
  configure_file(CMakeLists.txt.in benchmark-download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/benchmark-download )
  if(result)
    message(FATAL_ERROR "CMake step for benchmark failed: ${result}")
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/benchmark-download )
  if(result)
    message(FATAL_ERROR "Build step for benchmark failed: ${result}")
  endif()


  # Prevent overriding the parent project's compiler/linker
  # settings on Windows
  #set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Add benchmark directly to our build. This defines
  # the gtest and gtest_main targets.
  add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/benchmark-src
                   ${CMAKE_CURRENT_BINARY_DIR}/benchmark-build
                   EXCLUDE_FROM_ALL)

  # The gtest/gtest_main targets carry header search path
  # dependencies automatically when using CMake 2.8.11 or
  # later. Otherwise we have to add them here ourselves.
  if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
  endif()
endif(NOT USE_DOCKER)

#-------------------------------------------------------------

enable_testing()

#-------------------
# tests
#-------------------

file(GLOB_RECURSE ALL_BENCH_CPP *.cpp)

foreach(ONE_BENCH_CPP ${ALL_BENCH_CPP})

   get_filename_component(ONE_BENCH_EXEC ${ONE_BENCH_CPP} NAME_WE)

   # Avoid name collision 
   set(TARGET_NAME Bench_${ONE_BENCH_EXEC})

   add_executable(${TARGET_NAME} ${ONE_BENCH_CPP})

   target_include_directories(${TARGET_NAME} PRIVATE ../include)
  target_include_directories(${TARGET_NAME} PRIVATE ../src)
  target_include_directories(${TARGET_NAME} PRIVATE ../tests)

  target_compile_features(${TARGET_NAME} PRIVATE cxx_std_11)

   set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${ONE_BENCH_EXEC}) 
   find_package(Threads REQUIRED)
   target_link_libraries(${TARGET_NAME}
   PRIVATE
    Threads::Threads
    mpeg2ts
    benchmark
    ${CMAKE_THREAD_LIBS_INIT})

   # If you want to run benchmarks with the "make test" command, uncomment me
   add_test(${TARGET_NAME} ${ONE_BENCH_EXEC})
endforeach()
