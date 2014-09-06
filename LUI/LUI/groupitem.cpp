#include <QBrush>
#include <QObject>
#include <QPen>
#include <QtDebug>

#include "groupitem.h"
#include <protocoll.h>

GroupItem::GroupItem(int id, QGraphicsItem *parent):
    LuiItem(id, parent),
    leds(new QList<QList<LedItem*>*>),
    alignment(GroupItem::horizontal)
{
    setPen(Qt::SolidLine);
    QColor c(settings::groupitem::color);
    setBrush(QBrush(c.toHsv()));
    c = QColor(settings::leditem::color);
    this->group_color = c.toHsv();
    setAcceptDrops(true);

}

GroupItem::~GroupItem()
{
    foreach(QList<LedItem*>* row, *(this->leds)){
        delete row;
    }
    delete this->leds;
}

void GroupItem::addLed(LedItem *led)
{
    if(this->leds->length() == 0){
        QList<LedItem*>* new_row = new QList<LedItem*>;
        leds->append(new_row);
    }
    if(led->parentItem() != NULL){
        GroupItem *group = qgraphicsitem_cast<GroupItem *>(led->parentItem());
        group->removeLed(led);
    }
    this->leds->last()->append(led);
    led->setParentItem(this);
    //led->setGroupIndex(this->leds->length()-1, 0);
    refreshArea();
    //connect(led, SIGNAL(itemMoves(bool)), this, SLOT(refreshArea(bool)));
}

void GroupItem::removeLed(LedItem *led)
{
    led->setParentItem(NULL);
    //led->setGroupIndex(-1, -1);
    disconnect(led, 0, this, 0);
    //this->leds->removeAll(led);
    foreach(QList<LedItem*>* row, *(this->leds)){
        int idx = row->indexOf(led);
        if(idx != -1){
            row->removeAt(idx);
            if(row->isEmpty()){
                delete row;
                this->leds->removeAll(row);
            }
            break;
        }
    }
    refreshArea();
    if(this->leds->isEmpty()){
        emit groupEmpty(this);
    }
}

QPoint GroupItem::indexOf(LedItem *led)
{
    QList<LedItem*> *row;
    int row_idx, col_idx;
    for(row_idx=0; row_idx<this->leds->size(); row_idx++){
        row = this->leds->at(row_idx);
        col_idx = row->indexOf(led);
        if(col_idx != -1){
            return QPoint(row_idx, col_idx);
        }
    }
    return QPoint(-1,-1);
}

void GroupItem::makeEmpty()
{
    foreach(QList<LedItem*>* row, *(this->leds)){
        foreach( LedItem* led, *(row)){
            led->setParentItem(NULL);
            disconnect(led, 0, this, 0);
        }
        delete row;
    }
    this->leds->clear();
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
        foreach(QList<LedItem*>* row, *(this->leds)){
            foreach(LedItem *led, *(row)){
                led->setColor(color, true);
            }
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
    foreach(QList<LedItem*>* row, *(this->leds)){
        foreach(LedItem* led, *(row)){
            cmd.append(led->id());
        }
    }
    return cmd;
}

void GroupItem::refreshArea(bool item_moving)
{
    //qDebug() << "refresh" << item_moving;
    qreal x=0, y=0;
    foreach(QList<LedItem*>* row, *(this->leds)){
        x = 0;
        foreach(LedItem* led, *(row)){
            if(this->alignment == GroupItem::horizontal){
                led->setPos(x,y);
            } else {
                led->setPos(y,x);
            }
            //qDebug() << "led" << led->id() << "pos" << led->pos();
            x += settings::leditem::width + settings::leditem::spacing;
        }
        y += settings::leditem::height + settings::leditem::spacing;
    }

    QRectF r = this->childrenBoundingRect();
    int extra_space = 0;
    if(item_moving){
        extra_space = settings::groupitem::extra_space;
    }
    r.setWidth(r.width() + 2*(settings::groupitem::border + extra_space));
    r.setHeight(r.height() + 2*(settings::groupitem::border + extra_space));
    r.translate(-(settings::groupitem::border + extra_space), -(settings::groupitem::border + extra_space));
    setRect(r);
}

void GroupItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Mouse press on group";
    //refreshArea(false);
    /*QString leds;
    foreach(LedItem* led, *(this->grp)){
        leds += QString::number(led->id());
    }
    qDebug() << "leds:" << leds;*/
    LuiItem::mousePressEvent(event);
}

void GroupItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    Q_UNUSED(event)
    qDebug() << "contextMenuEvent on group" << id();
}

void GroupItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "group drag enter";
    LedItem* led = LedItem::unpackDragData(event->mimeData());
    if(led == NULL){
        LuiItem::dragEnterEvent(event);
    }
    event->accept();
    if(indexOf(led) == QPoint(-1,-1)){
        addLed(led);
    }
    refreshArea(true);
}

void GroupItem::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "group drag move";
    LedItem* led = LedItem::unpackDragData(event->mimeData());
    if(led == NULL){
        LuiItem::dragMoveEvent(event);
        return;
    }
    event->accept();
    int new_col_idx = int(event->pos().x()+settings::leditem::spacing/2)/(settings::leditem::width+settings::leditem::spacing);
    int new_row_idx = int(event->pos().y()+settings::leditem::spacing/2)/(settings::leditem::height+settings::leditem::spacing);
    if(event->pos().y() < 0){
        new_row_idx--;
    }
    //qDebug() << "idx" << new_col_idx << "," << new_row_idx;
    QList<LedItem*>* row;
    int led_idx;
    int row_idx;
    for(row_idx=0; row_idx<this->leds->size(); row_idx++){
        row = this->leds->at(row_idx);
        led_idx = row->indexOf(led);
        if(led_idx != -1){
            row->removeAt(led_idx);
            break;
        }
    }
    QList<LedItem*>* new_row;
    if( (new_row_idx < 0) || (new_row_idx >= this->leds->size()) ){
        new_row = new QList<LedItem*>;
        if(new_row_idx < 0){
            this->leds->prepend(new_row);
        } else {
            this->leds->append(new_row);
        }
    } else {
        if(row_idx != new_row_idx){
            new_row = this->leds->at(new_row_idx);
        } else {
            new_row = row;
        }
    }
    if(new_col_idx >= new_row->size()){
        new_row->append(led);
    } else {
        new_row->insert(new_col_idx, led);
    }
    if(row->isEmpty()){
        this->leds->removeAll(row);
        delete row;
        row = NULL;
        //qDebug() << "row deleted" << row_idx;
    }
    refreshArea(true);
}

void GroupItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "group leave";
    LedItem* led = LedItem::unpackDragData(event->mimeData());
    if(led == NULL){
        LuiItem::dragLeaveEvent(event);
        return;
    }
    event->accept();
    refreshArea(false);
}

void GroupItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "Group drop";
    LedItem* led = LedItem::unpackDragData(event->mimeData());
    if(led == NULL){
        LuiItem::dropEvent(event);
        return;
    }
    event->accept();
    //qDebug() << "dropped Led" << led_id << ok;
    refreshArea(false);
}

/*LedItem *GroupItem::unpackDragData(const QMimeData *data)
{
    if(!data->hasText()){
        return NULL;
    }
    if(data->text().compare("Led") != 0){
        return NULL;
    }
    bool ok;
    LedItem* led = (LedItem*)(data->data(settings::leditem::mimetype).toULongLong(&ok));
    if(!ok){
        return NULL;
    }
    return led;
}*/

























