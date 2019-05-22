#!/bin/sh
em++ -std=c++11 -O3 -s WASM=1 -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1 -s \
"EXTRA_EXPORTED_RUNTIME_METHODS=['cwrap', 'ccall']" \
-I include/mpeg2vid/ \
-I include/h264 \
-I src -I build \
-I include \
-I 3rd-party/plog-1.1.4/include \
-I 3rd-party/json-3.5.0/include \
WebAssembly/main.cc \
src/h264/H264Parser.cc \
src/GetBits.cc \
src/TsStatistics.cc \
src/TsParser.cc \
src/TsDemuxer.cc \
src/mpeg2vid/Mpeg2VideoParser.cc \
src/mpeg2vid/Mpeg2Codec.cc \
src/PsiTables.cc

