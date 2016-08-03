#include "geom_utils.h"

double utils::cv::angleBetween(const ::cv::Point2f &from,
                               const ::cv::Point2f &to)
{
  double dot = (from.x * to.x + from.y * to.y);
  double det = (from.x * to.y - from.y * to.x);

  return atan2(det, dot) / M_PI * 180;
}

::cv::Point2f utils::cv::elementMul(const ::cv::Point2f &a,
                                    const ::cv::Point2f &b)
{
  return ::cv::Point2f(a.x*b.x, a.y*b.y);
}
