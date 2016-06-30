#ifndef TRAJECTORY_LOADER_H
#define TRAJECTORY_LOADER_H

#include <string>
#include <exception>

#include "model/entities/trajectory.h"

namespace algorithmspkg
{

class TrajectoryLoader
{
 public:
  TrajectoryLoader();
  static modelpkg::Trajectory loadTrajectory(std::string trj_idx_path);

  /*
   * KeyPoints loader
   */
  static void loadKeyPoints(modelpkg::Trajectory &trj, std::string filename);
  static void calculateKeyPoints(modelpkg::Trajectory &trj,
                                 cv::Ptr<cv::Feature2D> detector);
  static void sortKeyPointsByResponse(modelpkg::Trajectory &trj);
  static void saveKeyPoints(const modelpkg::Trajectory &trj,
                            std::string filename);

  /*
   * Descriptions loader
   */

  static void loadDescriptions(modelpkg::Trajectory &trj, std::string filename);
  static void calculateDescriptions(modelpkg::Trajectory &trj,
                                    cv::Ptr<cv::Feature2D> descriptor);
  static void saveDescriptions(const modelpkg::Trajectory &trj,
                               std::string filename);



  class Exception: public std::runtime_error
  {
   public:
    Exception(const std::string &what):
      std::runtime_error("TrajectoryLoader: " + what)
    {}
  };

  class NoFileExist: public Exception
  {
   public:
    NoFileExist(const std::string filename):
      Exception("Cannot open file: " + filename)
    {}
  };

 private:
  static modelpkg::Map loadMapFromRow(std::vector<std::string> params);
};

}

#endif // TRAJECTORY_LOADER_H
