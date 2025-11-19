#include "pc_list_model.h"

PCListModel::PCListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int PCListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_pcs.size();
}

QVariant PCListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_pcs.size())
        return QVariant();

    const PCInfo &pc = m_pcs[index.row()];
    switch (role) {
    case PcIdRole:
        return pc.pcId;
    case PcNameRole:
        return pc.pcName;
    case IpAddressRole:
        return pc.ipAddress;
    case StatusRole:
        return pc.status;
    case OsTypeRole:
        return pc.osType;
    case LastSeenRole:
        return pc.lastSeen;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> PCListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PcIdRole] = "pcId";
    roles[PcNameRole] = "pcName";
    roles[IpAddressRole] = "ipAddress";
    roles[StatusRole] = "status";
    roles[OsTypeRole] = "osType";
    roles[LastSeenRole] = "lastSeen";
    return roles;
}

void PCListModel::addPC(const QString &pcId, const QString &pcName,
                        const QString &ipAddress, const QString &status,
                        const QString &osType, const QDateTime &lastSeen)
{
    beginInsertRows(QModelIndex(), m_pcs.size(), m_pcs.size());
    m_pcs.append({pcId, pcName, ipAddress, status, osType, lastSeen});
    endInsertRows();
}

void PCListModel::clearPCs()
{
    beginResetModel();
    m_pcs.clear();
    endResetModel();
}
