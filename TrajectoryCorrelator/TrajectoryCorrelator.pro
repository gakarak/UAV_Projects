QT += core

CONFIG += c++11

TARGET = TrajectoryCorrelator
CONFIG -= app_bundle

TEMPLATE = app

!include(../TrajectoryVisualizer/TrajectoryAlgorithms.pri)\
{ error("TrajectoryAlgorithms.pri not found") }

SOURCES += main.cpp
