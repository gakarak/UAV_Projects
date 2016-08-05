#include "main_controller.h"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <tr1/functional>

#include <QApplication>
#include <QPixmap>
#include <QPointF>
#include <QDebug>

#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>

#include "utils/acmopencv.h"
#include "utils/csv.h"
#include "utils/gradient_density.h"
#include "utils/geom_utils.h"

#include "config_singleton.h"
#include "algorithms/trajectory_loader.h"
#include "algorithms/transformator.h"

using namespace std;
using namespace modelpkg;
using namespace controllerpkg;
using namespace algorithmspkg;

MainController::MainController()
{
    initDetectors();
    initDescriptors();
    trj_recovers.assign(2, RestorerPtr());

    trajectories_selected_frames.assign(2, vector<int>());
    accumulative_trj_cuts.assign(2, vector<int>());
}

void MainController::loadOrCalculateModel(int trj_num,
                                          int detector_idx, int descriptor_idx,
                                          size_t max_key_points_per_frame)
{
  ConfigSingleton &cfg = ConfigSingleton::getInstance();
  Trajectory &trj = model->getTrajectory(trj_num);

  string detector_name = detectors_names[detector_idx].toStdString();
  string descriptor_name = descriptors_names[descriptor_idx].toStdString();

  string path_to_kp_bin = cfg.getPathToKeyPoints(trj_num,
                                                 detector_name);
  string path_to_descr_bin = cfg.getPathToDescriptors(trj_num,
                                             detector_name,
                                             descriptor_name);

  trj_loader.loadOrCalculateKeyPoints(trj, path_to_kp_bin,
                                             detectors[detector_idx], true);
  trj_loader.sortKeyPointsByResponse(trj, max_key_points_per_frame);

  bool isAllKeyPoints = max_key_points_per_frame == 0;
  trj_loader.loadOrCalculateDescriptions(trj, path_to_descr_bin,
                                                descriptors[descriptor_idx],
                                                isAllKeyPoints);
  if (!isAllKeyPoints)
  {//check for right descriptions, otherwise truncate to key_points size
    for (size_t frame_num = 0; frame_num < trj.getFramesCount(); frame_num++)
    {
      const auto &kps = trj.getFrameAllKeyPoints(frame_num);
      auto &descrs = trj.getFrameDescription(frame_num);
      if (kps.size() != descrs.rows)
      {
        cv::Rect roi;
        descrs = descrs(cv::Rect(0, 0, descrs.cols, kps.size()));
      }
    }
  }

  //preparing trajectory recover
  RestorerPtr &restorer = trj_recovers[trj_num];
  restorer = make_shared<RestorerByCloud>(detectors[detector_idx],
                                          descriptors[descriptor_idx]);

  for (size_t frame_num = 0; frame_num < trj.getFramesCount(); frame_num++)
  {
      restorer->addFrame(trj.getFrame(frame_num).image_center,
                         trj.getFrameAllKeyPoints(frame_num),
                         trj.getFrameDescription(frame_num),
                         trj.getFrame(frame_num).pos_m,
                         trj.getFrame(frame_num).angle,
                         trj.getFrame(frame_num).m_per_px);
  }

  //stupid way to find out if the model ready for manipulations
  if (model->getTrajectory(0).getFramesCount() != 0 &&
      !model->getTrajectory(0).getFrameAllKeyPoints(0).empty() &&
      model->getTrajectory(1).getFramesCount() != 0 &&
      !model->getTrajectory(1).getFrameAllKeyPoints(0).empty()
      )
  {
    view->setEnabledDataManipulating(true);
  }

  this->showKeyPointsNew(trj_num);
}

