cmake_minimum_required (VERSION 2.6)

SET( PROJECT_NAME "cvdrone-cmake" )
PROJECT ( ${PROJECT_NAME} )

if(NOT CMAKE_BUILD_TYPE)
#    set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

#####################################################################
find_package( OpenCV REQUIRED )
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories( "${PROJECT_SOURCE_DIR}/include" ${OpenCV_INCLUDE_DIR} )

if( UNIX )
    set(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")
endif()

#####################################################################
set( LIB_DRONE "cvdrone"  )
set( LIB_UTLFS "util_fs"  )

add_library( ${LIB_DRONE} STATIC
    "src/ardrone.cpp"
    "src/command.cpp"
    "src/config.cpp"
    "src/navdata.cpp"
    "src/tcp.cpp"
    "src/udp.cpp"
    "src/video.cpp"
    "include/uvlc.h"
    "include/ardrone.h")

set(EXT_LIB
	pthread
	avutil
	avformat
	avcodec
	swscale)

add_library( ${LIB_UTLFS} STATIC
    "src/lib_util_fs.cpp"
    "include/lib_util_fs.h")

#####################################################################
set( TARGET_1   "Run_CVDrone_Main_Test_${CMAKE_BUILD_TYPE}"      )
set( TARGET_2   "Sample_Camera_Calibration_${CMAKE_BUILD_TYPE}"  )
set( TARGET_3   "Sample_Condens_Tracking_${CMAKE_BUILD_TYPE}"    )
set( TARGET_4   "Sample_DeadReckoning_${CMAKE_BUILD_TYPE}"       )
set( TARGET_5   "Sample_DeadReconing_Kalman_${CMAKE_BUILD_TYPE}" )
set( TARGET_6   "Sample_Default_${CMAKE_BUILD_TYPE}"             )
set( TARGET_7   "Sample_Detection_${CMAKE_BUILD_TYPE}"           )
set( TARGET_8   "Sample_Flight_Animatino_${CMAKE_BUILD_TYPE}"    )
set( TARGET_9   "Sample_HOG_${CMAKE_BUILD_TYPE}"                 )
set( TARGET_10  "Sample_Hough_Cirlce_${CMAKE_BUILD_TYPE}"        )
set( TARGET_11  "Sample_Kalman_Tracking_${CMAKE_BUILD_TYPE}"     )
set( TARGET_12  "Sample_LED_Animation_${CMAKE_BUILD_TYPE}"       )
set( TARGET_13  "Sample_Minimal_${CMAKE_BUILD_TYPE}"             )
set( TARGET_14  "Sample_Navdata_${CMAKE_BUILD_TYPE}"             )
set( TARGET_15  "Sample_Optical_Flow_${CMAKE_BUILD_TYPE}"        )
set( TARGET_16  "Sample_Stitching_${CMAKE_BUILD_TYPE}"           )
set( TARGET_17  "Sample_Vanishing_Point_${CMAKE_BUILD_TYPE}"     )
set( TARGET_18  "Sample_Video_Record_${CMAKE_BUILD_TYPE}"        )
set( TARGET_19  "Sample_Video_Writer_${CMAKE_BUILD_TYPE}"        )
#
set( TARGET_20  "Video_Writer_From_Drone_${CMAKE_BUILD_TYPE}"    )

set( TARGET_1_CPP       "main.cpp"                                )
set( TARGET_2_CPP       "samples/sample_camera_calibration.cpp"   )
set( TARGET_3_CPP       "samples/sample_condens_tracking.cpp"     )
set( TARGET_4_CPP       "samples/sample_deadreckoning.cpp"        )
set( TARGET_5_CPP       "samples/sample_deadreckoning_kalman.cpp" )
set( TARGET_6_CPP       "samples/sample_default.cpp"              )
set( TARGET_7_CPP       "samples/sample_detection.cpp"            )
set( TARGET_8_CPP       "samples/sample_flight_animation.cpp"     )
set( TARGET_9_CPP       "samples/sample_hog.cpp"                  )
set( TARGET_10_CPP      "samples/sample_hough_circle.cpp"         )
set( TARGET_11_CPP      "samples/sample_kalman_tracking.cpp"      )
set( TARGET_12_CPP      "samples/sample_led_animation.cpp"        )
set( TARGET_13_CPP      "samples/sample_minimal.cpp"              )
set( TARGET_14_CPP      "samples/sample_navdata.cpp"              )
set( TARGET_15_CPP      "samples/sample_optical_flow.cpp"         )
set( TARGET_16_CPP      "samples/sample_stitching.cpp"            )
set( TARGET_17_CPP      "samples/sample_vanishing_point.cpp"      )
set( TARGET_18_CPP      "samples/sample_video_record.cpp"         )
set( TARGET_19_CPP      "samples/sample_video_writer.cpp"         )
#
set( TARGET_20_CPP      "video_writer_from_drone.cpp"             )


### add_executable("${TARGET_2}"  "${TARGET_2_CPP}" )
### add_executable("${TARGET_1}"  "${TARGET_1_CPP}" )
### add_executable("${TARGET_3}"  "${TARGET_3_CPP}" )
### add_executable("${TARGET_4}"  "${TARGET_4_CPP}" )
### add_executable("${TARGET_5}"  "${TARGET_5_CPP}" )
### add_executable("${TARGET_6}"  "${TARGET_6_CPP}" )
### add_executable("${TARGET_7}"  "${TARGET_7_CPP}" )
### add_executable("${TARGET_8}"  "${TARGET_8_CPP}" )
### add_executable("${TARGET_9}"  "${TARGET_9_CPP}" )
### add_executable("${TARGET_10}" "${TARGET_10_CPP}")
### add_executable("${TARGET_11}" "${TARGET_11_CPP}")
### add_executable("${TARGET_12}" "${TARGET_12_CPP}")
### add_executable("${TARGET_13}" "${TARGET_13_CPP}")
### add_executable("${TARGET_14}" "${TARGET_14_CPP}")
### add_executable("${TARGET_15}" "${TARGET_15_CPP}")
### add_executable("${TARGET_16}" "${TARGET_16_CPP}")
### add_executable("${TARGET_17}" "${TARGET_17_CPP}")
### add_executable("${TARGET_18}" "${TARGET_18_CPP}")
### add_executable("${TARGET_19}" "${TARGET_19_CPP}")
#
add_executable("${TARGET_20}" "${TARGET_20_CPP}")

###target_link_libraries("${TARGET_1}"  ${EXT_LIB} ${LIB_DRONE} ${EXT_LIB} ${OpenCV_LIBS})

###target_link_libraries("${TARGET_2}"  pthread ${LIB_DRONE} ${OpenCV_LIBS})
###target_link_libraries("${TARGET_3}"  pthread ${LIB_DRONE} ${OpenCV_LIBS})
###target_link_libraries("${TARGET_4}"  pthread ${LIB_DRONE} ${OpenCV_LIBS})
###target_link_libraries("${TARGET_5}"  pthread ${LIB_DRONE} ${OpenCV_LIBS})
###target_link_libraries("${TARGET_6}"  pthread ${LIB_DRONE} ${OpenCV_LIBS})
###target_link_libraries("${TARGET_7}"  pthread ${LIB_DRONE} ${OpenCV_LIBS})
###target_link_libraries("${TARGET_8}"  pthread ${LIB_DRONE} ${OpenCV_LIBS})
###target_link_libraries("${TARGET_9}"  pthread ${LIB_DRONE} ${OpenCV_LIBS})
###target_link_libraries("${TARGET_10}" pthread ${LIB_DRONE} ${OpenCV_LIBS})
###target_link_libraries("${TARGET_11}" pthread ${LIB_DRONE} ${OpenCV_LIBS})
###target_link_libraries("${TARGET_12}" pthread ${LIB_DRONE} ${OpenCV_LIBS})
###target_link_libraries("${TARGET_13}" pthread ${LIB_DRONE} ${OpenCV_LIBS})
###target_link_libraries("${TARGET_14}" pthread ${LIB_DRONE} ${OpenCV_LIBS})
###target_link_libraries("${TARGET_15}" pthread ${LIB_DRONE} ${OpenCV_LIBS})
###target_link_libraries("${TARGET_16}" pthread ${LIB_DRONE} ${OpenCV_LIBS})
###target_link_libraries("${TARGET_17}" pthread ${LIB_DRONE} ${OpenCV_LIBS})
###target_link_libraries("${TARGET_18}" pthread ${LIB_DRONE} ${OpenCV_LIBS})
###target_link_libraries("${TARGET_19}" pthread ${LIB_DRONE} ${OpenCV_LIBS})
#
target_link_libraries("${TARGET_20}" ${EXT_LIB} ${LIB_DRONE} ${EXT_LIB} ${LIB_UTLFS} ${OpenCV_LIBS})

##get_cmake_property(_variableNames VARIABLES)
##foreach (_variableName ${_variableNames})
##    message(STATUS "${_variableName}=${${_variableName}}")
##endforeach()
