#include "fixturemimedata.h"

FixtureMimeData::FixtureMimeData()
{
    fixtureId = -1;
    sourceDataPos = -1;
}

bool FixtureMimeData::hasFormat(const QString &format) const
{
    if (FixtureMimeData::channelFormat() == format)
        return true;

    return RecyclerMimeData::hasFormat(format);
}

