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

        void pushBackFrame(const Map &frame);
        void removeFrame(int frame_num);
        void setFrameKeyPoints(int frame_num, const std::vector<cv::KeyPoint> &frame_key_points);
        void setFrameDescription(int frame_num, const cv::Mat &description);
        void setFrameQuality(int frame_num, double quality);

        size_t getFramesCount() const  { return frames.size(); }

        std::vector<cv::KeyPoint>&  getFrameAllKeyPoints(int frame_num) { return key_points[frame_num]; }

        const Map&                          getFrame(int frame_num)                     const { return frames[frame_num]; }
        const std::vector<cv::KeyPoint>&    getFrameAllKeyPoints(int frame_num)         const { return key_points[frame_num]; }
        const cv::KeyPoint&                 getFrameKeyPoint(int frame_num, int kp_num) const { return key_points[frame_num][kp_num]; }
        const cv::Mat&                      getFrameDescription(int frame_num)          const { return descriptions[frame_num]; }
              double                        getFrameQuality(int frame_num)              const { return frames_quality[frame_num]; }



        const std::vector<Map>&                         getAllFrames()          const { return frames; }
        const std::vector<std::vector<cv::KeyPoint>>&   getAllKeyPoints()       const { return key_points; }
        const std::vector<cv::Mat>&                     getAllDescriptions()    const { return descriptions; }
        const std::vector<double>&                      getAllFramesQuality()   const { return frames_quality; }


    private:

        std::vector<Map> frames;

        std::vector<std::vector<cv::KeyPoint>> key_points;
        std::vector<cv::Mat> descriptions; //for each frame of key points

        std::vector<double> frames_quality;
    };

}

#endif // TRAJECTORY_H
