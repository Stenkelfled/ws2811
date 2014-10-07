#include <QtDebug>

#include "sequencegroupitem.h"
#include <settings.h>

SequenceGroupItem::SequenceGroupItem(LedGroupItem *led_group, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    my_led_group(led_group),
    my_rect(new QRectF(0,0,settings::sequencegroupitem::name_text_width+10,settings::sequencegroupitem::height))
{
    connect(led_group, SIGNAL(nameChanged(QString,LedGroupItem*)), this, SLOT(changeName(QString)));
}

SequenceGroupItem::~SequenceGroupItem(){
    delete this->my_rect;

}

QRectF SequenceGroupItem::boundingRect() const{
    return *(this->my_rect);
}

void SequenceGroupItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->setBrush(QBrush(settings::sequencegroupitem::background_color));
    painter->setPen(Qt::NoPen);
    painter->drawRect(*(this->my_rect));

    QString name = this->my_led_group->name();
    if(name.length() > settings::sequencegroupitem::name_text_chars){
        name.resize(settings::sequencegroupitem::name_text_chars - 3);
        name.append("...");
    }

    painter->setPen(settings::sequencegroupitem::name_text_size);
    painter->setFont(QFont("MS Shell Dlg 2", settings::leditem::height/2, QFont::DemiBold));
    painter->drawText(QPoint(2,(settings::sequencegroupitem::height
                                + settings::sequencegroupitem::name_text_size)/2)
                      , name);

}

void SequenceGroupItem::setVPos(int pos)
{
    this->setPos(0,pos);
}

LedGroupItem *SequenceGroupItem::led_group()
{
    return this->my_led_group;
}

void SequenceGroupItem::changeName(QString new_name)
{
    this->update();
}
