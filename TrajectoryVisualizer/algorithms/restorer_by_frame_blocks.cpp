#include "restorer_by_frame_blocks.h"
#include "transformator.h"

#include <iostream>

#include <QFile>
#include <QTextStream>

using namespace algorithmspkg;

RestorerByFrameBlocks::RestorerByFrameBlocks(
                                FeatureBasedRestorer::DetectorPtr   detector,
                                FeatureBasedRestorer::DescriptorPtr descriptor,
                                FeatureBasedRestorer::MatcherPtr    matcher,
                                size_t max_key_points_per_frame)
  : RestorerByFrame(detector, descriptor, matcher, max_key_points_per_frame)
{

}

double RestorerByFrameBlocks::recoverLocation(const cv::Point2f &frame_center,
                                                    cv::Point2f &pos,
                                                    double      &angle,
                                                    double      &scale)
{
  auto query_kps_store = query_key_points;
  auto query_descrs_store = query_descriptions;
  std::vector<int> local_to_full_idx(query_kps_store.size(), 0);
  std::iota(local_to_full_idx.begin(), local_to_full_idx.end(), 0);
  MatchesList matches_store;
  auto best_homography = cv::Mat();

  std::vector<cv::Rect2f> rects = {cv::Rect2f(cv::Point2f(0, 0), //full rect
                                              2*frame_center)};
  for (int i = 0; i < 2; i++)
  for (int j = 0; j < 2; j++)
  {
    rects.push_back(cv::Rect2f(
                          cv::Point2f(j*frame_center.x, i*frame_center.y),
                          cv::Point2f((j+1)*frame_center.x, (i+1)*frame_center.y)
                          )
                    );
  }

  const auto center_half = frame_center / 2.;
  rects.push_back(cv::Rect2f( //center rect
                        frame_center - center_half,
                        frame_center + center_half
                        )
                  );

  double max_confidence = 0;
  for (size_t rect_num = 0; rect_num < rects.size(); rect_num++)
  {
    const cv::Rect2f &rect = rects[rect_num];
    if (rect_num != 0) //because initially all key points in full rect
    {
      query_key_points.clear();
      query_descriptions = cv::Mat();
      local_to_full_idx.clear();

      //settings contained points
      for (size_t kp_num = 0; kp_num < query_kps_store.size(); kp_num++)
      {
        const auto &pt = query_kps_store[kp_num].pt;
        if (rect.contains(pt))
        {
          local_to_full_idx.push_back(kp_num);
          query_key_points.push_back(query_kps_store[kp_num]);
          query_descriptions.push_back(query_descrs_store.row(kp_num));
        }
      }
    }

    cv::Point2f t_pos(0, 0);
    double t_angle = 0;
    double t_scale = 0;
    double confidence = RestorerByFrame::recoverLocation(rect,
                                                         t_pos, t_angle,
                                                         t_scale);

    std::cout << "Rect " << rect_num << " confidence: " << confidence <<
                                                                      std::endl;

    if (confidence > max_confidence)
    {
      pos = Transformator::transform(frame_center, homography);
      angle = t_angle;
      scale = t_scale;
      max_confidence = confidence;
      matches_store = matches;

      //matches local idx recover to full image idx
      //fast rely on assumptation that matches ordered
      //queryIdx as 0, 1, 2, 3, 4...
      for (auto &match: matches_store)
      {
        match.queryIdx = local_to_full_idx[match.queryIdx];
      }

      homography.copyTo(best_homography);
    }

    //rename files
    std::vector<QString> file_names = {"_maskConfidence.csv",
                                       "_areaConfidence.csv",
                                       "_scales.csv"};
    for (auto f_name: file_names)
    {
      QString new_name = QString::number(rect_num) + f_name;
      QFile f(f_name);
      if (!f.rename(new_name))
      {
        QFile new_f(new_name);
        new_f.open(QIODevice::WriteOnly | QIODevice::Append);
        f.open(QIODevice::ReadOnly);

        QTextStream in(&f);
        QString line = in.readLine() + "\n";
        new_f.write(line.toStdString().c_str(), line.size());
        new_f.close();
        f.close();

        f.remove();
      }
    }
  }

  matches = matches_store;
  query_key_points = query_kps_store;
  query_descriptions = query_descrs_store;
  best_homography.copyTo(homography);
  return max_confidence;
}
