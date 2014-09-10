#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QtDebug>

#include <algorithm>

#include <global.h>
#include <groupitem.h>
#include <leditem.h>
#include <luiitem.h>
#include <ledscene.h>
#include <settings.h>

LedScene::LedScene(QObject *parent) :
    QGraphicsScene(parent),
    groups(new QList<GroupItem*>),
    leds(new QList<LedItem*>),
    selection_start(settings::ledscene::invalid_pos),
    selected_item(NULL),
    current_drag_item(NULL)
{
    QColor selection_rect_color(Qt::black);
    selection_rect_color.setAlpha(80);
    this->selection_rect_item = addRect(0, 0, 0, 0, QPen(Qt::black, 1, Qt::DashLine), QBrush(selection_rect_color));
    this->selection_rect_item->setZValue(10000);

}

void LedScene::fillDefault()
{
    GroupItem* grp = newGroup();
    for(qint16 i=0; i<GLOBAL_LED_COUNT; i++){
        LedItem* led = newLed(i);
        grp->addLed(led);
    }
}

LedItem *LedScene::newLed(qint16 id)
{
    LedItem *led = new LedItem(id); //items will be removed automatically on scene deletion
    this->leds->append(led);
    return led;
}

void LedScene::group()
{    
    GroupItem *new_grp = newGroup();
    QList<QGraphicsItem*> selection = selectedItems();
    std::sort(selection.begin(), selection.end(), LuiItem::positionLowerThan);
    foreach(QGraphicsItem *itm, selection){
        LedItem *led = qgraphicsitem_cast<LedItem*>(itm);
        if(led != NULL){
            new_grp->addLed(led);
        }
    }
}

void LedScene::ungroup()
{
    foreach(QGraphicsItem *itm, selectedItems()){
        LedItem *led = qgraphicsitem_cast<LedItem*>(itm);
        if(led != NULL){
            GroupItem *grp = qgraphicsitem_cast<GroupItem*>(led->parentItem());
            if(grp != NULL){
                grp->removeLed(led);
            }
        }
        GroupItem *grp = qgraphicsitem_cast<GroupItem*>(itm);
        if(grp != NULL){
            grp->makeEmpty();
        }
    }
}

void LedScene::removeGroup(GroupItem* group)
{
    int id = group->id();
    this->groups->removeAt(id);
    removeItem((QGraphicsItem*)group);
    for(;id<this->groups->length();id++){
    //foreach(GroupItem *item, this->groups){
        this->groups->at(id)->changeId(id);
    }
}

void LedScene::selectAll()
{
    foreach(QGraphicsItem *itm, items()){
        itm->setSelected(true);
    }
}

LedItem *LedScene::getLed(int id)
{
    return (*(this->leds))[id];
}

void LedScene::updateColor(QColor color)
{
    //qDebug() << "newColor" << color;

    this->selected_item->setColor(color);
}

GroupItem* LedScene::newGroup()
{
    GroupItem *grp = new GroupItem(this->groups->length(), NULL);
    this->groups->append(grp);
    connect(grp, SIGNAL(groupEmpty(GroupItem*)), this, SLOT(removeGroup(GroupItem*)));
    addItem(grp);
    return grp;
}

void LedScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug() << "scene press";
    //qDebug() << "item:" << itemAt(event->scenePos(), QTransform());
    //qDebug() << "mousegrabber:" << mouseGrabberItem();
    switch(event->button()){
    case Qt::LeftButton:
        {
        if(itemAt(event->scenePos(), QTransform()) != NULL){
            QGraphicsScene::mousePressEvent(event);
        } else {
            this->selection_rect_item->setRect(QRectF(event->scenePos(),QSizeF(0.1, 0.1)));
            this->selection_start = event->scenePos();
        }
        break;
        }
    case Qt::MiddleButton:
        break;
    default:
        break;
    }
}

void LedScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->selection_start != settings::ledscene::invalid_pos){
        QPointF diff = event->scenePos() - this->selection_start;
        QRectF r;
        if(diff.x() >= 0){
            if(diff.y() >= 0){
                r = QRectF(this->selection_start, event->scenePos());
            } else {
                r = QRectF(this->selection_start.x(), event->scenePos().y(), diff.x(), -diff.y());
            }
        } else {
            if(diff.y() >= 0){
                r = QRectF(event->scenePos().x(), this->selection_start.y(), -diff.x(), diff.y());
            } else {
                r = QRectF(event->scenePos(), this->selection_start);
            }
        }
        this->selection_rect_item->setRect(r);
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void LedScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->selection_start != settings::ledscene::invalid_pos){
        QPainterPath sel_path = QPainterPath();
        sel_path.addRect(this->selection_rect_item->rect());
        setSelectionArea(sel_path, Qt::ContainsItemShape);
        this->selection_rect_item->setRect(QRectF(0, 0, 0, 0));
        this->selection_start = settings::ledscene::invalid_pos;
    }
    QGraphicsScene::mouseReleaseEvent(event);
    QList<QGraphicsItem*> items = selectedItems();
    if(items.length() == 0){
        selectedItemChanged(NULL);
    } else if(items.length() == 1){
        selectedItemChanged( (LuiItem*)(items.at(0)) );
    }
}

void LedScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if(LedItem::unpackDragData(event->mimeData()) == NULL){
        event->ignore();
        return;
    }
    //qDebug() << "scene drag enter";
    QGraphicsItem *itm = itemAt(event->scenePos(), QTransform());
    if(itm != NULL){
        while(!itm->acceptDrops()){
            if(itm->parentItem() == NULL){
                break;
            }
            itm = itm->parentItem();
        }
        this->current_drag_item = itm;
        event->setPos(event->scenePos());
        sendEvent(itm, event);
    }
}

void LedScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if(LedItem::unpackDragData(event->mimeData()) == NULL){
        event->ignore();
        return;
    }
    //qDebug() << "scene drag move";
    QGraphicsItem *itm = itemAt(event->scenePos(), QTransform());
    if(itm != NULL){
        while(!itm->acceptDrops()){
            if(itm->parentItem() == NULL){
                break;
            }
            itm = itm->parentItem();
        }
        event->setPos(itm->mapFromScene(event->scenePos()));
        if(itm != this->current_drag_item){
            if(this->current_drag_item != NULL){
                sendEvent(this->current_drag_item, copyEvent(event, QEvent::GraphicsSceneDragLeave));
            }
            sendEvent(itm, copyEvent(event, QEvent::GraphicsSceneDragEnter));
            this->current_drag_item = itm;
        }
        sendEvent(itm, event);
        return;
    }
    //no item under mouse
    if(this->current_drag_item != NULL){
        //send dragLeaveEvent to current_dragItem
        event->setPos(this->current_drag_item->mapFromScene(event->scenePos()));
        sendEvent(this->current_drag_item, copyEvent(event, QEvent::GraphicsSceneDragLeave));
        this->current_drag_item = NULL;
    }
    event->accept();
}

void LedScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    if(LedItem::unpackDragData(event->mimeData()) == NULL){
        event->ignore();
        return;
    }
    //qDebug() << "scene drag leave";
    QGraphicsItem *itm = itemAt(event->scenePos(), QTransform());
    if(itm != NULL){
        while(!itm->acceptDrops()){
            if(itm->parentItem() == NULL){
                break;
            }
            itm = itm->parentItem();
        }
        event->setPos(event->scenePos());
        sendEvent(itm, event);
    } else {
        if(this->current_drag_item != NULL){
            event->setPos(event->scenePos());
            sendEvent(this->current_drag_item, copyEvent(event, QEvent::GraphicsSceneDragLeave));
            this->current_drag_item = NULL;
        }
    }
    event->accept();
}

void LedScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    LedItem* led = LedItem::unpackDragData(event->mimeData());
    if(led == NULL){
        event->ignore();
        return;
    }
    //qDebug() << "scene drop";
    if(this->current_drag_item != NULL){
        //dropping it on some item
        event->setPos(event->scenePos());
        sendEvent(this->current_drag_item, event);
        this->current_drag_item = NULL;
    } else {
        //dropping it directly on the scene
        GroupItem* grp = newGroup();
        grp->addLed(led);
        QPointF grp_pos(event->scenePos());
        grp->setX(grp->x() - (settings::leditem::width+settings::groupitem::border) );
        grp->setY(grp->y() - (settings::leditem::height+settings::groupitem::border) );
        grp->setPos(grp_pos);
    }
    if(mouseGrabberItem() != NULL){
        //qDebug() << "mousegrabber:" << mouseGrabberItem();
        QGraphicsScene::mouseReleaseEvent(new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMouseRelease));
    }
    event->accept();
}

void LedScene::selectedItemChanged(LuiItem *item)
{
    if(item != this->selected_item){
        this->selected_item = item;
        if(item == NULL){
            emit selectedItemStatusChanged(false);
        } else {
            emit selectedItemStatusChanged(true);
            emit selectedItemColorChanged(item->color());
        }
    }
}

QGraphicsSceneDragDropEvent *LedScene::copyEvent(QGraphicsSceneDragDropEvent *old_event, QEvent::Type type)
{
    QGraphicsSceneDragDropEvent *new_event = new QGraphicsSceneDragDropEvent(type);
    new_event->setMimeData(old_event->mimeData());
    new_event->setPos(old_event->pos());
    return new_event;
}

QDataStream &operator<<(QDataStream &stream, const LedScene &scene){
    stream << (quint16)scene.groups->size();
    foreach(GroupItem* group, *(scene.groups)){
        stream << *group;
    }
    return stream;
}

QDataStream &operator>>(QDataStream &stream, LedScene &scene){
    quint16 group_count;
    stream >> group_count;
    qDebug() << "scene<<" << group_count << "groups";
    GroupItem *group;
    for(quint16 i=0;i<group_count;i++){
        group = scene.newGroup();
        stream >> *group;
    }
    return stream;
}
























