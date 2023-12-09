cmake_minimum_required(VERSION 3.13)

##############################
#### MAIN
project(Tinywav
        DESCRIPTION "A minimal C library for reading and writing (16b-int & 32b-float) WAV audio files."
        HOMEPAGE_URL "https://github.com/mhroth/tinywav")

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB source_files "tinywav.c" "tinywav.h")
add_library(${PROJECT_NAME} ${source_files})

# TEST TARGET
set(TEST_NAME "${PROJECT_NAME}Test")
file(GLOB_RECURSE source_test "test/tests/*.cpp")
file(GLOB main_file "test/main.cpp" "test/TestCommon.hpp")
list(APPEND source_test ${main_file})
add_executable(${TEST_NAME} ${source_test})
target_link_libraries(${TEST_NAME} PUBLIC ${PROJECT_NAME})
target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# create source groups
source_group("" FILES ${source_files})
source_group("Tests" FILES ${source_test})

# Add external-utils
target_include_directories(${TEST_NAME} SYSTEM PRIVATE test/external-utils)

# Explicitly set CMP0110 to "NEW" to allow whitespace in tests names
if (POLICY CMP0110)
  cmake_policy(SET CMP0110 NEW)
endif()

## ENABLE THE USE OF CTEST 
include("test/external-utils/catch2/ParseAndAddCatchTests.cmake")
#include(CTest) # this will generate lots of additional targets
enable_testing()
ParseAndAddCatchTests(${TEST_NAME})
