#*****************************************************************
#
#    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
#
#    mpeg2ts - mpeg2ts cmdline_test.py
#
#    This file is part of mpeg2ts (Mpeg2 Transport Stream Library).
#
#    Unless you have obtained mpeg2ts under a different license,
#    this version of mpeg2ts is mpeg2ts|GPL.
#    Mpeg2ts|GPL is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License as
#    published by the Free Software Foundation; either version 2,
#    or (at your option) any later version.
#
#    Mpeg2ts|GPL is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with mpeg2ts|GPL; see the file COPYING. If not, write to
#    the Free Software Foundation, 59 Temple Place - Suite 330,
#    Boston, MA 02111-1307, USA.
#
#******************************************************************/
import pytest
import logging

logging.basicConfig(filename='cmdline_test.log', level=logging.DEBUG)
log = logging.getLogger("cmdline_test")


@pytest.fixture
def parser():
    """Get access to the TsParser binary to allow running it for
    testing or to get the command to start it."""
    from .tsparser import TsParser
    return TsParser(log)


def test_help_message(parser):
    """Verify that help option is displayed"""
    out = parser.start(extra_args=['--help'])
    assert "Mpeg2ts lib simple command-line:" in out[1], "No help text in output"
    assert "USAGE: ./tsparser [-h] [-v] [-p PID] [-w PID] [-m ts|pes|es] [-l log-level] [-i file]" \
           in out[1], "Wrong help output"


def test_asset_list(asset_list):
    """Test we have all assets"""
    print(asset_list)
    assert "Dolby_ATMOS_Helicopter_h264_ac3_eac3_192B.m2ts" in asset_list[0]
    assert "GoT-HBO.ts" in asset_list[1]
    assert "RuBeatles_h265_aac_short.ts" in asset_list[2]
    assert "Safari_Dolby_Digital_Plus_h264_aac3LCRLRLFE_192B.m2ts" in asset_list[3]
    assert "avsync_mpeg2_ac3LR.ts" in asset_list[4]
    assert "eurosport.ts" in asset_list[5]
    assert "newmobcal1920_mpeg2_ac3LR.ts" in asset_list[6]


def test_asset(asset_h264_dolby_atmos):
    """Test we can get the first asset"""
    streams = asset_h264_dolby_atmos.get_streams()
    assert 4113 == streams[0]['Pid']
    assert 'video' in streams[0]['StreamType']

    assert 4352 == streams[1]['Pid']
    assert 'audio' in streams[1]['StreamType']

    assert 4353 == streams[2]['Pid']
    assert 'audio' in streams[2]['StreamType']

    asset = asset_h264_dolby_atmos.get_asset()
    assert "Dolby_ATMOS_Helicopter_h264_ac3_eac3_192B.m2ts" in asset


def test_parse_dolby_asset_pat(parser, asset_h264_dolby_atmos):
    """Test we can parse the first asset"""
    log.debug("test_parse_dolby_asset_pat")
    asset = asset_h264_dolby_atmos.get_asset()
    assert "Dolby_ATMOS_Helicopter_h264_ac3_eac3_192B.m2ts" in asset
    out = parser.start(extra_args=['--input', asset, '--pid', 0])
    log.debug(out[0])
    log.debug(out[1])
    assert "programs.size(): 2" in out[1]
    assert "network_PID: 31" in out[1]
    assert "program_map_PID: 256" in out[1]

def test_parse_dolby_asset_pat_short_option(parser, asset_h264_dolby_atmos):
    """Test we can parse the first asset with short option"""
    log.debug("test_parse_dolby_asset_pat_short_option")
    asset = asset_h264_dolby_atmos.get_asset()
    assert "Dolby_ATMOS_Helicopter_h264_ac3_eac3_192B.m2ts" in asset
    out = parser.start(extra_args=['-i', asset, '-p', 0])
    log.debug(out[0])
    log.debug(out[1])
    assert "programs.size(): 2" in out[1]
    assert "network_PID: 31" in out[1]
    assert "program_map_PID: 256" in out[1]


