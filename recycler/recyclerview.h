#ifndef RECYCLERVIEW_H
#define RECYCLERVIEW_H

#include "recyclerviewpool.h"
#include "recyclerviewadapter.h"

#include <QAbstractScrollArea>
#include <QVector>


class RecyclerView : public QAbstractScrollArea
{
public:
    explicit RecyclerView(RecyclerViewAdapter *adapter, int itemHeight = 100, QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void scrollContentsBy(int dx, int dy) override;

private:
    ViewHolder *populateItem(int dataPos, int y);
    void populateItemsBelow(int startDataPos);
    void populateItemsAbove(int startDataPos);

    int getItemPos(int dataPos) const;
    int totalItemHeight() const;


    RecyclerViewPool pool;
    RecyclerViewAdapter *adapter;

    const int numExtraActive = 3;
    int itemHeight;
};

#endif // RECYCLERVIEW_H
