#include <QBrush>
#include <QPen>
#include <QtDebug>

#include "groupitem.h"

GroupItem::GroupItem(int id, QGraphicsItem *parent):
    LuiItem(id, parent),
    grp(new QList<LedItem*>)
{
    setPen(Qt::NoPen);
    setBrush(QBrush(Qt::gray));
}

void GroupItem::addLed(LedItem *led)
{
    this->grp->append(led);
    led->setParentItem(this);
    QRectF r = this->childrenBoundingRect();
    r.setWidth(r.width() + 2*settings::groupitem::border);
    r.setHeight(r.height() + 2*settings::groupitem::border);
    r.translate(-settings::groupitem::border, -settings::groupitem::border);
    setRect(r);
}

void GroupItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Mouse press on group";
}

