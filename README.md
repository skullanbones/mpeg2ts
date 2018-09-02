# tslib
Artifacts:
```
Win32: tslib.dll tslib.lib
Linux: tslib.so tslib.a
Both: tslib.h
```
Applications:
```
Win32: None, TBD
Linux: tsparser
```

## SW Arcitechture
There is currently only 1 set of API under the namespace tslib and its exported
currently under TsDemuxer.h Soon there will only be 1 h-file: tslib.h
![](https://github.com/skullanbones/ts-lib/blob/develop/Ts-lib_SW_Architecture.png)

## Releases
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

## Linux

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


### Docker image
To just use the latest image just pull from our private registry/repository @ DockerHub:
```
docker pull heliconwave/circleci:v1
```
To build the image your self:
```
make docker-image
```
To spin-up a container:
```
make docker-bash
```
From here you can use the 3rd-party dependencies like
tstools, clang-format-5.0 etc...

### How to test it
In order to run all unit tests just type:
```
make test
```
This will spin up a docker container with gtest/gmock and execute all tests.

## Windows
Currently only WIN32 (x86) is supported unser VC14 VS2015 compiler which has a fairly large C++11
support. There is a VS2015 solution file unser msvc/2015 for this project. No unit tests or main application exist at current state. It's is a pure DLL project that builds tslib.dll artifacts under windows. No cross compilation exist.

## Continuous integration (CI)
For CI we use CircleCI which will automatically run all unit tests after a commit either
in a branch, pull-request or integration to master. You can check the status tests in any
branch by the portal:
[CircleCI](https://circleci.com/gh/skullanbones/ts-lib)

## Static code analysis
For static and feedback in pull-requests we use a tool called Codacy which will run different
tools to check for mistakes and coding quality/best practises. You can check that status here:
[Codacy](https://app.codacy.com/app/skullanbones/ts-lib/dashboard)

## Acronyms
| Abbreviation  | Meaning                             |
|---------------|-------------------------------------|
| VCS           | Version Control System              |
| DVCS          | Distributed Version Control System  |
| SW            | Software                            |
| MC            | Multicast                           |
| CI            | Continuous Integration              |