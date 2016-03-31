project (zlib C)

set(CMAKE_CURRENT_SOURCE_DIR "${Hamjet_SOURCE_DIR}/thirdparty/zlib-1.2.8")
set(ZLIB_INCLUDES "${Hamjet_SOURCE_DIR}/thirdparty/zlib-1.2.8")

file(GLOB libFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
file(GLOB libHeaders "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library(libzlib STATIC ${libFiles} ${libHeaders})
target_include_directories(libzlib PUBLIC ${ZLIB_INCLUDES})
set_target_properties(libzlib PROPERTIES FOLDER "hamjet/thirdparty")
