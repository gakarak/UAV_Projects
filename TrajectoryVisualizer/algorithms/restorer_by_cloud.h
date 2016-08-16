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
                  MatcherPtr matcher = cv::DescriptorMatcher::create("FlannBased"),
                  size_t max_key_points_per_frame = 0);

  void addFrame(const cv::Mat &frame,
                const cv::Point2f &pos, double angle, double scale) override;

  void addFrame(const cv::Point2f &image_center,
                const KeyPointsList &key_points,
                const cv::Mat &descriptions,
                const cv::Point2f &pos, double angle, double scale) override;

  double recoverLocation(const cv::Rect2f &frame_rect, cv::Point2f &pos,
                                 double &angle, double &scale) override;

  size_t getFramesCount() const override;
  const KeyPointsList& getFrameKeyPoints(size_t frame_num) const override;
  const cv::Mat& getFrameDescriptions(size_t frame_num) const override;

  void save(std::string filename) override;
  void load(std::string filename) override;
private:
  double calculateConfidence() const noexcept;
  std::vector<char> homography_mask;

  std::vector<KeyPointsList>  frames_key_points;

  MatchesList rough_matches;
};

}

#endif // RESTORER_BY_CLOUD_H
