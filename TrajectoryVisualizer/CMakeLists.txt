cmake_minimum_required(VERSION 2.6)

#
# проверка Unicode (UTF-8):
# 偉大和強大的俄語。
# اللغة الروسية العظمى والأقوياء.
# Η μεγάλη και πανίσχυρη ρωσική γλώσσα.
# השפה הרוסיה הגדולה והעצומה.
# 大いなるロシア語。
# 위대하고 강력한 러시아어 언어입니다.
# Wielki i potężny język rosyjski.
#

PROJECT(TrajectoryVisualizer)

set( TARGET_0 "TrajectoryVisualizer" )

set(CMAKE_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    #set(CMAKE_BUILD_TYPE "Debug")
endif()

set(PLATFORM "x32")
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
    set(PLATFORM "x64")
endif()

set(BUILD_PREFIX "${CMAKE_BUILD_TYPE}_${PLATFORM}")
message("build prefix=[${BUILD_PREFIX}]")

################################
#FIXME: Only for Unix?
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

################################
FIND_PACKAGE( OpenCV REQUIRED )

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else()
    message(STATUS "OpenMP not found!")
endif()

################################
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/model)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/model/entities)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/view)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})

add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")


SET(PRJ_SRC
		main.cpp
		controller/main_controller.cpp
		model/entities/trajectory.cpp
		model/main_model.cpp
		view/graphics_direction_item.cpp
		view/graphics_keypoint_item.cpp
		view/graphics_map_item.cpp
		view/graphics_map_scene.cpp
		view/graphics_map_view.cpp
		view/graphics_orientation_item.cpp
		view/graphics_trajectory_item.cpp
		view/main_view.cpp)

SET(PRJ_HDR	
		view/main_view.h
		controller/main_controller.h
		model/dataset.h
		model/entities/map.h
		model/entities/trajectory.h
		model/main_model.h
		model/trajectory_info.h
		utils/acmopencv.h
		utils/csv.h
		view/graphics_direction_item.h
		view/graphics_keypoint_item.h
		view/graphics_map_item.h
		view/graphics_map_scene.h
		view/graphics_map_view.h
		view/graphics_orientation_item.h
		view/graphics_trajectory_item.h)

SET(PRJ_FORMS	main_view.ui )
##SET(PRJ_RES	resources.qrc)

QT5_WRAP_UI(PRJ_FORMS_HDR	${PRJ_FORMS})
##QT5_ADD_RESOURCES(PRJ_RES_RCC	${PRJ_RES})

ADD_EXECUTABLE( ${TARGET_0}_${BUILD_PREFIX}		${PRJ_SRC} ${PRJ_HDR_MOC} ${PRJ_FORMS_HDR} ${PRJ_RES_RCC})
TARGET_LINK_LIBRARIES( ${TARGET_0}_${BUILD_PREFIX}
			    ${LIB_MVPTREE} ${LIB_DSCINT} ${LIB_DSCCOO} ${LIB_DSC} ${LIB_MMAP}
			    ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} ${OpenCV_LIBS})


##get_cmake_property(_variableNames VARIABLES)
##foreach (_variableName ${_variableNames})
##   message(STATUS "${_variableName}=${${_variableName}}")
##endforeach()
