# Webassembly
With binary code directly executed by a browser as a plugin with embedded ABI from Javascript anything is now possible. Therefore a good begining is to be able to build mpeg2ts lib with em++ (Emscripten toolchain). Here is the toolchain command via CMake:

```Bash
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=install -DCMAKE_TOOLCHAIN_FILE:FILEPATH=<your repo path>/cmake/toolchains/emscripten-wasm.cmake -DEMSCRIPTEN_PREFIX=<path> ..
```