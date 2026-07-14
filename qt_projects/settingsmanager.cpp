#include "settingsmanager.h"
#include <QSettings>
AppSettings SettingsManager::load(){
    QSettings settings("anllenge","DeviceTool");
    AppSettings appSettings;
    appSettings.tcpIp=settings.value("tcp/ip","127.0.0.1").toString();
    appSettings.tcpPort=settings.value("tcp/port","8888").toString();
    appSettings.serialBaudRate=settings.value("serial/baudRate","115200").toString();
    return appSettings;
}
void SettingsManager::save(const AppSettings &appSettings){
    QSettings settings("anllenge","DeviceTool");
    settings.setValue("tcp/ip",appSettings.tcpIp);
    settings.setValue("tcp/port",appSettings.tcpPort);
    settings.setValue("serial/baudRate",appSettings.serialBaudRate);
}