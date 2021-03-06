#*****************************************************************
#
#    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
#
#    mpeg2ts - mpeg2ts conftest.py
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
import xmltodict
import subprocess
import pytest
import os.path

BASE_URL = "https://s3-us-west-2.amazonaws.com/tslibteststreams"
DOWNLOAD_DIR = "component_tests/downloaded_files"


class Downloader():
    """
    Download with curl file over HTTP for any chunk_size.
    Settings include chunk_size, timeout and download folder.
    """
    def __init__(self, timeout="20", chunk_size="0-10000000",
                 download_dir=DOWNLOAD_DIR):
        self.timeout = timeout
        self.chunk_size = chunk_size
        self.download_dir = download_dir

    def file_exist(self, file_name):
        path = self.download_dir + "/" + file_name
        return os.path.isfile(path)

    def download_asset(self, name, streams):
        status = self.download_file(name)
        print(status)
        if not status:
            print("Error downloading asset..")
        return Asset(self.get_path(name), streams)

    def download_file(self, file_name, asset_list=False):
        if asset_list:
            url = BASE_URL
        else:
            url = BASE_URL + "/" + file_name
        print("Downloading asset %s" % url)
        # threading.Thread(target=self._wget_dl, args=(url, destination, try_number, time_out, log_file)).start()
        if not self.file_exist(file_name):
            if self.curl_dl(url, file_name) == 0:
                return True
            else:
                return False
        else:
            return True

    def get_path(self, file_name):
        abspath = os.path.abspath("./")
        return abspath + "/" + self.download_dir + "/" + file_name

    def curl_dl(self, url, file_name):
        # TODO Check curl command exists
        output = self.download_dir + "/" + file_name
        command=["curl", url, "--range", self.chunk_size, "--output", output,
                 "--create-dirs", "--connect-timeout", self.timeout]
        try:
            download_state=subprocess.call(command)
        except Exception as e:
            print(e)
        return download_state


class Asset(object):
    def __init__(self, path, elementary_streams):
        self.path = path
        self.streams = elementary_streams

    def get_asset(self):
        return self.path

    def get_streams(self):
        return self.streams

    def get_pmt(self):
        for stream in self.streams:
            if stream["StreamType"] == "pmt":
                return stream


@pytest.fixture(scope='session')
def downloader():
    """
    Fixture for downloading files over HTTP
    :return:
    """
    downloader_dir = DOWNLOAD_DIR
    yield Downloader(download_dir=downloader_dir)


@pytest.fixture(scope='session')
def asset_list(downloader):
    """
    Returns the list of assets stored in AWS
    :param downloader:
    :return: A list with assets to use for testing
    """
    downloader.download_file("assets.xml", asset_list=True)
    assets = []
    with open(DOWNLOAD_DIR + '/assets.xml') as fd:
        data = xmltodict.parse(fd.read())
        contents = data['ListBucketResult']['Contents']

        for asset in contents:
            assets.append(asset['Key'])
        print(assets)
    return assets


@pytest.fixture(scope='session', params=[
    ('Dolby_ATMOS_Helicopter_h264_ac3_eac3_192B.m2ts',
     [{"Pid": 4113, "StreamType": "video"},
      {"Pid": 4352, "StreamType": "audio"},
      {"Pid": 4353, "StreamType": "audio"},
      {"Pid": 256, "StreamType": "pmt"}])
])
def asset_h264_dolby_atmos(request, downloader):
    """
    Asset for dolby atmos
    :param request:
    :param downloader:
    :return: Returns the asset
    """
    name, streams = request.param
    return downloader.download_asset(name, streams)


@pytest.fixture(scope='session', params=[
    ('RuBeatles_h265_aac_short.ts',
     [{"Pid": 301, "StreamType": "video"},
      {"Pid": 302, "StreamType": "audio"},
      {"Pid": 300, "StreamType": "pmt"}])
])
def asset_h265_aac_rubeatles_atmos(request, downloader):
    """
    Asset for dolby atmos
    :param request:
    :param downloader:
    :return: Returns the asset
    """
    name, streams = request.param
    return downloader.download_asset(name, streams)


@pytest.fixture(scope='session', params=[
    ('GoT-HBO.ts',
     [{"Pid": 110, "StreamType": "video"},
      {"Pid": 210, "StreamType": "audio"},
      {"Pid": 50, "StreamType": "pmt"}])
])
def asset_h264_138183_got_hbo(request, downloader):
    """
    Asset for HBO atmos
    :param request:
    :param downloader:
    :return: Returns the asset
    """
    name, streams = request.param
    return downloader.download_asset(name, streams)

@pytest.fixture(scope='session', params=[
    ('Safari_Dolby_Digital_Plus_h264_aac3LCRLRLFE_192B.m2ts',
     [{"Pid": 110, "StreamType": "video"},
      {"Pid": 210, "StreamType": "audio"},
      {"Pid": 50, "StreamType": "pmt"}])
])
def asset_Safari_Dolby_Digital_Plus_h264_aac3LCRLRLFE_192B(request, downloader):
    """
    Asset for HBO atmos
    :param request:
    :param downloader:
    :return: Returns the asset
    """
    name, streams = request.param
    return downloader.download_asset(name, streams)

@pytest.fixture(scope='session', params=[
    ('avsync_mpeg2_ac3LR.ts',
     [{"Pid": 49, "StreamType": "video"},
      {"Pid": 50, "StreamType": "audio"},
      {"Pid": 32, "StreamType": "pmt"}])
])
def asset_avsync_mpeg2_ac3LR(request, downloader):
    """
    Asset for mpeg2 ac3LR
    :param request:
    :param downloader:
    :return: Returns the asset
    """
    name, streams = request.param
    return downloader.download_asset(name, streams)


@pytest.fixture(scope='session', params=[
    ('newmobcal1920_mpeg2_ac3LR.ts',
     [{"Pid": 49, "StreamType": "video"},
      {"Pid": 50, "StreamType": "audio"},
      {"Pid": 32, "StreamType": "pmt"}])
])
def asset_newmobcal1920_mpeg2_ac3LR(request, downloader):
    """
    Asset for newmobcal1920
    :param request:
    :param downloader:
    :return: Returns the asset
    """
    name, streams = request.param
    return downloader.download_asset(name, streams)

@pytest.fixture(scope='session', params=[
    ('eurosport.ts',
     [{"Pid": 101, "StreamType": "video"},
      {"Pid": 105, "StreamType": "audio"},
      {"Pid": 115, "StreamType": "audio"},
      {"Pid": 99, "StreamType": "pmt"}])
])
def asset_eurosport(request, downloader):
    """
    Asset for eurosport
    :param request:
    :param downloader:
    :return: Returns the asset
    """
    name, streams = request.param
    return downloader.download_asset(name, streams)
