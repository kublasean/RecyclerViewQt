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

    bool hasFormat(const QString &format) const override;

    QPoint hotspot;
    QString channelName;

    static QString channelFormat() { return "application/channel-slider-data"; }
};

#endif // CHANNELMIMEDATA_H