void MainController::calculateMatches()
{
  isFirstMatchingOnSecond = trajectories_selected_frames[1].empty();
  int from_trj_num = isFirstMatchingOnSecond? 0: 1;
  int to_trj_num = isFirstMatchingOnSecond? 1: 0;

  int frame_num = trajectories_selected_frames[from_trj_num][0];
  Trajectory &from_trj = model->getTrajectory(from_trj_num);
  auto &query_frame = from_trj.getFrame(frame_num);

  //cv::Mat homography;
  RestorerPtr &restorer = trj_recovers[to_trj_num];
  restorer->setQueryKeyPoints(from_trj.getFrameAllKeyPoints(frame_num));
  restorer->setQueryDescriptions(from_trj.getFrameDescription(frame_num));

  cv::Point2f pos;
  double angle;
  double scale;
  double confidence = restorer->recoverLocation(query_frame.image_center,
                                                pos, angle, scale);

  cout << "Confidence: " << confidence <<
          "\tPos: " << pos <<
          "\tAngle: " << angle <<
          "\tScale: " << scale << endl;

  view->setGhostRecovery(utils::cv::toQPointF(pos),
                         query_frame.image.cols*scale,
                         query_frame.image.rows*scale,
                         angle);

  this->showMatches();

}

void MainController::recoverTrajectory(double score_thres)
{
    /*RestorerPtr &restorer = this->trj_recovers[0];
    Trajectory &trj = model->getTrajectory(1);

    frames_to_hide_by_score.clear();
    for (int frame_num = 0; frame_num < trj.getFramesCount(); frame_num++)
    {
        Map &frame = trj.getFrame(frame_num);

        cv::Mat homography;
        std::vector<cv::DMatch> matches;

        restorer->setQueryKeyPoints(trj.getFrameAllKeyPoints(frame_num));
        restorer->setQueryDescriptions(trj.getFrameDescription(frame_num));

        double score = restorer->recoverTrajectory(trj.getFrameAllKeyPoints(frame_num),
                                  trj.getFrameDescription(frame_num),
                                  homography, matches);
        cout << "Score: " << score << endl;

        if (score > score_thres)
        {
//          Transformator::getParams(homography, frame.pos_m, frame.angle,
//                                               frame.m_per_px);
          cv::Point2f center(frame.image.cols/2., frame.image.rows/2.);
          cv::Point2f rotate_pt(center.x+10, center.y);

          cv::Point2f bounded_center = Transformator::transform(center, homography);
          cv::Point2f bounded_rotate_pt = Transformator::transform(rotate_pt, homography);

          frame.pos_m = bounded_center;
          frame.angle = utils::cv::angleBetween(rotate_pt-center,
                                                bounded_rotate_pt - bounded_center);
          frame.m_per_px = cv::norm(bounded_rotate_pt - bounded_center) / cv::norm(rotate_pt - center);
        }
        else
        {
          frames_to_hide_by_score.push_back(frame_num);
        }
    }


    showTrajectory(1);*/
}

void MainController::loadIni(string ini_filename)
{
    clear();

    ConfigSingleton &cfg = ConfigSingleton::getInstance();
    try
    {
        cfg.loadIni(ini_filename);
        loadTrajectory(0, cfg.getPathToTrajectoryCsv(0));
        loadTrajectory(1, cfg.getPathToTrajectoryCsv(1));

        view->setTrajectoryPath(0, QString::fromStdString(
                                  cfg.getPathToTrajectoryCsv(0)));
        view->setTrajectoryPath(1, QString::fromStdString(
                                  cfg.getPathToTrajectoryCsv(1)));
        view->setEnabledDataCalculating(true);
        view->setEnabledDataManipulating(false);
    }
    catch (ConfigSingleton::Exception &e)
    {
        this->showException(e.what());
    }
}

