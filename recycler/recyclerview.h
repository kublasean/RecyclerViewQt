#ifndef RECYCLERVIEW_H
#define RECYCLERVIEW_H

#include "recyclerviewpool.h"
#include "recyclerviewadapter.h"

#include <QAbstractItemView>
#include <QVector>


class RecyclerView : public QAbstractItemView
{
public:
    explicit RecyclerView(RecyclerViewAdapter *adapter, int itemHeight = 50, QWidget *parent = nullptr);

    QModelIndex indexAt(const QPoint &point) const override;
    void scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint = EnsureVisible) override;
    QRect visualRect(const QModelIndex &index) const override;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;

    void scrollContentsBy(int dx, int dy) override;

    int horizontalOffset() const override { return 0; }
    int verticalOffset() const override;
    bool isIndexHidden(const QModelIndex &index) const override { return false; }
    QModelIndex moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags flags) override;
    QRegion	visualRegionForSelection(const QItemSelection &selection) const override;
    bool edit(const QModelIndex &index, QAbstractItemView::EditTrigger trigger, QEvent *event) override { return false; }

protected slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>()) override {}

private:
    ViewHolder *populateItem(int dataPos, int y);
    void populateItemsBelow(int startDataPos);
    void populateItemsAbove(int startDataPos);

    int getItemPos(int dataPos) const;
    int totalItemHeight() const;


    RecyclerViewPool pool;
    RecyclerViewAdapter *adapter;

    const int numExtraActive = 0;
    int itemHeight;
};

#endif // RECYCLERVIEW_H
