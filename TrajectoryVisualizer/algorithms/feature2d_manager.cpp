#include "feature2d_manager.h"

#include <algorithm>
#include <iostream>

Feature2DManager::Feature2DManager()
{
  initDetectors();
  initDescriptors();
}

void Feature2DManager::initDetectors()
{
  addDetector(cv::xfeatures2d::SIFT::create(), "SIFT");
  addDetector(cv::xfeatures2d::SURF::create(), "SURF");
  addDetector(cv::KAZE::create(), "KAZE");
  addDetector(cv::AKAZE::create(), "AKAZE");
  addDetector(cv::ORB::create(), "ORB");
  addDetector(cv::BRISK::create(), "BRISK");
  addDetector(cv::MSER::create(), "MSER");
  addDetector(cv::SimpleBlobDetector::create(), "SimpleBlobDetector");
  addDetector(cv::xfeatures2d::StarDetector::create(), "StarDetector");
  addDetector(cv::xfeatures2d::MSDDetector::create(), "MSDDetector");
}

void Feature2DManager::initDescriptors()
{
  addDescriptor(cv::xfeatures2d::SIFT::create(), "SIFT");
  addDescriptor(cv::xfeatures2d::SURF::create(), "SURF");
  addDescriptor(cv::KAZE::create(), "KAZE");
  addDescriptor(cv::AKAZE::create(), "AKAZE");
  addDescriptor(cv::ORB::create(), "ORB");
  addDescriptor(cv::BRISK::create(), "BRISK");
  addDescriptor(cv::xfeatures2d::FREAK::create(), "FREAK");
  addDescriptor(cv::xfeatures2d::BriefDescriptorExtractor::create(),
                "BriefDescriptor");
  addDescriptor(cv::xfeatures2d::LUCID::create(11, 3), "LUCID_23x23_5x5");
  addDescriptor(cv::xfeatures2d::LATCH::create(), "LATCH");
  addDescriptor(cv::xfeatures2d::DAISY::create(), "DAISY");
}

void Feature2DManager::addDetector(DetectorPtr detector, std::string name)
{
  bool isExist = std::find(detectors_names.begin(), detectors_names.end(),
                           name) != detectors_names.end();
  if (!isExist)
  {
    detectors.push_back(detector);
    detectors_names.push_back(name);
  }
  else
  {
    std::clog << "Feature2DManager: this name already exists" << std::endl;
  }
}

void Feature2DManager::addDescriptor(DescriptorPtr descriptor, std::string name)
{
  bool isExist = std::find(descriptors_names.begin(), descriptors_names.end(),
                           name) != descriptors_names.end();
  if (!isExist)
  {
    descriptors.push_back(descriptor);
    descriptors_names.push_back(name);
  }
  else
  {
    std::clog << "Feature2DManager: this name already exists" << std::endl;
  }
}

const cv::Ptr<cv::Feature2D> Feature2DManager::getDetector(std::string name)
                                                                          const
{
  DetectorPtr result;

  auto elem = std::find(detectors_names.begin(), detectors_names.end(),
                        name);

  bool isExist = elem != detectors_names.end();
  if (isExist)
  {
    size_t i = elem - detectors_names.begin();
    result = detectors[i];
  }

  return result;
}

const cv::Ptr<cv::Feature2D> Feature2DManager::getDescriptor(std::string name)
                                                                          const
{
  DescriptorPtr result;

  auto elem = std::find(descriptors_names.begin(), descriptors_names.end(),
                        name);

  bool isExist = elem != descriptors_names.end();
  if (isExist)
  {
    size_t i = elem - descriptors_names.begin();
    result = descriptors[i];
  }

  return result;
}


const std::vector<std::string> &Feature2DManager::getDetectorsNames() const
{
  return detectors_names;
}

const std::vector<std::string> &Feature2DManager::getDescriptorsNames() const
{
  return descriptors_names;
}
