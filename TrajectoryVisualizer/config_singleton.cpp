#include "config_singleton.h"

#include <iostream>
#include <fstream>

#include <QString>
#include <QSettings>

using namespace std;

ConfigSingleton::ConfigSingleton()
  : gradient_m_per_px(4), quality_threshold(60),
    path_to_map_csv(""), path_to_trj1_csv(""), path_to_trj2_csv("")
{
}

void ConfigSingleton::loadIni(string path_to_ini)
{
    this->path_to_ini = path_to_ini;

    if ( !ifstream(path_to_ini) )
    {
        throw Exception("file '" + path_to_ini + "' cannot be opened");
    }

    QSettings ini(QString::fromStdString(path_to_ini), QSettings::IniFormat);

    if (ini.contains("Map/path_to_img"))
    {
        path_to_map_csv = ini.value("Map/path_to_img").toString().toStdString();
        if (ini.contains("Map/meters_per_pixel"))
        {
          map_m_per_px = ini.value("Map/meters_per_pixel").toDouble();
        }
        else
        {
          throw Exception("ini file doesn't contain the key: Map/meters_per_pixel");
        }
    }

    if (ini.contains("Trajectory1/path_to_csv"))
    {
        path_to_trj1_csv = ini.value("Trajectory1/path_to_csv").toString().toStdString();
    }

    if (ini.contains("Trajectory2/path_to_csv"))
    {
        path_to_trj2_csv = ini.value("Trajectory2/path_to_csv").toString().toStdString();
    }

    if (ini.contains("Common/quality_threshold"))
    {
        quality_threshold = ini.value("Common/quality_threshold").toDouble();
    }
    else
    {
      quality_threshold = 60;
      clog << "Quality threshold missed in '" + path_to_ini + "'" << endl;
      clog << "Quality threshold default value " << quality_threshold << endl;
    }

    quality_threshold = ini.value("Common/quality_threshold").toDouble();

    if (ini.contains("Common/gradient_meters_per_pixel"))
    {
        gradient_m_per_px = ini.value("Common/gradient_meters_per_pixel").toDouble();
    }
    else
    {
        gradient_m_per_px = 4;
        clog << "Common meters per pixel for gradient missed in '" + path_to_ini + "'" << endl;
        clog << "Common meters per pixel for gradient default value " << gradient_m_per_px << endl;
        //throw Exception("ini file doesn't contain the key: Common/gradient_meters_per_pixel");
    }

}

string ConfigSingleton::getPathToKeyPoints(int trj_num, string detector_name)
{
    string path_to_trj = trj_num == 0? path_to_trj1_csv: path_to_trj2_csv;
    return path_to_trj + "_" + detector_name + "_key_points.bin";
}

string ConfigSingleton::getPathToDescriptors(int trj_num, string detector_name, string descriptor_name)
{
    string path_to_trj = trj_num == 0? path_to_trj1_csv: path_to_trj2_csv;
    return path_to_trj + "_" + detector_name + "_" + descriptor_name + "_descriptors.bin";
}

string ConfigSingleton::getPathToKDTree(int trj_num , string detector_name, string descriptor_name)
{
    string path_to_trj = trj_num == 0? path_to_trj1_csv: path_to_trj2_csv;
    return path_to_trj + "_" + detector_name + "_" + descriptor_name + "_KDTree.bin";
}

string ConfigSingleton::getPathToKeyPoints(string path_to_trj_csv,
                                           string detector_name)
{
  return path_to_trj_csv + "_" + detector_name + "_key_points.bin";
}

string ConfigSingleton::getPathToDescriptors(string path_to_trj_csv,
                                             string detector_name,
                                             string descriptor_name)
{
  return path_to_trj_csv + "_" + detector_name + "_" + descriptor_name + "_descriptors.xml";
}

void ConfigSingleton::setPathToTrajectoryCsv(int trj_num, string path)
{
  if (trj_num == 0)
  {
    path_to_trj1_csv = path;
  }
  else if (trj_num == 1)
  {
    path_to_trj2_csv = path;
  }
}

string ConfigSingleton::getPathToTrajectoryCsv(int trj_num)
{
  if (trj_num == 0)
  {
    return path_to_trj1_csv;
  }
  else if (trj_num == 1)
  {
    return path_to_trj2_csv;
  }
  else
  {
    return "";
  }
}
