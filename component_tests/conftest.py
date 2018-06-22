import xmltodict
import subprocess
import pytest
import os.path

BASE_URL = "https://s3-us-west-2.amazonaws.com/tslibteststreams"

class Downloader():
    """
    Download with curl file over HTTP for any chunk_size.
    Settings include chunk_size, timeout and download folder.
    """
    def __init__(self, timeout = "20", chunk_size = "0-10000000", download_dir="downloaded_files"):
        self.timeout = timeout
        self.chunk_size = chunk_size
        self.download_dir = download_dir

    def file_exist(self, file_name):
        path = self.download_dir + "/" + file_name
        return os.path.isfile(path)

    def download_file(self, url, file_name):
        print("Downloading asset %s" % url)
        #threading.Thread(target=self._wget_dl, args=(url, destination, try_number, time_out, log_file)).start()
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
        command=["curl", url, "--range", self.chunk_size,"--output", output,  "--create-dirs", "--connect-timeout", self.timeout]
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


@pytest.fixture(scope='session')
def downloader():
    """
    Fixture for downloading files over HTTP
    :return:
    """
    downloader_dir = 'downloaded_files'
    yield Downloader(download_dir=downloader_dir)


@pytest.fixture(scope='session')
def asset_list(downloader):
    """
    Returns the list of assets stored in AWS
    :param downloader:
    :return: A list with assets to use for testing
    """
    downloader.download_file(BASE_URL, "assets.xml")
    assets = []
    with open('downloaded_files/assets.xml') as fd:
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
      {"Pid": 4353, "StreamType": "audio"}])
])
def asset_h264_dolby_atmos(request, downloader):
    """
    Asset for dolby atmos
    :param request:
    :param downloader:
    :return: Returns the asset
    """
    name, streams = request.param
    url = BASE_URL + "/" + name
    status = downloader.download_file(url, name)
    print(status)
    if not status:
        print("Error downloading asset..")
    return Asset(downloader.get_path(name), streams)

