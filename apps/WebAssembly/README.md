# Webassembly
With binary code directly executed by a browser as a plugin with embedded ABI from Javascript anything is now possible. Therefore a good begining is to be able to build mpeg2ts lib with em++ (Emscripten toolchain). Here is the toolchain command via CMake:

## Building
First it requires you to first run the toolchain environment scipt (finding the programs):
```Bash
source ./emsdk_env.sh
```

then building via cmake (assumes you are in the root of the repo):
```Bash
mkdir build
cd build/

emcmake cmake -DCMAKE_CXX_STANDARD=11 -DENABLE_TESTS=OFF -DENABLE_WEBASSEMBLY=ON ..
```

## Run the app
After building as described above copy the artifacts `a.out.js` and `a.out.wasm` to your source apps folder and 
open the `index.html` in a browser and you are runing the web app.


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