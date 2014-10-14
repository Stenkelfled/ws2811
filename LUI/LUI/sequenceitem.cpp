#include "sequenceitem.h"
#include <sequencescene.h>
#include <settings.h>

SequenceItem::SequenceItem(sequencetype type, QColor color, QColor gradient, QGraphicsItem *parent):
    QGraphicsItem(parent),
    my_type(type),
    my_color(color),
    my_end_color(gradient),
    my_length(50)
{
    setFlags(QGraphicsItem::ItemIsSelectable);
    this->setPos(0, 0.5*(settings::sequencegroupitem::height - settings::sequenceitem::height));
}

QRectF SequenceItem::boundingRect() const
{
    return QRectF(0, 0, this->width(), settings::sequenceitem::height);
}

void SequenceItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    if(this->my_type == singlecolor){
        painter->setBrush(QBrush(this->my_color));
    } else {
        QBrush brush(Qt::red);
        //brush.setStyle(Qt::LinearGradientPattern);
        painter->setBrush(brush);
    }
    painter->setPen(Qt::NoPen);
    painter->drawRect(this->boundingRect());

    /*if(option->state & QStyle::State_Selected){
        painter->setPen(QPen(QBrush(settings::sequencegroupitem::text_color), 1, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        QRectF rect = QRectF(*(this->my_rect));
        rect.setHeight(rect.height()-1);
        rect.setWidth(rect.width()-1);
        painter->drawRect(rect);
    }*/
}

qreal SequenceItem::width() const
{
    SequenceScene *scene = (SequenceScene*)(this->scene());
    return scene->pixelsPer10ms()*this->my_length;
}
