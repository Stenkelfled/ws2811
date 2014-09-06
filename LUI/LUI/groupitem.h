#ifndef GROUPITEM_H
#define GROUPITEM_H
#include <QtWidgets>

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
    ~GroupItem();
    void addLed(LedItem *led);
    void removeLed(LedItem *led);
    QPoint indexOf(LedItem *led);
    void makeEmpty();
    QColor color();
    void setColor(QColor color);

    virtual QByteArray getUsbCmd();
    //LedItem* unpackDragData(const QMimeData *data);

public slots:
    void refreshArea(bool plus_extra_space=false);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent * event);
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent * event);
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
    virtual void dropEvent(QGraphicsSceneDragDropEvent * event);

signals:
    void groupEmpty(GroupItem* group);

private:

    QList<QList<LedItem*>*> *leds; //list(pointer) of row(pointer)s of led(pointer)s
    QColor group_color;
    groupAlignment alignment;
};

#endif // GROUPITEM_H
