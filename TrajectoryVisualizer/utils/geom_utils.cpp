#include "geom_utils.h"

double utils::cv::angleBetween(const ::cv::Point2f &v1, const ::cv::Point2f &v2)
{
  double dot = (v1.x * v2.x + v1.y * v2.y);
  double det = (v1.x * v2.y - v1.y * v2.x);

  return atan2(det, dot) / M_PI * 180;
}