def test_parse_dolby_asset_pmt(parser, asset_h264_dolby_atmos):
    """Test we can parse the first asset"""
    log.debug("test_parse_dolby_asset_pmt")
    asset = asset_h264_dolby_atmos.get_asset()
    pmt = asset_h264_dolby_atmos.get_pmt()
    log.debug(pmt)
    assert "Dolby_ATMOS_Helicopter_h264_ac3_eac3_192B.m2ts" in asset
    out = parser.start(extra_args=['--input', asset, '--pid', pmt['Pid'], '--pid', 4113])
    log.debug(out[0])
    log.debug(out[1])
    assert "PMT at Ts packet: 1" in out[1]
    assert "PCR_PID: 4097" in out[1]
    assert "program_info_length: 12" in out[1]
    assert "streams.size(): 3" in out[1]

    assert "stream_type: STREAMTYPE_VIDEO_H264,  (27)" in out[1]
    assert "elementary_PID: 4113" in out[1]
    assert "ES_info_length: 10" in out[1]

    assert "stream_type: STREAMTYPE_AUDIO_DOLBY_TRUE_HD,  (131)" in out[1]
    assert "elementary_PID: 4352" in out[1]
    assert "ES_info_length: 12" in out[1]

    assert "stream_type: STREAMTYPE_AUDIO_AC3_PLUS,  (132)" in out[1]
    assert "elementary_PID: 4353" in out[1]
    assert "ES_info_length: 12" in out[1]


def test_parse_rubeatles_asset_pmt(parser, asset_h265_aac_rubeatles_atmos):
    """
    Test new asset
    :param parser:
    :param asset_h265_aac_rubeatles_atmos:
    :return:
    """
    log.debug("test_parse_rubeatles_asset_pmt")
    asset = asset_h265_aac_rubeatles_atmos.get_asset()
    pmt = asset_h265_aac_rubeatles_atmos.get_pmt()
    log.debug(pmt)
    assert "RuBeatles_h265_aac_short.ts" in asset
    out = parser.start(extra_args=['--input', asset, '--pid', pmt['Pid']])
    log.debug(out[0])
    log.debug(out[1])


def test_parse_got_hbo_pat(parser, asset_h264_138183_got_hbo):
    """
    Test HBO asset PAT table
    :param parser:
    :param asset_h264_138183_got_hbo:
    :return:
    """
    log.debug("test_parse_got_hbo_pat")
    asset = asset_h264_138183_got_hbo.get_asset()
    assert "GoT-HBO.ts" in asset
    out = parser.start(extra_args=['--input', asset, '--pid', 0])
    log.debug(out[0])
    log.debug(out[1])
    assert "programs.size(): 15" in out[1]

    assert "program 0" in out[1]
    assert "program_number: 0" in out[1]
    assert "network_PID: 16" in out[1]

    assert "program 1" in out[1]
    assert "program_number: 10501" in out[1]
    assert "program_map_PID: 41" in out[1]

    assert "program 2" in out[1]
    assert "program_number: 10502" in out[1]
    assert "program_map_PID: 42" in out[1]

    assert "program 3" in out[1]
    assert "program_number: 10503" in out[1]
    assert "program_map_PID: 43" in out[1]

    assert "program 4" in out[1]
    assert "program_number: 10504" in out[1]
    assert "program_map_PID: 44" in out[1]

    assert "program 5" in out[1]
    assert "program_number: 10505" in out[1]
    assert "program_map_PID: 45" in out[1]

    assert "program 6" in out[1]
    assert "program_number: 10506" in out[1]
    assert "program_map_PID: 46" in out[1]

    assert "program 7" in out[1]
    assert "program_number: 10507" in out[1]
    assert "program_map_PID: 47" in out[1]

    assert "program 8" in out[1]
    assert "program_number: 10508" in out[1]
    assert "program_map_PID: 48" in out[1]

    assert "program 9" in out[1]
    assert "program_number: 10509" in out[1]
    assert "program_map_PID: 49" in out[1]

    assert "program 10" in out[1]
    assert "program_number: 10510" in out[1]
    assert "program_map_PID: 50" in out[1]

    assert "program 11" in out[1]
    assert "program_number: 10511" in out[1]
    assert "program_map_PID: 93" in out[1]

    assert "program 12" in out[1]
    assert "program_number: 10552" in out[1]
    assert "program_map_PID: 52" in out[1]

    assert "program 13" in out[1]
    assert "program_number: 10512" in out[1]
    assert "program_map_PID: 51" in out[1]

    assert "program 14" in out[1]
    assert "program_number: 10513" in out[1]
    assert "program_map_PID: 53" in out[1]


