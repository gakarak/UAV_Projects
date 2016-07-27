#ifndef ILOCATION_RESTORER_H
#define ILOCATION_RESTORER_H

#include <vector>
#include <string>

#include <opencv2/core.hpp>

namespace algorithmspkg
{

class ILocationRestorer
{
 public:
  ILocationRestorer() {}
  virtual ~ILocationRestorer() {}

  /**
   * @brief addFrame - adds bounded frame to map a.k.a. trajectory
   * @param frame - image
   * @param pos - position of the frame center in preffered units
   * @param angle - counterclockwise in degrees [-180; 180], 0 is east
   * @param scale - scale from pixels to preffered units
   */
  virtual void addFrame(const cv::Mat &frame,
                        const cv::Point2f &pos, double angle, double scale) = 0;

  /**
   * @brief recoverLocation - return bounded location
   * @param query_frame - image for definition of location
   * @param pos - out position in preffered units
   * @param angle - out counterclockwise in degrees [-180; 180], 0 is east
   * @param scale - scale from pixels to preffered units
   * @return confidence (the strength of the result) [0..1]
   */
  virtual double recoverLocation(const cv::Mat &query_frame,
                                 cv::Point2f &pos,
                                 double &angle,
                                 double &scale) = 0;
  //should be same as above
  //virtual Location recoverLocation(const cv::Mat &que_frame) = 0;
};

/*
 * @brief The Location struct needed for returning pos, angle and scale.
 * angle usually must be in degrees, but it can be different according to
 * the algorithm
 *
struct Location
{
  cv::Point2f pos;
  double angle;
  double scale;
};*/

}

#endif // ILOCATION_RESTORER_H
