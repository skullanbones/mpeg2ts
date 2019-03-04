#!/bin/bash
# Script to package both debug and release build with CPack

echo "1. Create debug/ release/ folder"

mkdir debug
mkdir release

echo "2 .Configure and build /debug"
cd debug
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=install ..
make -j $(nproc)
cd ..

echo "3. Configure and build /release"
cd release
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install ..
make -j $(nproc)
cd ..

echo "4. Create CPackConfig.cmake"
echo 'include("release/CPackConfig.cmake")
set(CPACK_INSTALL_CMAKE_PROJECTS
    "debug;mpeg2ts;ALL;/"
    "release;mpeg2ts;ALL;/"
)' > CPackConfig.cmake

echo "5. Generate package"
cpack --config CPackConfig.cmake
