#include "recyclerviewadapter.h"

RecyclerViewAdapter::RecyclerViewAdapter(QAbstractItemModel *dataModel, QObject *parent)
    : QObject{parent}
{
    Q_ASSERT(dataModel != nullptr);
    this->dataModel = dataModel;
}

ViewHolder *RecyclerViewAdapter::recycleViewHolder(ViewHolder *vh)
{
    Q_ASSERT(vh != nullptr);

    onRecycleViewHolder(vh);
    vh->getItemView()->hide();
    return vh;
}

void RecyclerViewAdapter::bindViewHolder(ViewHolder *vh, int dataPos)
{
    Q_ASSERT(vh != nullptr);

    onBindViewHolder(vh, dataPos);
    activeViewMap.insert(dataPos, vh);
}

ViewHolder *RecyclerViewAdapter::findViewHolder(int dataPos) const
{
    return activeViewMap.value(dataPos, nullptr);
}
