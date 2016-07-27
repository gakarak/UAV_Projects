#ifndef RESTORER_BY_CLOUD_H
#define RESTORER_BY_CLOUD_H

#include "feature_based_restorer.h"

namespace algorithmspkg
{

class RestorerByCloud : public FeatureBasedRestorer
{
public:
  RestorerByCloud() = delete;
  RestorerByCloud(DetectorPtr detector,
                  DescriptorPtr descriptor,
                  MatcherPtr matcher, size_t max_key_points_per_frame = 0);

  void addFrame(const cv::Mat &frame,
                const cv::Point2f &pos, double angle, double scale) override;

  void addFrame(const cv::Point2f &image_center,
                const KeyPointsList &key_points,
                const cv::Mat &descriptions,
                const cv::Point2f &pos, double angle, double scale) override;

  double recoverLocation(cv::Point2f &pos,
                                 double &angle, double &scale) override;

  size_t getFramesCount() const override;
  const KeyPointsList& getFrameKeyPoints(size_t frame_num) const override;
  const cv::Mat& getFrameDescriptions(size_t frame_num) const override;


  MatcherPtr    getMatcher() const;

private:
  std::vector<KeyPointsList>  frames_key_points;

  MatchesList rough_matches;

  MatcherPtr matcher;

  const size_t max_key_points_per_frame;
};

}

#endif // RESTORER_BY_CLOUD_H
