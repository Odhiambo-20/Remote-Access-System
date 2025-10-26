#include "settings_manager.h"
SettingsManager::SettingsManager(QObject *parent) : QObject(parent) {}
void SettingsManager::saveSetting(const QString &key, const QString &value) {}
QString SettingsManager::loadSetting(const QString &key, const QString &def) { return def; }
