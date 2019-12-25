# MPEG-2 Transport Stream utility library
[![CircleCI](https://circleci.com/gh/skullanbones/mpeg2ts.svg?style=svg&circle-token=d25332617e91ffe0b3b0fd0ebd9ac73caa416f35)](https://circleci.com/gh/skullanbones/mpeg2ts)

A fast, cross-platform and modern C++ SDK for all your MPEG-2 transport stream media format needs following
international specification ISO/IEC 13818-1. The standard is also called H.222 including both TS and PS. The library is platform independent only using
C++11. Mpeg2ts has been tested on the following operating systems:
* Linux (Ubuntu 18.04 LTS and 16.04 LTS)
* Windows (Windows 10)
* Mac OS X (Sierra)

To simplify the build process on all these operating systems, CMake is being used.

Cross platform Artifacts:
```
Win32: mpeg2ts.dll mpeg2ts.lib
Linux: libmpeg2ts.so libmpeg2ts.a
Apple: libmpeg2ts.dylib libmpeg2ts.a
All OS: mpeg2ts.h
```
Sample Applications:
```
Win32: TsUtilities located under samples
(Note: You need copy it's dependency mpeg2ts.dll to where you
installed/built the sample_tsutilities.exe)
Linux: tsparser
```

## Requirements
C++11 is the minimal requirement. The library is written as platform independent code and tested on Mac OS X (Sierra), Ubuntu 16.04/18.04, Windows 10.

## Libs
In order to parse mpeg2 and h264 codecs, 2 extra libs are supplied. In total there are the 3 following libs:

| Lib name              | Meaning                     |
|----------------------------------|-----------------------------|
| libmpeg2ts.so         | mpeg2 transport stream parser
| libmpeg2codec.so      | mpeg2 codec parser
| libh264codec.so       | h264 codec parser

## SW Architecture
There are 2 sets of APIs, the core API called mpeg2ts under the same namespace and a utility API to facilitate the usage of the API for more convinient usage (namespace tsutil). These are the APIs:
* mpeg2ts.h    The core and fundamental API for all mpeg2ts usage
* TsUtilities.h High level API to simplify usage
![](images/Ts-lib_SW_Architecture.png)

## Settings
Edit the `settings.json` file to change log level, log file name and other properties. These are global settings for the mpeg2ts set of libraries. Log levels are:
```
VERBOSE
DEBUG
INFO
WARNING
ERROR
FATAL
NONE
```
where VERBOSE is the maximum log output as compared to NONE which generates no output. The default log output file is `mpeg2ts_log.csv` in csv style for easier use.

## Building
To simplify the crosscompile process we use CMake. Under Linux, Mac and Windows just do this:
```Bash
mkdir build
cd build/
cmake -DCMAKE_BUILD_TYPE=Debug|Release ..
make
```
You will get the following artifacts:
```Bash
libmpeg2ts.a
tsparser*
```
To also build the shared libraries you need to tell CMake to build those:
```Bash
cmake -DCMAKE_BUILD_TYPE=Debug|Release -DBUILD_SHARED_LIBS=YES ..
```
which result in `libmpeg2ts.so*` on Linux, `libmpeg2ts.dylib*` on Mac and `libmpeg2ts.dll*` on Windows.


If you wanna speed up the build you can type `cmake --build . -- -j$(nproc)` instead of `make` in the 4th command above.

## Installation
In order to install this library you can type:
```Bash
cmake -DCMAKE_INSTALL_PREFIX=../install ..
make -j $(nproc)
make install
```
and now you will find the installed libraries and headers in `install` directory. Omit `CMAKE_INSTALL_PREFIX` to install in system default `/usr/local/lib`.

## Packaging
To generate a package with both debug and release build type (Mac & Linux):
```Bash
./gen_package.sh
```
or on Windows
```Bash
./gen_package.bat
```
This will generate a package
```Bash
mpeg2ts-0.4.0-Linux.tar.gz   (Linux)
mpeg2ts-0.4.0-Darwin.tar.gz  (Apple)
mpeg2ts-0.4.0-win32.zip      (Windows)
```
for example containing only shared libs.

## Usage in other CMake projects
To find this package using CMake simply use find_package:
```Bash
find_package(mpeg2ts REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC mpeg2ts::mpeg2ts)
```

If you want to use mpeg2ts lib installation with your project you need to set the `CMAKE_PREFIX_PATH` to where mpeg2ts-lib is being installed if it wasn't installed host under your system (`/usr/local`).

## Tsparser
### How to run it
Type `make help` to see all make targets. To start runing the lib:
```
cd build/
make all
cd ..
./build/apps/tsparser/tsparser --input assets/test1.ts
```
Check help in command line (CLI):
```
./build/apps/tsparser/tsparser --help
```
Add option --write with the PES PID for writing PES packets to file.
```
./build/apps/tsparser/tsparser --write 2504 --input assets/bbc_one.ts
```
Just print PSI tables / PES header can be done by --pid option and the PID.
```
./build/apps/tsparser/tsparser --pid 258 --input assets/bbc_one.ts
```

## Docker
To simplify building mpeg2ts, building dependencies and tools have been bundled into a docker container. This image can be build locally or downloaded from docker hub. To start using docker commands, simplest is to source the tool

    source docker/docker-commands.sh

Check `docker/` README for more information.

## Tests
In order to run all tests just type (currently only available under Linux):
```Bash
make component-tests
make component-benchmark-tests
make unit-tests
```
This will run component tests, benchmark and unit-tests. Component tests are developed via `pytest` while the unit tests are via google tests/mocks (`gtest`). The different layers like API, and tsutilities are tested as a component (the whole library as such) while class functions are tested as a unit.

## Windows
Since CMake is used for cross platform build, you first need install CMake on your Windows system if you havent done that already.

### Visual Studio 2017
To open the CMake project under Windows you need at least Visual Studio 2017 with CMake support (make sure you installed CMake support). Open the root CMakeLists.txt project file and you are ready to build under Windows.

### VS Code
To open the CMake project by VS Code under Windows, just open the root folder. 
Select the supplied Kit (CMake & CMake Tools plugins are required) to simplify builds with predefined options. Existing kits are:

| Kit name                         | Meaning                     |
|----------------------------------|-----------------------------|
| Windows-SharedLib-VS2017-x64     | 64 bits build DLL on Windows, no tests
| Windows-SharedLib-VS2017-x86     | 32 bits build DLL on Windows, no tests
| Windows-Gtests-StaticLib-VS2017-x86 | 32 bits build gtests/benchmarch on Windows
| Windows-Gtests-StaticLib-VS2017-x64 | 64 bits build gtests/benchmarch on Windows

Currently there is a bug in gmock which only make it possible to link statically and not dynamically
with gmock hence the reason for the  `Windows-Gtests-StaticLib-VS2017-x86`.

### Visual Studio prior 2017
To cross compile for older VS versions, use powershell and cmake command line with a generator, for example:
```
cmake -G "Visual Studio 14 2015" --arch "x86" -DENABLE_TESTS=OFF -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=OFF ..
```

Possible VS generators are:

| Generators                       | Meaning                     |
|----------------------------------|-----------------------------|
| Visual Studio 15 2017 [arch]     | Generates Visual Studio 2017 project files.
| Visual Studio 14 2015 [arch]     | Generates Visual Studio 2015 project files.
| Visual Studio 12 2013 [arch]     | Generates Visual Studio 2013 project files.
| Visual Studio 11 2012 [arch]     | Generates Visual Studio 2012 project files.
| Visual Studio 10 2010 [arch]     | Generates Visual Studio 2010 project files.
| Visual Studio 9 2008 [arch]      | Generates Visual Studio 2008 project files.

where arch can be x86, Win64 or ARM. To build your new solution type:
```
cmake --build .
or
cmake --build . --target mpeg2ts
```

### Generate Release
Run the
```
./gen_package.bat
```
to create a new release on Windows via CMake and CPack.

## Continuous integration (CI)
For CI mpeg2ts use CircleCI which will automatically run all unit tests after a commit either
in a branch, pull-request or integration to master. You can check the status tests in any
branch by the portal:
[CircleCI](https://circleci.com/gh/skullanbones/mpeg2ts/tree/develop)

## Static code analysis
Use `make cppcheck` and `make clang-tidy`. This however requires one to have installed either `cppcheck` or `clang-tidy` on you host machine. If that is not the case, then we recommend using docker interactive via:

    source docker/docker-commands.sh
    docker-interactive
    docker@1ca...: make cppcheck

## Acronyms
| Abbreviation  | Meaning                             |
|---------------|-------------------------------------|
| API           | Application Program Interface       |
| CI            | Continuous Integration              |
| DVCS          | Distributed Version Control System  |
| IEC           | International Electrotechnical Commission  |
| ISO           | International Organization for Standardization  |
| MC            | Multicast                           |
| MPEG          | Moving Picture Experts Group        |
| OSS           | Open Source Software                |
| PS            | Program Stream                      |
| SDK           | Software Development Kit            |
| SW            | Software                            |
| TS            | Transport Stream                    |
| VCS           | Version Control System              |


## Technologies / Open Source Software (OSS)
| Technology    | Version minimal requirement         |
|---------------|-------------------------------------|
| C++           | C++11                               |
| Docker        | 17.x                                |
| CMake         | 3.11                                |
| GNU Make      | Ubuntu 16.04                        |
| GCC           | Ubuntu 16.04 (5.4)                  |
| Python        | Python 3.x                          |
| Git           | 2.x                                 |
| Google test   | 1.8.x                               |
