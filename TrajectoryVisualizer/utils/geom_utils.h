#ifndef GEOM_UTILS_H
#define GEOM_UTILS_H

#include <opencv2/core.hpp>

namespace utils
{

namespace cv
{

  /**
   * @brief angleBetween vector from and to
   *        positive is direction from x to y (normally counter-clockwise)
   *                                          (if y reversed then clockwise)
   * @param from
   * @param to
   * @return
   */
  double angleBetween(const ::cv::Point2f &from, const ::cv::Point2f &to);

}

}

#endif // GEOM_UTILS_H
