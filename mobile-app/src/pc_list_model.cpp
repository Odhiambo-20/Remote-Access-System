#include "pc_list_model.h"

PCListModel::PCListModel(QObject *parent) : QAbstractListModel(parent) {
}

int PCListModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_pcList.count();
}

QVariant PCListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_pcList.count())
        return QVariant();

    const PCInfo &pc = m_pcList[index.row()];

    switch (role) {
    case PcIdRole:
        return pc.pcId;
    case HostnameRole:
        return pc.hostname;
    case UsernameRole:
        return pc.username;
    case IsOnlineRole:
        return pc.isOnline;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> PCListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[PcIdRole] = "pcId";
    roles[HostnameRole] = "hostname";
    roles[UsernameRole] = "username";
    roles[IsOnlineRole] = "isOnline";
    return roles;
}

void PCListModel::addPC(const QString &pcId, const QString &hostname, const QString &username, bool isOnline) {
    beginInsertRows(QModelIndex(), m_pcList.count(), m_pcList.count());
    PCInfo info;
    info.pcId = pcId;
    info.hostname = hostname;
    info.username = username;
    info.isOnline = isOnline;
    m_pcList.append(info);
    endInsertRows();
}

void PCListModel::clear() {
    beginResetModel();
    m_pcList.clear();
    endResetModel();
}

void PCListModel::updatePCStatus(const QString &pcId, bool isOnline) {
    for (int i = 0; i < m_pcList.count(); ++i) {
        if (m_pcList[i].pcId == pcId) {
            m_pcList[i].isOnline = isOnline;
            QModelIndex idx = index(i);
            emit dataChanged(idx, idx, {IsOnlineRole});
            break;
        }
    }
}
