#ifndef FIXTUREMIMEDATA_H
#define FIXTUREMIMEDATA_H

#include "recycler/recyclermimedata.h"

#include <QPoint>
#include <QModelIndex>

struct FixtureChannel {
    QString name;

    FixtureChannel(QString name) { this->name = name; }
    FixtureChannel() { this->name = "INVALID"; }
};

struct Fixture {
    QString name;
    QVector<FixtureChannel> channels;
};

class FixtureMimeData : public RecyclerMimeData
{
    Q_OBJECT
public:
    explicit FixtureMimeData();

    bool hasFormat(const QString &format) const override;

    Fixture fixture;
    QPoint hotspot;
    int fixtureId;
    int sourceDataPos;

    static QString channelFormat() { return "application/channel-slider-data"; }
};

#endif // FIXTUREMIMEDATA_H
