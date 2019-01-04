#include <benchmark/benchmark.h>
#include <cstring>

#include "h264/H264Parser.h"
#include "mpeg2vid/Mpeg2VideoParser.h"
#include "CodecTestData.h"
#include "EsParser.h"

/// DUMMY tests

static void BM_StringCreation(benchmark::State& state) {
  for (auto _ : state)
    std::string empty_string;
}
// Register the function as a benchmark
BENCHMARK(BM_StringCreation);

// Define another benchmark
static void BM_StringCopy(benchmark::State& state) {
  std::string x = "hello";
  for (auto _ : state)
    std::string copy(x);
}
BENCHMARK(BM_StringCopy);

/////////// parser bench

//// H264

static void BM_H264EsParser_findStartCodes(benchmark::State& state)
{
    h264::H264EsParser parser;

    for (auto _ : state) {
        std::vector<std::size_t> ind = parser.findStartCodes(h262_with_sequence_header_code);
    }
}
BENCHMARK(BM_H264EsParser_findStartCodes);


static void BM_H264EsParser_parse(benchmark::State& state)
{
    h264::H264EsParser parser;

    for (auto _ : state) {
        std::vector<std::shared_ptr<EsInfo>> data = parser.parse(h262_with_sequence_header_code);
    }
}
BENCHMARK(BM_H264EsParser_parse);

////// H262

static void BM_Mpeg2VideoEsParser_findStartCodes(benchmark::State& state)
{
    mpeg2::Mpeg2VideoEsParser parser;

    for (auto _ : state) {
        std::vector<std::size_t> ind = parser.findStartCodes(h262_with_sequence_header_code);
    }
}
BENCHMARK(BM_H264EsParser_findStartCodes);

static void BM_Mpeg2VideoEsParser_parse(benchmark::State& state)
{
    mpeg2::Mpeg2VideoEsParser parser;

    for (auto _ : state) {
        std::vector<std::shared_ptr<EsInfo>> data = parser.parse(h262_with_sequence_header_code);
    }
}
BENCHMARK(BM_Mpeg2VideoEsParser_parse);
