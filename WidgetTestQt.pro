QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# Use shadow build, no debug/release subfolders, so that OUT_PWD is actually location of deployed executable
CONFIG -= debug_and_release

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    channel-slider/channelslideradapter.cpp \
    channel-slider/channelsliderviewholder.cpp \
    channel-slider/customlistitem.cpp \
    main.cpp \
    mainwindow.cpp \
    recycler/recyclerview.cpp \
    recycler/recyclerviewadapter.cpp \
    recycler/recyclerviewpool.cpp \
    recycler/styledrecyclerlistitem.cpp \
    recycler/viewholder.cpp

HEADERS += \
    channel-slider/channelslideradapter.h \
    channel-slider/channelsliderviewholder.h \
    channel-slider/customlistitem.h \
    mainwindow.h \
    recycler/recyclerview.h \
    recycler/recyclerviewadapter.h \
    recycler/recyclerviewpool.h \
    recycler/styledrecyclerlistitem.h \
    recycler/viewholder.h

FORMS += \
    channel-slider/customlistitem.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
