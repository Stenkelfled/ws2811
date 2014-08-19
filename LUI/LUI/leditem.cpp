#include <QBrush>
#include <QFont>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QtDebug>

#include "leditem.h"



LedItem::LedItem(int id, QGraphicsItem *parent):
    LuiItem(id, settings::leditem::width, settings::leditem::height, parent),
    is_moving(false),
    color_from_group(true),
    group_index(QPoint(-1,-1))
{
    this->id_text = new QGraphicsTextItem(QString::number(this->id()), this); //items will be removed automtically on led deletion
    this->id_text->setFont(QFont("MS Shell Dlg 2", settings::leditem::height/2, QFont::DemiBold));
    this->id_text->setPos(this->rect().center() - this->id_text->boundingRect().center());
    QColor c(settings::leditem::color);
    setBrush(QBrush(c.toHsv()));
    setColor(c.toHsv());
    setPen(QPen(Qt::NoPen));
    //setAcceptedMouseButtons(Qt::LeftButton);
}

QColor LedItem::color()
{
    return this->my_color;
}

QPoint LedItem::groupIndex()
{
    return this->group_index;
}

bool LedItem::hasColorFromGroup()
{
    return this->color_from_group;
}

void LedItem::setColor(QColor color)
{
    this->setColor(color, false);
}

void LedItem::setColor(QColor color, bool from_group)
{
    this->my_color = color;
    if(color.value() != 0){
        color.setHsv(color.hue(), color.saturation(), DISPLAY_HSV(color.value()));
    }
    QBrush b = brush();
    b.setColor(color);
    setBrush(b);
    updateTextColor();
    this->color_from_group = from_group;
}

void LedItem::setGroupIndex(QPoint pos)
{
    this->group_index = pos;
}

void LedItem::setGroupIndex(int x, int y)
{
    this->group_index.setX(x);
    this->group_index.setY(y);
}

void LedItem::updateTextColor(){
    QColor col = this->brush().color();
    QColor color(Qt::white);
    if( (col.redF()*0.229 + col.greenF()*0.587 + col.blueF()*0.114) > 186.0/360.0){
        color = Qt::black;
    }
    this->id_text->setDefaultTextColor(color);
}

/*void LedItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->is_moving == false){
        qDebug() << "Led move start";
        this->is_moving = true;
        emit itemMoves(true);
    }
    LuiItem::mousePressEvent(event);
}*/

void LedItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->is_moving){
        this->is_moving = false;
        emit itemMoves(false);
    }
    QGraphicsRectItem::mouseReleaseEvent(event);
}

QVariant LedItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change==QGraphicsItem::ItemSelectedHasChanged || change==QGraphicsItem::ItemVisibleHasChanged){
        this->is_moving = false;
    }
    if(change == QGraphicsItem::ItemPositionHasChanged){
        if(this->is_moving == false){
            this->is_moving = true;
            emit itemMoves(true);
        }
    }
    return QGraphicsRectItem::itemChange(change, value);
}

















