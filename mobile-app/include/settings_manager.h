#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H
#include <QObject>
#include <QString>
class SettingsManager : public QObject {
    Q_OBJECT
public:
    explicit SettingsManager(QObject *parent = nullptr);
    Q_INVOKABLE void saveSetting(const QString &key, const QString &value);
    Q_INVOKABLE QString loadSetting(const QString &key, const QString &def = "");
private:
};
#endif
