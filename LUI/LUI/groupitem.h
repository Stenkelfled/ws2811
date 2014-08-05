#ifndef GROUPITEM_H
#define GROUPITEM_H

#include <leditem.h>
#include <luiitem.h>

class GroupItem : public LuiItem
{
public:
    explicit GroupItem(int id, QGraphicsItem *parent = 0);
    void addLed(LedItem *led);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    QList<LedItem*> *grp;
};

#endif // GROUPITEM_H
