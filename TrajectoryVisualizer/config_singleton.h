#ifndef CONFIG_SINGLETON_H
#define CONFIG_SINGLETON_H

#include <string>

#include <QSettings>


class ConfigSingleton
{
public:
    ConfigSingleton(ConfigSingleton const&) = delete;
    void operator=(ConfigSingleton const&)  = delete;

    static ConfigSingleton& getInstance()
    {
        static ConfigSingleton    instance; // Guaranteed to be destroyed.
                                            // Instantiated on first use.

        //settings.
        return instance;
    }

    void loadIni(std::string path_to_ini);

    std::string getPathToIni()                  { return path_to_ini; }

    std::string getPathToMapCsv()               { return path_to_map_csv; }
    double      getMapMetersPerPixel()          { return map_m_per_px; }

    std::string getPathToFirstTrajectoryCsv()   { return path_to_trj1_csv; }
    std::string getPathToSecondTrajectoryCsv()  { return path_to_trj2_csv; }

    double      getCommonMetersPerPixel()       { return common_m_per_px; }
private:
    ConfigSingleton();

    std::string path_to_ini;

    std::string path_to_map_csv;
    double      map_m_per_px;

    std::string path_to_trj1_csv;
    std::string path_to_trj2_csv;

    double      common_m_per_px;
};

#endif // CONFIG_SINGLETON_H
