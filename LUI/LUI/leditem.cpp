#include <QBrush>

#include "leditem.h"

LedItem::LedItem(int id, qreal x, qreal y):
    QGraphicsRectItem(x, y, settings::leditem::width, settings::leditem::height),
    my_id(id)
{
    setBrush(QBrush(settings::leditem::color));
}

int LedItem::id(){
    return this->my_id;
}
