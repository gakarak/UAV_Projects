#include "local_restorer_by_frame.h"

using namespace algorithmspkg;

LocalRestorerByFrame::LocalRestorerByFrame(DetectorPtr detector,
                                           DescriptorPtr descriptor,
                                           MatcherPtr matcher,
                                           size_t max_key_points_per_frame)
  : RestorerByFrame(detector, descriptor, matcher,
                         max_key_points_per_frame)
{
  free_place_x = 0;
}

void LocalRestorerByFrame::addFrame(const cv::Mat &frame,
                                    const cv::Point2f &pos,
                                    double angle, double scale)
{
  addFrame(frame);
}

void LocalRestorerByFrame::addFrame(const cv::Mat &frame)
{
  cv::Point2f pos(free_place_x + frame.rows / 2.,
                     frame.cols / 2.);

  RestorerByFrame::addFrame(frame, cv::Point2f(0, 0), 0, 1);

  free_place_x += frame.rows;
}

void LocalRestorerByFrame::addFrame(const cv::Point2f &image_center,
                                    const KeyPointsList &key_points,
                                    const cv::Mat &descriptions,
                                    const cv::Point2f &pos,
                                    double angle, double scale)
{
  addFrame(image_center, key_points, descriptions);
}

void LocalRestorerByFrame::addFrame(const cv::Point2f &image_center,
                                    const KeyPointsList &key_points,
                                    const cv::Mat &descriptions)
{
  cv::Point2f pos(free_place_x + image_center.x,
                     image_center.y);
  RestorerByFrame::addFrame(image_center, key_points, descriptions,
                            cv::Point2f(0, 0), 0, 1);

  free_place_x += image_center.x*2;
}
