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

double RestorerByFrame::recoverLocation(const cv::Point2f &que_frame_center,
                                        cv::Point2f &pos,
                                        double &angle, double &scale)
{
  //points for findHomography
  static std::vector<cv::Point2f> query_pts;
  static std::vector<cv::Point2f> train_pts;

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
    double areaConfidence = calculateAreaConfidence(que_frame_center, frame_num);

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
      pos =  Transformator::transform(que_frame_center, homography);

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
    const cv::Point2f &query_frame_center, int compared_frame_num) const
{
  if (!homography.empty())
  {
    FramePolygon query_frame_polygon = calculateFramePolygon(query_frame_center,
                                                             homography);
    const auto &compared_frame_polygon = frames_polygons[compared_frame_num];

    //calculate intersection with QPointF
    QPolygonF query;
    for (const auto &pt: query_frame_polygon)
    {
      query << utils::cv::toQPointF(pt);
    }
    //query << utils::cv::toQPointF(query_frame_polygon[0]);

    QPolygonF compared;
    for (const auto &pt: compared_frame_polygon)
    {
      compared << utils::cv::toQPointF(pt);
    }
    //compared << utils::cv::toQPointF(compared_frame_polygon[0]);

    QPolygonF intersection = compared.intersected(query);

    std::vector<cv::Point2f> v;
    for (const auto &pt: intersection.toStdVector())
    {
      v.push_back(utils::cv::toPoint2f(pt));
    }
    /*std::cout << "[";
    std::copy(query_frame_polygon.begin(), query_frame_polygon.end()-1,
              ostream_iterator<cv::Point2f>(cout, ", "));
    std::cout << query_frame_polygon.back() << "]" << endl;

    std::cout << "[";
    std::copy(compared_frame_polygon.begin(), compared_frame_polygon.end()-1,
              ostream_iterator<cv::Point2f>(cout, ", "));
    std::cout << compared_frame_polygon.back() << "]" << endl;

    std::cout << "[";
    std::copy(v.begin(), v.end()-1, ostream_iterator<cv::Point2f>(cout, ", "));
    std::cout << v.back() << "]" << endl;*/
    if (v.size() == 0)
    {
      std::cout << "lol" << std::endl;
      return 0;
    }


    double inter_area = cv::contourArea(v);
    return inter_area / frames_area[compared_frame_num];
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
   cv::Mat transform = Transformator::getTransformation({//transformations
                                    Transformator::getTranslate(-frame_center),
                                    Transformator::getRotate(angle),
                                    Transformator::getScale(scale),
                                    Transformator::getTranslate(pos)
                                 });
   return calculateFramePolygon(frame_center, transform);
}

RestorerByFrame::FramePolygon RestorerByFrame::calculateFramePolygon(
      const cv::Point2f &frame_center, const cv::Mat &homography) const
{
  FramePolygon result(4, cv::Point2f(0, 0));

  result[2] = frame_center*2; //yield width, height
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
