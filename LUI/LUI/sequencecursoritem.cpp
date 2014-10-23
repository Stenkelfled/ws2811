#include <QtDebug>

#include "sequencecursoritem.h"
#include <sequenceitem.h>
#include <utils.h>

SequenceCursorItem::SequenceCursorItem(QGraphicsItem *parent) :
    QGraphicsItem(parent),
    my_rect(QRect(0, 0, settings::sequencecursoritem::width, 0))
{
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
}

QRectF SequenceCursorItem::boundingRect() const
{
    return this->my_rect;
}

void SequenceCursorItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    painter->setBrush(QBrush(settings::sequencecursoritem::dark_color));
    painter->setPen(Qt::NoPen);
    painter->drawRect(this->my_rect);

    //paint all parts with dark background in bright color
    painter->setBrush(QBrush(settings::sequencecursoritem::bright_color));
    SequenceItem *seq;
    foreach(QGraphicsItem *item, this->collidingItems(Qt::IntersectsItemShape)){
        seq = qgraphicsitem_cast<SequenceItem*>(item);
        if(seq != NULL){
            if(!utils::isBrightBackground(seq->color())){
                QRectF other_rect = mapRectFromItem(seq, seq->boundingRect());
                other_rect.setLeft(0);
                other_rect.setWidth(settings::sequencecursoritem::width);
                //qDebug() << "dark bg found:" << other_rect << other_rect.bottomRight() << "width" << other_rect.width();
                painter->drawRect( other_rect );
            }
        }
    }
}

void SequenceCursorItem::setHeight(int groups)
{
    this->my_rect.setHeight( 2*settings::sequencecursoritem::additional_height
                             + groups*settings::sequencegroupitem::height
                             + (groups-1)*settings::sequencegroupitem::group_space);
    qDebug() << "new height:" << this->my_rect << groups;
}











