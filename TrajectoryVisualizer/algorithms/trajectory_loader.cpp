#include "trajectory_loader.h"

#include <fstream>

#include "utils/csv.h"

using namespace algorithmspkg;
using namespace modelpkg;
using namespace cv;
using namespace std;

TrajectoryLoader::TrajectoryLoader()
{
}

Trajectory TrajectoryLoader::loadTrajectory(string trj_idx_path)
{
  Trajectory trj;

  auto parsed_csv = utils::csvtools::read_csv(trj_idx_path);

  size_t found = trj_idx_path.find_last_of("/\\");
  string folder = trj_idx_path.substr(0,found);

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

Map TrajectoryLoader::loadMapFromRow(vector<string> params)
{
    //need for atof, because '.' is not delimeter of float part
    char *saved_locale;
    saved_locale = setlocale(LC_NUMERIC, "C");


    double x_m = atof(params[1].c_str());
    double y_m = atof(params[2].c_str());

    double angle = atof(params[3].c_str());
    double m_per_px = atof(params[4].c_str());


    setlocale(LC_NUMERIC, saved_locale);

    return Map(params[0], x_m, y_m, angle, m_per_px);
}

/*
 * KeyPoints loader
 */

void TrajectoryLoader::loadKeyPoints(Trajectory &trj, string filename)
{
  ifstream in(filename, ios::binary);
  if (!in)
  {
    throw TrajectoryLoader::NoFileExist(filename);
  }

  //maybe format checking
  size_t framesCount = 0;
  in.read(reinterpret_cast<char*>(&framesCount), sizeof(framesCount));
  for (size_t frame_num = 0; frame_num < framesCount; frame_num++)
  {
    auto &mutable_frame_kps = trj.getFrameAllKeyPoints(frame_num);
    mutable_frame_kps.clear();

    size_t keyPointsCount = 0;
    in.read(reinterpret_cast<char*>(&keyPointsCount), sizeof(keyPointsCount));
    for (size_t kp_num = 0; kp_num < keyPointsCount; kp_num++)
    {
      cv::KeyPoint kp;
      in.read(reinterpret_cast<char*>(&kp), sizeof(kp));
      mutable_frame_kps.push_back(kp);
    }
  }
}

void TrajectoryLoader::calculateKeyPoints(Trajectory &trj,
                                         Ptr<Feature2D> detector)
{
  for (size_t frame_num = 0; frame_num < trj.getFramesCount(); frame_num++)
  {
    const auto &frame = trj.getFrame(frame_num);
    auto &mutable_frame_kps = trj.getFrameAllKeyPoints(frame_num);

    detector->detect(frame.image, mutable_frame_kps);
  }
}

void TrajectoryLoader::sortKeyPointsByResponse(Trajectory &trj)
{
  for (size_t frame_num = 0; frame_num < trj.getFramesCount(); frame_num++)
  {
    auto &mutable_frame_kps = trj.getFrameAllKeyPoints(frame_num);
    //sort by response
    std::sort(mutable_frame_kps.begin(), mutable_frame_kps.end(),
              []( const cv::KeyPoint &left, const cv::KeyPoint &right ) ->
              bool { return left.response > right.response; });
  }
}

void TrajectoryLoader::saveKeyPoints(const Trajectory &trj, string filename)
{
  ofstream out(filename, ios::binary);

  size_t framesCount = trj.getFramesCount();
  out.write(reinterpret_cast<char*>(&framesCount), sizeof(framesCount));
  for (size_t frame_num = 0; frame_num < framesCount; frame_num++)
  {
      const auto &frame_key_points = trj.getFrameAllKeyPoints(frame_num);

      size_t keyPointsCount = frame_key_points.size();
      out.write(reinterpret_cast<char*>(&keyPointsCount), sizeof(keyPointsCount));
      for (size_t kp_num = 0; kp_num < keyPointsCount; kp_num++)
      {
          out.write(reinterpret_cast<const char*>(&frame_key_points[kp_num]),
                    sizeof(frame_key_points[kp_num]));
      }
  }
}

/*
 * Descriptions loader
 */

void TrajectoryLoader::loadDescriptions(Trajectory &trj, string filename)
{
  cv::FileStorage file(filename, cv::FileStorage::READ);

  if (!file.isOpened())
  {
    throw TrajectoryLoader::NoFileExist(filename);
  }

  int framesCount = 0;
  file["count"] >> framesCount;
  for (int frame_num = 0; frame_num < framesCount; frame_num++)
  {
    cv::Mat descr;
    file["img"+to_string(frame_num)] >> descr;

    trj.setFrameDescription(frame_num, descr);
  }
  file.release();
}

void TrajectoryLoader::calculateDescriptions(Trajectory &trj,
                                             Ptr<Feature2D> descriptor)
{
  for (size_t frame_num = 0; frame_num < trj.getFramesCount(); frame_num++)
  {
      Mat descr;
      descriptor->compute(trj.getFrame(frame_num).image,
                          trj.getFrameAllKeyPoints(frame_num), descr);

      trj.setFrameDescription(frame_num, descr);
  }
}

void TrajectoryLoader::saveDescriptions(const Trajectory &trj, string filename)
{
  cv::FileStorage file(filename, cv::FileStorage::WRITE);

  int framesCount = trj.getFramesCount();
  file << "count" << framesCount;
  for (int frame_num = 0; frame_num < framesCount; frame_num++)
  {
      file << "img"+to_string(frame_num) << trj.getFrameDescription(frame_num);
  }
  file.release();
}
