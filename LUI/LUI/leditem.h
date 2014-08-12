#ifndef LEDITEM_H
#define LEDITEM_H

#include <QVariant>

#include <luiitem.h>
#include <settings.h>

class LedItem : public LuiItem
{
    Q_OBJECT
public:
    explicit LedItem(int id, qreal x, qreal y, QGraphicsItem *parent = 0);
    void updateTextColor();
    virtual LuiItemType luitype();

protected:
    //void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);

signals:
    void itemMoved();

private:
    QGraphicsTextItem *id_text;
    bool is_moving;
};

#endif // LEDITEM_H
