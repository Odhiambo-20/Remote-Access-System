#ifndef PC_LIST_MODEL_H
#define PC_LIST_MODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QList>

struct PCInfo {
    QString pcId;
    QString pcName;
    QString ipAddress;
    QString status;
    QString osType;
    QDateTime lastSeen;
};

class PCListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum PCRoles {
        PcIdRole = Qt::UserRole + 1,
        PcNameRole,
        IpAddressRole,
        StatusRole,
        OsTypeRole,
        LastSeenRole
    };

    explicit PCListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addPC(const QString &pcId, const QString &pcName,
                           const QString &ipAddress, const QString &status,
                           const QString &osType, const QDateTime &lastSeen);
    Q_INVOKABLE void clearPCs();

private:
    QList<PCInfo> m_pcs;
};

#endif // PC_LIST_MODEL_H
