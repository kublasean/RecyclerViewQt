#ifndef CHANNELSLIDERADAPTER_H
#define CHANNELSLIDERADAPTER_H

#include "recycler/recyclerviewadapter.h"

#include <QObject>

class ChannelSliderAdapter : public RecyclerViewAdapter
{
    Q_OBJECT
public:
    explicit ChannelSliderAdapter(QAbstractItemModel *model, QObject *parent = nullptr);

    ViewHolder *createViewHolder() override;
    void onBindViewHolder(ViewHolder *vh, int dataPos) override;
    void onRecycleViewHolder(ViewHolder *vh) const override;

private slots:
    void newUserChannelValue(int dataPos, int val);

    void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
};

#endif // CHANNELSLIDERADAPTER_H
