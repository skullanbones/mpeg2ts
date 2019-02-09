cmake_minimum_required(VERSION 3.0)
project(3rd-party)
set(PLOG_VERSION 1.1.4)
set(NLOHMANN_VERSION 3.5.0)


# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

include(ExternalProject)

############# plog

ExternalProject_Add(
    plog
    URL ${CMAKE_CURRENT_SOURCE_DIR}/plog-${PLOG_VERSION}.tar.gz
    INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/plog-${PLOG_VERSION}
    CMAKE_ARGS  -D CMAKE_BUILD_TYPE=Release
                -D CMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

############# nlohmann/json

ExternalProject_Add(
    json
    URL ${CMAKE_CURRENT_SOURCE_DIR}/json-${NLOHMANN_VERSION}.tar.gz
    INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/json-${NLOHMANN_VERSION}
    CMAKE_ARGS  -D CMAKE_BUILD_TYPE=Release
                -D BUILD_TESTING=OFF
                -D CMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)