def test_parse_got_hbo_pmt(parser, asset_h264_138183_got_hbo):
    """
    Test HBO asset PMT table
    :param parser:
    :param asset_h264_138183_got_hbo:
    :return:
    """
    log.debug("test_parse_got_hbo_pmt")
    asset = asset_h264_138183_got_hbo.get_asset()
    pmt = asset_h264_138183_got_hbo.get_pmt()
    assert "GoT-HBO.ts" in asset
    out = parser.start(extra_args=['--input', asset, '--pid', pmt['Pid']])
    log.debug(out[0])
    log.debug(out[1])
    assert "PMT at Ts packet: 1260" in out[1]
    assert "PCR_PID: 110" in out[1]
    assert "program_info_length: 104" in out[1]
    assert "streams.size(): 5" in out[1]

    assert "stream_type: STREAMTYPE_VIDEO_H264,  (27)" in out[1]
    assert "elementary_PID: 110" in out[1]
    assert "ES_info_length: 97" in out[1]

    assert "stream_type: STREAMTYPE_AUDIO_MPEG2,  (4)" in out[1]
    assert "elementary_PID: 210" in out[1]
    assert "ES_info_length: 92" in out[1]

    assert "stream_type: STREAMTYPE_PRIVATE_PES,  (6)" in out[1]
    assert "elementary_PID: 310" in out[1]
    assert "ES_info_length: 107" in out[1]

    assert "stream_type: STREAMTYPE_PRIVATE_PES,  (6)" in out[1]
    assert "elementary_PID: 1410" in out[1]
    assert "ES_info_length: 10" in out[1]

    assert "stream_type: STREAMTYPE_PRIVATE_PES,  (6)" in out[1]
    assert "elementary_PID: 1310" in out[1]
    assert "ES_info_length: 20" in out[1]


#def test_parse_dolby_digital_plus(parser, asset_Safari_Dolby_Digital_Plus_h264_aac3LCRLRLFE_192B):
#    """
#    :param parser:
#    :param asset_h264_138183_Safari_Dolby_Digital_Plus_h264_aac3LCRLRLFE_192B:
#    :return:
#    """
#    asset = asset_Safari_Dolby_Digital_Plus_h264_aac3LCRLRLFE_192B.get_asset()


def test_parse_avsync_mpeg2_ac3LR_PAT(parser, asset_avsync_mpeg2_ac3LR):
    """
    Test Avsync PAT table
    :param parser:
    :param asset_avsync_mpeg2_ac3LR
    :return:
    """
    log.debug("test_parse_avsync_mpeg2_ac3LR_PAT")
    asset = asset_avsync_mpeg2_ac3LR.get_asset()
    assert "avsync_mpeg2_ac3LR.ts" in asset
    out = parser.start(extra_args=['--input', asset, '--pid', 0])
    log.debug(out[0])
    log.debug(out[1])
    assert "programs.size(): 1" in out[1]

    assert "program 0" in out[1]
    assert "program_number: 1" in out[1]
    assert "program_map_PID: 32" in out[1]