void MainController::loadTrajectory(int trj_num, string trj_filename)
{
  ConfigSingleton &cfg = ConfigSingleton::getInstance();
  try
  {    
    Trajectory new_trj = trj_loader.loadTrajectory(trj_filename);

    model->setTrajectory(trj_num, new_trj);
    this->calculateFramesQuality(trj_num);

    cfg.setPathToTrajectoryCsv(trj_num, trj_filename);

    this->showTrajectory(trj_num);
    view->setEnabledDataCalculating(true, trj_num);
    view->setEnabledDataManipulating(false);
  }
  catch (runtime_error er)
  {
      showException(er.what());

      QString prev = QString::fromStdString(cfg.getPathToTrajectoryCsv(trj_num));
      view->setTrajectoryPath(trj_num, prev);
  }
}

void MainController::loadMainMap(string filename, double meters_per_pixel)
{
    model->getMainMap() = modelpkg::Map(filename, 0, 0, 0, meters_per_pixel);

    double center_x_m = model->getMainMap().image.cols / 2.0 * meters_per_pixel;
    double center_y_m = model->getMainMap().image.rows / 2.0 * meters_per_pixel;
    model->getMainMap().pos_m = cv::Point2f(center_x_m, center_y_m);

    this->showMainMap();
}

void MainController::showTrajectory(int trj_num)
{
    view->getMatchesItem().clear();
    view->clearGhostRecovery();

    vector<QPixmap> qpixs;
    vector<QPointF> center_coords_px;
    vector<double>  angles;
    vector<double>  meter_per_pixels;
    const vector<double> &qualities = model->getTrajectory(trj_num).getAllFramesQuality();

    for (const auto &frame: model->getTrajectory(trj_num).getAllFrames())
    {
        QPointF center_px;
        center_px.setX( frame.pos_m.x / frame.m_per_px );
        center_px.setY( frame.pos_m.y / frame.m_per_px );

        qpixs.push_back( utils::ASM::cvMatToQPixmap(frame.image) );
        center_coords_px.push_back( center_px );
        angles.push_back( frame.angle );
        meter_per_pixels.push_back( frame.m_per_px );
    }
    if (trj_num == 0)
    {
        view->setFirstTrajectory(qpixs, center_coords_px, angles, meter_per_pixels, qualities);
    }
    else
    {
        view->setSecondTrajectory(qpixs, center_coords_px, angles, meter_per_pixels, qualities);
    }
}

void MainController::showMainMap()
{
    QPixmap qpix = utils::ASM::cvMatToQPixmap(model->getMainMap().image);
    view->setMainMap(qpix, model->getMainMap().m_per_px);
}

void MainController::showKeyPointsNew(int trj_num)
{
    RestorerPtr &restorer = trj_recovers[trj_num];


    view->getMatchesItem().clear();
    view->clearGhostRecovery();

    view->getTrajectoryItem(trj_num).clearKeyPoints();
    view->setProgressBarTask("Drawing key points");

    for (size_t i = 0; i < restorer->getFramesCount(); i++)
    {
      auto kps = restorer->getFrameKeyPoints(i);
      for (size_t kp_num = 0; kp_num < min(kps.size(), (size_t)500); kp_num++)
      {
        auto &kp = kps[kp_num];
        view->getTrajectoryItem(trj_num).addKeyPointNew(
              utils::cv::toQPointF(kp.pt),
              kp.angle,
              kp.size/2.,
              1,
              QColor(255, 0, 0));
        view->getMatchesItem().clear();
      }
      view->setProgressBarValue(i+1, restorer->getFramesCount());
    }

}

