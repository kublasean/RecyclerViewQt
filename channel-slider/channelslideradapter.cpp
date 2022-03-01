#include "channelslideradapter.h"
#include "customlistitem.h"
#include "channelsliderviewholder.h"

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

void ChannelSliderAdapter::onBindViewHolder(ViewHolder *vh, int dataPos)
{
    Q_ASSERT(dataModel->hasIndex(dataPos, 0));

    ChannelSliderViewHolder *channelViewHolder = qobject_cast<ChannelSliderViewHolder *>(vh);
    Q_ASSERT(channelViewHolder != nullptr);

    CustomListItem *view = qobject_cast<CustomListItem *>(vh->getItemView());
    Q_ASSERT(view != nullptr);

    QModelIndex index = dataModel->index(dataPos, 0);
    view->setChannel(dataPos+1, dataModel->data(index).toString());
    view->setValue(dataModel->data(index, Qt::UserRole).toInt());
    vh->dataPos = dataPos;
    connect(channelViewHolder, &ChannelSliderViewHolder::valueChanged, this, &ChannelSliderAdapter::newUserChannelValue);
}

// When the user interacts with the slider update the model data
void ChannelSliderAdapter::newUserChannelValue(int dataPos, int val)
{
    qDebug() << "NEW USER VAL" << dataPos << val;
    Q_ASSERT(dataModel->hasIndex(dataPos, 0));

    QModelIndex index = dataModel->index(dataPos, 0);
    dataModel->setData(index, val, Qt::UserRole);

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

        dataModel->setData(dataModel->index(it->row(), 0), val, Qt::UserRole);
    }
}

// Update the slider when the model data changes
// TODO: do all rows, not just the first
void ChannelSliderAdapter::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    for (int i=topLeft.row(); i<=bottomRight.row(); i++) {
        qDebug() << "DATA CHANGED" << i;

        ViewHolder *vh = findViewHolder(i);
        if (vh == nullptr)
            return;

        CustomListItem *view = qobject_cast<CustomListItem *>(vh->getItemView());
        Q_ASSERT(view != nullptr);
        view->setValue(dataModel->data(dataModel->index(i, 0), Qt::UserRole).toInt());
    }
}

void ChannelSliderAdapter::setSelectionModel(QItemSelectionModel *model)
{
    selectionModel = model;
}