def test_parse_avsync_mpeg2_ac3LR_PMT(parser, asset_avsync_mpeg2_ac3LR):
    """
    Test Avsync PMT table
    :param parser:
    :param asset_avsync_mpeg2_ac3LR
    :return:
    """
    log.debug("test_parse_avsync_mpeg2_ac3LR_PMT")
    asset = asset_avsync_mpeg2_ac3LR.get_asset()
    pmt = asset_avsync_mpeg2_ac3LR.get_pmt()
    assert "avsync_mpeg2_ac3LR.ts" in asset
    out = parser.start(extra_args=['--input', asset, '--pid', pmt['Pid']])
    log.debug(out[0])
    log.debug(out[1])
    assert "PMT at Ts packet: 1" in out[1]
    assert "PCR_PID: 48" in out[1]
    assert "program_info_length: 0" in out[1]
    assert "streams.size(): 2" in out[1]

    assert "stream_type: STREAMTYPE_VIDEO_MPEG2,  (2)" in out[1]
    assert "elementary_PID: 49" in out[1]
    assert "ES_info_length: 3" in out[1]

    assert "stream_type: STREAMTYPE_AUDIO_AC3,  (129)" in out[1]
    assert "elementary_PID: 50" in out[1]
    assert "ES_info_length: 3" in out[1]


def test_parse_newmobcal1920_mpeg2_ac3LR_PAT(parser, asset_newmobcal1920_mpeg2_ac3LR):
    """
    Test Newmbcal1920 PAT table
    :param parser:
    :param asset_newmobcal1920_mpeg2_ac3LR:
    :return:
    """
    log.debug("test_parse_newmobcal1920_mpeg2_ac3LR_PAT")
    asset = asset_newmobcal1920_mpeg2_ac3LR.get_asset()
    assert "newmobcal1920_mpeg2_ac3LR.ts" in asset
    out = parser.start(extra_args=['--input', asset, '--pid', 0])
    log.debug(out[0])
    log.debug(out[1])
    assert "programs.size(): 1" in out[1]

    assert "program 0" in out[1]
    assert "program_number: 1" in out[1]
    assert "program_map_PID: 32" in out[1]


def test_parse_newmobcal1920_mpeg2_ac3LR_PMT(parser, asset_newmobcal1920_mpeg2_ac3LR):
    """
    Test Newmbcal1920 PMT table
    :param parser:
    :param asset_newmobcal1920_mpeg2_ac3LR:
    :return:
    """
    log.debug("test_parse_newmobcal1920_mpeg2_ac3LR_PMT")
    asset = asset_newmobcal1920_mpeg2_ac3LR.get_asset()
    pmt = asset_newmobcal1920_mpeg2_ac3LR.get_pmt()
    assert "newmobcal1920_mpeg2_ac3LR.ts" in asset
    out = parser.start(extra_args=['--input', asset, '--pid', pmt['Pid']])
    log.debug(out[0])
    log.debug(out[1])
    assert "PMT at Ts packet: 1" in out[1]
    assert "PCR_PID: 48" in out[1]
    assert "program_info_length: 0" in out[1]
    assert "streams.size(): 2" in out[1]

    assert "stream_type: STREAMTYPE_VIDEO_MPEG2,  (2)" in out[1]
    assert "elementary_PID: 49" in out[1]
    assert "ES_info_length: 3" in out[1]

    assert "stream_type: STREAMTYPE_AUDIO_AC3,  (129)" in out[1]
    assert "elementary_PID: 50" in out[1]
    assert "ES_info_length: 3" in out[1]


def test_parse_mpeg2_eurosport(parser, asset_eurosport):
    """
    Test eurosport mpeg2 video parsing
    :param parser:
    :param asset_eurosport:
    :return:
    """
    log.debug("test_parse_h262_eurosport")
    asset = asset_eurosport.get_asset()
    assert "eurosport.ts" in asset
    out = parser.start(extra_args=['--input', asset, '--pid', 101, '-l', 'DEBUG'])
    log.debug(out[0])
    log.debug(out[1])
    print(out[1])
    assert "sequence_header_code" in out[1]
    assert "size: 704 x 576, aspect: 3x4, frame rate: 25" in out[1]


