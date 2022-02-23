#include "recyclerview.h"

#include <QDebug>
#include <QScrollBar>

RecyclerView::RecyclerView(RecyclerViewAdapter *adapter, int itemHeight, QWidget *parent) :
    QAbstractScrollArea(parent)
{
    Q_ASSERT(adapter != nullptr);

    this->adapter = adapter;
    this->adapter->setParent(this);

    if (itemHeight <= 0) {
        itemHeight = 100;
    }
    this->itemHeight = itemHeight;

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    verticalScrollBar()->setSingleStep(itemHeight / 4);
    verticalScrollBar()->setRange(0, totalItemHeight() - viewport()->height());
    verticalScrollBar()->setPageStep(viewport()->height());
    pool.maxSize = viewport()->height() / itemHeight + numExtraActive;

    //qDebug() << "POOL MAX" << pool.maxSize;

    //qDebug() << "NUM ROWS: " << adapter->model()->rowCount();
}

int RecyclerView::totalItemHeight() const
{
    return adapter->model()->rowCount() * itemHeight;
}

int RecyclerView::getItemPos(int dataPos) const
{
    //int ret = viewport()->rect().top() - verticalScrollBar()->value() + itemHeight * dataPos;
    //qDebug() << "Get item pos " << dataPos << " ret: " << ret << " itemHeight: " << itemHeight << " top: " << viewport()->rect().top();
    return viewport()->rect().top() - verticalScrollBar()->value() + itemHeight * dataPos;
}

ViewHolder *RecyclerView::populateItem(int dataPos, int y)
{
    ViewHolder *vh = pool.getRecycledView();
    if (vh == nullptr)
        vh = adapter->createViewHolder();

    QWidget *view = vh->getItemView();
    view->setParent(viewport());
    adapter->bindViewHolder(vh, dataPos);

    view->resize(viewport()->width(), itemHeight);
    view->move(viewport()->rect().left(), y);
    view->show();

    qDebug() << "POPULATE " << dataPos << view->pos();
    return vh;
}

// TODO: refactor the following into one function that works both ways
void RecyclerView::populateItemsBelow(int startDataPos)
{
    int bottom = viewport()->rect().bottom();

    if (getItemPos(startDataPos) > bottom || startDataPos < 0)
        return;

    //qDebug() << "NUM ROWS: " << adapter->model()->rowCount() << "i: " << startDataPos;

    for (int i=startDataPos, numExtra=0; i<adapter->model()->rowCount(); i++) {
        int newY = getItemPos(i);

        if (newY > bottom) {
            if (numExtra == numExtraActive) {
                break;
            }
            numExtra++;
        }
        populateItem(i, newY);
    }
}
void RecyclerView::populateItemsAbove(int startDataPos)
{
    int top = viewport()->rect().top();
    if (getItemPos(startDataPos) < top || startDataPos < 0)
        return;

    for (int i=startDataPos, numExtra=0; i>=0; i--) {
        int newY = getItemPos(i);

        if (newY + itemHeight < top) {
            if (numExtra == numExtraActive) {
                break;
            }
            numExtra++;
        }
        populateItem(i, newY);
    }
}

void RecyclerView::resizeEvent(QResizeEvent *event)
{
    verticalScrollBar()->setPageStep(viewport()->height());
    verticalScrollBar()->setRange(0, totalItemHeight() - viewport()->height());
    pool.maxSize = viewport()->height() / itemHeight + numExtraActive;
    //qDebug() << "POOL MAX" << pool.maxSize;

    for (auto it=adapter->activeViewMap.begin(); it!=adapter->activeViewMap.end(); it++) {
        it.value()->getItemView()->resize(viewport()->width(), itemHeight);
    }

    if (adapter->activeViewMap.isEmpty()) {
        int startDataPos = verticalScrollBar()->value() / itemHeight;
        populateItemsBelow(startDataPos);
    }
    else if (adapter->activeViewMap.last()->getItemView()->rect().bottom() < viewport()->rect().bottom()) {
        populateItemsBelow(adapter->activeViewMap.lastKey() + 1);
    }
}

void RecyclerView::scrollContentsBy(int dx, int dy)
{
    int vvalue = verticalScrollBar()->value();

    // Special case for when we "jump" positions in the scrollbar
    if (qAbs(dy) > adapter->activeViewMap.size() * itemHeight) {
        qDebug() << "JUMP";

        // Recycle everything
        for (auto it=adapter->activeViewMap.begin(); it!=adapter->activeViewMap.end(); it++)
            pool.putRecycledView(it.value());
        adapter->activeViewMap.clear();

        int startDataPos = vvalue / itemHeight;
        populateItemsBelow(startDataPos);
        return;
    }

    QList<int> removeList;

    int numExtraAbove = 0, numExtraBelow = 0;
    for (auto it=adapter->activeViewMap.begin(); it!=adapter->activeViewMap.end(); it++) {
        QWidget *widget = it.value()->getItemView();

        widget->move(viewport()->x(), getItemPos(it.key()));

        if (widget->y() + widget->height() < viewport()->y()) {
            numExtraAbove++;
        }

        if (widget->y() > viewport()->rect().bottom()) {
            if (numExtraBelow >= numExtraActive)
                removeList << it.key();
            numExtraBelow++;
        }
    }

    //qDebug() << "extra above: " << numExtraAbove;
    for (int i=0; i<numExtraAbove - numExtraActive; i++) {
        removeList << (adapter->activeViewMap.begin() + i).key();
    }

    if (!removeList.isEmpty()) {
        for (int i=0; i<removeList.size(); i++) {
            int index = removeList[i];
            qDebug() << "recycle at dataPos: " << index;
        }
    }

    // Recycle widgets out of sight
    for (int i=0; i<removeList.size(); i++) {
        ViewHolder *vh = adapter->activeViewMap.take(removeList[i]);
        pool.putRecycledView(adapter->recycleViewHolder(vh));
    }

    // Add items to top/bottom if nec.
    if (adapter->activeViewMap.first()->getItemView()->y() > viewport()->rect().top()) {
        populateItemsAbove(adapter->activeViewMap.firstKey() - 1);
    }
    else if (adapter->activeViewMap.last()->getItemView()->rect().bottom() < viewport()->rect().bottom()) {
        populateItemsBelow(adapter->activeViewMap.lastKey() + 1);
    }
}
