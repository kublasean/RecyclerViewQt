#ifndef CHANNELMIMEDATA_H
#define CHANNELMIMEDATA_H

#include <QMimeData>
#include <QPoint>
#include <QModelIndex>

class ChannelMimeData : public QMimeData
{
    Q_OBJECT
public:
    explicit ChannelMimeData();

    QPoint hotspot;
    QString channelName;
};

#endif // CHANNELMIMEDATA_H
