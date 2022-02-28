#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "recycler/recyclerview.h"
#include "channel-slider/channelslideradapter.h"
#include <QWidget>

#include <QVBoxLayout>
#include <QSlider>
#include <QPushButton>
#include <QScrollBar>
#include <QDockWidget>
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

    QDockWidget *widget = new QDockWidget();
    QPushButton *topButton = new QPushButton();
    widget->setWidget(topButton);

    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, widget);

    ChannelSliderAdapter *adapter = new ChannelSliderAdapter(model);
    RecyclerView *recycler = new RecyclerView(adapter);
    adapter->setSelectionModel(recycler->selectionModel());

    setCentralWidget(recycler);

    connect(topButton, &QPushButton::pressed, this, [recycler]() {
        recycler->verticalScrollBar()->setValue(0);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

