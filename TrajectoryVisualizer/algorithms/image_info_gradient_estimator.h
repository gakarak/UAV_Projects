#ifndef IMAGE_INFO_GRADIENT_ESTIMATOR_H
#define IMAGE_INFO_GRADIENT_ESTIMATOR_H

#include "image_info_estimator.h"

#include "utils/gradient_density.h"

namespace algorithmspkg
{

class ImageInfoGradientEstimator: public ImageInfoEstimator<double>
{
 public:
  ImageInfoGradientEstimator(double scale = 1);

  double estimate(const cv::Mat &image) override;
  double getMaxEstimationValue() override;

  void setScale(double scale) { this->scale = scale;  }

 private:
  double gradientDensity(const cv::Mat &image);

  double scale;

  double max_value;
};

/**
 * @brief ImageInfoGradientEstimator::ImageInfoGradientEstimator
 * calculates gradient density preliminarily resizes image on scale
 *
 * @param scale
 */
ImageInfoGradientEstimator::ImageInfoGradientEstimator(double scale,
                                                       double threshold)
    : ImageInfoEstimator<double>(), scale(scale), threshold(threshold)
{
  max_value = (4*255 + /*max gradient on X with Sobel operator*/
               4*255 /*max gradient on Y with Sobel operator*/
               ) / 2.0; /*such function in gradientDensity*/
}

double ImageInfoGradientEstimator::estimate(const cv::Mat &image)
{
  cv::Size new_size(image.size().width * scale, image.size().height * scale);

  cv::Mat resized;
  cv::resize(image, resized, new_size);

  double estimation = gradientDensity(resized);

  return estimation;
}

double ImageInfoGradientEstimator::getMaxEstimationValue()
{
  return max_value;
}

double ImageInfoGradientEstimator::gradientDensity(const cv::Mat &image)
{
  {
    cv::Point2f center(image.cols / 2, image.rows / 2);
    int radius = std::min(image.cols, image.rows) / 2;

    cv::Mat grad_x;
    cv::Mat grad_y;

    cv::Sobel(image, grad_x, CV_16S, 1, 0);
    cv::Sobel(image, grad_y, CV_16S, 0, 1);

    cv::Mat abs_grad_x;
    cv::Mat abs_grad_y;

    cv::convertScaleAbs(grad_x, abs_grad_x);
    cv::convertScaleAbs(grad_y, abs_grad_y);

    cv::Mat grad_value;
    cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad_value);

    double grad_density = 0;
    int pixel_count = 0;

    for (int i = 0; i < grad_value.rows; i++)
    {
      for (int j = 0; j < grad_value.cols; j++)
      {
        if (sqrt(pow(i - center.y, 2) + pow(j - center.x, 2)) < radius)
        {
          grad_density += grad_value.at<uchar>(i, j);
          pixel_count++;
        }
      }
    }

    return grad_density / pixel_count;
  }
}

}

#endif // IMAGE_INFO_GRADIENT_ESTIMATOR_H
