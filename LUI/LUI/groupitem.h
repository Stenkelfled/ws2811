#ifndef GROUPITEM_H
#define GROUPITEM_H

#include <leditem.h>
#include <luiitem.h>

class GroupItem : public LuiItem
{
    Q_OBJECT
public:
    enum groupAlignment {horizontal, vertical};
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
    void refreshArea(bool plus_extra_space=false);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent * event);
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
    virtual void dropEvent(QGraphicsSceneDragDropEvent * event);

signals:
    void groupEmpty(GroupItem* group);

private:
    QList<LedItem*> *grp;
    QColor group_color;
    groupAlignment alignment;
};

#endif // GROUPITEM_H
