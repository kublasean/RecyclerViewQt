#ifndef DRAGLABELWIDGET_H
#define DRAGLABELWIDGET_H

#include <QLabel>
#include <QPoint>

#include "channel-slider/fixturemimedata.h"

class DragLabelWidget : public QLabel
{
    Q_OBJECT
public:
    DragLabelWidget(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPoint dragPos;
    Fixture fixture;
};

#endif // DRAGLABELWIDGET_H
