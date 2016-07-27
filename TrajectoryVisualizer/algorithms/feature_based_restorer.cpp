#include "feature_based_restorer.h"
#include "transformator.h"

using namespace algorithmspkg;

FeatureBasedRestorer::FeatureBasedRestorer(DetectorPtr detector,
                                           DescriptorPtr descriptor)
  : detector(detector), descriptor(descriptor)
{
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

  return recoverLocation(pos, angle, scale);
}

const FeatureBasedRestorer::MatchesList&
            FeatureBasedRestorer::getLastMatches() const
{
  return matches;
}


FeatureBasedRestorer::DetectorPtr FeatureBasedRestorer::getDetector() const
{
  return detector;
}

FeatureBasedRestorer::DescriptorPtr FeatureBasedRestorer::getDescriptor() const
{
  return descriptor;
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


  for (int i = 0; i < to_transform.size(); i++)
  {
    key_points[i].pt = to_transform[i];
  }
}
