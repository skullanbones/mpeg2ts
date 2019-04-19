REM Create releases for Windows with CMake

$echo off
REM *** Get source dir
set SRC_DIR=%~dp0
REM echo %SRC_DIR%

echo %SRC_DIR%

title Build mpeg2ts
echo Build mpeg2ts Windows


echo "1. Create Release/ folders for libraries"

set BUILD_RELEASE_DIR=Release
set BUILD_DEBUG_DIR=Debug

echo %BUILD_RELEASE_DIR% exist, removing %SRC_DIR%\%BUILD_RELEASE_DIR% and creating new one...

if exist %BUILD_RELEASE_DIR% (
    echo %BUILD_RELEASE_DIR% exist, removing %SRC_DIR%\%BUILD_RELEASE_DIR% and creating new one...
    rmdir /s /q %SRC_DIR%\%BUILD_RELEASE_DIR%
)

echo create new directory: %BUILD_RELEASE_DIR%
mkdir %BUILD_RELEASE_DIR%

cd %SRC_DIR%\%BUILD_RELEASE_DIR%
dir

echo "2.a Configure CMake"
echo Configuring for x86
set CMAKE_CONFIGURE_CMD=cmake -G "Visual Studio 15 2017" --arch "x86" -DENABLE_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF ..

echo CMake configure command:
echo %CMAKE_CONFIGURE_CMD%
%CMAKE_CONFIGURE_CMD%
if %errorlevel% neq 0 exit /b %errorlevel%

echo "2.b Build CMake"
set CMAKE_BUILD_CMD=cmake --build . --config Release --target mpeg2ts
echo CMake build command:
echo %CMAKE_BUILD_CMD%
%CMAKE_BUILD_CMD%
if %errorlevel% neq 0 exit /b %errorlevel%

cd %SRC_DIR%
echo "3. Create CPackConfig.cmake"
echo include("Release/CPackConfig.cmake") > CPackConfig.cmake
echo set(CPACK_INSTALL_CMAKE_PROJECTS "Release;mpeg2ts;ALL;/") >> CPackConfig.cmake

echo "4. Generate package"
cpack -G WIX --config CPackConfig.cmake
