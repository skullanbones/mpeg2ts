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

option(USE_DOXYGEN "Use Doxygen for inline documentation." OFF)

if(NOT USE_DOXYGEN)
    message(STATUS "    Not using Doxygen!")
    return()
endif()

message(STATUS "    Using Doxygen")

# Require dot, treat the other components as optional
find_package(
    Doxygen 
    OPTIONAL_COMPONENTS dot mscgen dia
)

if(NOT DOXYGEN_FOUND)
    message(WARNING "    Doxygen not found on your system. Bailing out...")
    return()
else()
    message(STATUS "    doxygen found: ${DOXYGEN_EXECUTABLE}")

    doxygen_add_docs(
        doxygen
        ${PROJECT_SOURCE_DIR}/README.md ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/samples
        COMMENT "Generate doxygen pages"
    )
endif()
