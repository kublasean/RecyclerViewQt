#include "viewholder.h"
#include "styledrecyclerlistitem.h"

#include <QDebug>

static int numViewHolderAlloc = 0;

ViewHolder::ViewHolder(QObject *parent, QWidget *view)
    : QObject(parent)
{
    Q_ASSERT(view != nullptr);
    itemView = view;
    dataPos = -1;

    //qDebug() << "NUM VH ALLOC: " << ++numViewHolderAlloc;
}

void ViewHolder::onSelectionChanged(bool isSelected)
{
    selected = isSelected;

    StyledRecyclerListItem *item = qobject_cast<StyledRecyclerListItem *>(itemView);
    if (item != nullptr) {
        item->setSelected(isSelected);
    }
}

ViewHolder::~ViewHolder()
{
    if (itemView != nullptr) {
        itemView->deleteLater();
    }

    //qDebug() << "NUM VH ALLOC: " << --numViewHolderAlloc;
}
