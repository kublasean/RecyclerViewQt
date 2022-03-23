#include "recyclermimedata.h"

RecyclerMimeData::RecyclerMimeData()
{
    rows = 1;
}

bool RecyclerMimeData::hasFormat(const QString &format) const
{
    if (format == RecyclerMimeData::recyclerFormat())
        return true;

    return QMimeData::hasFormat(format);
}
