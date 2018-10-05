cmake_minimum_required(VERSION 2.8)
project(json_validator)

set(CMAKE_VERBOSE_MAKEFILE off)
set(GTEST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/deps/googletest/googletest)
set(GOOGLE_TEST_INCLUDE_PATH ${GTEST_PATH}/include)
set(TESTS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(UNIT_TESTS_PATH ${TESTS_PATH}/unit)
set(PERFORMANCE_TESTS_PATH ${TESTS_PATH}/performance)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pthread -std=c++11 -Wall -Werror")
set(DEPS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/deps)

# Unit tests
add_subdirectory(${GTEST_PATH})
add_subdirectory(${DEPS_PATH}/cJSON)
enable_testing()
include_directories(
  ${GTEST_PATH}/include ${GTEST_PATH}
  ${DEPS_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
)
set(UNIT_TEST_FILES
  ${UNIT_TESTS_PATH}/complete_functionality_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/array_validator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/boolean_validator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/int_validator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/object_validator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/string_validator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/validation_result.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/validator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/json_adapters/adapter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/json_adapters/cjson_adapter.hpp
)

add_executable(unit_tests ${UNIT_TEST_FILES})
target_link_libraries(unit_tests pthread)
target_link_libraries(unit_tests gtest gtest_main)
target_link_libraries(unit_tests cJSON)

# Performance tests
set(PERFORMANCE_TEST_FILES
  ${PERFORMANCE_TESTS_PATH}/performance.cpp
)

add_executable(performance_tests ${PERFORMANCE_TEST_FILES})
target_link_libraries(performance_tests cJSON)
