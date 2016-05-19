#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <vector>

#include <opencv2/features2d.hpp>

#include "map.h"

namespace modelpkg
{
    struct Trajectory
    {
        Trajectory() {}

        std::vector<Map> maps;

        std::vector<std::vector<cv::KeyPoint>> key_points;
        std::vector<cv::Mat> descriptions; //for each frame of key points

        std::vector<double> map_quality;
    };

}

#endif // TRAJECTORY_H
