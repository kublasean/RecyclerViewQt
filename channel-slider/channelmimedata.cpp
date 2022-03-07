#include "channelmimedata.h"

ChannelMimeData::ChannelMimeData()
{

}

bool ChannelMimeData::hasFormat(const QString &format) const
{
    return format == ChannelMimeData::channelFormat();
}

