#ifndef TRANSFORMATOR_H
#define TRANSFORMATOR_H

#include <vector>

#include <opencv2/core.hpp>

namespace algorithms{

class Transformator
{
 public:
  Transformator();

  static std::vector<cv::Point2f> transform(const std::vector<cv::Point2f> &pts,
                                            const cv::Point2f &center_shift,
                                            double angle, double scale,
                                            const cv::Point2f &scaled_shift);
  static std::vector<cv::Point2f> transform(const std::vector<cv::Point2f> &pts,
                                            const cv::Mat &transformation);



  static cv::Mat getTranslate(cv::Point2f shift);
  static cv::Mat getRotate(double angle /*degrees*/);
  static cv::Mat getScale(double scale);
  static cv::Mat getScale(cv::Point2f scale);
};

}

#endif // TRANSFORMATOR_H
