
add_subdirectory(lua)
add_subdirectory(sdl2)
add_subdirectory(zlib)
add_subdirectory(lpng)

set(THIRDPARTY_LIBS liblua SDL2main SDL2-static libzlib libpng PARENT_SCOPE)
