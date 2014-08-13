#include <QBrush>
#include <QFont>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QtDebug>

#include "leditem.h"



LedItem::LedItem(int id, qreal x, qreal y, QGraphicsItem *parent):
    LuiItem(id, x, y, settings::leditem::width, settings::leditem::height, parent),
    is_moving(false)
{
    setBrush(QBrush(Qt::black));//settings::leditem::color));
    setPen(QPen(Qt::NoPen));
    this->id_text = new QGraphicsTextItem(QString::number(this->id()), this); //items will be removed automtically on led deletion
    this->id_text->setFont(QFont("MS Shell Dlg 2", settings::leditem::height/2, QFont::DemiBold));
    this->id_text->setPos(this->rect().center() - this->id_text->boundingRect().center());
    updateTextColor();
    //setAcceptedMouseButtons(Qt::LeftButton);
}

void LedItem::updateTextColor(){
    QColor col = this->brush().color();
    QColor color(Qt::white);
    if( (col.redF()*0.229 + col.greenF()*0.587 + col.blueF()*0.114) > 186.0/360.0){
        color = Qt::black;
    }
    this->id_text->setDefaultTextColor(color);
}

LuiItemType LedItem::luitype()
{
    return LuiItemType::led;
}

/*void LedItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    LuiItem::mousePressEvent(event);
}*/

void LedItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->is_moving){
        this->is_moving = false;
        emit itemMoved();
    }
    QGraphicsRectItem::mouseReleaseEvent(event);
}

QVariant LedItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == QGraphicsItem::ItemPositionHasChanged){
        this->is_moving = true;
    }
    return QGraphicsRectItem::itemChange(change, value);
}

















