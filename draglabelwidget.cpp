#include "draglabelwidget.h"

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

    fixture.name = "DJ Lights";
    fixture.channels.append(FixtureChannel("Master Dimmer"));
    fixture.channels.append(FixtureChannel("Red"));
    fixture.channels.append(FixtureChannel("Green"));
    fixture.channels.append(FixtureChannel("Blue"));

    setText(fixture.name);
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

    FixtureMimeData *data = new FixtureMimeData();
    data->fixture = fixture;
    data->rows = fixture.channels.count();

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
