import xmltodict
import urllib.request

ASSETS_URL = "https://s3-us-west-2.amazonaws.com/tslibteststreams"

file  = urllib.request.urlopen(ASSETS_URL)
data = file.read()
file.close()
data = xmltodict.parse(data)
contents = data['ListBucketResult']['Contents']

# find each asset and append to list
assets = []
for asset in contents:
    assets.append(asset['Key'])

print(assets)
