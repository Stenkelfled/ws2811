#ifndef LEDSCENE_H
#define LEDSCENE_H

#include <QGraphicsScene>

#include <groupitem.h>
#include <luiitem.h>

class LedScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit LedScene(QObject *parent = 0);
    void fillDefault();
    LedItem* newLed(qint16 id=0);

signals:
    void selectedItemStatusChanged(bool status);
    void selectedItemColorChanged(QColor color);

public slots:
    void ungroup();
    void group();
    void removeGroup(GroupItem *group);
    void selectAll();
    LedItem* getLed(int id);

    void updateColor(QColor color);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event);

private:
    void selectedItemChanged(LuiItem* item);
    void sortSelection(QList<QGraphicsItem*> &items);
    static QGraphicsSceneDragDropEvent* copyEvent(QGraphicsSceneDragDropEvent *old_event, QEvent::Type type);
    GroupItem* newGroup();
    QList<GroupItem*> *groups;
    QList<LedItem*> *leds;
    QGraphicsRectItem *selection_rect_item;
    QPointF selection_start;
    LuiItem* selected_item;
    QGraphicsItem *current_drag_item;

    friend QDataStream &operator<<(QDataStream &, const LedScene &);
    friend QDataStream &operator>>(QDataStream &, LedScene &);

};

#endif // LEDSCENE_H
