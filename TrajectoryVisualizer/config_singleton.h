#ifndef CONFIG_SINGLETON_H
#define CONFIG_SINGLETON_H

#include <QString>
#include <QSettings>


class ConfigSingleton
{
public:
    static ConfigSingleton& getInstance()
    {
        static ConfigSingleton    instance; // Guaranteed to be destroyed.
                                            // Instantiated on first use.

        //settings.
        return instance;
    }

    ConfigSingleton(ConfigSingleton const&) = delete;
    void operator=(ConfigSingleton const&)  = delete;

    QString path_to_ini;

private:
    ConfigSingleton();
};

#endif // CONFIG_SINGLETON_H
