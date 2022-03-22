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

    //virtual void onSelectionChanged(bool isSelected);
    //bool getIsSelected() const { return selected; }
    QWidget *getItemView() const { return itemView; }
    int dataPos;

private:
    QPointer<QWidget> itemView;
    //bool selected;
};

#endif // VIEWHOLDER_H
