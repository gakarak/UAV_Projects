#include "restorer_by_frame.h"

#include <algorithm>
#include <iostream>
#include <fstream>

#include <QPolygonF>

#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

#include "algorithms/transformator.h"
#include "utils/gradient_density.h"

using namespace algorithmspkg;
using namespace std;

RestorerByFrame::RestorerByFrame(DetectorPtr detector,
                                 DescriptorPtr descriptor,
                                 MatcherPtr matcher,
                                 size_t max_key_points_per_frame)
  : FeatureBasedRestorer(detector, descriptor, matcher,
                         max_key_points_per_frame)
{
}

void RestorerByFrame::addFrame(const cv::Mat &frame,
                               const cv::Point2f &pos,
                               double angle, double scale)
{
  frames_key_points.push_back(KeyPointsList());
  std::vector<cv::KeyPoint> &key_points = frames_key_points.back();

  getDetector()->detect(frame, frames_key_points.back());

  std::sort(key_points.begin(), key_points.end(),
            []( const cv::KeyPoint &left, const cv::KeyPoint &right ) ->
            bool { return left.response > right.response; });

  if (getMaxKeyPointsPerFrame() != 0 &&
      getMaxKeyPointsPerFrame() < key_points.size())
  {
    key_points.resize(getMaxKeyPointsPerFrame());
  }
  key_points.shrink_to_fit();

  cv::Mat descriptions;
  getDescriptor()->compute(frame, frames_key_points.back(),
                     descriptions);

  matchers.push_back(getMatcher()->clone(true));
  matchers.back()->add(descriptions);

  cv::Point2f image_center(frame.cols/2., frame.rows/2.);
  transformKeyPointsPosition(key_points, image_center,
                             pos, angle, scale);

  frames_polygons.push_back(calculateFramePolygon(image_center, pos, angle,
                                                  scale));
  frames_area.push_back(cv::contourArea(frames_polygons.back()));
}

void RestorerByFrame::addFrame(const cv::Point2f &image_center,
                               const KeyPointsList &key_points,
                               const cv::Mat &descriptions,
                               const cv::Point2f &pos,
                               double angle, double scale)
{
  frames_key_points.push_back(key_points);
  matchers.push_back(getMatcher()->clone(true));
  matchers.back()->add(descriptions);

  transformKeyPointsPosition(frames_key_points.back(), image_center,
                             pos, angle, scale);

  frames_polygons.push_back(calculateFramePolygon(image_center, pos, angle,
                                                  scale));
  frames_area.push_back(cv::contourArea(frames_polygons.back()));
}

double RestorerByFrame::recoverLocation(const cv::Rect2f &que_frame_rect,
                                        cv::Point2f &pos,
                                        double &angle, double &scale)
{
  //points for findHomography
  static std::vector<cv::Point2f> query_pts;
  static std::vector<cv::Point2f> train_pts;
  auto best_homography = cv::Mat();

  pos = cv::Point2f(0, 0);
  angle = scale = 0;
  matches.clear();

  if (query_key_points.empty())
  {
    return 0;
  }

  double max_confidence = 0;

  std::ofstream maskConfidenceOut("_maskConfidence.csv", ios_base::app);
  std::ofstream areaConfidenceOut("_areaConfidence.csv", ios_base::app);
  std::ofstream scalesOut("_scales.csv", ios_base::app);

  for (size_t frame_num = 0; frame_num < matchers.size(); frame_num++)
  {
    rough_matches.clear();
    matchers[frame_num]->match(query_descriptions, rough_matches);


    train_pts.clear();
    query_pts.clear();
    for (const cv::DMatch &match: rough_matches)
    {
      train_pts.push_back(frames_key_points[frame_num][match.trainIdx].pt);
      query_pts.push_back(query_key_points[match.queryIdx].pt);
    }

    homography_mask.clear();
    homography = cv::findHomography(query_pts, train_pts,
                                           cv::RANSAC, 3, homography_mask);

    cv::Point2f shift;
    double angle_temp;
    double scale_temp;
    Transformator::getParams(homography, shift, angle_temp, scale_temp);

    double maskConfidence = calculateMaskConfidence();
    double areaConfidence = calculateAreaConfidence(que_frame_rect, frame_num);

    maskConfidenceOut << maskConfidence;
    areaConfidenceOut << areaConfidence;
    scalesOut << scale_temp;
    if (frame_num + 1 != matchers.size())
    {
      maskConfidenceOut << ", ";
      areaConfidenceOut << ", ";
      scalesOut << ", ";
    }

    if (areaConfidence > max_confidence)
    {
      cv::Point2f shift;
      Transformator::getParams(homography, shift, angle, scale);
      cv::Point2f que_center = (que_frame_rect.tl() + que_frame_rect.br()) / 2.;
      pos =  Transformator::transform(que_center, homography);

      homography.copyTo(best_homography);

      matches.clear();
      for (size_t i = 0; i < homography_mask.size(); i++)
      {
        if (homography_mask[i])
        {
          matches.push_back(rough_matches[i]);
          matches.back().imgIdx = frame_num;
        }
      }

      max_confidence = areaConfidence;
    }
  }

  maskConfidenceOut << endl;
  areaConfidenceOut << endl;
  scalesOut << endl;

  best_homography.copyTo(homography);

  return max_confidence;
}

