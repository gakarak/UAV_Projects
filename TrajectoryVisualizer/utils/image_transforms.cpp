#include "image_transforms.h"

#include <opencv2/imgproc.hpp>

#include "algorithms/transformator.h"

using namespace cv;
using namespace algorithmspkg;

Mat cv::scaleRotateCropImage(Mat image, double scale, double angle){
  const double sqrt_2 = std::sqrt(2);

  Point2i center(image.cols/2, image.rows/2);
  Point2i anticenter(-center.x, -center.y); //center to zero shift

  int min_side = std::min(image.cols, image.rows);
  int crop_shift_size = scale * min_side/(2*sqrt_2);
  Point2i crop_shift(crop_shift_size, crop_shift_size);
  Size2i new_size = crop_shift*2;


  Mat transform = Transformator::getTransformation({
                                    Transformator::getTranslate(anticenter),
                                    Transformator::getRotate(angle),
                                    Transformator::getScale(scale),
                                    Transformator::getTranslate(crop_shift)
                                 });

  Mat result;
  warpPerspective(image, result, transform, new_size);

  return result;
}

Mat cv::getSquareImage(Mat image){
  Point2i center(image.cols/2, image.rows/2);

  int side_length = std::min(image.rows, image.cols);
  Point2i left_top = center - Point2i(side_length / 2, side_length / 2);
  Size2i rect_size(side_length, side_length);

  return image(Rect2i(left_top, rect_size));
}
