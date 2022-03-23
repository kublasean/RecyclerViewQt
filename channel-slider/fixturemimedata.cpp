#include "fixturemimedata.h"

FixtureMimeData::FixtureMimeData()
{

}

bool FixtureMimeData::hasFormat(const QString &format) const
{
    if (FixtureMimeData::channelFormat() == format)
        return true;

    return RecyclerMimeData::hasFormat(format);
}

