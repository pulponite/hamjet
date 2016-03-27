
add_subdirectory(lua)
add_subdirectory(sdl2)

set(THIRDPARTY_LIBS liblua SDL2main SDL2-static PARENT_SCOPE)

set(FOOBAR "hello" "world" PARENT_SCOPE)
