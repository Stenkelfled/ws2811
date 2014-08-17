#ifndef GROUPITEM_H
#define GROUPITEM_H

#include <leditem.h>
#include <luiitem.h>

class GroupItem : public LuiItem
{
    Q_OBJECT
public:
    enum { Type = UserType + 2 };
    int type() const {return Type;}

    explicit GroupItem(int id, QGraphicsItem *parent = 0);
    void addLed(LedItem *led);
    void removeLed(LedItem *led);
    void makeEmpty();
    QColor color();
    void setColor(QColor color);

    virtual QByteArray getUsbCmd();

public slots:
    void refreshArea();

protected:
    //void mousePressEvent(QGraphicsSceneMouseEvent *event);

signals:
    void groupEmpty(GroupItem* group);

private:
    QList<LedItem*> *grp;
    QColor group_color;
};

#endif // GROUPITEM_H
