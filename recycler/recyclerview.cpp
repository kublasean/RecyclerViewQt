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
    //verticalScrollBar()->setRange(0, totalItemHeight() - viewport()->height());
    //verticalScrollBar()->setPageStep(viewport()->height());
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
    if (getItemPos(startDataPos) + itemHeight < top || startDataPos < 0)
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

        // Adjust our minWidth to that of an item if this is the first item we have created
        if (!adapter->activeViewMap.isEmpty()) {
            QWidget *widget = adapter->activeViewMap.first()->getItemView();
            if (widget->minimumSizeHint().isValid()) {
                setMinimumWidth(widget->minimumSizeHint().width() + verticalScrollBar()->sizeHint().width());
            } else {
                setMinimumWidth(widget->minimumWidth() + verticalScrollBar()->sizeHint().width());
            }
        }

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

    viewport()->scroll(dx, dy);

    // Recycle views out of sight
    for (auto it=adapter->activeViewMap.begin(); it!=adapter->activeViewMap.end();) {
        if (!viewport()->rect().intersects(it.value()->getItemView()->geometry())) {
            //qDebug() << "ERASE " << it.key();
            pool.putRecycledView(adapter->recycleViewHolder(it.value()));
            it = adapter->activeViewMap.erase(it);
        } else {
            it++;
        }
    }

    if (adapter->activeViewMap.isEmpty())
        return;


    if (adapter->activeViewMap.first()->getItemView()->y() > viewport()->rect().top()) {
        //qDebug() << "POP ABOVE " << adapter->activeViewMap.firstKey() - 1;
        populateItemsAbove(adapter->activeViewMap.firstKey() - 1);
    }
    else if (adapter->activeViewMap.last()->getItemView()->rect().bottom() < viewport()->rect().bottom()) {
        populateItemsBelow(adapter->activeViewMap.lastKey() + 1);
    }
}
