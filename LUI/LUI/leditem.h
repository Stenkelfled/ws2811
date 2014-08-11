#ifndef LEDITEM_H
#define LEDITEM_H

#include <luiitem.h>
#include <settings.h>

class LedItem : public LuiItem
{
    //Q_OBJECT
public:
    explicit LedItem(int id, qreal x, qreal y, QGraphicsItem *parent = 0);
    void updateTextColor();
    virtual LuiItemType luitype();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    QGraphicsTextItem *id_text;
};

#endif // LEDITEM_H
