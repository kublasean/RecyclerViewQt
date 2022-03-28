#include "channelslideradapter.h"
#include "customlistitem.h"
#include "channelsliderviewholder.h"
#include "channelitemmodel.h"

#include <QDebug>

ChannelSliderAdapter::ChannelSliderAdapter(QAbstractItemModel *model, QObject *parent)
    : RecyclerViewAdapter(model, parent)
{
    connect(dataModel, &QAbstractItemModel::dataChanged, this, &ChannelSliderAdapter::onDataChanged);
}

ViewHolder *ChannelSliderAdapter::createViewHolder()
{
    CustomListItem *view = new CustomListItem();
    return new ChannelSliderViewHolder(this, view);
}

void ChannelSliderAdapter::onRecycleViewHolder(ViewHolder *vh) const
{
    ChannelSliderViewHolder *customViewHolder = qobject_cast<ChannelSliderViewHolder *>(vh);
    Q_ASSERT(customViewHolder != nullptr);

    // Disconnect everything from valueChanged signal
    disconnect(customViewHolder, &ChannelSliderViewHolder::valueChanged, nullptr, nullptr);
}

void ChannelSliderAdapter::updateViewHolder(ChannelSliderViewHolder *vh, ChannelUserData userData)
{
    ChannelSliderViewHolder *channelViewHolder = qobject_cast<ChannelSliderViewHolder *>(vh);
    Q_ASSERT(channelViewHolder != nullptr);

    CustomListItem *view = qobject_cast<CustomListItem *>(vh->getItemView());
    Q_ASSERT(view != nullptr);

    view->setDisplayData(userData.channel, userData.name, userData.isHeader);
    view->setEnabled(userData.enabled);
    view->setValue(userData.value);
}

void ChannelSliderAdapter::onBindViewHolder(ViewHolder *vh, int dataPos)
{
    ChannelSliderViewHolder *channelViewHolder = qobject_cast<ChannelSliderViewHolder *>(vh);
    channelViewHolder->dataPos = dataPos;

    ChannelUserData userData = qvariant_cast<ChannelUserData>(dataModel->data(dataModel->index(dataPos, 0), Qt::UserRole));
    updateViewHolder(channelViewHolder, userData);

    connect(channelViewHolder, &ChannelSliderViewHolder::valueChanged, this, &ChannelSliderAdapter::newUserChannelValue);
}

// When the user interacts with the slider update the model data
void ChannelSliderAdapter::newUserChannelValue(int dataPos, int val)
{
    qDebug() << "NEW USER VAL" << dataPos << val;
    Q_ASSERT(dataModel->hasIndex(dataPos, 0));

    QModelIndex index = dataModel->index(dataPos, 0);
    dataModel->setData(index, val, Qt::EditRole);

    if (selectionModel == nullptr || !selectionModel->isRowSelected(dataPos))
        return;

    QModelIndexList list = selectionModel->selectedIndexes();
    for (auto it=list.begin(); it!=list.end(); it++) {
        if (it->row() == dataPos) {
            continue;
        }

        // Update view if visible
        ViewHolder *vh = findViewHolder(it->row());
        if (vh != nullptr) {
            CustomListItem *view = qobject_cast<CustomListItem *>(vh->getItemView());
            Q_ASSERT(view != nullptr);
            view->blockSignals(true);
            view->setValue(val);
            view->blockSignals(false);
        }

        dataModel->setData(dataModel->index(it->row(), 0), val, Qt::EditRole);
    }
}

// Update the slider when the model data changes
void ChannelSliderAdapter::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    for (int i=topLeft.row(); i<=bottomRight.row(); i++) {
        qDebug() << "DATA CHANGED" << i;

        ViewHolder *vh = findViewHolder(i);
        if (vh == nullptr)
            continue;

        if (roles.isEmpty() || roles.contains(Qt::UserRole)) {
            ChannelSliderViewHolder *channelVh = qobject_cast<ChannelSliderViewHolder *>(vh);
            Q_ASSERT(channelVh != nullptr);

            ChannelUserData userData = qvariant_cast<ChannelUserData>(dataModel->data(dataModel->index(i, 0), Qt::UserRole));
            updateViewHolder(channelVh, userData);
        } else if (roles.contains(Qt::EditRole)) {
            CustomListItem *view = qobject_cast<CustomListItem *>(vh->getItemView());
            view->setValue(dataModel->data(dataModel->index(i, 0), Qt::EditRole).toInt());
        }
    }
}

void ChannelSliderAdapter::setSelectionModel(QItemSelectionModel *model)
{
    selectionModel = model;
}


