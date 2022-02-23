#ifndef CUSTOMLISTITEM_H
#define CUSTOMLISTITEM_H

#include <QWidget>

namespace Ui {
class CustomListItem;
}

class CustomListItem : public QWidget
{
    Q_OBJECT

public:
    explicit CustomListItem(QWidget *parent = nullptr);
    ~CustomListItem();

    void setChannel(int channel, const QString &channelName);
    int getValue() const;

public slots:
    void setValue(int value);

signals:
    void valueChanged(int newValue);

private:
    Ui::CustomListItem *ui;
};

#endif // CUSTOMLISTITEM_H
