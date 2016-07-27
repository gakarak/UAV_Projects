#include "transformator.h"

#include <opencv2/calib3d.hpp>

#include "utils/geom_utils.h"

using namespace algorithmspkg;
using namespace std;
using namespace cv;

Transformator::Transformator()
{

}

/**
 * @brief Transformator::transform
 * translate -center_shift, then rotate, then scale, then shift in scaled units
 * @param pts
 * @param center_shift
 * @param angle
 * @param scale
 * @param scaled_shift
 * @return
 */
/*
vector<Point2f> Transformator::transform(const vector<Point2f> &pts,
                                                const Point2f &center_shift,
                                                double angle, double scale,
                                                const Point2f &scaled_shift)
{
    Mat transformation = getTranslate(scaled_shift).mul(getScale(scale).mul(
                            getRotate(angle).mul(getTranslate(-center_shift))));

    return Transformator::transform(pts, transformation);
}*/

Point2f Transformator::transform(const cv::Point2f &pt,
                                 initializer_list<Mat> transformations)
{
  std::vector<cv::Point2f> wrap(1, pt);
  return Transformator::transform(wrap, transformations).front();
}

Point2f Transformator::transform(const Point2f &pt, const Mat &transformation)
{
  std::vector<cv::Point2f> wrap(1, pt);
  return Transformator::transform(wrap, transformation).front();
}

/**
 * @brief Transformator::transform
 * @param pts
 * @param transformations list of successive transformations
 * @return
 */
vector<Point2f> Transformator::transform(const vector<Point2f> &pts,
                                         initializer_list<Mat> transformations)
{
  Mat final_transform = *transformations.begin();
  for (auto it = transformations.begin()+1; it != transformations.end(); it++)
  {
    final_transform = (*it)*final_transform;
  }

  return Transformator::transform(pts, final_transform);
}

std::vector<Point2f> Transformator::transform(const std::vector<Point2f> &pts,
                                              const Mat &transformation)
{
  vector<Point2f> result;

  perspectiveTransform(pts, result, transformation);

  return result;
}

void Transformator::getParams(const Mat &homography,
                              cv::Point2f &shift, double &angle, double &scale)
{
  if (homography.rows == 3 &&
      homography.cols == 3)
  {
    Point2f pt1(0, 0);
    Point2f pt2(0, 1);

    Point2f t_pt1 = transform(pt1, homography);
    Point2f t_pt2 = transform(pt2, homography);

    shift = t_pt1 - pt1;
    angle = utils::cv::angleBetween(pt2 - pt1, t_pt2 - t_pt1);
    scale = cv::norm(t_pt2 - t_pt1) / cv::norm(pt2 - pt1);
  }
  else
  {
    shift = cv::Point2f(0, 0);
    angle = scale = 0;
  }
}

Mat Transformator::getTranslate(Point2f shift)
{
  return Mat_<double>(3, 3) << 1, 0, shift.x,
      0, 1, shift.y,
      0, 0,    1   ;
}

Mat Transformator::getRotate(double angle)
{
  double radians = angle/180*M_PI;
  return Mat_<double>(3, 3) << cos(radians), sin(radians), 0,
      -sin(radians), cos(radians), 0,
      0,            0,       1;
}

Mat Transformator::getScale(double scale)
{
  return Mat_<double>(3, 3) << scale, 0, 0,
      0, scale, 0,
      0,     0, 1;
}

Mat Transformator::getScale(Point2f scale)
{
  return Mat_<double>(3, 3) << scale.x, 0, 0,
      0, scale.y, 0,
      0,       0, 1;
}
