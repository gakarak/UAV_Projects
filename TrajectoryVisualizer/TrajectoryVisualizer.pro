#-------------------------------------------------
#
# Project created by QtCreator 2016-04-20T10:53:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TrajectoryVisualizer
TEMPLATE = app

include(TrajectoryAlgorithms.pri)

SOURCES += main.cpp\
    view/main_view.cpp \
    controller/main_controller.cpp \
    model/main_model.cpp \
    view/graphics_map_item.cpp \
    view/graphics_map_scene.cpp \
    view/graphics_map_view.cpp \
    view/graphics_orientation_item.cpp \
    view/graphics_trajectory_item.cpp \
    view/graphics_direction_item.cpp \
    view/graphics_keypoint_item.cpp \
    config_singleton.cpp \
    view/graphics_fast_keypoint_item.cpp \
    view/graphics_frame_item.cpp \
    view/graphics_matches_item.cpp \

HEADERS  += \
    controller/main_controller.h \
    view/main_view.h \
    model/main_model.h \
    view/graphics_map_item.h \
    view/graphics_map_scene.h \
    view/graphics_map_view.h \
    view/graphics_orientation_item.h \
    view/graphics_trajectory_item.h \
    view/graphics_direction_item.h \
    view/graphics_keypoint_item.h \
    config_singleton.h \
    view/graphics_fast_keypoint_item.h \
    view/graphics_frame_item.h \
    view/graphics_matches_item.h \

FORMS    += main_view.ui

DISTFILES +=
