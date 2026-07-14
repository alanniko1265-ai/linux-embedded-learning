#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H
#include <QString>
struct AppSettings{
    QString tcpIp;
    QString tcpPort;
    QString serialBaudRate;
};
class SettingsManager{
public:
    static AppSettings load();
    static void save(const AppSettings &settings);
};

#endif // SETTINGSMANAGER_H
