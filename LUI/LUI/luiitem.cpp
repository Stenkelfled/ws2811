#include <QtDebug>

#include "luiitem.h"

LuiItem::LuiItem(int id, QGraphicsItem *parent) :
    QGraphicsRectItem(parent),
    my_id(id)
{
}

LuiItem::LuiItem(int id, qreal x, qreal y, int rect_width, int rect_height, QGraphicsItem *parent) :
    QGraphicsRectItem(x, y, rect_width, rect_height, parent),
    my_id(id)
{
}

int LuiItem::id()
{
    return this->my_id;
}
