cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

option(USE_IWYU "Use include-what-you-use for static include analysis" OFF)

if (USE_IWYU)

  find_program(iwyu_path NAMES include-what-you-use iwyu)
  if(NOT iwyu_path)
    message(FATAL_ERROR "Could not find the program include-what-you-use")
  else()
    message(STATUS "Using include-what-you-use include analysis.")
  endif()

  set_property(TARGET mpeg2ts PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})

else()
  message(STATUS "Note include-what-you-use is NOT enabled.")
endif()
