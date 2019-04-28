cmake_minimum_required(VERSION 3.10)

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
    mpeg2codec
    h264codec
    ${CMAKE_THREAD_LIBS_INIT})

   # If you want to run benchmarks with the "make test" command, uncomment me
   #add_test(${TARGET_NAME} ${ONE_BENCH_EXEC})
endforeach()
