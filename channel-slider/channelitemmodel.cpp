#include "channelitemmodel.h"
#include "channelmimedata.h"

ChannelItemModel::ChannelItemModel(QObject *parent)
    : QStandardItemModel{parent}
{

}

Qt::DropActions ChannelItemModel::supportedDropActions() const
{
    return Qt::DropAction::CopyAction;
}

bool ChannelItemModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return false;

    if (action != Qt::DropAction::CopyAction)
        return false;

    if (qobject_cast<ChannelMimeData *>(data) == nullptr)
        return false;

    return true;
}

bool ChannelItemModel::dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (!canDropMimeData(mimeData, action, row, column, parent))
        return false;

    ChannelMimeData *chMimeData = qobject_cast<ChannelMimeData *>(mimeData);
    if (chMimeData == nullptr)
        return false;

    QModelIndex id = index(row, column, parent);

    ChannelUserData userData;
    userData.enabled = true;
    userData.channel = row+1;
    userData.value = 0;
    userData.name = chMimeData->channelName;

    QVariant val;
    val.setValue(userData);

    setData(index(row, column, parent), val, Qt::UserRole);
}

bool ChannelItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::UserRole)
        return false;
    if (!value.canConvert<ChannelUserData>())
        return false;

    return QStandardItemModel::setData(index, value, role);
}
