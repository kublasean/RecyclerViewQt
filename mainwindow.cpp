#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "recycler/recyclerview.h"
#include "channel-slider/channelslideradapter.h"
#include <QWidget>

#include <QVBoxLayout>
#include <QSlider>
#include <QPushButton>
#include <QScrollBar>

#include <QStandardItemModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QStandardItemModel *model = new QStandardItemModel(this);

    for (int i=0; i<512; i++) {
        QStandardItem *item = new QStandardItem(QString::number(i+1));
        item->setData("Channel " + QString::number(i+1), Qt::ToolTipRole);
        item->setData(0, Qt::UserRole);
        model->appendRow(item);
    }

    RecyclerView *recycler = new RecyclerView(new ChannelSliderAdapter(model));


    ui->verticalLayout->insertWidget(1, recycler);

    connect(ui->pushButton_2, &QPushButton::pressed, [recycler]() {
        int min = recycler->verticalScrollBar()->minimum();
        recycler->verticalScrollBar()->setValue(min);
    });

    connect(ui->pushButton, &QPushButton::pressed, [recycler]() {
        int max = recycler->verticalScrollBar()->maximum();
        recycler->verticalScrollBar()->setValue(max);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

