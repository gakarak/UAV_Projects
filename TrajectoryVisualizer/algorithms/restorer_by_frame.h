#ifndef RESTORER_BY_FRAME_H
#define RESTORER_BY_FRAME_H

#include "feature_based_restorer.h"

namespace algorithmspkg
{

class RestorerByFrame : public FeatureBasedRestorer
{
public:
  //there is must be array<cv::Point2f, 4>, but InputArray accepts only vector
  //so contract about 4 items in this type
  //0-(0,0),1(0, height), 2(width, height), 3(width, 0)
  using FramePolygon = std::vector<cv::Point2f>;

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

  double recoverLocation(const cv::Rect2f &que_frame_rect,
                         cv::Point2f &pos, double &angle, double &scale) override;

  size_t getFramesCount() const override;
  const KeyPointsList &getFrameKeyPoints(size_t frame_num) const override;
  const cv::Mat &getFrameDescriptions(size_t frame_num) const override;

  void save(std::string filename) override;
  void load(std::string filename) override;

private:
  double calculateMaskConfidence() const noexcept;
  double calculateAreaConfidence(const cv::Rect2f &query_frame_rect,
                                 int base_frame_num) const;
  FramePolygon calculateFramePolygon(const cv::Point2f &frame_center,
                                     const cv::Point2f &pos, double angle,
                                     double scale) const;
  FramePolygon calculateFramePolygon(const cv::Point2f &frame_center,
                                     const cv::Mat &homography) const;
  FramePolygon calculateFramePolygon(const cv::Rect2f &frame_rect,
                                     const cv::Point2f &pos, double angle,
                                     double scale) const;
  FramePolygon calculateFramePolygon(const cv::Rect2f &frame_rect,
                                     const cv::Mat &homography) const;

  std::vector<char>           homography_mask;

  std::vector<FramePolygon>   frames_polygons; //in pixels_size*scale
  std::vector<double>         frames_area;

  std::vector<KeyPointsList>  frames_key_points;
  std::vector<MatcherPtr>     matchers; //for each frame

  MatchesList rough_matches;
};

}

#endif // RESTORER_BY_FRAME_H
