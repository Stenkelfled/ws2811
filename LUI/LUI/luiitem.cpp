#include <QBrush>
#include <QGraphicsScene>
#include <QPen>
#include <QtDebug>

#include "luiitem.h"

LuiItem::LuiItem(int id, QGraphicsItem *parent) :
    QObject(),
    QGraphicsRectItem(parent),
    my_id(id)
{
    initFlags();
}

LuiItem::LuiItem(int id, int rect_width, int rect_height, QGraphicsItem *parent) :
    QGraphicsRectItem(0, 0, rect_width, rect_height, parent),
    my_id(id)
{
    initFlags();
}

QColor LuiItem::color()
{
    return this->brush().color();
}

void LuiItem::changeId(int id)
{
    this->my_id = id;
}

int LuiItem::id()
{
    return this->my_id;
}

void LuiItem::setColor(QColor color)
{
    QBrush b = brush();
    b.setColor(color);
    setBrush(b);
}

QByteArray LuiItem::getUsbCmd()
{
    return QByteArray(); //only return an empty array --> nothing will be added
}

bool LuiItem::positionLowerThan(const QGraphicsItem *itm1, const QGraphicsItem *itm2)
{
    QPointF spos1 = itm1->mapToScene(0,0);
    QPointF spos2 = itm2->mapToScene(0,0);
    if(spos1.x() != spos2.x()){
        return (spos1.x() < spos2.x());
    }
    if(itm1->pos().x() != itm2->pos().x()){
        return (itm1->pos().x() < itm2->pos().x());
    }
    if(itm1->pos().y() != itm2->pos().y()){
        return (itm1->pos().y() < itm2->pos().y());
    }
    return (spos1.y() < spos2.y());
}

void LuiItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    scene()->clearSelection();
}

void LuiItem::initFlags()
{
    setFlags(QGraphicsItem::ItemIsSelectable
             | QGraphicsItem::ItemIsMovable
             | QGraphicsItem::ItemSendsGeometryChanges);
}
