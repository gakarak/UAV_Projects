#ifndef CONFIG_SINGLETON_H
#define CONFIG_SINGLETON_H

#include <string>
#include <vector>
#include <exception>

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
        return instance;
    }

    void loadIni(std::string path_to_ini);

    std::string getPathToKeyPoints(int trj_num, std::string detector_name);
    std::string getPathToDescriptors(int trj_num, std::string detector_name, std::string descriptor_name);

    static std::string getPathToKeyPoints(std::string path_to_trj_csv,
                                          std::string detector_name);
    static std::string getPathToDescriptors(std::string path_to_trj_csv,
                                            std::string detector_name,
                                            std::string descriptor_name);

    //getters
    std::string getPathToIni()                  { return path_to_ini; }

    std::string getPathToMapCsv()               { return path_to_map_csv; }
    double      getMapMetersPerPixel()          { return map_m_per_px; }

    std::string getPathToFirstTrajectoryCsv()   { return path_to_trj1_csv; }
    std::string getPathToSecondTrajectoryCsv()  { return path_to_trj2_csv; }

    double      getGradientMetersPerPixel()     { return gradient_m_per_px; }

    double      getQualityThreshold()           { return quality_threshold; }
    //exceptions
    class Exception: public std::runtime_error
    {
    public:
        Exception(const std::string &what): std::runtime_error("ConfigSingleton: " + what)
        {}
    };
private:
    ConfigSingleton();

    std::string path_to_ini;

    std::string path_to_map_csv;
    double      map_m_per_px;

    std::string path_to_trj1_csv;
    std::string path_to_trj2_csv;

    double      gradient_m_per_px;
    double      quality_threshold;
};

#endif // CONFIG_SINGLETON_H
