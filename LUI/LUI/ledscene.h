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
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    void selectedItemChanged(LuiItem* item);
    GroupItem* newGroup();
    QList<GroupItem*> *groups;
    QGraphicsRectItem *selection_rect_item;
    QPointF selection_start;
    LuiItem* selected_item;

};

#endif // LEDSCENE_H
