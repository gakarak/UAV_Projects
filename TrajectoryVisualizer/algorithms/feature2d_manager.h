#ifndef FEATURE2D_MANAGER_H
#define FEATURE2D_MANAGER_H

#include <vector>
#include <opencv2/xfeatures2d.hpp>

class Feature2DManager
{
 public:
  using DetectorPtr = cv::Ptr<cv::Feature2D>;
  using DescriptorPtr = cv::Ptr<cv::Feature2D>;

  Feature2DManager();

  void addDetector(DetectorPtr detector, std::string name);
  void addDescriptor(DescriptorPtr descriptor, std::string name);

  const DetectorPtr getDetector(std::string name) const;
  const DescriptorPtr getDescriptor(std::string name) const;

  const std::vector<std::string>& getDetectorsNames() const;
  const std::vector<std::string>& getDescriptorsNames() const;

 private:
  void initDetectors();
  void initDescriptors();

  std::vector<DetectorPtr> detectors;
  std::vector<std::string> detectors_names;

  std::vector<DescriptorPtr> descriptors;
  std::vector<std::string> descriptors_names;
};

#endif // FEATURE2D_MANAGER_H
