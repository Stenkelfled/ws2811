#ifndef LEDSCENE_H
#define LEDSCENE_H

#include <QGraphicsScene>

#include <groupitem.h>

class LedScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit LedScene(QObject *parent = 0);

signals:

public slots:
    void ungroup();
    void group();
    void removeGroup(GroupItem *grp);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

};

#endif // LEDSCENE_H
