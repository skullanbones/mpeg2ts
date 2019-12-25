#!/bin/bash
# Script to package both debug and release build with CPack
#*****************************************************************
#
#    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
#
#    mpeg2ts - mpeg2ts gen_package.sh
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

echo "1. Create debug/ release/ folders for shared libraries"

build_types=(Debug Release)

for i in ${build_types[@]}; do
    echo mkdir: $i
    mkdir $i
done

echo "2. Configure CMake"
for type in ${build_types[@]}; do
    echo "Configure and build / $type shared lib"
    cd $type
    cmake -DCMAKE_BUILD_TYPE=$type -DBUILD_SHARED_LIBS=YES -DENABLE_TESTS=OFF ..
    make -j $(nproc)
    cd ..
done


echo "3. Create CPackConfig.cmake"
echo 'include("Release/CPackConfig.cmake")
set(CPACK_INSTALL_CMAKE_PROJECTS
    "Debug;mpeg2ts;ALL;/"
    "Release;mpeg2ts;ALL;/"
)' > CPackConfig.cmake

echo "4. Generate package"
cpack --config CPackConfig.cmake
