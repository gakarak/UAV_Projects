#ifndef IMAGE_TRANSFORMS_H
#define IMAGE_TRANSFORMS_H

#include <opencv2/core.hpp>


namespace cv{

/**
   * @brief ScaleRotateCropImage crops image without black corners (square img)
   * @param image - image
   * @param scale - for example 1.1
   * @param angle - in degree
   * @return scaled, rotated  square image (NOT max rectangle)
   */
  Mat scaleRotateCropImage(Mat image, double scale, double angle);

  /**
   * @brief getSquareImage
   * @param image
   * @return cropped max-square from center
   */
  Mat getSquareImage(Mat image);

}

#endif // IMAGE_TRANSFORMS_H
