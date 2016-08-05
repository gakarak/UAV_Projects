#include "feature_based_restorer.h"
#include "transformator.h"

using namespace algorithmspkg;

FeatureBasedRestorer::FeatureBasedRestorer(DetectorPtr detector,
                                           DescriptorPtr descriptor,
                                           MatcherPtr matcher,
                                           size_t max_key_points_per_frame)
  : detector(detector), descriptor(descriptor), matcher(matcher),
    max_key_points_per_frame(max_key_points_per_frame)
{
}

void FeatureBasedRestorer::setQueryKeyPoints(const KeyPointsList &key_points)
{
  query_key_points = key_points;
}

void FeatureBasedRestorer::setQueryDescriptions(const cv::Mat &descriptions)
{
  query_descriptions = descriptions;
}

const FeatureBasedRestorer::KeyPointsList
              &FeatureBasedRestorer::getQueryKeyPoints() const
{
  return query_key_points;
}

const cv::Mat &FeatureBasedRestorer::getQueryDescriptions() const
{
  return query_descriptions;
}

double FeatureBasedRestorer::recoverLocation(const cv::Mat &query_frame,
                                             cv::Point2f &pos,
                                             double &angle,
                                             double &scale)
{
  query_key_points.clear();

  detector->detect(query_frame, query_key_points);
  descriptor->compute(query_frame, query_key_points,
                                   query_descriptions);

  cv::Point2f image_center( query_frame.cols/2., query_frame.rows/2. );

  return recoverLocation(image_center, pos, angle, scale);
}

const FeatureBasedRestorer::MatchesList&
            FeatureBasedRestorer::getLastMatches() const
{
  return matches;
}

const cv::Mat& FeatureBasedRestorer::getLastHomography() const
{
  return homography;
}


FeatureBasedRestorer::DetectorPtr FeatureBasedRestorer::getDetector() const
{
  return detector;
}

FeatureBasedRestorer::DescriptorPtr FeatureBasedRestorer::getDescriptor() const
{
  return descriptor;
}

FeatureBasedRestorer::MatcherPtr FeatureBasedRestorer::getMatcher() const
{
  return matcher;
}

size_t FeatureBasedRestorer::getMaxKeyPointsPerFrame() const
{
  return max_key_points_per_frame;
}

void FeatureBasedRestorer::transformKeyPointsPosition(
                    FeatureBasedRestorer::KeyPointsList &key_points,
                    const cv::Point2f &image_center,
                    const cv::Point2f &pos, double angle, double scale)
{
  std::vector<cv::Point2f> to_transform;
  for (const auto &kp : key_points)
  {
    to_transform.push_back(kp.pt);
  }

  //from local to trajectory coords
  to_transform = Transformator::transform(to_transform, {//transformations
                                  Transformator::getTranslate(-image_center),
                                  Transformator::getRotate(angle),
                                  Transformator::getScale(scale),
                                  Transformator::getTranslate(pos)
                               });


  for (size_t i = 0; i < to_transform.size(); i++)
  {
    key_points[i].pt = to_transform[i];
    key_points[i].angle += angle;
    key_points[i].size *= scale;
  }
}
