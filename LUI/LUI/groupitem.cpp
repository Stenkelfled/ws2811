#include <QBrush>
#include <QPen>
#include <QtDebug>

#include "groupitem.h"

GroupItem::GroupItem(int id, QGraphicsItem *parent):
    LuiItem(id, parent),
    grp(new QGraphicsItemGroup(this))
{
    setPen(Qt::NoPen);
    setBrush(QBrush(Qt::gray));
    //setRect(0,0,10,10);
    /*QRectF r = rect();
    r.setHeight(40);
    r.setWidth(30);
    setRect(r);
    qDebug() << rect();*/
}

void GroupItem::addLed(LedItem *led)
{
    this->grp->addToGroup(led);
    QRectF r = this->grp->boundingRect();
    r.setWidth(r.width() + 2*settings::groupitem::border);
    r.setHeight(r.height() + 2*settings::groupitem::border);
    r.translate(-settings::groupitem::border, -settings::groupitem::border);
    setRect(r);
}

