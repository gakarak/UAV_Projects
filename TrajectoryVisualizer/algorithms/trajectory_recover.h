#ifndef TRAJECTORY_RECOVER_H
#define TRAJECTORY_RECOVER_H

#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/xfeatures2d.hpp>

namespace algorithmspkg{

class TrajectoryRecover
{
 public:

  TrajectoryRecover(
      cv::Ptr<cv::Feature2D> detector = cv::xfeatures2d::SIFT::create(),
      cv::Ptr<cv::Feature2D> descriptor = cv::xfeatures2d::SIFT::create());

  void addFrame(const cv::Mat &frame, const cv::Point2f &frame_pos_m,
                double angle, double meters_per_pixel);

  void addFrame(const cv::Mat                   &frame,
                const std::vector<cv::KeyPoint> &key_points,
                const cv::Mat                   &descriptors,
                const cv::Point2f               &frame_pos_m,
                double angle, double meters_per_pixel);

  //void addBoundedKeyPoints(const std::vector<cv::KeyPoint> &key_points);

  void recoverTrajectory(const cv::Mat &que_frame,
                         std::vector<cv::KeyPoint> &key_points,
                         cv::Mat &descriptors, cv::Mat &homography,
                         std::vector<cv::DMatch> &matches);
  void recoverTrajectory(const std::vector<cv::KeyPoint> &que_key_points,
                         const cv::Mat &que_descriptors,
                         cv::Mat &homography,
                         std::vector<cv::DMatch> &matches);

  void setDetector(cv::Ptr<cv::Feature2D> detector);
  void setDescriptor(cv::Ptr<cv::Feature2D> descriptor);
  void setQualityValidator();

  const std::vector<cv::KeyPoint>& getKeyPointsCloud() const;
  const cv::Mat&                   getDescriptorsCloud() const;

 private:

  cv::Ptr<cv::Feature2D> detector;
  cv::Ptr<cv::Feature2D> descriptor;

  cv::FlannBasedMatcher matcher;
  bool                  matcher_trained;

  //each point in meters
  std::vector<cv::KeyPoint>   key_points_cloud;
  cv::Mat                     descriptors_cloud;
};

}

#endif // TRAJECTORY_RECOVER_H
