#ifndef GROUPITEM_H
#define GROUPITEM_H

#include <leditem.h>
#include <luiitem.h>

class GroupItem : public LuiItem
{
    Q_OBJECT
public:
    explicit GroupItem(int id, QGraphicsItem *parent = 0);
    void addLed(LedItem *led);
    void removeLed(LedItem *led);
    void makeEmpty();
    virtual LuiItemType luitype();
    void setColor(QColor color);

public slots:
    void refreshArea();

protected:
    //void mousePressEvent(QGraphicsSceneMouseEvent *event);

signals:
    void groupEmpty(GroupItem* group);

private:
    QList<LedItem*> *grp;
};

#endif // GROUPITEM_H
