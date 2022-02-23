#include "recyclerviewpool.h"
#include <QDebug>

RecyclerViewPool::RecyclerViewPool(int max)
{
    maxSize = max;
}

RecyclerViewPool::~RecyclerViewPool()
{
    clear();
}

void RecyclerViewPool::clear()
{
    for (int i=0; i<recycledList.size(); i++) {
        recycledList[i]->deleteLater();
        recycledList[i] = nullptr;
    }

    recycledList.clear();
}

ViewHolder *RecyclerViewPool::getRecycledView()
{
    qDebug() << "GET VIEW, POOL SIZE: " << recycledList.size()-1;
    return recycledList.isEmpty() ? nullptr : recycledList.takeLast();
}

void RecyclerViewPool::putRecycledView(ViewHolder *scrap)
{
    Q_ASSERT(scrap != nullptr);

    if (recycledList.size() == maxSize) {
        delete scrap;
        return;
    }
    recycledList.push_back(scrap);
    qDebug() << "PUT VIEW, POOL SIZE: " << recycledList.size();
}
