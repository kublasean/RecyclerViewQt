#ifndef RECYCLERVIEWPOOL_H
#define RECYCLERVIEWPOOL_H

#include "viewholder.h"

class RecyclerViewPool
{
public:
    RecyclerViewPool(int max = 10);
    ~RecyclerViewPool();

    void clear();
    ViewHolder *getRecycledView();
    void putRecycledView(ViewHolder *scrap);

    int maxSize;

private:
    QVector<ViewHolder *> recycledList;
};

#endif // RECYCLERVIEWPOOL_H
