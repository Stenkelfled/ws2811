#include <QBrush>
#include <QObject>
#include <QPen>
#include <QtDebug>

#include "groupitem.h"

GroupItem::GroupItem(int id, QGraphicsItem *parent):
    LuiItem(id, parent),
    grp(new QList<LedItem*>),
    group_color(Qt::black)
{
    setPen(Qt::NoPen);
    setBrush(QBrush(Qt::gray));
}

void GroupItem::addLed(LedItem *led)
{
    this->grp->append(led);
    led->setParentItem(this);
    refreshArea();
    connect(led, SIGNAL(itemMoved()), this, SLOT(refreshArea()));
}

void GroupItem::removeLed(LedItem *led)
{
    led->setParentItem(NULL);
    disconnect(led, 0, this, 0);
    this->grp->removeAll(led);
    refreshArea();
    if(this->grp->size() == 0){
        emit groupEmpty(this);
    }
}

void GroupItem::makeEmpty()
{
    foreach( LedItem* led, *(this->grp)){
        led->setParentItem(NULL);
        disconnect(led, 0, this, 0);
    }
    this->grp->clear();
    emit groupEmpty(this);
}

LuiItemType GroupItem::luitype()
{
    return LuiItemType::group;
}

QColor GroupItem::color()
{
    return this->group_color;
}

void GroupItem::setColor(QColor color)
{
    if(color != this->group_color){
        this->group_color = color;
        foreach(LedItem *led, *(this->grp)){
            led->setColor(color, true);
        }
    }
}

/*void GroupItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug() << "Mouse press on group";
    LuiItem::mousePressEvent(event);
}*/

void GroupItem::refreshArea()
{
    QRectF r = this->childrenBoundingRect();
    r.setWidth(r.width() + 2*settings::groupitem::border);
    r.setHeight(r.height() + 2*settings::groupitem::border);
    r.translate(-settings::groupitem::border, -settings::groupitem::border);
    setRect(r);
}

