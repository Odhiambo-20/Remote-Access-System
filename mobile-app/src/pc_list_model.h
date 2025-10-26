#ifndef PC_LIST_MODEL_H
#define PC_LIST_MODEL_H

#include <QAbstractListModel>
#include <QList>

struct PCInfo {
    QString pcId;
    QString hostname;
    QString username;
    bool isOnline;
};

class PCListModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum PCRoles {
        PcIdRole = Qt::UserRole + 1,
        HostnameRole,
        UsernameRole,
        IsOnlineRole
    };

    explicit PCListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addPC(const QString &pcId, const QString &hostname, const QString &username, bool isOnline);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void updatePCStatus(const QString &pcId, bool isOnline);

private:
    QList<PCInfo> m_pcList;
};

#endif
