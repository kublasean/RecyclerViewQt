#include "customlistitem.h"
#include "ui_customlistitem.h"

#include <QSlider>

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
    setBackgroundRole(QPalette::Base);
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
    ui->nameLabel->setText(name);

    if (isHeader) {
        setBackgroundRole(QPalette::Window);
        ui->spinBox->hide();
        ui->slider->hide();
        ui->channelLabel->hide();
        ui->nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

        QFont font;
        font.setFamily("Helvetica");
        font.setBold(true);
        font.setPointSize(16);

        ui->nameLabel->setFont(font);
    } else {
        setBackgroundRole(QPalette::Base);
        ui->channelLabel->show();
        ui->channelLabel->setText(QString::number(channel));
        ui->slider->show();
        ui->spinBox->show();
        ui->nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }

    updateGeometry();

}

int CustomListItem::getValue() const
{
    return ui->slider->value();
}

