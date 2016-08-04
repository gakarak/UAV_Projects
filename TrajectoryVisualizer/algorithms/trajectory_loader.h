#ifndef TRAJECTORY_LOADER_H
#define TRAJECTORY_LOADER_H

#include <string>
#include <exception>

#include "model/entities/trajectory.h"
#include "progress_bar_notifier.h"

namespace algorithmspkg
{

class TrajectoryLoader: public ProgressBarNotifier
{
public:
  TrajectoryLoader();

  modelpkg::Trajectory loadTrajectory(std::string trj_idx_path);

  /*
   * KeyPoints loader
   */

  /**
   * @brief loadOrCalculateKeyPoints
   * try load key points otherwise calculate
   * @param trj
   * @param filename
   * @param detector
   * @param save save if calculated
   */
  void loadOrCalculateKeyPoints(modelpkg::Trajectory &trj,
                                std::string filename,
                                cv::Ptr<cv::Feature2D> detector,
                                bool save = false);
  void loadKeyPoints(modelpkg::Trajectory &trj, std::string filename);
  void calculateKeyPoints(modelpkg::Trajectory &trj,
                          cv::Ptr<cv::Feature2D> detector);
  /**
   * @brief TrajectoryLoader::sortKeyPointsByResponse
   * This method isn't sort descriptions
   * @param trj
   */
  void sortKeyPointsByResponse(modelpkg::Trajectory &trj);
  void saveKeyPoints(const modelpkg::Trajectory &trj,
                     std::string filename);

  /*
   * Descriptions loader
   */

  /**
   * @brief loadOrCalculateDescriptions
   * try load descriptions otherwise calculate
   * @param trj
   * @param filename
   * @param descriptor
   * @param save save if calculated
   */
  void loadOrCalculateDescriptions(modelpkg::Trajectory &trj,
                                   std::string filename,
                                   cv::Ptr<cv::Feature2D> descriptor,
                                   bool save = false);
  void loadDescriptions(modelpkg::Trajectory &trj, std::string filename);
  void calculateDescriptions(modelpkg::Trajectory &trj,
                             cv::Ptr<cv::Feature2D> descriptor);
  void saveDescriptions(const modelpkg::Trajectory &trj,
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
