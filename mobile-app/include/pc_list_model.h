#ifndef PC_LIST_MODEL_H
#define PC_LIST_MODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QString>
#include <QDateTime>

struct PCInfo {
    QString id;
    QString name;
    QString ipAddress;
    QString status;
    QString osType;
    QString lastSeen;
};

class PCListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    
public:
    enum PCRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        IpAddressRole,
        StatusRole,
        OsTypeRole,
        LastSeenRole
    };
    
    explicit PCListModel(QObject *parent = nullptr);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    int count() const { return m_pcList.count(); }
    
    Q_INVOKABLE void addPC(const QString &id, const QString &name, 
                           const QString &ipAddress, const QString &status,
                           const QString &osType);
    Q_INVOKABLE void updatePC(const QString &id, const QString &status);
    Q_INVOKABLE void removePC(const QString &id);
    Q_INVOKABLE void clear();
    Q_INVOKABLE QVariantMap get(int index) const;
    
signals:
    void countChanged();
    
private:
    QList<PCInfo> m_pcList;
    int findPCIndex(const QString &id) const;
};

#endif // PC_LIST_MODEL_H
