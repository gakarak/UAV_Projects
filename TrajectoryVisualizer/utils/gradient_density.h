#ifndef GRADIENT_DENSITY_H
#define GRADIENT_DENSITY_H

#include <QPointF>

#include <opencv2/core.hpp>

namespace utils
{

namespace cv
{

  double gradientDensity(::cv::Mat image);

  QPointF toQPointF(::cv::Point2f pt);
  ::cv::Point2f toPoint2f(QPointF pt);

}

}

#endif // GRADIENT_DENSITY_H
