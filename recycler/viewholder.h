#ifndef VIEWHOLDER_H
#define VIEWHOLDER_H

#include <QObject>
#include <QWidget>
#include <QPointer>

class ViewHolder : public QObject
{
    Q_OBJECT
public:
    explicit ViewHolder(QObject *parent, QWidget *view);
    ~ViewHolder();

    QWidget *getItemView() const { return itemView; }
    int dataPos;

private:
    QPointer<QWidget> itemView;
};

#endif // VIEWHOLDER_H
