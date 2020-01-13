#*****************************************************************
#
#    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
#
#    mpeg2ts - mpeg2ts bench
#
#    This file is part of mpeg2ts (Mpeg2 Transport Stream Library).
#
#    Unless you have obtained mpeg2ts under a different license,
#    this version of mpeg2ts is mpeg2ts|GPL.
#    Mpeg2ts|GPL is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License as
#    published by the Free Software Foundation; either version 2,
#    or (at your option) any later version.
#
#    Mpeg2ts|GPL is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with mpeg2ts|GPL; see the file COPYING. If not, write to
#    the Free Software Foundation, 59 Temple Place - Suite 330,
#    Boston, MA 02111-1307, USA.
#
#******************************************************************/
cmake_minimum_required(VERSION 3.5)

#-------------------
# tests
#-------------------

file(GLOB_RECURSE ALL_BENCH_CPP *.cpp)

foreach(ONE_BENCH_CPP ${ALL_BENCH_CPP})
    get_filename_component(ONE_BENCH_EXEC ${ONE_BENCH_CPP} NAME_WE)

    # Avoid name collision 
    set(TARGET_NAME Bench_${ONE_BENCH_EXEC})

    add_executable(${TARGET_NAME} ${ONE_BENCH_CPP})

    target_include_directories(${TARGET_NAME} 
        PRIVATE 
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/tests># For CodecTestData.h
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/mpeg2ts/src># For EsParser.h
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/h264codec/src># For H264Parser.h
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/mpeg2codec/src># For Mpeg2VideoParser.h
    )

    if(CMAKE_MINOR_VERSION GREATER 7)
        target_compile_features(${TARGET_NAME} PRIVATE cxx_std_11)
    endif()

    set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${ONE_BENCH_EXEC}) 
    find_package(Threads REQUIRED)
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            Threads::Threads
            mpeg2ts
            benchmark
            mpeg2codec
            h264codec
            ${CMAKE_THREAD_LIBS_INIT}
   )

   # If you want to run benchmarks with the "make test" command, uncomment me
   #add_test(${TARGET_NAME} ${ONE_BENCH_EXEC})
endforeach()
