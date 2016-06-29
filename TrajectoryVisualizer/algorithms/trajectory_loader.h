#ifndef TRAJECTORY_LOADER_H
#define TRAJECTORY_LOADER_H

#include <string>
#include <exception>

#include "model/entities/trajectory.h"

namespace algorithms
{

class TrajectoryLoader
{
 public:
  TrajectoryLoader();
  static modelpkg::Trajectory load(std::string trj_idx_path);

 private:
  static modelpkg::Map loadMapFromRow(std::vector<std::string> params);
};

}

#endif // TRAJECTORY_LOADER_H
