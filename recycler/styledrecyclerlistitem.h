#ifndef STYLEDRECYCLERLISTITEM_H
#define STYLEDRECYCLERLISTITEM_H

#include <QFrame>
#include <QRegion>

class StyledRecyclerListItem : public QWidget
{
    Q_OBJECT
public:
    explicit StyledRecyclerListItem(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QRegion borderPaintRect() const;
    int borderLineWidth = 1;

};

#endif // STYLEDRECYCLERLISTITEM_H
