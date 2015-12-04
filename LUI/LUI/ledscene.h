#ifndef LEDSCENE_H
#define LEDSCENE_H

#include <QGraphicsScene>

#include <ledgroupitem.h>
#include <luiitem.h>

class LedScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit LedScene(QObject *parent = 0);
    void fillDefault();
    qint16 newLed();

signals:
    void selectedGroupChanged(LedGroupItem* group);
    void selectedGroupNameChanged(QString name);
    void newGroupAdded(LedGroupItem* group);
    void groupRemoved(LedGroupItem* group);

public slots:
    void makeNew();
    //void ungroup();
    void group();
    void removeGroup(LedGroupItem *group);
    void changeSelectedGroup(bool state, LedGroupItem *group);
    void selectAll();
    LedItem* getLed(int id);
    void updateGroupName(QString name, LedGroupItem *group);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event);

private:
    //void selectedItemChanged(LuiItem* item);
    void sortSelection(QList<QGraphicsItem*> &items);
    static QGraphicsSceneDragDropEvent* copyEvent(QGraphicsSceneDragDropEvent *old_event, QEvent::Type type);
    LedGroupItem* newGroup();
    QList<LedGroupItem*> groups;
    QList<LedItem*> leds;
    QGraphicsRectItem *selection_rect_item;
    QPointF selection_start;
    /**
     * @brief The item, that is currently under the item being dragged around. (NULL if no item)
     */
    QGraphicsItem *current_drag_item;
    LedGroupItem *my_selected_group;

    friend QDataStream &operator<<(QDataStream &, const LedScene &);
    friend QDataStream &operator>>(QDataStream &, LedScene &);

};

#endif // LEDSCENE_H
