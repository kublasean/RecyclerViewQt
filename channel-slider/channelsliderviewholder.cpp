#include "channelsliderviewholder.h"
#include <QDebug>

ChannelSliderViewHolder::ChannelSliderViewHolder(QObject *parent, CustomListItem *view)
    : ViewHolder(parent, view)
{
    Q_ASSERT(view != nullptr);

    connect(view, &CustomListItem::valueChanged, this, &ChannelSliderViewHolder::onNewChannelValue);
    connect(view, &CustomListItem::dragStarted, this, &ChannelSliderViewHolder::dragStarted);
}

void ChannelSliderViewHolder::onNewChannelValue(int val)
{
    qDebug() << "NEW CHANNEL VALUE";
    emit valueChanged(dataPos, val);
}
