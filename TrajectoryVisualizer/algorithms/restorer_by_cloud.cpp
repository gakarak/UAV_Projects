#include "restorer_by_cloud.h"

#include <algorithm>

#include <opencv2/calib3d.hpp>

#include "transformator.h"

using namespace algorithmspkg;

RestorerByCloud::RestorerByCloud(FeatureBasedRestorer::DetectorPtr detector,
                                 FeatureBasedRestorer::DescriptorPtr descriptor,
                                 FeatureBasedRestorer::MatcherPtr matcher,
                                 size_t max_key_points_per_frame)
  : FeatureBasedRestorer(detector, descriptor), matcher(matcher),
    max_key_points_per_frame(max_key_points_per_frame)
{
}

void RestorerByCloud::addFrame(const cv::Mat &frame,
                               const cv::Point2f &pos,
                               double angle, double scale)
{
  frames_key_points.push_back(KeyPointsList());
  std::vector<cv::KeyPoint> &key_points = frames_key_points.back();

  getDetector()->detect(frame, frames_key_points.back());

  std::sort(key_points.begin(), key_points.end(),
            []( const cv::KeyPoint &left, const cv::KeyPoint &right ) ->
            bool { return left.response > right.response; });

  if (max_key_points_per_frame != 0 &&
      max_key_points_per_frame < key_points.size())
  {
    key_points.resize(max_key_points_per_frame);
  }
  key_points.shrink_to_fit();



  cv::Mat descriptions;
  getDescriptor()->compute(frame, frames_key_points.back(),
                     descriptions);

  matcher->add(descriptions);

  cv::Point2f image_center(frame.cols/2., frame.rows/2.);
  transformKeyPointsPosition(key_points, image_center,
                             pos, angle, scale);
}

void RestorerByCloud::addFrame(const cv::Point2f &image_center,
                               const KeyPointsList &key_points,
                               const cv::Mat &descriptions,
                               const cv::Point2f &pos,
                               double angle, double scale)
{
  frames_key_points.push_back(key_points);
  matcher->add(descriptions);

  transformKeyPointsPosition(frames_key_points.back(), image_center,
                             pos, angle, scale);
}

double RestorerByCloud::recoverLocation(cv::Point2f &pos,
                               double &angle, double &scale)
{
  rough_matches.clear();
  if (query_key_points.empty())
  {
    pos = cv::Point2f(0, 0);
    angle = scale = 0;
    return 0;
  }

  matcher->match(query_descriptions, rough_matches);

  //prepare points for findHomography
  std::vector<cv::Point2f> query_pts;
  std::vector<cv::Point2f> train_pts;

  for (const cv::DMatch &match: rough_matches)
  {
    train_pts.push_back(frames_key_points[match.imgIdx][match.trainIdx].pt);
    query_pts.push_back(query_key_points[match.queryIdx].pt);
  }

  std::vector<char> mask;
  cv::Mat homography = cv::findHomography(query_pts, train_pts,
                                         cv::RANSAC, 3, mask);
  Transformator::getParams(homography, pos, angle, scale);

  matches.clear();
  size_t count = 0;
  for (size_t i = 0; i < mask.size(); i++)
  {
    if (mask[i])
    {
      count++;
      matches.push_back(rough_matches[i]);
    }
  }
  double score = count / double(mask.size());

  return score;
}

size_t RestorerByCloud::getFramesCount() const
{
  return frames_key_points.size();
}

const FeatureBasedRestorer::KeyPointsList&
            RestorerByCloud::getFrameKeyPoints(size_t frame_num) const
{
  return frames_key_points[frame_num];
}

const cv::Mat&
            RestorerByCloud::getFrameDescriptions(size_t frame_num) const
{
  return matcher->getTrainDescriptors()[frame_num];
}

FeatureBasedRestorer::MatcherPtr RestorerByCloud::getMatcher() const
{
  return matcher;
}
