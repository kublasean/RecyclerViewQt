#include "channelitemmodel.h"
#include "fixturemimedata.h"

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
    return { FixtureMimeData::channelFormat() };
}

bool ChannelItemModel::canDropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    if (!checkIndex(parent, CheckIndexOption::IndexIsValid)) {
        return false;
    }

    if (action != Qt::DropAction::CopyAction)
        return false;

    const FixtureMimeData *fixtureMimeData = qobject_cast<const FixtureMimeData *>(mimeData);
    if (fixtureMimeData == nullptr)
        return false;

    int endRow = parent.row() + fixtureMimeData->fixture.channels.count();

    for (int i=parent.row(); i<endRow; i++) {
        if (i >= rowCount(QModelIndex()) || channels[i].enabled) {
            return false;
        }
    }

    return true;
}

bool ChannelItemModel::dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (!canDropMimeData(mimeData, action, row, column, parent))
        return false;

    const FixtureMimeData *fixtureMimeData = qobject_cast<const FixtureMimeData *>(mimeData);
    Q_ASSERT(fixtureMimeData != nullptr);

    for (int i=0; i<fixtureMimeData->fixture.channels.count(); i++) {
        int row = parent.row() + i;
        QModelIndex channelIndex = index(row);
        ChannelUserData userData;
        userData.enabled = true;
        userData.channel = row+1;
        userData.value = 0;
        userData.name = fixtureMimeData->fixture.channels[i].name;

        QVariant val;
        val.setValue(userData);

        setData(channelIndex, val, Qt::UserRole);
    }
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

QVariant ChannelItemModel::headerData(int row, Qt::Orientation orientation, int role) const
{
    return channels[row].fixtureId;
}

