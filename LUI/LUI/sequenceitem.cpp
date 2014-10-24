#include "sequenceitem.h"
#include <sequencescene.h>
#include <settings.h>
#include <utils.h>

SequenceItem::SequenceItem(sequencetype type, QColor color, QColor gradient, QGraphicsItem *parent):
    QGraphicsObject(parent),
    my_type(type),
    my_color(color),
    my_end_color(gradient),
    my_length(50)
{
    setFlags(QGraphicsItem::ItemIsSelectable);
    this->setPos(0, settings::sequencegroupitem::extra_border);
}

QRectF SequenceItem::boundingRect() const
{
    return QRectF(0, 0, this->width(), settings::sequenceitem::height);
}

void SequenceItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    if(this->my_type == singlecolor){
        painter->setBrush(QBrush(utils::toDisplayColor(this->my_color)));
    } else {
        QBrush brush(Qt::red);
        //brush.setStyle(Qt::LinearGradientPattern);
        painter->setBrush(brush);
    }
    painter->setPen(Qt::NoPen);
    painter->drawRect(this->boundingRect());

    if(option->state & QStyle::State_Selected){
        QColor line_color;
        if(utils::isBrightBackground(this->my_color)){
            line_color = Qt::black;
        } else {
            line_color = Qt::white;
        }
        painter->setPen(QPen(QBrush(line_color), 1, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        QRectF rect = QRectF(this->boundingRect());
        rect.setHeight(rect.height()-1);
        rect.setWidth(rect.width()-1);
        painter->drawRect(rect);
    }
}

QColor SequenceItem::color() const
{
    return this->my_color;
}

void SequenceItem::setSingleColor(QColor color)
{
    color.toHsv();
    if( this->my_color != color){
        this->my_type = SequenceItem::singlecolor;
        this->my_color = color;
        this->update();
        ((SequenceScene*)(this->scene()))->refreshGroupColors();
    }
}

qreal SequenceItem::width() const
{
    SequenceScene *scene = (SequenceScene*)(this->scene());
    return scene->pixelsPer10ms()*this->my_length;
}

void SequenceItem::refreshGroupColor(int pos)
{
    if(this->my_type == SequenceItem::singlecolor){
        ((SequenceGroupItem*)(this->parentItem()))->refreshGroupColor(this->my_color);
    }
}

void SequenceItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    if(this->isSelected()){
        if(this->my_type == SequenceItem::singlecolor){
            emit needs_color_selector();
        }
    }
}















