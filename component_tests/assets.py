import xmltodict
import urllib.request
import subprocess

BASE_URL = "https://s3-us-west-2.amazonaws.com/tslibteststreams"

file  = urllib.request.urlopen(BASE_URL)
data = file.read()
file.close()
data = xmltodict.parse(data)
contents = data['ListBucketResult']['Contents']

# find each asset and append to list
assets = []
for asset in contents:
    assets.append(asset['Key'])

print(assets)

#############
class Downloader():
    def __init__(self, timeout = "20", chunk_size = "0-10000000", download_dir="downloaded_files"):
        self.timeout = timeout
        self.chunk_size = chunk_size
        self.download_dir = download_dir

    def download_file(self, url, file_name = "file_name"):
        print("Downloading asset %s" % url)
        #threading.Thread(target=self._wget_dl, args=(url, destination, try_number, time_out, log_file)).start()
        if self.curl_dl(url, file_name) == 0:
            return True
        else:
            return False


    def curl_dl(self,url, file_name):
        # TODO Check curl command exists
        output = self.download_dir + "/" + file_name
        command=["curl", url, "--range", self.chunk_size,"--output", output,  "--create-dirs", "--connect-timeout", self.timeout]
        try:
            download_state=subprocess.call(command)
        except Exception as e:
            print(e)
        #if download_state==0 => successfull download
        return download_state


test = Downloader()
test.download_file(BASE_URL + assets[0])
