#include "geom_utils.h"

double utils::cv::angleBetween(const ::cv::Point2f &from,
                               const ::cv::Point2f &to)
{
  double dot = (from.x * to.x + from.y * to.y);
  double det = (from.x * to.y - from.y * to.x);

  return atan2(det, dot) / M_PI * 180;
}
