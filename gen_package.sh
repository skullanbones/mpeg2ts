#!/bin/bash
# Script to package both debug and release build with CPack

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
