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

LuiItem::LuiItem(int id, qreal x, qreal y, int rect_width, int rect_height, QGraphicsItem *parent) :
    QGraphicsRectItem(x, y, rect_width, rect_height, parent),
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
