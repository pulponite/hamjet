set(gtest_force_shared_crt ON CACHE BOOL "Force shared lib use by googletest")
add_subdirectory("${Hamjet_SOURCE_DIR}/thirdparty/googletest-d225acc" "${CMAKE_CURRENT_BINARY_DIR}/googletest")

set_target_properties(gmock PROPERTIES FOLDER "hamjet/thirdparty")
set_target_properties(gmock_main PROPERTIES FOLDER "hamjet/thirdparty")
set_target_properties(gtest PROPERTIES FOLDER "hamjet/thirdparty")
set_target_properties(gtest_main PROPERTIES FOLDER "hamjet/thirdparty")
