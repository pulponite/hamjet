project (lpng C)

set(CMAKE_CURRENT_SOURCE_DIR "${Hamjet_SOURCE_DIR}/thirdparty/lpng1621")
set(LPNG_INCLUDES "${Hamjet_SOURCE_DIR}/thirdparty/lpng1621")

file(GLOB libFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
file(GLOB libHeaders "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library(libpng STATIC ${libFiles} ${libHeaders})
target_include_directories(libpng PUBLIC ${LPNG_INCLUDES})
target_link_libraries(libpng libzlib)
set_target_properties(libpng PROPERTIES FOLDER "hamjet/thirdparty")
