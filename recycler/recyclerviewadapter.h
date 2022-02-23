#ifndef RECYCLERVIEWADAPTER_H
#define RECYCLERVIEWADAPTER_H

#include "viewholder.h"

#include <QObject>
#include <QAbstractItemModel>
#include <QMap>

class RecyclerViewAdapter : public QObject
{
    Q_OBJECT
public:
    explicit RecyclerViewAdapter(QAbstractItemModel *model, QObject *parent = nullptr);

    virtual ViewHolder *createViewHolder() = 0;
    virtual void onBindViewHolder(ViewHolder *vh, int dataPos) = 0;
    virtual void onRecycleViewHolder(ViewHolder *vh) const {}

    QAbstractItemModel *model() { return dataModel; }

protected:    
    QAbstractItemModel *dataModel;
    ViewHolder *findViewHolder(int dataPos) const;

private:
    void bindViewHolder(ViewHolder *vh, int dataPos);
    ViewHolder *recycleViewHolder(ViewHolder *vh);


    QMap<int, ViewHolder *> activeViewMap;

    friend class RecyclerView;
};

#endif // RECYCLERVIEWADAPTER_H
