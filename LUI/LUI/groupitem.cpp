#include <QBrush>
#include <QPen>

#include "groupitem.h"

GroupItem::GroupItem(int id, QGraphicsItem *parent):
    LuiItem(id, parent)
{
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
}

/*void GroupItem::addLed(LedItem led)
{

}*/

