#include "recyclerview.h"
#include "recyclermimedata.h"

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

    if (itemHeight <= 50) {
        itemHeight = 50;
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
    int row = (point.y() - viewport()->rect().top() + verticalScrollBar()->value() - itemMargin) / (itemHeight + itemMargin);

    if (!model()->hasIndex(row, 0))
        return QModelIndex();

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

// The rectangle of just the item, no margins
QRect RecyclerView::visualRect(const QModelIndex &index) const
{
    return QRect(sectionMargin, getItemPos(index.row()), viewport()->width() - sectionMargin, itemHeight);
}

// Includes margins around the items
QRect RecyclerView::sectionRect(int startRow, int endRow) const
{
    int numRows = (endRow - startRow + 1);
    int sectionHeight = numRows*itemHeight + (numRows + 1)*itemMargin;

    return QRect(sectionMargin, getItemPos(startRow) - itemMargin, viewport()->width() - sectionMargin, sectionHeight);
}

// Height of all items and margins
int RecyclerView::totalItemHeight() const
{
    return model()->rowCount() * itemHeight + (model()->rowCount()+1) * itemMargin;
}

// Gets the display y position (position accounting for scrolling)
int RecyclerView::getItemPos(int dataPos) const
{
    return viewport()->rect().top() - verticalScrollBar()->value() + itemHeight * dataPos + (dataPos+1)*itemMargin;
}

// dataPos is the row, y should be the display position of row
void RecyclerView::populateItem(int dataPos, int y)
{
    QModelIndex index = model()->index(dataPos, 0);
    Q_ASSERT(model()->checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    ViewHolder *vh = pool.getRecycledView();
    if (vh == nullptr)
        vh = adapter->createViewHolder();

    QWidget *view = vh->getItemView();
    view->setParent(viewport());

    Qt::ItemFlags flags = model()->flags(index);
    /*if (flags.testFlag(Qt::ItemIsSelectable)) {
        vh->onSelectionChanged(selectionModel()->isRowSelected(dataPos));
    } else {
        vh->onSelectionChanged(false);
    }*/
    view->setEnabled(flags.testFlag(Qt::ItemIsEnabled));

    adapter->bindViewHolder(vh, dataPos);

    view->resize(viewport()->width() - itemMargin*2 - sectionMargin, itemHeight);
    view->move(viewport()->rect().left() + itemMargin + sectionMargin, y);
    view->show();

    //qDebug() << "POPULATE " << dataPos << view->pos();
}

// TODO: refactor the following into one function that works both ways
void RecyclerView::populateItemsBelow(int startDataPos)
{
    int bottom = viewport()->rect().bottom();

    if (getItemPos(startDataPos) > bottom || startDataPos < 0)
        return;

    for (int i=startDataPos, numExtra=0; i<model()->rowCount(); i++) {
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
        it.value()->getItemView()->resize(viewport()->width() - itemMargin*2 - sectionMargin, itemHeight);
    }

    if (adapter->activeViewMap.isEmpty()) {
        int startDataPos = indexAt(QPoint(0,0)).row();
        populateItemsBelow(startDataPos);

        // Adjust our minWidth to that of an item if this is the first item we have created
        if (!adapter->activeViewMap.isEmpty()) {
            QWidget *widget = adapter->activeViewMap.first()->getItemView();
            if (widget->minimumSizeHint().isValid()) {
                setMinimumWidth(widget->minimumSizeHint().width() + itemMargin*2 + sectionMargin + verticalScrollBar()->sizeHint().width());
            } else {
                setMinimumWidth(widget->minimumWidth() + itemMargin*2 + sectionMargin + verticalScrollBar()->sizeHint().width());
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


    // Can drop here, parent class will trigger repaint
    if (event->isAccepted()) {
        int row = indexAt(event->pos()).row();
        const RecyclerMimeData *mimeData = qobject_cast<const RecyclerMimeData *>(event->mimeData());
        Q_ASSERT(mimeData != nullptr);
        int endRow = row + mimeData->rows - 1;
        dragPos = sectionRect(row, endRow);
    }

    // Can't drop here, repaint the area where the drop rectangle highlight was
    // Set dragPos to NULL
    else {
        QRect updateRect = dragPos;
        dragPos = QRect();
        viewport()->update(updateRect);
    }
}

void RecyclerView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex item = indexAt(event->pos());
    if (!item.isValid() || !item.flags().testFlag(Qt::ItemIsEnabled) || !item.flags().testFlag(Qt::ItemIsSelectable)) {
        selectionModel()->clear();
    }
    QAbstractItemView::mousePressEvent(event);
}

void RecyclerView::keyPressEvent(QKeyEvent *event)
{
    if (!event->matches(QKeySequence::SelectAll)) {
        QAbstractItemView::keyPressEvent(event);
        return;
    }

    QItemSelection selection;

    int startRow = -1, endRow = -1;
    for (int row=0; row<model()->rowCount(); row++) {
        QModelIndex item = model()->index(row, 0);
        if (item.flags() & Qt::ItemIsEnabled && item.flags() & Qt::ItemIsSelectable) {
            if (startRow == -1) {
                startRow = row;
                endRow = row;
            } else {
                if (row == endRow + 1) {
                    endRow = row;
                } else {
                    selection.merge(QItemSelection(model()->index(startRow, 0), model()->index(endRow, 0)), QItemSelectionModel::Select);
                    startRow = row;
                    endRow = row;
                }
            }
        }
    }

    if (startRow != -1 && endRow != -1) {
        selection.merge(QItemSelection(model()->index(startRow, 0), model()->index(endRow, 0)), QItemSelectionModel::Select);
    }
    selectionModel()->select(selection, QItemSelectionModel::Select);
}


QRect RecyclerView::marginHelper(const QRect &rect) const
{
    int drawShift = itemMargin / 2;
    return rect.marginsRemoved(QMargins(drawShift, drawShift, drawShift, drawShift));
}


void RecyclerView::paintEvent(QPaintEvent *event)
{
    //qDebug() << "PAINT BOUNDS:" << event->region().boundingRect();

    QPainter painter;
    painter.begin(viewport());
    painter.save();

    QPen pen;
    pen.setWidth(itemMargin);
    pen.setBrush(palette().highlight());
    pen.setJoinStyle(Qt::MiterJoin);
    painter.setPen(pen);

    // Selection border painting
    const QItemSelection selection = selectionModel()->selection();
    for (auto it=selection.begin(); it!=selection.end(); it++) {
        painter.drawRect(marginHelper(sectionRect(it->top(), it->bottom())));
    }

    // Drop-zone border painting
    if (state() == QAbstractItemView::DraggingState && !dragPos.isNull()) {
        QBrush brush = pen.brush();
        brush.setColor(brush.color().lighter());
        pen.setBrush(brush);
        painter.setPen(pen);
        painter.drawRect(marginHelper(dragPos));
    }

    painter.restore();
    painter.end();
}

void RecyclerView::resetActiveViews()
{
    // Recycle everything
    for (auto it=adapter->activeViewMap.begin(); it!=adapter->activeViewMap.end(); it++)
        pool.putRecycledView(it.value());
    adapter->activeViewMap.clear();

    int startDataPos = (verticalScrollBar()->value() - itemMargin) / (itemHeight + itemMargin);
    populateItemsBelow(startDataPos);
}

void RecyclerView::scrollContentsBy(int dx, int dy)
{
    // Special case for when we "jump" positions in the scrollbar
    if (qAbs(dy) > (adapter->activeViewMap.size() * itemHeight + (adapter->activeViewMap.size()+1)*itemMargin)) {
        resetActiveViews();
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
        populateItemsAbove(adapter->activeViewMap.firstKey() - 1);
    }
    else if ((adapter->activeViewMap.last()->getItemView()->rect().bottom() + itemMargin) < viewport()->rect().bottom()) {
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

void RecyclerView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags flags)
{
    Q_ASSERT(selectionModel() != nullptr);

    qDebug() << "SET SELECTION" << rect;

    QRect normRect = rect.normalized(); // we don't want negative width or height

    QModelIndex topLeft = indexAt(normRect.topLeft());
    QModelIndex bottomRight = indexAt(normRect.bottomRight());

    QItemSelection selection;

    int startRow = -1, endRow = -1;
    for (int row=topLeft.row(); row<=bottomRight.row(); row++) {
        QModelIndex item = model()->index(row, 0);
        if (item.flags() & Qt::ItemIsEnabled && item.flags() & Qt::ItemIsSelectable) {
            if (startRow == -1) {
                startRow = row;
                endRow = row;
            } else {
                if (row == endRow + 1) {
                    endRow = row;
                } else {
                    selection.merge(QItemSelection(model()->index(startRow, 0), model()->index(endRow, 0)), QItemSelectionModel::Select);
                    startRow = row;
                    endRow = row;
                }
            }
        }
    }

    if (startRow != -1 && endRow != -1) {
        selection.merge(QItemSelection(model()->index(startRow, 0), model()->index(endRow, 0)), QItemSelectionModel::Select);
    }

    selectionModel()->select(selection, flags);
}

QRegion	RecyclerView::visualRegionForSelection(const QItemSelection &selection) const
{    
    QRegion region;
    for (auto it=selection.begin(); it!=selection.end(); it++) {
        //qDebug() << "TOP INDEX" << it->top() << "BOTTOM INDEX" << it->bottom();
        region = region.united(sectionRect(it->top(), it->bottom()));
    }

    //qDebug() << "UPDATING" << region.boundingRect();
    return region;
}

void RecyclerView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{    
    QRegion region = visualRegionForSelection(selected);
    region = region.united(visualRegionForSelection(deselected));
    viewport()->update(region);
}

void RecyclerView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{

}

void RecyclerView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    //qDebug() << "ROWS INSERTED" << parent.row() << start << end;
    verticalScrollBar()->setRange(0, totalItemHeight() - viewport()->height());
    resetActiveViews();
}


