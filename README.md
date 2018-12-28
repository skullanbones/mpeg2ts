# MPEG-2 Transport Stream utility library
A fast, modern C++ SDK for all your MPEG-2 transport stream media format needs following
international specification ISO/IEC 13818-1.

Artifacts:
```
Win32: mpeg2ts.dll mpeg2ts.lib
Linux: libmpeg2ts.so libmpeg2ts.a
Both: mpeg2ts.h
```
Applications:
```
Win32: TestTsLib located under msvs/2015
Linux: tsparser
```

## SW Architecture
There are 2 sets of APIs, the core API called mpeg2ts under the same namespace and a utility API to facilitate the usage of the API for more convinient usage (namespace tsutil). These are the APIs:
* mpeg2ts.h    The core and fundamental API for all mpeg2ts usage
* TsUtilities.h High level API to simplify usage
![](images/Ts-lib_SW_Architecture.png)

## Releases
*V0.1*

* Added first version of API (mpeg2ts.h)
* Added Windows support and build artifacts (mpeg2ts.lib/dll)
* Added Linux build artifacts (libmpeg2ts.so/a)
* Added high-level TsUtilities.h API for convinience
* Added first version H.264 support
* Added support for short version CLI for tsparser (Linux only)
* Fixed bug not finding and printing out error for settings.json

*V0.0.2.rc1*

* Add logging libary: Plog
* Fixed bug 68: Cannot parse Hbo Asset
* Add component-tests to CircleCi
* Fixed bug 66: Dolby asset caused crash
* Support Multiple Program Transport Streams (MPTS)
* Added file input --input option to CLI
* Fixed bug 49: Could not skip more than 64 bits in parser.
* Added non root user to Docker containers

*V0.0.1*

* build so lib
* Added es write
* fixed PCR bug
* Added build folder
* Added PES parsing
* Added multi PES cli option
* Added PMT parsing
* Added PAT parsing
* Added Demuxer

## CMake
![](images/cmake.png)

To simplify the crosscompile process we use CMake. Under Linux just do this:
```
mkdir build
cd build/
cmake ..
make
```
You will get the following artifacts:
```
libmpeg2ts.so*
libmpeg2ts_static.a
tsparser*
```
If you wanna speed up the build you can type `cmake --build . -- -j16` instead of `make` in the 4th command above.

## Linux Make
This is the traditionall way of building using GNU Make. This is left for legacy purpose and before all targets been ported to CMake. The recommended way of building this library is CMake.

### How to run it
Type `make help` to see all make targets. To start runing the lib:
```
make all
./tsparser --input assets/test1.ts
```
Check help in command line (CLI):
```
./tsparser --help --input assets/test1.ts
```
Add option --write with the PES PID for writing PES packets to file.
```
./tsparser --write 2504 --input assets/bbc_one.ts
```
Just print PSI tables / PES header can be done by --pid option and the PID.
```
./tsparser --pid 258 --input assets/bbc_one.ts
```

### Docker
![](images/docker.png)

Some targets requires docker in order to run since you most likely will not have
all build dependencies in your native environment. To virtualize the Application
build time dependencies they have been collected inside a docker image following
docker best practises. You only need to remember to source the 
```
source docker-commands.sh
```
and you will be ready to run commands inside the docker container by:
```
docker-make unit-tests
```
for example.
If you want to run a custom bash command you can do it by:
```
docker-bash make help
```
for instance. To get an interactive bash session type:
```
docker-interactive
```

### Docker image
To just use the latest image just pull from our private registry/repository @ DockerHub:
```
docker pull heliconwave/circleci:v1
```
To build the image your self:
```
make docker-image
```

### How to test it
In order to run all unit tests just type:
```
make test
```
This will spin up a docker container with gtest/gmock and execute all tests.

## Windows
Currently only WIN32 (x86) is supported with VS2015/VC14 compiler which has a fairly large C++11 support. 
There is a VS2015 solution file under msvc/2015 for this project. Unit tests (google test) is in same solution as 
mpeg2ts solution and needs the lib to be build in static mode to access all internal classes/symbols. There is a main application
called TestTsLib that uses the dynamic mpeg2ts.dll library. To build mpeg2ts.dll project open msvc/2015/mpegts2ts.sln and
change to dynamic project. These are the VS2015 solution files:
* mpeg2ts.sln
* TestTsLib.sln

## Continuous integration (CI)
![](images/circleci.png)

For CI we use CircleCI which will automatically run all unit tests after a commit either
in a branch, pull-request or integration to master. You can check the status tests in any
branch by the portal:
[CircleCI](https://circleci.com/gh/skullanbones/ts-lib)

## Static code analysis
Right now there is no online tool. Use `docker-make cppcheck` or `docker-make clang-tidy`.

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
* C++
* Docker
* CMake
* GNU Make
* GCC
* Python
* Git
* Google test

![](images/ts_lib_oss.png)
