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
  setProgressBarTitle("Loading trajectory", true);
  Trajectory trj;

  auto parsed_csv = utils::csvtools::read_csv(trj_idx_path);

  size_t found = trj_idx_path.find_last_of("/\\");
  string folder = trj_idx_path.substr(0,found);

  int count = 0;
  for (auto &row : parsed_csv)
  {
    if (row[0] == "Path")
    {//column names
      continue;
    }
    row[0] = folder + "/" + row[0];
    Map frame = loadMapFromRow(row);
    trj.pushBackFrame(frame);

    notifyProgressBar(++count + 1, parsed_csv.size());
  }

  return trj;
}

void TrajectoryLoader::loadOrCalculateKeyPoints(Trajectory &trj,
                                                string filename,
                                                cv::Ptr<Feature2D> detector,
                                                bool save)
{
  try
  {
    loadKeyPoints(trj, filename);
  }
  catch (TrajectoryLoader::NoFileExist &e)
  {
    calculateKeyPoints(trj, detector);
    if (save)
    {
      saveKeyPoints(trj, filename);
    }
  }
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

  setProgressBarTitle("Loading key points");

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

    notifyProgressBar(frame_num + 1, framesCount);
  }
}

void TrajectoryLoader::calculateKeyPoints(Trajectory &trj,
                                          Ptr<Feature2D> detector)
{
  setProgressBarTitle("Calculating key points");
  for (size_t frame_num = 0; frame_num < trj.getFramesCount(); frame_num++)
  {
    const auto &frame = trj.getFrame(frame_num);
    auto &mutable_frame_kps = trj.getFrameAllKeyPoints(frame_num);

    detector->detect(frame.image, mutable_frame_kps);

    notifyProgressBar(frame_num + 1, trj.getFramesCount());
  }
}

void TrajectoryLoader::sortKeyPointsByResponse(Trajectory &trj,
                                               size_t max_key_points_per_frame)
{
  for (size_t frame_num = 0; frame_num < trj.getFramesCount(); frame_num++)
  {
    auto &mutable_frame_kps = trj.getFrameAllKeyPoints(frame_num);
    //sort by response
    std::sort(mutable_frame_kps.begin(), mutable_frame_kps.end(),
              []( const cv::KeyPoint &left, const cv::KeyPoint &right ) ->
              bool { return left.response > right.response; });

    if (max_key_points_per_frame != 0 &&
        max_key_points_per_frame < mutable_frame_kps.size())
    {
      mutable_frame_kps.resize(max_key_points_per_frame);
      mutable_frame_kps.shrink_to_fit();
    }
  }
}

void TrajectoryLoader::saveKeyPoints(const Trajectory &trj, string filename)
{
  ofstream out(filename, ios::binary);

  setProgressBarTitle("Saving key points");

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

    notifyProgressBar(frame_num + 1, framesCount);
  }
}

void TrajectoryLoader::loadOrCalculateDescriptions(Trajectory &trj,
                                                   string filename,
                                                   cv::Ptr<Feature2D> descriptor,
                                                   bool save)
{
  try
  {
    loadDescriptions(trj, filename);
  }
  catch (TrajectoryLoader::NoFileExist &e)
  {
    calculateDescriptions(trj, descriptor);
    if (save)
    {
      saveDescriptions(trj, filename);
    }
  }
}

/*
 * Descriptions loader
 */

void TrajectoryLoader::loadDescriptions(Trajectory &trj, string filename)
{
  ifstream in(filename, ios::binary);
  if (!in)
  {
    throw TrajectoryLoader::NoFileExist(filename);
  }

  setProgressBarTitle("Loading descriptions");

  //maybe format checking
  size_t framesCount = 0;
  in.read(reinterpret_cast<char*>(&framesCount), sizeof(framesCount));
  for (size_t frame_num = 0; frame_num < framesCount; frame_num++)
  {
    int rows = 0;
    in.read(reinterpret_cast<char*>(&rows),
            sizeof(rows));
    int cols = 0;
    in.read(reinterpret_cast<char*>(&cols),
            sizeof(cols));
    int type = 0;
    in.read(reinterpret_cast<char*>(&type),
            sizeof(type));

    cv::Mat descriptions(rows, cols, type);

    in.read(reinterpret_cast<char*>(descriptions.data),
            rows*cols*descriptions.elemSize());

    trj.setFrameDescription(frame_num, descriptions);

    notifyProgressBar(frame_num + 1, framesCount);
  }
}

void TrajectoryLoader::calculateDescriptions(Trajectory &trj,
                                             Ptr<Feature2D> descriptor)
{
  setProgressBarTitle("Calculating descriptions");

  for (size_t frame_num = 0; frame_num < trj.getFramesCount(); frame_num++)
  {
    Mat descr;
    descriptor->compute(trj.getFrame(frame_num).image,
                        trj.getFrameAllKeyPoints(frame_num), descr);

    trj.setFrameDescription(frame_num, descr);

    notifyProgressBar(frame_num + 1, trj.getFramesCount());
  }
}

void TrajectoryLoader::saveDescriptions(const Trajectory &trj, string filename)
{
  ofstream out(filename, ios::binary);

  setProgressBarTitle("Saving descriptions");

  size_t framesCount = trj.getFramesCount();
  out.write(reinterpret_cast<char*>(&framesCount), sizeof(framesCount));
  for (size_t frame_num = 0; frame_num < framesCount; frame_num++)
  {
    const auto &frame_description = trj.getFrameDescription(frame_num);

    int rows = frame_description.rows;
    out.write(reinterpret_cast<char*>(&rows),
              sizeof(rows));
    int cols = frame_description.cols;
    out.write(reinterpret_cast<char*>(&cols),
              sizeof(cols));
    int type = frame_description.type();
    out.write(reinterpret_cast<char*>(&type),
              sizeof(type));

    out.write(reinterpret_cast<char*>(frame_description.data),
              rows*cols*frame_description.elemSize());

    notifyProgressBar(frame_num + 1, framesCount);
  }
}
