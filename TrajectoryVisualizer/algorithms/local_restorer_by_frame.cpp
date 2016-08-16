#include "local_restorer_by_frame.h"

using namespace algorithmspkg;

LocalRestorerByFrame::LocalRestorerByFrame(DetectorPtr detector,
                                           DescriptorPtr descriptor,
                                           MatcherPtr matcher,
                                           size_t max_key_points_per_frame)
  : RestorerByFrame(detector, descriptor, matcher,
                         max_key_points_per_frame)
{
}

void LocalRestorerByFrame::addFrame(const cv::Mat &frame,
                                    const cv::Point2f &pos,
                                    double angle, double scale)
{
  RestorerByFrame::addFrame(frame, pos, angle, 1);
}

void LocalRestorerByFrame::addFrame(const cv::Point2f &image_center,
                                    const KeyPointsList &key_points,
                                    const cv::Mat &descriptions,
                                    const cv::Point2f &pos,
                                    double angle, double scale)
{
  RestorerByFrame::addFrame(image_center, key_points, descriptions,
                            pos, angle, 1);
}
