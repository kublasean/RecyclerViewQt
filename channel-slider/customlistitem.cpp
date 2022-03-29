#include "customlistitem.h"
#include "ui_customlistitem.h"

#include <QSlider>
#include <QMouseEvent>
#include <QDebug>

CustomListItem::CustomListItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomListItem)
{
    ui->setupUi(this);
    ui->slider->setTracking(true);

    connect(ui->slider, &QSlider::valueChanged, ui->spinBox, &QSpinBox::setValue);
    connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), ui->slider, &QSlider::setValue);
    connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &CustomListItem::valueChanged);

    setAutoFillBackground(true);    
}

CustomListItem::~CustomListItem()
{
    delete ui;
}

void CustomListItem::setValue(int value)
{
    ui->slider->setValue(value);
}

void CustomListItem::setDisplayData(int channel, const QString &name, bool isHeader)
{
    if (isHeader) {
        setBackgroundRole(QPalette::Window);
        ui->headerLabel->setText(name);
        ui->stackedWidget->setCurrentWidget(ui->headerPage);
    } else {
        setBackgroundRole(QPalette::Base);
        ui->channelLabel->setText(QString::number(channel));
        ui->nameLabel->setText(name);
        ui->stackedWidget->setCurrentWidget(ui->channelPage);
    }
}

int CustomListItem::getValue() const
{
    return ui->slider->value();
}

void CustomListItem::mousePressEvent(QMouseEvent *event)
{

    qDebug() << "CustomListItem mouse press event";
    if (event->button() == Qt::LeftButton && ui->stackedWidget->currentWidget() == ui->headerPage) {
        dragPos = event->pos();
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void CustomListItem::mouseMoveEvent(QMouseEvent *event)
{

    // Are we sure it's ok to compare pointers (eek)?
    // Only want to be able to drag from a header row
    if (ui->stackedWidget->currentWidget() != ui->headerPage) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    if (dragPos.isNull() || !(event->buttons() & Qt::LeftButton)) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    if ((event->pos() - dragPos).manhattanLength() < QApplication::startDragDistance()) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    event->accept();
    emit dragStarted();
    //QWidget::mouseMoveEvent(event);

    // Reset to null
    dragPos = QPoint();
}

