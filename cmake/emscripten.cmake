#*****************************************************************
#
#    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
#
#    mpeg2ts - mpeg2ts cmake
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
#*******************************************************************/
cmake_minimum_required(VERSION 3.10)

if(NOT ENABLE_WEBASSEMBLY)
    message(STATUS "    Not using emscripten!")
    return()
endif()

message(STATUS "    Using emscripten")

if(NOT EMSCRIPTEN_PREFIX)
    message(STATUS "    EMSCRIPTEN_PREFIX not defined!")
    if(DEFINED ENV{EMSCRIPTEN})
        file(TO_CMAKE_PATH "$ENV{EMSCRIPTEN}" EMSCRIPTEN_PREFIX)
        set(EMSCRIPTEN_PREFIX $ENV{EMSCRIPTEN})
    else()
        set(EMSCRIPTEN_PREFIX "/usr/lib/emscripten")
        message(WARNING "    Could not find Emscripten toolchain, using /usr/lib/emscripten instead!")
    endif()
endif()