void MainController::showMatches()
{
  int from_trj_num = isFirstMatchingOnSecond? 0: 1;
  int to_trj_num = isFirstMatchingOnSecond? 1: 0;

  int frame_num = trajectories_selected_frames[from_trj_num][0];
  auto &from_frame = model->getTrajectory(from_trj_num).getFrame(frame_num);

  RestorerPtr &restorer = trj_recovers[to_trj_num];
  auto &matches = restorer->getLastMatches();

  view->getMatchesItem().clear();
  for (size_t match_num = 0; match_num < matches.size(); match_num++)
  {
    const cv::DMatch &match = matches[match_num];
    cv::Point2f to_pt = restorer->getFrameKeyPoints(
                                            match.imgIdx)[match.trainIdx].pt;
    cv::Point2f from_pt = restorer->getQueryKeyPoints()[match.queryIdx].pt;

    //from_pt = Transformator::transform(from_pt, restorer->getLastHomography());
    from_pt = Transformator::transform(from_pt, {//transformations
                                    Transformator::getTranslate(-from_frame.image_center),
                                    Transformator::getRotate(from_frame.angle),
                                    Transformator::getScale(from_frame.m_per_px),
                                    Transformator::getTranslate(from_frame.pos_m)
                                 });

    if (isFirstMatchingOnSecond)
    {
      view->getMatchesItem().addLine(utils::cv::toQPointF(from_pt),
                                     utils::cv::toQPointF(to_pt));
    }
    else
    {
      view->getMatchesItem().addLine(utils::cv::toQPointF(to_pt),
                                     utils::cv::toQPointF(from_pt));
    }
  }
}

void MainController::filterByScore(bool isFilter)
{
    if (isFilter)
    {
      for (int frame_num: frames_to_hide_by_score)
      {
        view->getTrajectoryItem(1).hideFrame(frame_num);
      }
    }
    else
    {
      for (int frame_num: frames_to_hide_by_score)
      {
        view->getTrajectoryItem(1).showFrame(frame_num);
      }
    }
}

void MainController::setView(std::shared_ptr<viewpkg::MainView> view)
{
  this->view = view;

  trj_loader.setTitleSetter([this](std::string title, bool reset)
  {
    this->view->setProgressBarTask(QString::fromStdString(title), reset);
  });
  trj_loader.setProgressNotifier(tr1::bind(
                                   &viewpkg::MainView::setProgressBarValue,
                                   this->view,
                                   tr1::placeholders::_1,
                                   tr1::placeholders::_2,
                                   tr1::placeholders::_3)
                                 );
}

void MainController::selectedFrame(int trj_num, int frame_num)
{
    trajectories_selected_frames[trj_num].push_back(frame_num);
}

void MainController::unselectedFrame(int trj_num, int frame_num)
{
    auto &selected_frames = trajectories_selected_frames[trj_num];
    auto where = find(selected_frames.begin(), selected_frames.end(), frame_num);
    if (where != selected_frames.end())
    {
        selected_frames.erase(where);
    }
}

void MainController::showException(string what)
{
    clog << "Exception! " << what << endl;
    view->showException(QString::fromStdString(what));
}

void MainController::showView()
{
    view->setDetectors(detectors_names);
    view->setDescriptors(descriptors_names);
    view->setEnabledDataCalculating(false);
    view->setEnabledDataManipulating(false);

    view->show();
}

/*
 * PRIVATE SECTION
 */

void MainController::calculateFramesQuality()
{
    double threshold = ConfigSingleton::getInstance().getQualityThreshold();

    for (size_t trj_num = 0; trj_num < model->getTrajectoriesCount(); trj_num++)
    {
        Trajectory &trj = model->getTrajectory(trj_num);

        for (size_t frame_num = 0; frame_num < trj.getFramesCount(); frame_num++)
        {
            const auto &frame = trj.getFrame(frame_num);

            double scale = frame.m_per_px / ConfigSingleton::getInstance().getGradientMetersPerPixel();
            cv::Size new_size(frame.image.size().width * scale, frame.image.size().height * scale);

            cv::Mat resized;
            cv::resize(frame.image, resized, new_size);

            double quality = std::min( 1., utils::cv::gradientDensity(resized) / threshold );

            trj.setFrameQuality(frame_num, quality);
        }
    }

}

