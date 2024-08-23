cmake_minimum_required(VERSION 3.10)

if(POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif()

cmake_policy(SET CMP0048 NEW)
if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW)
endif()

if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW) # CMake 3.12
endif()

if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

# Set the project name and version
project(FES_API VERSION 1.0)

include(CheckFunctionExists)
include(CheckCXXSourceRuns)

# Specify the C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

if(NOT WIN32)
  if(NOT CMAKE_CXX_FLAGS MATCHES "-Wall$")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  endif()
  if(NOT CMAKE_CXX_COMPILER MATCHES "icpc$" AND NOT CMAKE_CXX_FLAGS MATCHES
                                                "-Wpedantic$")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic")
  endif()
endif()


check_function_exists(pow POW_FUNCTION_EXISTS)
if(NOT POW_FUNCTION_EXISTS)
  unset(POW_FUNCTION_EXISTS CACHE)
  list(APPEND CMAKE_REQUIRED_LIBRARIES m)
  check_function_exists(pow POW_FUNCTION_EXISTS)
  if(POW_FUNCTION_EXISTS)
    set(MATH_LIBRARY
        m
        CACHE STRING "" FORCE)
  else()
    message(FATAL_ERROR "Failed making the pow() function available")
  endif()
endif()

# CMake module search path
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;"
                      "${CMAKE_MODULE_PATH}")

# By default, build type is set to release, with debugging information.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# API documentation if (BUILD_DOC) add_subdirectory(docs) endif()

# NetCDF
find_package(NetCDF 4.1.1 REQUIRED)
if(NOT EXISTS ${NETCDF_INCLUDE_DIRS}/netcdf.h)
  message(FATAL_ERROR "Unable to find NetCDF C-Header files")
endif()
include_directories(${NETCDF_INCLUDE_DIRS})

# uthash
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/uthash/src)

# Include directories
include_directories(include)
install(FILES include/fes.h DESTINATION include)

add_subdirectory(src)

# Add executable for fes_api
add_executable(fes_api src/fes_api.c)

# Add other source files from the src directory
file(GLOB SOURCES "src/*.c")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/fes_api.c")
target_sources(fes_api PRIVATE ${SOURCES})

# Link necessary libraries to the executable
target_link_libraries(fes_api ${NETCDF_LIBRARIES} m)  # Add other libraries if needed

# Install header files
install(FILES include/fes.h DESTINATION include)

# Add other source files from the src directory
include(CPack)