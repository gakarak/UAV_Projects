#include "restorer_by_frame.h"

#include <algorithm>
#include <iostream>

#include <opencv2/calib3d.hpp>

#include <algorithms/transformator.h>

using namespace algorithmspkg;

RestorerByFrame::RestorerByFrame(DetectorPtr detector,
                                 DescriptorPtr descriptor,
                                 MatcherPtr matcher,
                                 size_t max_key_points_per_frame)
  : FeatureBasedRestorer(detector, descriptor, matcher,
                         max_key_points_per_frame)
{
}

void RestorerByFrame::addFrame(const cv::Mat &frame,
                               const cv::Point2f &pos,
                               double angle, double scale)
{
  frames_key_points.push_back(KeyPointsList());
  std::vector<cv::KeyPoint> &key_points = frames_key_points.back();

  getDetector()->detect(frame, frames_key_points.back());

  std::sort(key_points.begin(), key_points.end(),
            []( const cv::KeyPoint &left, const cv::KeyPoint &right ) ->
            bool { return left.response > right.response; });

  if (getMaxKeyPointsPerFrame() != 0 &&
      getMaxKeyPointsPerFrame() < key_points.size())
  {
    key_points.resize(getMaxKeyPointsPerFrame());
  }
  key_points.shrink_to_fit();



  cv::Mat descriptions;
  getDescriptor()->compute(frame, frames_key_points.back(),
                     descriptions);

  matchers.push_back(getMatcher()->clone(true));
  matchers.back()->add(descriptions);

  cv::Point2f image_center(frame.cols/2., frame.rows/2.);
  transformKeyPointsPosition(key_points, image_center,
                             pos, angle, scale);
}

void RestorerByFrame::addFrame(const cv::Point2f &image_center,
                               const KeyPointsList &key_points,
                               const cv::Mat &descriptions,
                               const cv::Point2f &pos,
                               double angle, double scale)
{
  frames_key_points.push_back(key_points);
  matchers.push_back(getMatcher()->clone(true));
  matchers.back()->add(descriptions);

  transformKeyPointsPosition(frames_key_points.back(), image_center,
                             pos, angle, scale);
}

double RestorerByFrame::recoverLocation(const cv::Point2f &frame_center,
                                        cv::Point2f &pos,
                                        double &angle, double &scale)
{
  //points for findHomography
  static std::vector<cv::Point2f> query_pts;
  static std::vector<cv::Point2f> train_pts;

  pos = cv::Point2f(0, 0);
  angle = scale = 0;
  matches.clear();

  if (query_key_points.empty())
  {
    return 0;
  }

  double max_confidence = 0;
  for (size_t frame_num = 0; frame_num < matchers.size(); frame_num++)
  {
    rough_matches.clear();
    matchers[frame_num]->match(query_descriptions, rough_matches);


    train_pts.clear();
    query_pts.clear();
    for (const cv::DMatch &match: rough_matches)
    {
      train_pts.push_back(frames_key_points[frame_num][match.trainIdx].pt);
      query_pts.push_back(query_key_points[match.queryIdx].pt);
    }

    homography_mask.clear();
    homography = cv::findHomography(query_pts, train_pts,
                                           cv::RANSAC, 3, homography_mask);

    double confidence = calculateConfidence();
    if (confidence > max_confidence)
    {
      cv::Point2f shift;
      Transformator::getParams(homography, shift, angle, scale);
      pos =  Transformator::transform(frame_center, homography);

      matches.clear();
      for (size_t i = 0; i < homography_mask.size(); i++)
      {
        if (homography_mask[i])
        {
          matches.push_back(rough_matches[i]);
          matches.back().imgIdx = frame_num;
        }
      }

      max_confidence = confidence;
    }
  }

  return max_confidence;
}

double RestorerByFrame::calculateConfidence() noexcept
{
  if (!homography_mask.empty())
  {
    size_t count = std::count_if(homography_mask.begin(), homography_mask.end(),
                        [] (char x) -> bool { return static_cast<bool>(x); });
    return count / double(homography_mask.size());
  }
  else
  {
    return 0;
  }
}


size_t RestorerByFrame::getFramesCount() const
{
  return frames_key_points.size();
}

const FeatureBasedRestorer::KeyPointsList&
            RestorerByFrame::getFrameKeyPoints(size_t frame_num) const
{
  return frames_key_points[frame_num];
}

const cv::Mat&
            RestorerByFrame::getFrameDescriptions(size_t frame_num) const
{
  return getMatcher()->getTrainDescriptors()[frame_num];
}

void RestorerByFrame::save(std::string filename)
{
  std::clog << "No implementation for saving RestorerByFrame" << std::endl;
}

void RestorerByFrame::load(std::string filename)
{
  std::clog << "No implementation for loading RestorerByFrame" << std::endl;
}
