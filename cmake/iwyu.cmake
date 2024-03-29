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
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

option(USE_IWYU "Use include-what-you-use for static include analysis" OFF)

if (NOT USE_IWYU)
    message(STATUS "    Not using include-what-you-use! To enable it,  append -DUSE_IWYU=ON")
    return()
endif()

message(STATUS "    Using include-what-you-use")

find_program(
    iwyu_exe
    NAMES include-what-you-use iwyu
)

if(NOT iwyu_exe)
    message(WARNING "    Could not find the program include-what-you-use, baling out...")
    return()
else()
    message(STATUS "    Using include-what-you-use include analysis.")
endif()

set_property(TARGET mpeg2ts PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_exe})
