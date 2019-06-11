# Webassembly
With binary code directly executed by a browser as a plugin with embedded ABI from Javascript anything is now possible. Therefore a good begining is to be able to build mpeg2ts lib with em++ (Emscripten toolchain). Here is the toolchain command via CMake:

```Bash
emcmake cmake -DCMAKE_CXX_STANDARD=11 -DENABLE_TESTS=OFF -DENABLE_WEBASSEMBLY=ON ..
```
but it requires you to first run the environment scipt:
```Bash
source ./emsdk_env.sh
```


## Installing the emscripten SDK
Building the latest SDK:
```Bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk/
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

Building a certain version:
```Bash
./emsdk install sdk-tag-1.37.9-64bit
./emsdk activate sdk-tag-1.37.9-64bit
source ./emsdk_env.sh
```