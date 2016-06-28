#ifndef IMAGE_INFO_ESTIMATOR_H
#define IMAGE_INFO_ESTIMATOR_H

#include <opencv2/core.hpp>

namespace algorithmspkg
{

template<typename InfoType>
class ImageInfoEstimator
{
 public:
  ImageInfoEstimator();
  virtual ~ImageInfoEstimator();

  virtual InfoType estimate(const cv::Mat &image) = 0;
  virtual InfoType getMaxEstimationValue() = 0;

};

template<typename InfoType>
ImageInfoEstimator<InfoType>::ImageInfoEstimator()
{
}

template<typename InfoType>
ImageInfoEstimator<InfoType>::~ImageInfoEstimator()
{
}


}

#endif // IMAGE_INFO_ESTIMATOR_H
