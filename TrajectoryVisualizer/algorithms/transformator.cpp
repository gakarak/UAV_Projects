#include "transformator.h"

#include <opencv2/calib3d.hpp>

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
      final_transform = it->mul(final_transform);
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
