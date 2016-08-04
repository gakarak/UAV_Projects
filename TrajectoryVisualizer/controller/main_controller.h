#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include <QString>

#include <memory>
#include <string>
#include <vector>

#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

#include "view/main_view.h"
#include "model/main_model.h"
#include "algorithms/trajectory_loader.h"
#include "algorithms/restorer_by_cloud.h"

namespace controllerpkg
{

class MainController
{
 public:
  MainController();

  void loadOrCalculateModel(int trj_num,
                            int detector_idx, int descriptor_idx);
  void calculateMatches();
  void recoverTrajectory(double score_thres);

  void loadIni(std::string ini_filename);
  void loadTrajectory(int trj_num, std::string trj_filename);
  void loadMainMap(std::string filename, double meters_per_pixel);

  void showTrajectory(int trj_num);
  void showMainMap();
  void showKeyPointsNew(int trj_num);
  void showMatches();
  void filterByScore(bool isFilter);

  void setView(std::shared_ptr<viewpkg::MainView> view);
  void setModel(std::shared_ptr<modelpkg::MainModel> model)   { this->model = model; }

  void selectedFrame(int trj_num, int frame_num);
  void unselectedFrame(int trj_num, int frame_num);

  void showException(std::string what);
  void showView();

  const std::vector<QString>& getDetectorsNames() const { return detectors_names; }
  const std::vector<QString>& getDescriptorsNames() const { return descriptors_names; }

  cv::Ptr<cv::Feature2D> getDetector(int i) { return detectors[i]; }
  cv::Ptr<cv::Feature2D> getDescriptor(int i) { return descriptors[i]; }

  //exceptions
  class Exception: public std::runtime_error
  {
  public:
    Exception(const std::string &what): std::runtime_error("MainController: " + what)
    {}
  };

  class NoFileExist: public Exception
  {
  public:
    NoFileExist(const std::string filename): Exception("Cannot open file: " + filename)
    {}
  };

 private:
  void initDetectors();
  void initDescriptors();

  void clear();

  void calculateFramesQuality();
  void calculateFramesQuality(int trj_num);

  //params - filename, x_m, y_m, angle, m_per_px
  modelpkg::Map           loadMapFromRow(std::vector<std::string> params);
  modelpkg::Trajectory    loadTrjFromCsv(std::string csv_filename);

 private:
  std::shared_ptr<viewpkg::MainView> view;
  std::shared_ptr<modelpkg::MainModel> model;

  algorithmspkg::TrajectoryLoader trj_loader;

  std::vector<QString> detectors_names;
  std::vector<cv::Ptr<cv::Feature2D>> detectors;

  std::vector<QString> descriptors_names;
  std::vector<cv::Ptr<cv::Feature2D>> descriptors;
  std::vector<cv::NormTypes> norm_types;

  std::vector<std::vector<int>> trajectories_selected_frames;

  bool isFirstMatchingOnSecond;
  cv::Mat homography;
  std::vector<cv::DMatch> matches;
  std::vector<std::vector<int>> accumulative_trj_cuts;

  using RestorerPtr = std::shared_ptr<algorithmspkg::FeatureBasedRestorer>;


  std::vector<RestorerPtr> trj_recovers;
  std::vector<int> frames_to_hide_by_score;
};

}

#endif //MAIN_CONTROLLER_H
