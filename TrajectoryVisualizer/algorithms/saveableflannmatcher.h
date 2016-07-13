#ifndef SAVEABLEFLANNMATCHER_H
#define SAVEABLEFLANNMATCHER_H

#include <string>

#include <opencv2/xfeatures2d.hpp>

/**
 * @brief The SaveableFlannMatcher class
 * This class acesses to protected fields of flannBasedMatcher
 * indexParams and flannIndex
 */
class SaveableFlannMatcher : public cv::FlannBasedMatcher
{
 public:
  SaveableFlannMatcher(
      const cv::Ptr<cv::flann::IndexParams>& indexParams =
                                    cv::makePtr<cv::flann::KDTreeIndexParams>(),
      const cv::Ptr<cv::flann::SearchParams>& searchParams =
                                    cv::makePtr<cv::flann::SearchParams>())
    : cv::FlannBasedMatcher(indexParams, searchParams)
  {
  }

  virtual ~SaveableFlannMatcher()
  {
  }

  void readIndex(std::string filename)
  {
    indexParams->setAlgorithm(cvflann::FLANN_INDEX_SAVED);
    indexParams->setString("filename", filename);

    // construct flannIndex now, so printParams works
    train();
  }

  void writeIndex(std::string filename)
  {
    flannIndex->save(filename);
  }
};

#endif // SAVEABLEFLANNMATCHER_H
