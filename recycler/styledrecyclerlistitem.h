#ifndef STYLEDRECYCLERLISTITEM_H
#define STYLEDRECYCLERLISTITEM_H

#include <QFrame>

class StyledRecyclerListItem : public QWidget
{
    Q_OBJECT
public:
    explicit StyledRecyclerListItem(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    //void enterEvent(QEvent *event) override;
    //void leaveEvent(QEvent *event) override;

private:
    QColor previousColor;

};

#endif // STYLEDRECYCLERLISTITEM_H
