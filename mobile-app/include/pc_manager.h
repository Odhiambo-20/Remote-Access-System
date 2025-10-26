#ifndef PC_MANAGER_H
#define PC_MANAGER_H

#include <QObject>
#include <QStringList>

class PCManager : public QObject {
    Q_OBJECT

public:
    explicit PCManager(QObject *parent = nullptr);
    
    Q_INVOKABLE void queryPCList();
    Q_INVOKABLE void refreshPCList();

signals:
    void pcListUpdated(const QStringList &pcs);
    void errorOccurred(const QString &error);

private:
    QStringList m_pcList;
};

#endif // PC_MANAGER_H
