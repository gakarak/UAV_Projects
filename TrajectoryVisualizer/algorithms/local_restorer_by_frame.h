#ifndef LOCALRESTORERBYFRAME_H
#define LOCALRESTORERBYFRAME_H

#include "algorithms/restorer_by_frame.h"

namespace algorithmspkg {

class LocalRestorerByFrame : public RestorerByFrame
{
public:
  LocalRestorerByFrame() = delete;
  LocalRestorerByFrame(
              DetectorPtr detector,
              DescriptorPtr descriptor,
              MatcherPtr matcher = cv::DescriptorMatcher::create("FlannBased"),
              size_t max_key_points_per_frame = 0);

  void addFrame(const cv::Mat &frame,
                const cv::Point2f &pos, double angle, double scale) override;
  void addFrame(const cv::Mat &frame);

  void addFrame(const cv::Point2f &image_center,
                const KeyPointsList &key_points, const cv::Mat &descriptions,
                const cv::Point2f &pos, double angle, double scale) override;
  void addFrame(const cv::Point2f &image_center,
                const KeyPointsList &key_points, const cv::Mat &descriptions);

private:
  double free_place_x;
};


}

#endif // LOCALRESTORERBYFRAME_H