void MainController::calculateFramesQuality(int trj_num)
{
  double threshold = ConfigSingleton::getInstance().getQualityThreshold();
  Trajectory &trj = model->getTrajectory(trj_num);

  for (size_t frame_num = 0; frame_num < trj.getFramesCount(); frame_num++)
  {
      const auto &frame = trj.getFrame(frame_num);

      double scale = frame.m_per_px / ConfigSingleton::getInstance().getGradientMetersPerPixel();
      cv::Size new_size(frame.image.size().width * scale, frame.image.size().height * scale);

      cv::Mat resized;
      cv::resize(frame.image, resized, new_size);

      double quality = std::min( 1., utils::cv::gradientDensity(resized) / threshold );

      trj.setFrameQuality(frame_num, quality);
  }
}

void MainController::initDetectors()
{
    detectors_names.push_back("SIFT");
    detectors.push_back(cv::xfeatures2d::SIFT::create(100));

    detectors_names.push_back("SURF");
    detectors.push_back(cv::xfeatures2d::SURF::create());

    detectors_names.push_back("KAZE");
    detectors.push_back(cv::KAZE::create());

    detectors_names.push_back("AKAZE");
    detectors.push_back(cv::AKAZE::create());

    detectors_names.push_back("BRISK");
    detectors.push_back(cv::BRISK::create());

    detectors_names.push_back("ORB");
    detectors.push_back(cv::ORB::create());
}

void MainController::initDescriptors()
{
    descriptors_names.push_back("SIFT");
    descriptors.push_back(cv::xfeatures2d::SIFT::create());
    norm_types.push_back(cv::NORM_L2SQR);

    descriptors_names.push_back("SURF");
    descriptors.push_back(cv::xfeatures2d::SURF::create());
    norm_types.push_back(cv::NORM_L2SQR);

    descriptors_names.push_back("KAZE");
    descriptors.push_back(cv::KAZE::create());
    norm_types.push_back(cv::NORM_L2SQR);

    descriptors_names.push_back("AKAZE");
    descriptors.push_back(cv::AKAZE::create());
    norm_types.push_back(cv::NORM_L2SQR);

    descriptors_names.push_back("BRISK");
    descriptors.push_back(cv::BRISK::create());
    norm_types.push_back(cv::NORM_HAMMING);

    descriptors_names.push_back("ORB");
    descriptors.push_back(cv::ORB::create());
    norm_types.push_back(cv::NORM_HAMMING);

    descriptors_names.push_back("FREAK");
    descriptors.push_back(cv::xfeatures2d::FREAK::create());
    norm_types.push_back(cv::NORM_HAMMING);
}

void MainController::clear()
{
    for (auto &selected_frames: trajectories_selected_frames)
    {
        selected_frames.clear();
    }
}

Trajectory MainController::loadTrjFromCsv(string csv_filename)
{        
    Trajectory trj;

    auto parsed_csv = utils::csvtools::read_csv(csv_filename);

    size_t found = csv_filename.find_last_of("/\\");
    string folder = csv_filename.substr(0,found);

    for (auto &row : parsed_csv)
    {
        if (row[0] == "Path")
        {//column names
            continue;
        }
        row[0] = folder + "/" + row[0];
        Map frame = loadMapFromRow(row);
        trj.pushBackFrame(frame);
    }

    return trj;
}

Map MainController::loadMapFromRow(std::vector<string> params)
{
    //need for atof, because '.' is not delimeter of float part
    char *saved_locale;
    saved_locale = setlocale(LC_NUMERIC, "C");


    double x_m = atof(params[1].c_str()); //QString::fromStdString(params[1]).toDouble();
    double y_m = atof(params[2].c_str()); //QString::fromStdString(params[2]).toDouble();

    double angle = atof(params[3].c_str()); //QString::fromStdString(params[3]).toDouble();
    double m_per_px = atof(params[4].c_str()); //QString::fromStdString(params[4]).toDouble();


    setlocale(LC_NUMERIC, saved_locale);

    return Map(params[0], x_m, y_m, angle, m_per_px);
}
