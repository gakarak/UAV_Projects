#ifndef RESTORERBYFRAME_H
#define RESTORERBYFRAME_H

#include "feature_based_restorer.h"

namespace algorithmspkg
{

class RestorerByFrame : public FeatureBasedRestorer
{
public:
  RestorerByFrame() = delete;
  RestorerByFrame(DetectorPtr detector,
                  DescriptorPtr descriptor,
                  MatcherPtr matcher = cv::DescriptorMatcher::create("FlannBased"),
                  size_t max_key_points_per_frame = 0);

  void addFrame(const cv::Mat &frame,
                const cv::Point2f &pos, double angle, double scale) override;
  void addFrame(const cv::Point2f &image_center,
                const KeyPointsList &key_points,
                const cv::Mat &descriptions,
                const cv::Point2f &pos, double angle, double scale) override;

  double recoverLocation(const cv::Point2f &frame_center,
                         cv::Point2f &pos, double &angle, double &scale) override;

  size_t getFramesCount() const override;
  const KeyPointsList &getFrameKeyPoints(size_t frame_num) const override;
  const cv::Mat &getFrameDescriptions(size_t frame_num) const override;

  void save(std::string filename) override;
  void load(std::string filename) override;

private:
  double calculateConfidence() const noexcept;
  std::vector<char>           homography_mask;

  std::vector<KeyPointsList>  frames_key_points;
  std::vector<MatcherPtr>     matchers; //for each frame

  MatchesList rough_matches;
};

}

#endif // RESTORERBYFRAME_H
