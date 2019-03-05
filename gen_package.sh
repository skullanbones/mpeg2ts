#!/bin/bash
# Script to package both debug and release build with CPack

echo "1. Create debug/ release/ folders for static and shared libraries"

build_types=(debug_static release_static debug_shared release_shared)

for i in ${build_types[@]}; do
    echo mkdir: $i
    mkdir $i
done


echo "2 .Configure and build /debug static lib"
cd debug_static
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=install ..
make -j $(nproc)
cd ..

echo "3 .Configure and build /debug shared lib"
cd debug_shared
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=install -DBUILD_SHARED_LIBS=YES ..
make -j $(nproc)
cd ..

echo "4. Configure and build /release static lib"
cd release_static
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install ..
make -j $(nproc)
cd ..

echo "5. Configure and build /release shared lib"
cd release_shared
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install -DBUILD_SHARED_LIBS=YES ..
make -j $(nproc)
cd ..

echo "6. Create CPackConfig.cmake"
echo 'include("release_static/CPackConfig.cmake")
set(CPACK_INSTALL_CMAKE_PROJECTS
    "debug_static;mpeg2ts;ALL;/"
    "release_static;mpeg2ts;ALL;/"
    "debug_shared;mpeg2ts;ALL;/"
    "release_shared;mpeg2ts;ALL;/"
)' > CPackConfig.cmake

echo "7. Generate package"
cpack --config CPackConfig.cmake
