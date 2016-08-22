QT += gui

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += $$PWD
#{error($$PWD)}

SOURCES += \
    $$PWD/model/entities/trajectory.cpp \
    $$PWD/algorithms/trajectory_recover.cpp \
    $$PWD/algorithms/transformator.cpp \
    $$PWD/algorithms/trajectory_loader.cpp \
    $$PWD/utils/geom_utils.cpp \
    $$PWD/algorithms/feature_based_restorer.cpp \
    $$PWD/algorithms/restorer_by_cloud.cpp \
    $$PWD/algorithms/progress_bar_notifier.cpp \
    $$PWD/algorithms/restorer_by_frame.cpp \
    $$PWD/utils/gradient_density.cpp \
    $$PWD/algorithms/restorer_by_frame_blocks.cpp \
    $$PWD/algorithms/local_restorer_by_frame.cpp \
    $$PWD/algorithms/feature2d_manager.cpp

HEADERS  += \
    $$PWD/utils/csv.h \
    $$PWD/utils/acmopencv.h \
    $$PWD/model/entities/map.h \
    $$PWD/model/entities/trajectory.h \
    $$PWD/utils/gradient_density.h \
    $$PWD/algorithms/trajectory_recover.h \
    $$PWD/algorithms/transformator.h \
    $$PWD/algorithms/image_info_estimator.h \
    $$PWD/algorithms/image_info_gradient_estimator.h \
    $$PWD/algorithms/trajectory_loader.h \
    $$PWD/utils/geom_utils.h \
    $$PWD/algorithms/feature_based_restorer.h \
    $$PWD/algorithms/ilocation_restorer.h \
    $$PWD/algorithms/restorer_by_cloud.h \
    $$PWD/model/entities/location.h \
    $$PWD/algorithms/progress_bar_notifier.h \
    $$PWD/algorithms/restorer_by_frame.h \
    $$PWD/algorithms/restorer_by_frame_blocks.h \
    $$PWD/algorithms/saveable_flann_matcher.h \
    $$PWD/algorithms/local_restorer_by_frame.h \
    $$PWD/algorithms/feature2d_manager.h

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

