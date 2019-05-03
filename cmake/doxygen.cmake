cmake_minimum_required(VERSION 3.12)

# Require dot, treat the other components as optional
find_package(
    Doxygen 
    OPTIONAL_COMPONENTS dot mscgen dia
)

if(NOT DOXYGEN_FOUND)
    message(WARNING "Doxygen not found on your system. Bailing out...")
    return()
else()
    message(STATUS "doxygen found: ${DOXYGEN_EXECUTABLE}")

    doxygen_add_docs(
        doxygen
        ${PROJECT_SOURCE_DIR}/README.md ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/samples
        COMMENT "Generate doxygen pages"
    )
endif()
