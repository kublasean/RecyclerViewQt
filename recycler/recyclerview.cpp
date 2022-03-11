#include "recyclerview.h"
#include "styledrecyclerlistitem.h"

#include <QDebug>
#include <QScrollBar>
#include <QDragMoveEvent>
#include <QPainter>

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
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DropOnly);
    setAutoScroll(true);

    setItemDelegate(nullptr);

    verticalScrollBar()->setSingleStep(itemHeight / 4);
    pool.maxSize = viewport()->height() / itemHeight + numExtraActive;

    viewport()->setAutoFillBackground(true);
    viewport()->setBackgroundRole(QPalette::Window);
}

QModelIndex RecyclerView::indexAt(const QPoint &point) const
{
    //return QModelIndex();
    //int row = (point.y() - viewport()->rect().top() + verticalScrollBar()->value()) / itemHeight;

    int row = (point.y() - viewport()->rect().top() + verticalScrollBar()->value() - itemMargin) / (itemHeight + itemMargin);

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

    verticalScrollBar()->setValue(viewport()->rect().top() - yPos + itemHeight * index.row() + (index.row()+1) * itemMargin);
}

QRect RecyclerView::visualRect(const QModelIndex &index) const
{
    qDebug() << "VISUAL RECT" << index.row();
    //return QRect();
    return QRect(0, getItemPos(index.row()), viewport()->width(), itemHeight);
}

QRect RecyclerView::sectionRect(int startRow, int endRow) const
{
    int numRows = (endRow - startRow + 1);
    int sectionHeight = numRows*itemHeight + (numRows + 1)*itemMargin;

    return QRect(0, getItemPos(startRow) - itemMargin, viewport()->width(), sectionHeight);
}


int RecyclerView::totalItemHeight() const
{
    return adapter->model()->rowCount() * itemHeight + (adapter->model()->rowCount()+1) * itemMargin;
}

int RecyclerView::getItemPos(int dataPos) const
{
    return viewport()->rect().top() - verticalScrollBar()->value() + itemHeight * dataPos + (dataPos+1)*itemMargin;
}

ViewHolder *RecyclerView::populateItem(int dataPos, int y)
{
    ViewHolder *vh = pool.getRecycledView();
    if (vh == nullptr)
        vh = adapter->createViewHolder();

    QWidget *view = vh->getItemView();
    view->setParent(viewport());

    QModelIndex index = model()->index(dataPos, 0);
    if (index.isValid()) {
        Qt::ItemFlags flags = model()->flags(index);
        if (flags.testFlag(Qt::ItemIsSelectable)) {
            vh->onSelectionChanged(selectionModel()->isRowSelected(dataPos));
        }
        //view->setAcceptDrops(flags.testFlag(Qt::ItemIsDropEnabled));
        view->setEnabled(flags.testFlag(Qt::ItemIsEnabled));
    } else {
        vh->onSelectionChanged(false);
        //view->setAcceptDrops(false);
        view->setEnabled(false);
    }

    adapter->bindViewHolder(vh, dataPos);

    view->resize(viewport()->width() - itemMargin*2, itemHeight);
    view->move(viewport()->rect().left() + itemMargin, y);
    view->show();

    qDebug() << "POPULATE " << dataPos << view->pos();
    return vh;
}

// TODO: refactor the following into one function that works both ways
void RecyclerView::populateItemsBelow(int startDataPos)
{
    qDebug() << "POPLATE BELOW" << startDataPos;
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
        it.value()->getItemView()->resize(viewport()->width() - itemMargin*2, itemHeight);
    }

    if (adapter->activeViewMap.isEmpty()) {
        int startDataPos = (verticalScrollBar()->value() - itemMargin) / (itemHeight + itemMargin);
        populateItemsBelow(startDataPos);

        // Adjust our minWidth to that of an item if this is the first item we have created
        if (!adapter->activeViewMap.isEmpty()) {
            QWidget *widget = adapter->activeViewMap.first()->getItemView();
            if (widget->minimumSizeHint().isValid()) {
                setMinimumWidth(widget->minimumSizeHint().width() + itemMargin*2 + verticalScrollBar()->sizeHint().width());
            } else {
                setMinimumWidth(widget->minimumWidth() + itemMargin*2 + verticalScrollBar()->sizeHint().width());
            }
        }

    }
    else if (adapter->activeViewMap.last()->getItemView()->rect().bottom() < viewport()->rect().bottom()) {
        populateItemsBelow(adapter->activeViewMap.lastKey() + 1);
    }
}

