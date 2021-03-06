project(libhamjet)

set(CMAKE_CURRENT_SOURCE_DIR "${Hamjet_SOURCE_DIR}/libhamjet")
set(LIBHAMJET_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include")

file(GLOB sourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB includeFiles "${LIBHAMJET_INCLUDES}/Hamjet/*.hpp")
file(GLOB testFiles "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")

source_group("Source Files" FILES ${sourceFiles})
source_group("Include Files" FILES ${includeFiles})

add_library(libhamjet STATIC ${sourceFiles} ${includeFiles})
target_include_directories(libhamjet PUBLIC ${LIBHAMJET_INCLUDES})
target_link_libraries(libhamjet ${THIRDPARTY_LIBS})
set_target_properties(libhamjet PROPERTIES FOLDER "hamjet")

add_executable(libhamjet-test ${testFiles})
target_link_libraries(libhamjet-test libhamjet gtest)
set_target_properties(libhamjet-test PROPERTIES FOLDER "hamjet")

set(LIBHAMJET_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include" PARENT_SCOPE)
