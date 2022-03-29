#ifndef CHANNELSLIDERADAPTER_H
#define CHANNELSLIDERADAPTER_H

#include "recycler/recyclerviewadapter.h"
#include "channel-slider/channelitemmodel.h"
#include "channelsliderviewholder.h"

#include <QObject>
#include <QVector>
#include <QItemSelectionModel>

class ChannelSliderAdapter : public RecyclerViewAdapter
{
    Q_OBJECT
public:
    explicit ChannelSliderAdapter(QAbstractItemModel *model, QObject *parent = nullptr);

    ViewHolder *createViewHolder() override;
    void onBindViewHolder(ViewHolder *vh, int dataPos) override;
    void onRecycleViewHolder(ViewHolder *vh) const override;

    void setSelectionModel(QItemSelectionModel *selectionModel);

private:
    void updateViewHolder(ChannelSliderViewHolder *vh, ChannelUserData newUserData);

private slots:
    void newUserChannelValue(int dataPos, int val);

    void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void dragFixture();

private:
    QPointer<QItemSelectionModel> selectionModel;
};

#endif // CHANNELSLIDERADAPTER_H
