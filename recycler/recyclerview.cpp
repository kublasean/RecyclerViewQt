#include "recyclerview.h"

#include <QDebug>
#include <QScrollBar>

RecyclerView::RecyclerView(RecyclerViewAdapter *adapter, int itemHeight, QWidget *parent) :
    QAbstractItemView(parent)
{
    Q_ASSERT(adapter != nullptr);

    this->adapter = adapter;
    this->adapter->setParent(this);
    setModel(this->adapter->model());

    if (itemHeight <= 0) {
        itemHeight = 100;
    }
    this->itemHeight = itemHeight;

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    setItemDelegate(nullptr);

    verticalScrollBar()->setSingleStep(itemHeight / 4);
    pool.maxSize = viewport()->height() / itemHeight + numExtraActive;    
}

QModelIndex RecyclerView::indexAt(const QPoint &point) const
{
    //return QModelIndex();
    int row = (point.y() - viewport()->rect().top() + verticalScrollBar()->value()) / itemHeight;
    if (!model()->hasIndex(row, 0))
        return QModelIndex();

    qDebug() << "indexAt" << point.y() << "is" << row;
    return model()->index(row, 0);
}

void RecyclerView::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
{
    int yPos;

    switch (hint) {
    case ScrollHint::EnsureVisible:
        if (getItemPos(index.row()) < 0 || getItemPos(index.row()) > viewport()->rect().bottom()) {
            yPos = 0;
        } else {
            return;
        }
        break;
    case ScrollHint::PositionAtTop:
        yPos = 0;
        break;
    case ScrollHint::PositionAtCenter:
        yPos = viewport()->height() / 2;
        break;
    case ScrollHint::PositionAtBottom:
        yPos = viewport()->height() - itemHeight;
        break;
    }

    verticalScrollBar()->setValue(viewport()->rect().top() - yPos + itemHeight * index.row());
}

QRect RecyclerView::visualRect(const QModelIndex &index) const
{
    qDebug() << "VISUAL RECT" << index.row();
    //return QRect();
    return QRect(0, getItemPos(index.row()), width(), itemHeight);
}


int RecyclerView::totalItemHeight() const
{
    return adapter->model()->rowCount() * itemHeight;
}

int RecyclerView::getItemPos(int dataPos) const
{
    return viewport()->rect().top() - verticalScrollBar()->value() + itemHeight * dataPos;
}

ViewHolder *RecyclerView::populateItem(int dataPos, int y)
{
    ViewHolder *vh = pool.getRecycledView();
    if (vh == nullptr)
        vh = adapter->createViewHolder();

    QWidget *view = vh->getItemView();
    view->setParent(viewport());

    vh->onSelectionChanged(selectionModel()->isRowSelected(dataPos));

    if (dataPos % 2 == 0) {
        view->setBackgroundRole(QPalette::Base);
    } else {
        view->setBackgroundRole(QPalette::AlternateBase);
    }

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

//
// Re-implemented protected functions
//
void RecyclerView::resizeEvent(QResizeEvent *event)
{
    verticalScrollBar()->setPageStep(viewport()->height());
    verticalScrollBar()->setRange(0, totalItemHeight() - viewport()->height());
    pool.maxSize = viewport()->height() / itemHeight + numExtraActive;

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

int RecyclerView::verticalOffset() const
{
    return verticalScrollBar()->value();
}

// TODO
QModelIndex RecyclerView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    return QModelIndex();
}

// TODO
void RecyclerView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags flags)
{
    Q_ASSERT(selectionModel() != nullptr);

    QModelIndex topLeft = indexAt(rect.topLeft());
    QModelIndex bottomRight = indexAt(rect.bottomRight());

    if (topLeft.isValid() && bottomRight.isValid()) {
        qDebug() << "SELECTING";
        selectionModel()->select(QItemSelection(topLeft, bottomRight), flags);
    } else {
        qDebug() << "SELECTION INVALID" << rect;
        selectionModel()->select(QModelIndex(), flags);
    }
}

// TODO
QRegion	RecyclerView::visualRegionForSelection(const QItemSelection &selection) const
{
    /*QRegion region;
    for (auto it=selection.begin(); it!=selection.end(); it++) {
        QRect sectionRect(0, getItemPos(it->top()), width(), getItemPos(it->bottom()) + itemHeight);
        region = region.united(sectionRect);
    }
    return region;*/
    return QRegion();
}

void RecyclerView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QList<QModelIndex> list = selected.indexes();
    for (auto it=list.begin(); it!=list.end(); it++) {
        ViewHolder *vh = adapter->findViewHolder(it->row());
        if (vh == nullptr)
            continue;
        vh->onSelectionChanged(true);
    }

    list = deselected.indexes();
    for (auto it=list.begin(); it!=list.end(); it++) {
        ViewHolder *vh = adapter->findViewHolder(it->row());
        if (vh == nullptr)
            continue;

        vh->onSelectionChanged(false);
    }
}


