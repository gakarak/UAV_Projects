#ifndef FEATURE_BASED_RESTORER_H
#define FEATURE_BASED_RESTORER_H

#include "ilocation_restorer.h"

#include <opencv2/features2d.hpp>

namespace algorithmspkg
{

class FeatureBasedRestorer : public ILocationRestorer
{
 public:
  using KeyPointsList = std::vector<cv::KeyPoint>;
  using MatchesList = std::vector<cv::DMatch>;
  using DetectorPtr = cv::Ptr<cv::FeatureDetector>;
  using DescriptorPtr = cv::Ptr<cv::DescriptorExtractor>;
  using MatcherPtr = cv::Ptr<cv::DescriptorMatcher>;

  FeatureBasedRestorer() = delete;
  FeatureBasedRestorer(DetectorPtr detector,
                       DescriptorPtr descriptor,
                       MatcherPtr matcher = cv::DescriptorMatcher::
                                                create("FlannBased"),
                       size_t max_key_points_per_frame = 0);

  /**
   * @brief addFrame - adds precomputed key points and descrs, don't check them.
   * @param key_points - precomputed key points (must satisfy class invariant)
   * @param descriptions - precomputed descrs (must satisfy class invariant)
   * @param pos - position of the frame center in preffered units
   * @param angle - counterclockwise in degrees [-180; 180], 0 is east
   * @param scale - scale from pixels to preffered units
   */
  virtual void addFrame(const cv::Point2f &image_center,
                        const KeyPointsList &key_points,
                        const cv::Mat &descriptions,
                        const cv::Point2f &pos, double angle, double scale) = 0;

  double recoverLocation(const cv::Mat &query_frame,
                                   cv::Point2f &pos,
                                   double &angle,
                                   double &scale) override;

  /**
   * @brief recoverLocation - transforms frame_center to frame_rect
   *                          and recover with frame_rect
   * @param frame_center - center of frame in pixels, need for pos return and
   *                       areaConfidence
   * @param pos - out position in preffered units
   * @param angle - out counterclockwise in degrees [-180; 180], 0 is east
   * @param scale - scale from pixels to preffered units
   * @return confidence (the strength of the result) [0..1]
   */
  virtual double recoverLocation(const cv::Point2f &frame_center,
                                 cv::Point2f &pos,
                                 double &angle, double &scale);

  /**
   * @brief recoverLocation - recover position, angle and scale of setted query
   * @param frame_rect - need for pos return and areaConfidence
   * @param pos - out position in preffered units
   * @param angle - out counterclockwise in degrees [-180; 180], 0 is east
   * @param scale - out scale from pixels to preffered units
   * @return confidence (the strength of the result) [0..1]
   */
  virtual double recoverLocation(const cv::Rect2f &frame_rect,
                                 cv::Point2f &pos,
                                 double &angle, double &scale) = 0;

  virtual size_t getFramesCount() const = 0;
  virtual const KeyPointsList& getFrameKeyPoints(size_t frame_num) const = 0;
  virtual const cv::Mat& getFrameDescriptions(size_t frame_num) const = 0;

  void setQueryKeyPoints(const KeyPointsList &key_points);
  void setQueryDescriptions(const cv::Mat &descriptions);

  const KeyPointsList& getQueryKeyPoints() const;
  const cv::Mat& getQueryDescriptions() const;

  virtual const MatchesList& getLastMatches() const;
  virtual const cv::Mat& getLastHomography() const;

  DetectorPtr   getDetector() const;
  DescriptorPtr getDescriptor() const;
  MatcherPtr    getMatcher() const;
  size_t        getMaxKeyPointsPerFrame() const;

 protected:
  virtual void transformKeyPointsPosition(KeyPointsList &key_points,
                                          const cv::Point2f &image_center,
                                          const cv::Point2f &pos,
                                          double angle, double scale);

  KeyPointsList               query_key_points;
  cv::Mat                     query_descriptions;

  cv::Mat                     homography;
  MatchesList                 matches;

  bool isTrained;

 private:
  DetectorPtr detector;
  DescriptorPtr descriptor;
  MatcherPtr matcher;

  const size_t max_key_points_per_frame;
};

}

#endif // FEATURE_BASED_RESTORER_H
