#include "draglabelwidget.h"
#include "channel-slider/channelmimedata.h"

#include <QMouseEvent>
#include <QMimeData>
#include <QByteArray>
#include <QWindow>
#include <QDrag>
#include <QDebug>
#include <QApplication>

DragLabelWidget::DragLabelWidget(QWidget *parent)
    : QLabel(parent)
{
    setAutoFillBackground(true);
    setFrameShadow(QFrame::Raised);
    setFrameShape(QFrame::Panel);

    setText("Red Dimmer");
}

void DragLabelWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        dragPos = event->pos();
}

void DragLabelWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (dragPos.isNull() || !(event->buttons() & Qt::LeftButton))
        return;

    if ((event->pos() - dragPos).manhattanLength() < QApplication::startDragDistance())
        return;

    qDebug() << "EVENT POS" << event->pos() << "WIDGET POS" << pos();

    ChannelMimeData *data = new ChannelMimeData();
    data->channelName = text();
    data->hotspot = event->pos();

    qreal dpr = window()->windowHandle()->devicePixelRatio();
    QPixmap pixmap(size() * dpr);
    pixmap.setDevicePixelRatio(dpr);
    render(&pixmap);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(data);
    drag->setPixmap(pixmap);
    drag->setHotSpot(data->hotspot);

    drag->exec(Qt::CopyAction);
}
