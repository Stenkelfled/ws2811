#include <QBrush>
#include <QPen>
#include <QtDebug>

#include "luiitem.h"

LuiItem::LuiItem(int id, QGraphicsItem *parent) :
    QObject(),
    QGraphicsRectItem(parent),
    my_id(id)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

LuiItem::LuiItem(int id, qreal x, qreal y, int rect_width, int rect_height, QGraphicsItem *parent) :
    QGraphicsRectItem(x, y, rect_width, rect_height, parent),
    my_id(id)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

int LuiItem::id()
{
    return this->my_id;
}

LuiItemType LuiItem::luitype()
{
    return LuiItemType::none;
}
