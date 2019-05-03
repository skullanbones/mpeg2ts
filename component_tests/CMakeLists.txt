cmake_minimum_required(VERSION 3.6)

project(CmakeVirtualenv)

enable_testing()

set(PYTHON_VERSION 3)

# Find Python and Virtualenv. We don't actually use the output of the
# find_package, but it'll give nicer errors.
find_package(PythonInterp 2.7 REQUIRED)
find_program(VIRTUALENV virtualenv)
if(NOT VIRTUALENV)
    message(FATAL_ERROR "Could not find `virtualenv` in PATH. You may need install by sudo apt-get install virtualenv.")
    return()
endif()

set(VIRTUALENV ${VIRTUALENV} -p python${PYTHON_VERSION})

# Generate the virtualenv and ensure it's up to date.
add_custom_command(
    OUTPUT venv
    COMMAND ${VIRTUALENV} venv
)
add_custom_command(
    OUTPUT venv.stamp
    DEPENDS venv requirements.txt
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt requirements.txt
    COMMAND ./venv/bin/pip install -r requirements.txt --upgrade
)

# Build command line to run py.test.
set(PYTEST
    ${CMAKE_CURRENT_BINARY_DIR}/venv/bin/python${PYTHON_VERSION}
    ${CMAKE_CURRENT_BINARY_DIR}/venv/bin/py.test
)

add_custom_target(
    Tests
    ALL
    DEPENDS venv.stamp
    SOURCES requirements.txt
)

add_test(NAME run_tests
    COMMAND ${PYTEST} --benchmark-skip ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_test.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test (NAME python-tests
  COMMAND ${PYTHON_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_custom_target(
    component-tests
    COMMAND ${PYTEST} --benchmark-skip  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    component-benchmark-tests
    COMMAND ${PYTEST} --benchmark-enable --benchmark-only  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    version
    COMMAND ${PYTEST} --version
)
