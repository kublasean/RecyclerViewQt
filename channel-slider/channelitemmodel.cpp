#include "channelitemmodel.h"
#include "channelmimedata.h"

#include <QDebug>

ChannelItemModel::ChannelItemModel(QObject *parent)
    : QAbstractListModel{parent}
{
    for (int i=0; i<512; i++) {
        ChannelUserData userData;
        userData.channel = i;
        userData.enabled = false;
        userData.name = "test";
        userData.value = 0;

        channels[i] = userData;
    }
}

Qt::ItemFlags ChannelItemModel::flags(const QModelIndex &index) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid)) {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags itemFlags = QAbstractListModel::flags(index) | Qt::ItemIsDropEnabled;

    ChannelUserData userData = qvariant_cast<ChannelUserData>(data(index, Qt::UserRole));

    if (!userData.enabled) {
        itemFlags &= ~Qt::ItemIsEnabled;
        itemFlags &= ~Qt::ItemIsSelectable;
    }

    return itemFlags;
}

Qt::DropActions ChannelItemModel::supportedDropActions() const
{
    return Qt::DropAction::CopyAction;
}

QStringList ChannelItemModel::mimeTypes() const
{
    return { ChannelMimeData::channelFormat() };
}

bool ChannelItemModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    if (!checkIndex(parent, CheckIndexOption::IndexIsValid)) {
        return false;
    }

    if (action != Qt::DropAction::CopyAction)
        return false;

    if (qobject_cast<const ChannelMimeData *>(data) == nullptr)
        return false;

    return true;
}

bool ChannelItemModel::dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (!canDropMimeData(mimeData, action, row, column, parent))
        return false;

    const ChannelMimeData *chMimeData = qobject_cast<const ChannelMimeData *>(mimeData);
    if (chMimeData == nullptr)
        return false;

    ChannelUserData userData;
    userData.enabled = true;
    userData.channel = row+1;
    userData.value = 0;
    userData.name = chMimeData->channelName;

    QVariant val;
    val.setValue(userData);

    setData(parent, val, Qt::UserRole);
    return true;
}

bool ChannelItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{   
    if (!checkIndex(index, CheckIndexOption::IndexIsValid)) {
        return false;
    }

    int channel = channels[index.row()].channel;

    switch (role) {
    case Qt::UserRole:
        if (!value.canConvert<ChannelUserData>())
            return false;
        channels[index.row()] = qvariant_cast<ChannelUserData>(value);
        channels[index.row()].channel = channel;
        break;
    case Qt::EditRole:
        channels[index.row()].value = value.toInt();
        break;
    default:
        return false;
    }

    emit dataChanged(index, index, {role});
    return true;
}

QVariant ChannelItemModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid)) {
        return QVariant();
    }

    QVariant ret;

    switch (role) {
    case Qt::UserRole:
        ret.setValue(channels[index.row()]);
        break;
    case Qt::DisplayRole:
        ret = channels[index.row()].name;
        break;
    case Qt::EditRole:
        ret = channels[index.row()].value;
        break;
    default:
        break;
    }

    return ret;
}
