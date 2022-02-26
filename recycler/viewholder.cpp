#include "viewholder.h"

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

ViewHolder::~ViewHolder()
{
    if (itemView != nullptr) {
        itemView->deleteLater();
    }

    //qDebug() << "NUM VH ALLOC: " << --numViewHolderAlloc;
}
