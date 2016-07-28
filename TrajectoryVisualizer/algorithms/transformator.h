#ifndef TRANSFORMATOR_H
#define TRANSFORMATOR_H

#include <vector>
#include <initializer_list>

#include <opencv2/core.hpp>

namespace algorithmspkg{

class Transformator
{
 public:
  Transformator();

/*static std::vector<cv::Point2f> transform(const std::vector<cv::Point2f> &pts,
                                            const cv::Point2f &center_shift,
                                            double angle, double scale,
                                            const cv::Point2f &scaled_shift);*/
  static cv::Point2f transform(const cv::Point2f &pt,
                               std::initializer_list<cv::Mat> transformations);
  static cv::Point2f transform(const cv::Point2f &pt,
                               const cv::Mat &transformation);
  static std::vector<cv::Point2f> transform(const std::vector<cv::Point2f> &pts,
                                std::initializer_list<cv::Mat> transformations);
  static std::vector<cv::Point2f> transform(const std::vector<cv::Point2f> &pts,
                                            const cv::Mat &transformation);

  static void getParams(const cv::Mat &homography,
                        cv::Point2f &shift, double &angle, double &scale);

  static cv::Mat getTranslate(cv::Point2f shift);

  /**
   * @brief getRotate rotation from x to y (normally counter-clockwise)
   *                                       (if y reversed then clockwise)
   * @param angle - degrees
   * @return matrix 3x3
   */
  static cv::Mat getRotate(double angle /*degrees*/);
  static cv::Mat getScale(double scale);
  static cv::Mat getScale(cv::Point2f scale);
};

}

#endif // TRANSFORMATOR_H
