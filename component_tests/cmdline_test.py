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
    assert "Ts-lib simple command-line:" in out[1], "No help text in output"
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
    asset = asset_h264_dolby_atmos.get_asset()
    assert "Dolby_ATMOS_Helicopter_h264_ac3_eac3_192B.m2ts" in asset
    out = parser.start(extra_args=['--input', asset, '--pid', 0])
    log.debug(out[0])
    log.debug(out[1])
    assert "programs.size(): 2" in out[1]
    assert "network_PID: 31" in out[1]
    assert "program_map_PID: 256" in out[1]


def test_parse_dolby_asset_pmt(parser, asset_h264_dolby_atmos):
    """Test we can parse the first asset"""
    asset = asset_h264_dolby_atmos.get_asset()
    pmt = asset_h264_dolby_atmos.get_pmt()
    log.debug(pmt)
    assert "Dolby_ATMOS_Helicopter_h264_ac3_eac3_192B.m2ts" in asset
    out = parser.start(extra_args=['--input', asset, '--pid', pmt['Pid']])
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


def test_parse_rubeatles_asset_pmt(parser, asset_h2646_aac_rubeatles_atmos):
    """
    Test new asset
    :param parser:
    :param asset_h2646_aac_rubeatles_atmos:
    :return:
    """
    asset = asset_h2646_aac_rubeatles_atmos.get_asset()
    pmt = asset_h2646_aac_rubeatles_atmos.get_pmt()
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

def test_parse_eurosport(parser, asset_eurosport):
    """
    Test eurosport meg2 video parsing
    :param parser:
    :param asset_eurosport:
    :return:
    """
    asset = asset_eurosport.get_asset()
    assert "eurosport.ts" in asset
    out = parser.start(extra_args=['--input', asset, '--pid', 101, '-l', 'DEBUG'])
    log.debug(out[0])
    log.debug(out[1])
    print(out[1])
    assert "sequence_header_code size 704 x 576, aspect 3x4, frame rate 25" in out[1]