void RecyclerView::dragMoveEvent(QDragMoveEvent *event)
{
    QAbstractItemView::dragMoveEvent(event);


    int row = indexAt(event->pos()).row();
    ViewHolder *vh = adapter->findViewHolder(row);
    if (vh == nullptr)
        return;

    StyledRecyclerListItem *item = qobject_cast<StyledRecyclerListItem *>(vh->getItemView());
    if (item == nullptr)
        return;

    // TODO
}

void RecyclerView::paintEvent(QPaintEvent *event)
{
    qDebug() << "PAINT REGION:" << event->region().boundingRect();

    QPainter painter;
    painter.begin(viewport());
    painter.save();

    QPen pen;
    pen.setWidth(itemMargin);
    pen.setBrush(palette().highlight());
    pen.setJoinStyle(Qt::MiterJoin);
    painter.setPen(pen);

    const QItemSelection selection = selectionModel()->selection();
    for (auto it=selection.begin(); it!=selection.end(); it++) {
        /*QRegion section = event->region().intersected(sectionRect(it->top(), it->bottom()));
        for (auto rectIterator = section.begin(); rectIterator!=section.end(); rectIterator++) {
            //painter.fillRect(*rectIterator, palette().highlight());
            painter.drawRect(*rectIterator);

            qDebug() << *rectIterator;
        }*/
        int margin = itemMargin / 2;
        painter.drawRect(sectionRect(it->top(), it->bottom()).marginsRemoved(QMargins(margin, margin, margin, margin)));
    }

    painter.restore();
    painter.end();
}

void RecyclerView::scrollContentsBy(int dx, int dy)
{
    int vvalue = verticalScrollBar()->value();

    // Special case for when we "jump" positions in the scrollbar
    if (qAbs(dy) > (adapter->activeViewMap.size() * itemHeight + (adapter->activeViewMap.size()+1)*itemMargin)) {

        // Recycle everything
        for (auto it=adapter->activeViewMap.begin(); it!=adapter->activeViewMap.end(); it++)
            pool.putRecycledView(it.value());
        adapter->activeViewMap.clear();

        int startDataPos = (vvalue - itemMargin) / (itemHeight + itemMargin);
        populateItemsBelow(startDataPos);
        return;
    }

    viewport()->scroll(dx, dy);

    // Recycle views out of sight
    // TODO optimize based on scroll dir
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


    if (adapter->activeViewMap.first()->getItemView()->y() - itemMargin > viewport()->rect().top()) {
        //qDebug() << "POP ABOVE " << adapter->activeViewMap.firstKey() - 1;
        populateItemsAbove(adapter->activeViewMap.firstKey() - 1);
    }
    else if ((adapter->activeViewMap.last()->getItemView()->rect().bottom() + itemMargin) < viewport()->rect().bottom()) {
        populateItemsBelow(adapter->activeViewMap.lastKey() + 1);
    } else {
        qDebug() << "NO POP";
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
    qDebug() << "SELECTION:" << selection.count();

    QRegion region;
    for (auto it=selection.begin(); it!=selection.end(); it++) {
        qDebug() << "TOP INDEX" << it->top() << "BOTTOM INDEX" << it->bottom();
        region = region.united(sectionRect(it->top(), it->bottom()));
    }

    qDebug() << "UPDATING" << region.boundingRect();
    return region;
}

void RecyclerView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QRegion region = visualRegionForSelection(selected);
    region = region.united(visualRegionForSelection(deselected));

    qDebug() << "REGION UNITED:" << region.boundingRect();
    viewport()->update(region);

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


