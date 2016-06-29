#include "trajectory_loader.h"

#include "utils/csv.h"

using namespace algorithmspkg;
using namespace modelpkg;
using namespace std;

TrajectoryLoader::TrajectoryLoader()
{
}

Trajectory TrajectoryLoader::load(string trj_idx_path)
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
