cmake_minimum_required(VERSION 3.12)


if(NOT EMSCRIPTEN_PREFIX)
    message("EMSCRIPTEN_PREFIX not defined!")
    if(DEFINED ENV{EMSCRIPTEN})
        file(TO_CMAKE_PATH "$ENV{EMSCRIPTEN}" EMSCRIPTEN_PREFIX)
        set(EMSCRIPTEN_PREFIX $ENV{EMSCRIPTEN})
    else()
        set(EMSCRIPTEN_PREFIX "/usr/lib/emscripten")
        message(WARNING "Could not find Emscripten toolchain, using /usr/lib/emscripten instead!")
    endif()
endif()
