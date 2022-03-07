#ifndef STYLEDRECYCLERLISTITEM_H
#define STYLEDRECYCLERLISTITEM_H

#include <QFrame>
#include <QRegion>

class StyledRecyclerListItem : public QWidget
{
    Q_OBJECT
public:
    explicit StyledRecyclerListItem(QWidget *parent = nullptr);
    void setSelected(bool selected);
    void setIsDropSite(bool dropSite);

protected:
    void paintEvent(QPaintEvent *) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QRect selectionPaintRect() const;
    QRegion borderPaintRect() const;

    bool isSelected;
    bool isDropSite;
    int selectionLineWidth = 5;
    int borderLineWidth = 1;

};

#endif // STYLEDRECYCLERLISTITEM_H
