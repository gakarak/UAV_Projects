#
# cmake-build
# Author: ar
#

cmake_minimum_required(VERSION 2.6)

PROJECT(VideoNav8)

set(CMAKE_BUILD_TYPE "Debug")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()
set(PLATFORM "x32")
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
    set(PLATFORM "x64")
endif()
set(BUILD_PREFIX "${CMAKE_BUILD_TYPE}_${PLATFORM}")
message("build prefix=[${BUILD_PREFIX}]")

find_package( OpenCV REQUIRED )
set(CMAKE_INCLUDE_CURRENT_DIR ON)

##########################################
set( TARGET_0 "VideoNav")

##########################################
add_executable("${TARGET_0}_${BUILD_PREFIX}"            "${TARGET_0}.cpp"      )
target_link_libraries("${TARGET_0}_${BUILD_PREFIX}"     ${OpenCV_LIBS}  )

