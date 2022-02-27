#include "styledrecyclerlistitem.h"
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QPalette>

StyledRecyclerListItem::StyledRecyclerListItem(QWidget *parent)
    : QWidget{parent}
{
    setAutoFillBackground(true);

    QPalette pal = palette();
    pal.setColor(QPalette::Highlight, QColor("#cce4f7"));
    pal.setColor(QPalette::HighlightedText, pal.color(QPalette::Foreground));
    setPalette(pal);
}

/*
void StyledRecyclerListItem::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);
    setFrameShape(QFrame::Box);
    QPalette pal = palette();
    previousColor = pal.color(backgroundRole());
    pal.setColor(backgroundRole(), previousColor.darker(110));
    setPalette(pal);
}

void StyledRecyclerListItem::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    setFrameShape(QFrame::NoFrame);
    QPalette pal = palette();
    pal.setColor(backgroundRole(), previousColor);
    setPalette(pal);


}*/

void StyledRecyclerListItem::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    //qDebug() << "ITEM PAINT EVENT" << event->rect();
    //p.fillRect(event->rect(), Qt::red);
}

