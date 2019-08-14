#!/bin/sh

ROOT_FOLDER=$(pwd)/../..
APP_FOLDER=$ROOT_FOLDER/apps/WebAssembly
LIB_FOLDER=$ROOT_FOLDER/libs

em++ -std=c++11 -O3 -s WASM=1 -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1 -s \
"EXTRA_EXPORTED_RUNTIME_METHODS=['cwrap', 'ccall']" \
-I $LIB_FOLDER/mpeg2codec/include \
-I $LIB_FOLDER/h264codec/include \
-I $LIB_FOLDER/mpeg2codec/src \
-I $LIB_FOLDER/h264codec/src \
-I $LIB_FOLDER/mpeg2ts/src \
-I $ROOT_FOLDER/build/libs/mpeg2ts \
-I $LIB_FOLDER/mpeg2ts/include \
-I $ROOT_FOLDER/3rd-party/plog-1.1.4/include \
-I $ROOT_FOLDER/3rd-party/json-3.5.0/include \
$APP_FOLDER/src/main.cc \
$LIB_FOLDER/mpeg2ts/src/TsStatistics.cc \
$LIB_FOLDER/mpeg2ts/src/TsParser.cc \
$LIB_FOLDER/mpeg2ts/src/TsDemuxer.cc \
$LIB_FOLDER/mpeg2ts/src/GetBits.cc \
$LIB_FOLDER/mpeg2ts/src/PsiTables.cc \
$LIB_FOLDER/h264codec/src/H264Parser.cc \
$LIB_FOLDER/mpeg2codec/src/Mpeg2VideoParser.cc \
$LIB_FOLDER/mpeg2codec/src/Mpeg2Codec.cc \
