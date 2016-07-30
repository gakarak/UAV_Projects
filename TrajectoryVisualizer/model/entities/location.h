#ifndef LOCATION_H
#define LOCATION_H

#include <opencv2/core.hpp>

namespace modelpkg
{

struct Location
{

  Location(cv::Point2f pos = cv::Point2f(0, 0),
           double angle = 0, double scale = 1)
    : pos(pos), angle(angle), scale(scale)
  { }

  cv::Point2f pos;
  double angle;
  double scale; //from pixels to preffered units
};

}

#endif // LOCATION_H