def test_parse_h264_dolby(parser, asset_h264_dolby_atmos):
    """
    Test eurosport h264 video parsing
    :param parser:
    :param asset_h264_dolby_atmos:
    :return:
    """
    log.debug("test_parse_h264_dolby")
    asset = asset_h264_dolby_atmos.get_asset()
    assert "Dolby_ATMOS_Helicopter_h264_ac3_eac3_192B.m2ts" in asset
    out = parser.start(extra_args=['--input', asset, '--pid', 4113, '-l', 'DEBUG'])
    log.debug(out[0])
    log.debug(out[1])
    print(out[1])
    assert "nal: Access_unit_delimiter Access unit delimiter" in out[1]
    assert "nal: Sequence_parameter_set Sequence parameter set: profile: High level: 4.1" in out[1]
    assert "sps id: 0, luma bits: 8, chroma bits: 8, size: 1920 x 1088, ref pic: 4" in out[1]
    assert "nal: Picture_parameter_set Picture parameter set: entropy: CAVLC" in out[1]
    assert "sps id: 0pps id: 0" in out[1]


def test_b_parse_dolby_asset_pat(benchmark, parser, asset_h264_dolby_atmos):
    # benchmark test_parse_h264_dolby
    result = benchmark(test_parse_dolby_asset_pat, parser, asset_h264_dolby_atmos)


def test_b_parse_dolby_asset_pat_short_option(benchmark, parser, asset_h264_dolby_atmos):
    # benchmark test_parse_h264_dolby
    result = benchmark(test_parse_dolby_asset_pat_short_option, parser, asset_h264_dolby_atmos)


def test_b_parse_dolby_asset_pmt(benchmark, parser, asset_h264_dolby_atmos):
    # benchmark something, but add some arguments
    result = benchmark(test_parse_dolby_asset_pmt, parser, asset_h264_dolby_atmos)


def test_b_parse_rubeatles_asset_pmt(benchmark, parser, asset_h265_aac_rubeatles_atmos):
    # benchmark test_parse_h264_dolby
    result = benchmark(test_parse_rubeatles_asset_pmt, parser, asset_h265_aac_rubeatles_atmos)


def test_b_parse_got_hbo_pat(benchmark, parser, asset_h264_138183_got_hbo):
    # benchmark test_parse_h264_dolby
    result = benchmark(test_parse_got_hbo_pat, parser, asset_h264_138183_got_hbo)


def test_b_parse_got_hbo_pmt(benchmark, parser, asset_h264_138183_got_hbo):
    # benchmark test_parse_h264_dolby
    result = benchmark(test_parse_got_hbo_pmt, parser, asset_h264_138183_got_hbo)    


def test_b_parse_avsync_mpeg2_ac3LR_PAT(benchmark, parser, asset_avsync_mpeg2_ac3LR):
    # benchmark test_parse_h264_dolby
    result = benchmark(test_parse_avsync_mpeg2_ac3LR_PAT, parser, asset_avsync_mpeg2_ac3LR) 


def test_b_parse_avsync_mpeg2_ac3LR_PMT(benchmark, parser, asset_avsync_mpeg2_ac3LR):
    # benchmark test_parse_h264_dolby
    result = benchmark(test_parse_avsync_mpeg2_ac3LR_PMT, parser, asset_avsync_mpeg2_ac3LR) 


def test_b_parse_newmobcal1920_mpeg2_ac3LR_PAT(benchmark, parser, asset_newmobcal1920_mpeg2_ac3LR):
    # benchmark test_parse_h264_dolby
    result = benchmark(test_parse_newmobcal1920_mpeg2_ac3LR_PAT, parser, asset_newmobcal1920_mpeg2_ac3LR)


def test_b_parse_newmobcal1920_mpeg2_ac3LR_PMT(benchmark, parser, asset_newmobcal1920_mpeg2_ac3LR):
    # benchmark test_parse_h264_dolby
    result = benchmark(test_parse_newmobcal1920_mpeg2_ac3LR_PMT, parser, asset_newmobcal1920_mpeg2_ac3LR)


def test_b_parse_h262_eurosport(benchmark, parser, asset_eurosport):
    # benchmark test_parse_h264_dolby
    result = benchmark(test_parse_mpeg2_eurosport, parser, asset_eurosport)


def test_b_parse_h264_dolby(benchmark, parser, asset_h264_dolby_atmos):
    # benchmark test_parse_h264_dolby
    result = benchmark(test_parse_h264_dolby, parser, asset_h264_dolby_atmos)
