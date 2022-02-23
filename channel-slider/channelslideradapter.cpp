#include "channelslideradapter.h"
#include "customlistitem.h"
#include "channelsliderviewholder.h"

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

    connect(channelViewHolder, &ChannelSliderViewHolder::valueChanged, this, &ChannelSliderAdapter::newUserChannelValue);

    QModelIndex index = dataModel->index(dataPos, 0);
    view->setChannel(dataPos+1, dataModel->data(index).toString());
    view->setValue(dataModel->data(index, Qt::UserRole).toInt());
    vh->dataPos = dataPos;
}

// When the user interacts with the slider update the model data
void ChannelSliderAdapter::newUserChannelValue(int dataPos, int val)
{
    Q_ASSERT(dataModel->hasIndex(dataPos, 0));

    QModelIndex index = dataModel->index(dataPos, 0);
    dataModel->setData(index, val, Qt::UserRole);
}

// Update the slider when the model data changes
// TODO: do all rows, not just the first
void ChannelSliderAdapter::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    ViewHolder *vh = findViewHolder(topLeft.row());
    if (vh == nullptr)
        return;

    CustomListItem *view = qobject_cast<CustomListItem *>(vh->getItemView());
    Q_CHECK_PTR(view != nullptr);
    view->setValue(dataModel->data(topLeft, Qt::UserRole).toInt());
}


