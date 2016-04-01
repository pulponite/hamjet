if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  # Use `cmake . -Bbuild` to get build files.
  # Use `cmake --build build --config Release` to build dependencies.
  message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the SDL source code and call cmake from there")
endif()

## General setup of cmake

cmake_minimum_required(VERSION 3.5)
project(Hamjet)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Hamjet_SOURCE_DIR}/cmake")
set(BIN_DIR ${Hamjet_SOURCE_DIR}/bin)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory("cmake/thirdparty/")

message(STATUS "Top ${FOOBAR} ${THIRDPARTY_LIBS}")

add_subdirectory("cmake/libhamjet/")
