#ifndef GEOM_UTILS_H
#define GEOM_UTILS_H

#include <opencv2/core.hpp>

namespace utils
{

namespace cv
{

  double angleBetween(const ::cv::Point2f &v1, const ::cv::Point2f &v2);

}

}

#endif // GEOM_UTILS_H
