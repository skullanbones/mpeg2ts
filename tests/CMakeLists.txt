cmake_minimum_required(VERSION 3.0)

project(
    run_gtests
    VERSION 1.0.0
    DESCRIPTION "GTests for mpeg2ts lib"
    LANGUAGES CXX
)

enable_testing()


#-------------------
if(USE_DOCKER)
    # Locate GTest
    find_package(GTest REQUIRED)

    find_library(GMOCK_LIBRARY
                 NAMES gmock 
                       libgmock 
                       libgmock.a 
                 PATHS "${GTEST_DIR}"
                 PATH_SUFFIXES lib
    )

    include_directories(${GTEST_INCLUDE_DIRS})
endif(USE_DOCKER)

#-------------------
# tests
#-------------------
# Now simply link against gtest, gmock, gtest_main or gmock_main as needed. Eg
add_executable(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PRIVATE MPEG2TS_DLL_EXPORTS)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)

target_sources(${PROJECT_NAME}
    PRIVATE
        gtest_main.cc
        GetBits_Tests.cc
        Mpeg2VideoParser_Tests.cc
        PsiTables_Tests.cc
        TsDemuxer_Tests.cc
        TsPacketTestData.h
        TsParser_Tests.cc
        TsStatistics_Tests.cc
        TsUtilities_Tests.cc
        H264Parser_Tests.cc
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/.
        ${CMAKE_CURRENT_BINARY_DIR}/..
)

if(USE_DOCKER)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GMOCK_LIBRARY} ${GTEST_LIBRARY} pthread)
else(USE_DOCKER)
    find_package(Threads REQUIRED)
    target_link_libraries(
        ${PROJECT_NAME} 
        PRIVATE
            Threads::Threads 
            gmock
)
endif(USE_DOCKER)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        mpeg2ts
        mpeg2codec
        h264codec
)
