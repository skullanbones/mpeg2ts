import pytest
import logging

logging.basicConfig(filename='cmdline_test.log',level=logging.DEBUG)
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
    assert "USAGE: ./tsparser [-h] [-w PID] [-p PID] [-l log-level] [-i file]" in \
           out[1], "Wrong help output"

def test_asset_list(asset_list):
    """Test we have all assets"""
    print(asset_list)
    assert "Dolby_ATMOS_Helicopter_h264_ac3_eac3_192B.m2ts" in asset_list[0]
    assert "GoT-HBO.ts" in asset_list[1]
    assert "RuBeatles_h265_aac_short.ts" in asset_list[2]
    assert "Safari_ Dolby_Digital_Plus_h264_aac3LCRLRLFE_192B.m2ts" in asset_list[3]
    assert "avsync_mpeg2_ac3LR.ts" in asset_list[4]
    assert "newmobcal1920_mpeg2_ac3LR.ts" in asset_list[5]

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

def test_parse_asset(parser, asset_h264_dolby_atmos):
    """Test we can parse the first asset"""
    asset = asset_h264_dolby_atmos.get_asset()
    assert "Dolby_ATMOS_Helicopter_h264_ac3_eac3_192B.m2ts" in asset
    arg = '--input %s' % asset
    log.debug("use arg: %s" % arg)
    out = parser.start(extra_args=['--input', asset, '--pid', 0])
    log.debug(out[0])
    log.debug(out[1])
    assert "programs.size(): 2" in out[1]
    assert "network_PID: 31" in out[1]
    assert "program_map_PID: 256" in out[1]
