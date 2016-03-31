project(hamjet-demo)

file(GLOB inSource "${Hamjet_SOURCE_DIR}/demo/src/*.cpp")
file(GLOB inHeaders "${Hamjet_SOURCE_DIR}/demo/include/*.hpp")

message(STATUS "hamjet-demo ${inSource} ${inHeaders}")

add_executable(hamjet-demo ${inSource} ${inHeaders})

set_target_properties(hamjet-demo PROPERTIES FOLDER "hamjet")
target_link_libraries(hamjet-demo libhamjet)
