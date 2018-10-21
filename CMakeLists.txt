# CMakeLists.txt for the ROOT test programs.
# This shows nicely how to compile and link applications
# using the ROOT libraries on all supported platforms.
#
# Author: Pere Mato, 25/10/2010
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")
include(UMacros)
project(U2boot)
find_package(ROOT REQUIRED)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -ggdb8")
if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.0)
  set (CMAKE_CXX_STANDARD 11)
endif()
add_subdirectory(src)
add_subdirectory(data)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(setenvironment ENVIRONMENT PATH=${CMAKE_BINARY_DIR}/bin${cfgdir}:$ENV{PATH})

GENERATE_BASH()

get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()

