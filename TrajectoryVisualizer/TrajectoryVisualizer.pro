#-------------------------------------------------
#
# Project created by QtCreator 2016-04-20T10:53:23
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TrajectoryVisualizer
TEMPLATE = app

SOURCES += main.cpp\
    view/main_view.cpp \
    controller/main_controller.cpp \
    model/main_model.cpp \
    model/entities/trajectory.cpp \
    view/graphics_map_item.cpp \
    view/graphics_map_scene.cpp \
    view/graphics_map_view.cpp \
    view/graphics_orientation_item.cpp \
    view/graphics_trajectory_item.cpp \
    view/graphics_direction_item.cpp \
    view/graphics_keypoint_item.cpp \
    config_singleton.cpp

HEADERS  += \
    utils/csv.h \
    controller/main_controller.h \
    view/main_view.h \
    utils/acmopencv.h \
    model/entities/map.h \
    model/entities/trajectory.h \
    model/main_model.h \
    view/graphics_map_item.h \
    view/graphics_map_scene.h \
    view/graphics_map_view.h \
    view/graphics_orientation_item.h \
    view/graphics_trajectory_item.h \
    view/graphics_direction_item.h \
    view/graphics_keypoint_item.h \
    config_singleton.h

INCLUDEPATH += /home/ar/dev/opencv-3.1/include #/home/pisarik/Libs/opencv-3.1.0-build-debug/include
LIBS += -L/home/ar/dev/opencv-3.1/lib \ #/home/pisarik/Libs/opencv-3.1.0-build-debug/lib \
        -lopencv_cudabgsegm \
        -lopencv_cudaobjdetect \
        -lopencv_cudastereo \
        -lopencv_shape \
        -lopencv_stitching \
        -lopencv_cudafeatures2d \
        -lopencv_superres \
        -lopencv_cudacodec \
        -lopencv_videostab \
        #-lippicv \
        -lopencv_cudaoptflow \
        -lopencv_cudalegacy \
        -lopencv_calib3d \
        -lopencv_features2d \
        -lopencv_objdetect \
        -lopencv_highgui \
        -lopencv_videoio \
        -lopencv_photo \
        -lopencv_imgcodecs \
        -lopencv_cudawarping \
        -lopencv_cudaimgproc \
        -lopencv_cudafilters \
        -lopencv_video \
        -lopencv_ml \
        -lopencv_imgproc \
        -lopencv_flann \
        -lopencv_cudaarithm \
        -lopencv_core \
        -lopencv_cudev \
        -lopencv_xfeatures2d

FORMS    += main_view.ui
