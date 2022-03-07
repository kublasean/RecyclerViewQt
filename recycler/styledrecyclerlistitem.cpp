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
    isSelected = false;
    isDropSite = false;
    //QPalette pal = palette();
    //pal.setColor(QPalette::Highlight, QColor("#cce4f7"));
    //pal.setColor(QPalette::HighlightedText, pal.color(QPalette::Foreground));
    //setPalette(pal);
}

void StyledRecyclerListItem::setSelected(bool selected)
{
    isSelected = selected;
    update(selectionPaintRect());
}

void StyledRecyclerListItem::setIsDropSite(bool dropSite)
{
    isDropSite = dropSite;
    update(borderPaintRect());
}

QRect StyledRecyclerListItem::selectionPaintRect() const
{
    return QRect(0, 0, selectionLineWidth, height());
}

QRegion StyledRecyclerListItem::borderPaintRect() const
{
    QRegion region;

    region = region.united(QRect(0, 0, width(), borderLineWidth));
    region = region.united(QRect(0, height()-borderLineWidth, width(), borderLineWidth));
    region = region.united(QRect(0, 0, borderLineWidth, height()));
    region = region.united(QRect(width()-borderLineWidth, 0, borderLineWidth, height()));
    return region;
}



void StyledRecyclerListItem::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);

    update(borderPaintRect());

}

void StyledRecyclerListItem::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);

    update(borderPaintRect());

}

void StyledRecyclerListItem::paintEvent(QPaintEvent *event)
{
    /*QStyleOption opt;
    opt.init(this);*/
    QPainter p(this);

    if (underMouse() /*|| isDropSite*/) {
        QRegion region = borderPaintRect();
        for (auto it=region.begin(); it!=region.end(); it++) {
            p.fillRect(*it, palette().windowText());
        }
    }

    if (isSelected) {
        QRect selection = selectionPaintRect();
        if (event->region().intersects(selection)) {
            p.fillRect(event->region().intersected(selection).boundingRect(), palette().highlight());
        }
    }

    //style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}


