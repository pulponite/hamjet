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

## Configure 3rd party dependencies

set(THIRDPARTY_INCLUDE_DIRS)
set(THIRDPARTY_LIBS)

### SDL2
### Prevent shared lib from being built because the shared and static libs stomp on each other on windows.
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)
add_subdirectory("thirdparty/SDL2" "${CMAKE_BINARY_DIR}/SDL2")
set(THIRDPARTY_INCLUDE_DIRS ${THIRDPARTY_INCLUDE_DIRS} "thirdparty/SDL2/include")
set(THIRDPARTY_LIBS ${THIRDPARTY_LIBS} SDL2main SDL2-static)

### Lua
if(EMSCRIPTEN)
  message(STATUS "Lua: Disabling ULONGJMP and RELATIVE_LOADLIB for emscripten support")
  set(LUA_USE_ULONGJMP OFF)
  set(LUA_USE_RELATIVE_LOADLIB OFF)
endif()
add_subdirectory("thirdparty/lua" "${CMAKE_BINARY_DIR}/lua")
set(THIRDPARTY_INCLUDE_DIRS ${THIRDPARTY_INCLUDE_DIRS} "${CMAKE_BINARY_DIR}/lua" "thirdparty/lua/src")
set(THIRDPARTY_LIBS ${THIRDPARTY_LIBS} liblua_static)

## Build Hamjet itself

if(EMSCRIPTEN)
  SET(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

add_executable(Hamjet src/main.c)

include_directories(${THIRDPARTY_INCLUDE_DIRS})
target_link_libraries(Hamjet ${THIRDPARTY_LIBS})

install(TARGETS Hamjet RUNTIME DESTINATION ${BIN_DIR})
