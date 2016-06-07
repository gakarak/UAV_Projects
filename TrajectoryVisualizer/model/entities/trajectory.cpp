#include "trajectory.h"

#include "utils/csv.h"

using namespace modelpkg;
using namespace std;

void Trajectory::pushBackFrame(const Map &frame)
{
    frames.push_back(frame);
    key_points.push_back(vector<cv::KeyPoint>());
    descriptions.push_back(cv::Mat());
    frames_quality.push_back(0);
}

void Trajectory::setFrameKeyPoints(int frame_num, const std::vector<cv::KeyPoint> &frame_key_points)
{
    key_points[frame_num] = frame_key_points;
}

void Trajectory::setFrameDescription(int frame_num, const cv::Mat &description)
{
    descriptions[frame_num] = description;
}

void Trajectory::setFrameQuality(int frame_num, double quality)
{
    frames_quality[frame_num] = quality;
}
