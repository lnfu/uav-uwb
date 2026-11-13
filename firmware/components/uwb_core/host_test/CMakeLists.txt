cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)
set(EXTRA_COMPONENT_DIRS "../..")

# Find memory errors and undefined behavior, and record the coverage
set(test_options -fsanitize=address,undefined -fno-sanitize-recover=all --coverage)
idf_build_set_property(COMPILE_OPTIONS "${test_options}" APPEND)
idf_build_set_property(LINK_OPTIONS "${test_options}" APPEND)

project(uwb_core_host_test)
