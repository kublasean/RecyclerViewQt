#ifndef CHANNELSLIDERVIEWHOLDER_H
#define CHANNELSLIDERVIEWHOLDER_H

#include "recycler/viewholder.h"
#include "customlistitem.h"

#include <QObject>

class ChannelSliderViewHolder : public ViewHolder
{
    Q_OBJECT
public:
    explicit ChannelSliderViewHolder(QObject *parent, CustomListItem *view);
signals:
    void valueChanged(int dataPos, int newValue);
    void dragStarted();

private slots:
    void onNewChannelValue(int value);
};

#endif // CHANNELSLIDERVIEWHOLDER_H
