#ifndef RESTORER_BY_FRAME_BLOCKS_H
#define RESTORER_BY_FRAME_BLOCKS_H

#include "restorer_by_frame.h"

namespace algorithmspkg
{

class RestorerByFrameBlocks : public RestorerByFrame
{
 public:
  RestorerByFrameBlocks() = delete;
  RestorerByFrameBlocks(
            DetectorPtr   detector,
            DescriptorPtr descriptor,
            MatcherPtr    matcher = cv::DescriptorMatcher::create("FlannBased"),
            size_t max_key_points_per_frame = 0);

  double recoverLocation(const cv::Point2f &frame_center,
                         cv::Point2f &pos, double &angle,
                         double &scale) override;

};

}

#endif // RESTORER_BY_FRAME_BLOCKS_H
