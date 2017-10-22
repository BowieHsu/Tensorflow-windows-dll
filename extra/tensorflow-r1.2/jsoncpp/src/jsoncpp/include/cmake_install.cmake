# Install script for directory: F:/tensorflow-r1.2/tensorflow-r1.2/tensorflow/contrib/cmake/build/jsoncpp/src/jsoncpp/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/jsoncpp")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/json" TYPE FILE FILES
    "F:/tensorflow-r1.2/tensorflow-r1.2/tensorflow/contrib/cmake/build/jsoncpp/src/jsoncpp/include/json/allocator.h"
    "F:/tensorflow-r1.2/tensorflow-r1.2/tensorflow/contrib/cmake/build/jsoncpp/src/jsoncpp/include/json/assertions.h"
    "F:/tensorflow-r1.2/tensorflow-r1.2/tensorflow/contrib/cmake/build/jsoncpp/src/jsoncpp/include/json/autolink.h"
    "F:/tensorflow-r1.2/tensorflow-r1.2/tensorflow/contrib/cmake/build/jsoncpp/src/jsoncpp/include/json/config.h"
    "F:/tensorflow-r1.2/tensorflow-r1.2/tensorflow/contrib/cmake/build/jsoncpp/src/jsoncpp/include/json/features.h"
    "F:/tensorflow-r1.2/tensorflow-r1.2/tensorflow/contrib/cmake/build/jsoncpp/src/jsoncpp/include/json/forwards.h"
    "F:/tensorflow-r1.2/tensorflow-r1.2/tensorflow/contrib/cmake/build/jsoncpp/src/jsoncpp/include/json/json.h"
    "F:/tensorflow-r1.2/tensorflow-r1.2/tensorflow/contrib/cmake/build/jsoncpp/src/jsoncpp/include/json/reader.h"
    "F:/tensorflow-r1.2/tensorflow-r1.2/tensorflow/contrib/cmake/build/jsoncpp/src/jsoncpp/include/json/value.h"
    "F:/tensorflow-r1.2/tensorflow-r1.2/tensorflow/contrib/cmake/build/jsoncpp/src/jsoncpp/include/json/version.h"
    "F:/tensorflow-r1.2/tensorflow-r1.2/tensorflow/contrib/cmake/build/jsoncpp/src/jsoncpp/include/json/writer.h"
    )
endif()

