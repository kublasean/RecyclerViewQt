#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "recycler/recyclerview.h"
#include "channel-slider/channelslideradapter.h"
#include "channel-slider/channelitemmodel.h"
#include "draglabelwidget.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QPushButton>
#include <QScrollBar>
#include <QDockWidget>
#include <QStandardItemModel>
#include <QListView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //QDockWidget *widget = new QDockWidget();
    //widget->setWidget(topButton);
    //widget->setWidget(new DragLabelWidget());
    //addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, widget);

    ChannelSliderAdapter *adapter = new ChannelSliderAdapter(new ChannelItemModel(this));
    RecyclerView *recycler = new RecyclerView(adapter);
    adapter->setSelectionModel(recycler->selectionModel());

    //setCentralWidget(recycler);

    QWidget *main = new QWidget();
    main->setLayout(new QHBoxLayout());
    main->layout()->addWidget(recycler);
    main->layout()->addWidget(new DragLabelWidget());
    setCentralWidget(main);
}

MainWindow::~MainWindow()
{
    delete ui;
}

