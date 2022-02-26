#include "customlistitem.h"
#include "ui_customlistitem.h"

#include <QSlider>

CustomListItem::CustomListItem(QWidget *parent) :
    StyledRecyclerListItem(parent),
    ui(new Ui::CustomListItem)
{
    ui->setupUi(this);

    ui->slider->setTracking(true);

    connect(ui->slider, &QSlider::valueChanged, ui->spinBox, &QSpinBox::setValue);
    connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), ui->slider, &QSlider::setValue);
    connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &CustomListItem::valueChanged);
}

CustomListItem::~CustomListItem()
{
    delete ui;
}

void CustomListItem::setValue(int value)
{
    ui->slider->setValue(value);
}

void CustomListItem::setChannel(int channel, const QString &channelName)
{
    ui->channelLabel->setText(QString::number(channel));
    ui->nameLabel->setText(channelName);
}

int CustomListItem::getValue() const
{
    return ui->slider->value();
}

