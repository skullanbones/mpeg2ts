if(ENABLE_WEBASSEMBLY)
    add_subdirectory(WebAssembly)
endif()

# only build tsparser on UNIX (getopt only exist in Linux)
if (UNIX)
    add_subdirectory(tsparser)
endif(UNIX)