double RestorerByFrame::calculateMaskConfidence() const noexcept
{
  // confidence based on homograhy_mask
  if (!homography_mask.empty())
  {
    size_t count = std::count_if(homography_mask.begin(), homography_mask.end(),
                        [] (char x) -> bool { return static_cast<bool>(x); });
    return count / double(homography_mask.size());
  }
  else
  {
    return 0;
  }
}

double RestorerByFrame::calculateAreaConfidence(
    const cv::Rect2f &query_frame_rect, int base_frame_num) const
{
  cv::Point2f shift(0, 0);
  double angle = 0;
  double scale = 0;
  Transformator::getParams(homography, shift, angle, scale);

  if (!homography.empty() &&
      scale > 0.2 && scale < 2)
  {
    FramePolygon query_frame_polygon = calculateFramePolygon(query_frame_rect,
                                                             homography);
    const auto &base_frame_polygon = frames_polygons[base_frame_num];

    //calculate intersection with QPointF
    QPolygonF query;
    for (const auto &pt: query_frame_polygon)
    {
      query << utils::cv::toQPointF(pt);
    }
    //query << utils::cv::toQPointF(query_frame_polygon[0]);

    QPolygonF base;
    for (const auto &pt: base_frame_polygon)
    {
      base << utils::cv::toQPointF(pt);
    }
    //compared << utils::cv::toQPointF(compared_frame_polygon[0]);

    QPolygonF intersection = base.intersected(query);

    std::vector<cv::Point2f> inter_contour;
    for (const auto &pt: intersection.toStdVector())
    {
      inter_contour.push_back(utils::cv::toPoint2f(pt));
    }

    if (inter_contour.size() == 0)
    {
      std::cout << "No intersection" << std::endl;
      return 0;
    }


    double inter_area = cv::contourArea(inter_contour);
    return inter_area / frames_area[base_frame_num];
  }
  else
  {
    return 0;
  }
}

RestorerByFrame::FramePolygon RestorerByFrame::calculateFramePolygon(
    const cv::Point2f &frame_center, const cv::Point2f &pos,
    double angle, double scale) const
{
  cv::Rect2f frame_rect(cv::Point2f(0, 0), frame_center*2);

  return calculateFramePolygon(frame_rect, pos, angle, scale);
}

RestorerByFrame::FramePolygon RestorerByFrame::calculateFramePolygon(
      const cv::Point2f &frame_center, const cv::Mat &homography) const
{
  cv::Rect2f frame_rect(cv::Point2f(0, 0), frame_center*2);

  return calculateFramePolygon(frame_rect, homography);
}

RestorerByFrame::FramePolygon RestorerByFrame::calculateFramePolygon(
    const cv::Rect2f &frame_rect, const cv::Point2f &pos,
    double angle, double scale) const
{
  cv::Point2f center = (frame_rect.tl() + frame_rect.br()) / 2.;
  cv::Mat transform = Transformator::getTransformation({//transformations
                                   Transformator::getTranslate(-center),
                                   Transformator::getRotate(angle),
                                   Transformator::getScale(scale),
                                   Transformator::getTranslate(pos)
                                });
  return calculateFramePolygon(frame_rect, transform);
}

RestorerByFrame::FramePolygon RestorerByFrame::calculateFramePolygon(
      const cv::Rect2f &frame_rect, const cv::Mat &homography) const
{
  FramePolygon result(4, frame_rect.tl());

  result[2] = frame_rect.br();
  result[1].y = result[2].y;
  result[3].x = result[2].x;

  return Transformator::transform(result, homography);
}


size_t RestorerByFrame::getFramesCount() const
{
  return frames_key_points.size();
}

const FeatureBasedRestorer::KeyPointsList&
            RestorerByFrame::getFrameKeyPoints(size_t frame_num) const
{
  return frames_key_points[frame_num];
}

const cv::Mat&
            RestorerByFrame::getFrameDescriptions(size_t frame_num) const
{
  return getMatcher()->getTrainDescriptors()[frame_num];
}

void RestorerByFrame::save(std::string filename)
{
  std::clog << "No implementation for saving RestorerByFrame" << std::endl;
}

void RestorerByFrame::load(std::string filename)
{
  std::clog << "No implementation for loading RestorerByFrame" << std::endl;
}
