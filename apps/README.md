# Apps
These 2 apps are Command Line Interface apps that will help you parse either a transport-stream file or a h264 file.

## h264parser
Example:

    ./h264parser --input myvideo.h264

## tsparser
Example:

    ./tsparser --input assets/test1.ts

### More usage
Type `make help` to see all make targets. To start runing the lib:

    ./tsparser --input assets/test1.ts

Check help in command line (CLI):

    ./tsparser --help

Add option --write with the PES PID for writing PES packets to file.

    ./tsparser --write 2504 --input assets/bbc_one.ts

Just print PSI tables / PES header can be done by --pid option and the PID.

    ./tsparser --pid 258 --input assets/bbc_one.ts
