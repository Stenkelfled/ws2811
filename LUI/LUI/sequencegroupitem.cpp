#include <QtDebug>

#include "sequencegroupitem.h"
#include <settings.h>

SequenceGroupItem::SequenceGroupItem(LedGroupItem *led_group, colortype col_type, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    my_led_group(led_group),
    my_colortype(col_type),
    my_rect(new QRectF(0,0,settings::sequencegroupitem::name_text_width+settings::sequencegroupitem::extra_border,settings::sequencegroupitem::height))
{
    setFlags(QGraphicsItem::ItemIsSelectable);
    connect(led_group, SIGNAL(nameChanged(QString,LedGroupItem*)), this, SLOT(changeName(QString)));
    connect(led_group, SIGNAL(addedSequenceItem(SequenceItem*)), this, SLOT(addSequenceItem(SequenceItem*)));
}

SequenceGroupItem::~SequenceGroupItem(){
    delete this->my_rect;

}

QRectF SequenceGroupItem::boundingRect() const{
    //qDebug() << "group children bounding rect" << this->childrenBoundingRect();
    QRectF const &children_rect = this->childrenBoundingRect();
    this->my_rect->setWidth(children_rect.x() + children_rect.width() + settings::sequencegroupitem::extra_border);
    return *(this->my_rect);
}

void SequenceGroupItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    painter->setBrush(QBrush(settings::sequencegroupitem::background_color));
    painter->setPen(Qt::NoPen);
    painter->drawRect(*(this->my_rect));

    QString name = this->my_led_group->name();
    if(name.length() > settings::sequencegroupitem::name_text_chars){
        name.resize(settings::sequencegroupitem::name_text_chars - 3);
        name.append("...");
    }
    //name.append("\u221e");

    painter->setPen(settings::sequencegroupitem::name_text_size);
    painter->setFont(QFont("MS Shell Dlg 2", settings::leditem::height/2, QFont::DemiBold));
    painter->drawText(QPoint(2,(settings::sequencegroupitem::height
                                + settings::sequencegroupitem::name_text_size)/2)
                      , name);

    if(option->state & QStyle::State_Selected){
        painter->setPen(QPen(QBrush(settings::sequencegroupitem::text_color), 1, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        QRectF rect = QRectF(*(this->my_rect));
        rect.setHeight(rect.height()-1);
        rect.setWidth(rect.width()-1);
        painter->drawRect(rect);
    }

}

void SequenceGroupItem::refreshItemPotitions()
{
    int item_xpos = settings::sequencegroupitem::name_text_width + settings::sequencegroupitem::extra_border;
    foreach(SequenceItem *item, *(this->my_led_group->sequences())){
        item->setParentItem(this);
        item->setPos(item_xpos, item->pos().y());
        item_xpos += item->width() + settings::sequencegroupitem::space;
    }
}

void SequenceGroupItem::setVPos(int pos)
{
    this->setPos(0,pos);
}

void SequenceGroupItem::refreshGroupColor(QColor color)
{
    this->my_led_group->setColor(color);
}

void SequenceGroupItem::newSequence()
{
    this->my_led_group->newSequence();
    this->refreshItemPotitions();
}

LedGroupItem *SequenceGroupItem::led_group()
{
    return this->my_led_group;
}

void SequenceGroupItem::changeName(QString new_name)
{
    Q_UNUSED(new_name)
    this->update();
}

void SequenceGroupItem::addSequenceItem(SequenceItem *item)
{
    item->setParentItem(this);
    this->update();
}
