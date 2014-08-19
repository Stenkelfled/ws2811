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

signals:
    void selectedItemStatusChanged(bool status);
    void selectedItemColorChanged(QColor color);

public slots:
    void ungroup();
    void group();
    void removeGroup(GroupItem *group);
    void selectAll();

    void updateColor(QColor color);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    void selectedItemChanged(LuiItem* item);
    void sortSelection(QList<QGraphicsItem*> &items);
    GroupItem* newGroup();
    QList<GroupItem*> *groups;
    QGraphicsRectItem *selection_rect_item;
    QPointF selection_start;
    LuiItem* selected_item;

};

#endif // LEDSCENE_H
