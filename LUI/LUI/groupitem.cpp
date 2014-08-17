#include <QBrush>
#include <QObject>
#include <QPen>
#include <QtDebug>

#include "groupitem.h"
#include <protocoll.h>

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
    if(led->parentItem() != NULL){
        GroupItem *group = qgraphicsitem_cast<GroupItem *>(led->parentItem());
        group->removeLed(led);
    }
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

/**
 * @brief GroupItem::getUsbCmd
 * Generates the USB-command for this group. First generates a list of all leds in this group, then defines the sequence for this group.
 * @return The USB-command for this group
 */
QByteArray GroupItem::getUsbCmd()
{
    QByteArray cmd = QByteArray();
/*    int first_led;
    int current_led;
    int next_led;
    int step;
    int i = 0;

    while(i<this->grp->length()){
        current_led = this->grp->at(i)->id();
        i++;
        if( i >= this->grp->length() ){
            //only one led left -> don't start a new row and append this led as single led
            cmd.append(PR_GRP_LED_ADD);
            cmd.append(current_led);
            break;
        }
        //try a row of leds
        next_led = this->grp->at(i)->id();
        step = next_led-current_led;
        first_led = current_led;
        while( ++i <= this->grp->length()){
            current_led = next_led;
            if(i < this->grp->length()){
                next_led = this->grp->at(i)->id();
            } else {
                //these are the last two leds -> set the next led and step so, that tey won't fit
                next_led = current_led - 1;
                step = 1;
            }
            if((next_led-current_led)!=step){
                //the next led does not fit in the row -> finish this row and then start a new one
                cmd.append(PR_GRP_LED_ROW);
                cmd.append(first_led);
                cmd.append(current_led);
                cmd.append(step);
                break;
            }
            if(i>=(this->grp->length()-1)){
                cmd.append(PR_GRP_LED_ROW);
                cmd.append(first_led);
                cmd.append(next_led);
                cmd.append(step);
                i++; //increment the counter, that the outer loop will also exit
            }
        }
    }
    */

    //for debugging: only print all leds-ids in the group's order
    foreach(LedItem* led, *(this->grp)){
        cmd.append(led->id());
    }

    return cmd;
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

