/*****************************************************************
*
*    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
*
*    mpeg2ts - mpeg2ts benchmark
*
*    This file is part of mpeg2ts (Mpeg2 Transport Stream Library).
*
*    Unless you have obtained mpeg2ts under a different license,
*    this version of mpeg2ts is mpeg2ts|GPL.
*    Mpeg2ts|GPL is free software; you can redistribute it and/or
*    modify it under the terms of the GNU General Public License as
*    published by the Free Software Foundation; either version 2,
*    or (at your option) any later version.
*
*    Mpeg2ts|GPL is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with mpeg2ts|GPL; see the file COPYING.  If not, write to the
*    Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA
*    02111-1307, USA.
*
*******************************************************************/

#include <benchmark/benchmark.h>
#include <cstring>

#include "CodecTestData.h"
#include "EsParser.h"
#include "H264Parser.h"
#include "Mpeg2VideoParser.h"

/// DUMMY tests

static void BM_StringCreation(benchmark::State& state)
{
    for (auto _ : state)
        std::string empty_string;
}
// Register the function as a benchmark
BENCHMARK(BM_StringCreation);

// Define another benchmark
static void BM_StringCopy(benchmark::State& state)
{
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

    for (auto _ : state)
    {
        std::vector<std::size_t> ind = parser.findStartCodes(h262_with_sequence_header_code);
    }
}
BENCHMARK(BM_H264EsParser_findStartCodes);


static void BM_H264EsParser_parse(benchmark::State& state)
{
    h264::H264EsParser parser;

    for (auto _ : state)
    {
        std::vector<h264::EsInfoH264> data = parser.parse(h262_with_sequence_header_code);
    }
}
BENCHMARK(BM_H264EsParser_parse);

////// H262

static void BM_Mpeg2VideoEsParser_findStartCodes(benchmark::State& state)
{
    mpeg2::Mpeg2VideoEsParser parser;

    for (auto _ : state)
    {
        std::vector<std::size_t> ind = parser.findStartCodes(h262_with_sequence_header_code);
    }
}
BENCHMARK(BM_H264EsParser_findStartCodes);

static void BM_Mpeg2VideoEsParser_parse(benchmark::State& state)
{
    mpeg2::Mpeg2VideoEsParser parser;

    for (auto _ : state)
    {
        std::vector<mpeg2::EsInfoMpeg2> data = parser.parse(h262_with_sequence_header_code);
    }
}
BENCHMARK(BM_Mpeg2VideoEsParser_parse);

BENCHMARK_MAIN();
