#ifndef RECYCLERMIMEDATA_H
#define RECYCLERMIMEDATA_H

#include <QMimeData>

class RecyclerMimeData : public QMimeData
{
    Q_OBJECT
public:
    RecyclerMimeData();

    bool hasFormat(const QString &format) const override;
    static QString recyclerFormat() { return "application/recycler-data"; }

    int rows;
};

#endif // RECYCLERMIMEDATA_H
