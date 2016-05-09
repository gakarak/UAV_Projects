#include "config_singleton.h"

#include <iostream>
#include <fstream>

#include <QString>
#include <QSettings>

using namespace std;

ConfigSingleton::ConfigSingleton()
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

    if (!ini.contains("Map/path_to_img"))
    {
        throw Exception("ini file doesn't contain the key: Map/path_to_img");
    }

    if (!ini.contains("Map/meters_per_pixel"))
    {
        throw Exception("ini file doesn't contain the key: Map/meters_per_pixel");
    }

    if (!ini.contains("Trajectory1/path_to_csv"))
    {
        throw Exception("ini file doesn't contain the key: Trajectory1/path_to_csv");
    }

    if (!ini.contains("Trajectory2/path_to_csv"))
    {
        throw Exception("ini file doesn't contain the key: Trajectory2/path_to_csv");
    }

    if (!ini.contains("Common/quality_threshold"))
    {
        throw Exception("ini file doesn't contain the key: Common/quality_threshold");
    }

    path_to_map_csv = ini.value("Map/path_to_img").toString().toStdString();
    map_m_per_px = ini.value("Map/meters_per_pixel").toDouble();

    path_to_trj1_csv = ini.value("Trajectory1/path_to_csv").toString().toStdString();
    path_to_kp1_bin = ini.value("Trajectory1/path_to_key_points_bin", "").toString().toStdString();

    path_to_trj2_csv = ini.value("Trajectory2/path_to_csv").toString().toStdString();
    path_to_kp2_bin = ini.value("Trajectory2/path_to_key_points_bin", "").toString().toStdString();

    quality_threshold = ini.value("Common/quality_threshold").toDouble();

    if (ini.contains("Common/meters_per_pixel"))
    {
        common_m_per_px = ini.value("Common/meters_per_pixel").toDouble();
    }
    else
    {
        common_m_per_px = 2;
        clog << "Common meters per pixel missed in '" + path_to_ini + "'" << endl;
        clog << "Common meters per pixel default value " << common_m_per_px << endl;
        //throw Exception("ini file doesn't contain the key: Common/meters_per_pixel");
    }

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
