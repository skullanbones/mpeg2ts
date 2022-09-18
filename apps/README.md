# Apps
These 2 apps are Command Line Interface apps that will help you parse either a transport-stream file or a h264 file.

## h264parser
Example:

    ./h264parser --input myvideo.h264

Help dialog:

    H264 parser command-line tool:
    USAGE: ./h264parser [-h] [-v] [-l log-level] [-i file]
    Option Arguments:
        -h [ --help ]        Print help messages
        -v [ --version ]     Print library version
        -l [ --log-level NONE|FATAL|ERROR|WARNING|INFO|DEBUG|VERBOSE] Choose what logs are filtered, both file and stdout, default: DEBUG
        -i [ --input FILE]   Use input file for parsing


## tsparser
Example:

    ./tsparser --input assets/test1.ts

### More usage
To start runing the parser:

    ./tsparser --input assets/test1.ts

Check help in command line (CLI):

    ./tsparser --help

Add option --write with the PES PID for writing PES packets to file.

    ./tsparser --write 2306 --input assets/bbc_one.ts

Just print PSI tables / PES header can be done by --pid option and the PID.

    ./tsparser --pid 258 --input assets/bbc_one.ts

Help dialog:

    Mpeg2ts lib simple command-line:
    USAGE: ./tsparser [-h] [-v] [-p PID] [-w PID] [-m ts|pes|es] [-l log-level] [-i file]
    Option Arguments:
        -h [ --help ]        Print help messages
        -v [ --version ]     Print library version
        -p [ --pid PID]      Print PSI tables info with PID
        -w [ --write PID]    Writes PES packets with PID to file
        -m [ --wrmode type]  Choose what type of data is written[ts|pes|es]
        -l [ --log-level NONE|FATAL|ERROR|WARNING|INFO|DEBUG|VERBOSE] Choose what logs are filtered, both file and stdout, default: DEBUG
        -i [ --input FILE]   Use input file for parsing
