#include "trajectory_recover.h"

#include "transformator.h"

#include <chrono>
#include <iostream>

#include <opencv2/calib3d.hpp>

using namespace algorithmspkg;
using namespace std;
using namespace cv;
using namespace chrono;

TrajectoryRecover::TrajectoryRecover(cv::Ptr<cv::Feature2D> detector,
                                     cv::Ptr<cv::Feature2D> descriptor)
    : detector(detector), descriptor(descriptor)
{
}

void TrajectoryRecover::addFrame(const Mat &frame,
                                 const Point2f &frame_pos_m,
                                 double angle, double meters_per_pixel)
{
  matcher_trained = false;

  vector<KeyPoint> kps;
  detector->detect(frame, kps);

  cv::Mat descrs;
  descriptor->compute(frame, kps, descrs);

  this->addFrame(frame, kps, descrs, frame_pos_m, angle, meters_per_pixel);
}

void TrajectoryRecover::addFrame(const Mat &frame,
                                 const vector<KeyPoint> &key_points,
                                 Mat   descriptors,
                                 const Point2f &frame_pos_m,
                                 double angle, double meters_per_pixel)
{
  if (key_points.empty())
  {
    return;
  }
  matcher_trained = false;

  vector<Point2f> to_transform;
  vector<Point2f> transformed;
  for (const auto &kp : key_points)
  {
    to_transform.push_back(kp.pt);
  }

  //from local to trajectory coords
  Point2f center_shift = Point2f(frame.cols/2., frame.rows/2.);
  transformed = Transformator::transform(to_transform, {//transformations
                                  Transformator::getTranslate(-center_shift),
                                  Transformator::getRotate(angle),
                                  Transformator::getScale(meters_per_pixel),
                                  Transformator::getTranslate(frame_pos_m)
                               });
  /*transformed = to_transform;
  transformed = Transformator::transform(transformed, Transformator::getTranslate(-center_shift));
  transformed = Transformator::transform(transformed, Transformator::getRotate(angle));
  transformed = Transformator::transform(transformed, Transformator::getScale(meters_per_pixel));
  transformed = Transformator::transform(transformed, Transformator::getTranslate(frame_pos_m));*/

  for (size_t i = 0; i < transformed.size(); i++)
  {
    KeyPoint kp = key_points[i];
    kp.pt = transformed[i];
    kp.angle += angle;  //add frame angle
    kp.size *= meters_per_pixel;
    key_points_cloud.push_back(kp);
  }

  if (descriptors_cloud.empty())
  {
    descriptors_cloud = descriptors;
  }
  else
  {
    descriptors_cloud.push_back(descriptors);
  }
  //matcher.add(descriptors);
}

void TrajectoryRecover::recoverTrajectory(const Mat &que_frame,
                                          vector<KeyPoint> &key_points,
                                          Mat &descriptors, Mat &homography,
                                          vector<DMatch> &matches)
{
  //there is unnecessary to train matcher, because recoverTrajectory
  //overload below will be called

  key_points.clear();
  detector->detect(que_frame, key_points);
  descriptor->compute(que_frame, key_points, descriptors);

  this->recoverTrajectory(key_points, descriptors, homography, matches);
}

void TrajectoryRecover::recoverTrajectory(const vector<KeyPoint> &que_key_points,
                                          const Mat &que_descriptors,
                                          Mat &homography,
                                          vector<DMatch> &matches)
{
  if (que_key_points.empty())
  {
    homography = Mat();
    matches.clear();
    return;
  }
  if (!matcher_trained)
  {
    matcher.clear();
    if(descriptors_cloud.type() != CV_32F)
    {
      descriptors_cloud.convertTo(descriptors_cloud, CV_32F);
    }
    matcher.add(descriptors_cloud);
    matcher.train();
    matcher_trained = true;
  }

  Mat descriptions32f = que_descriptors;
  if (descriptions32f.type() != CV_32F)
  {
    descriptions32f.convertTo(descriptions32f, CV_32F);
  }

  vector<cv::DMatch> rough_matches;
  auto start_match_time = high_resolution_clock::now();

  matcher.match(descriptions32f,
                rough_matches);

  auto finish_match_time = high_resolution_clock::now();
  clog << "Match time: " <<
          duration_cast<milliseconds>(finish_match_time -
                                      start_match_time).count() << "ms\n";

  //prepare points for findHomography | using trajectories_kp_cloud
  //we need to transform to_trj_pts to points on map
  vector<Point2f> que_trj_pts;
  vector<Point2f> train_trj_pts;

  for (const DMatch &match: rough_matches)
  {
    train_trj_pts.push_back(key_points_cloud[match.trainIdx].pt);
    que_trj_pts.push_back(que_key_points[match.queryIdx].pt);
  }

  auto start_homo_time = high_resolution_clock::now();

  vector<char> mask;
  homography = findHomography(que_trj_pts, train_trj_pts, RANSAC, 3, mask);

  auto finish_homo_time = chrono::high_resolution_clock::now();
  clog << "Finding homography time: " <<
          duration_cast<milliseconds>(finish_homo_time -
                                      start_homo_time).count() << "ms\n";

  matches.clear();
  for (size_t i = 0; i < mask.size(); i++)
  {
    if (mask[i])
    {
      matches.push_back(rough_matches[i]);
    }
  }

}

void TrajectoryRecover::clear()
{
  matcher_trained = false;
  matcher.clear();
  key_points_cloud.clear();
  descriptors_cloud = cv::Mat();
}

void TrajectoryRecover::setDetector(cv::Ptr<cv::Feature2D> detector)
{
  this->detector = detector;
}

void TrajectoryRecover::setDescriptor(cv::Ptr<cv::Feature2D> descriptor)
{
  this->descriptor = descriptor;
}

const std::vector<cv::KeyPoint> &TrajectoryRecover::getKeyPointsCloud() const
{
  return key_points_cloud;
}

const cv::Mat &TrajectoryRecover::getDescriptorsCloud() const
{
  return descriptors_cloud;
}




