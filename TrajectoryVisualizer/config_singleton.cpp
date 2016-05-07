#include "config_singleton.h"

#include <QString>
#include <QSettings>

using namespace std;

ConfigSingleton::ConfigSingleton()
{

}

void ConfigSingleton::loadIni(string path_to_ini)
{
    this->path_to_ini = path_to_ini;

    QSettings ini(QString::fromStdString(path_to_ini), QSettings::IniFormat);

    path_to_map_csv = ini.value("Map/path_to_img").toString().toStdString();
    map_m_per_px = ini.value("Map/meters_per_pixel").toDouble();

    path_to_trj1_csv = ini.value("Trajectory1/path_to_csv").toString().toStdString();
    path_to_trj2_csv = ini.value("Trajectory2/path_to_csv").toString().toStdString();

    common_m_per_px = ini.value("Common/meters_per_pixel").toDouble();
}
