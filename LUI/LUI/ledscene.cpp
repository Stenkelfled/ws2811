#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QtDebug>

#include <algorithm>

#include <global.h>
#include <ledgroupitem.h>
#include <leditem.h>
#include <luiitem.h>
#include <ledscene.h>
#include <settings.h>

/**
 * @brief Scene to manage,lay out and group the leds
 * @param parent
 *
 *
 * There are as many LedItems in the LedScene as there are physically LEDs
 * connected to the Board. These are filled by LedScene::fillDefault(). You have
 * to make sure, that no LED is removed, because then one physical LED has no
 * counterpart in the GUI.
 */
LedScene::LedScene(QObject *parent) :
    QGraphicsScene(parent),
    selection_start(settings::ledscene::invalid_pos),
    current_drag_item(NULL),
    my_selected_group(NULL)
{
    QColor selection_rect_color(Qt::black);
    selection_rect_color.setAlpha(80);
    this->selection_rect_item = addRect(0, 0, 0, 0, QPen(Qt::black, 1, Qt::DashLine), QBrush(selection_rect_color));
    this->selection_rect_item->setZValue(10000);
    makeNew();
}

/**
 * @brief Fills the Scene with the default number of LEDs grouped to 1 group
 */
void LedScene::fillDefault()
{
    LedGroupItem* grp = newGroup();
    for(qint16 i=0; i<GLOBAL_LED_COUNT; i++){
        LedItem* led = getLed(newLed());
        grp->addLed(led);
    }
}

/**
 * @brief Add a new LED to the scene
 * @return ID of the new LED
 */
qint16 LedScene::newLed()
{
    qint16 id = leds.length();
    LedItem *led = new LedItem(id); //items will be removed automatically on scene deletion
    this->leds.append(led);
    return id;
}

/**
 * @brief Clear the scene and add the default LEDs
 */
void LedScene::makeNew()
{
    ///TODO: add functionality here
}

/**
 * @brief Take all selected LEDs to a new group. The LEDs are automatically removed from their old groups
 */
void LedScene::group()
{    
    LedGroupItem *new_grp = newGroup();
    QList<QGraphicsItem*> selection = selectedItems();
    std::sort(selection.begin(), selection.end(), LuiItem::positionLowerThan);
    foreach(QGraphicsItem *itm, selection){
        LedItem *led = qgraphicsitem_cast<LedItem*>(itm);
        if(led != NULL){
            new_grp->addLed(led);
        }
    }
}

/**
 * @brief A LedGroupItem signalled, that it is empty -> Remove the LedGroupItem from LedScene
 * @param group The LedGroupItemItem to remove
 */
void LedScene::removeGroup(LedGroupItem* group)
{
    int id = group->id();
    emit groupRemoved(group);
    this->groups.removeAt(id);
    removeItem((QGraphicsItem*)group);
    //Refresh group IDs
    for(;id<this->groups.length();id++){
        this->groups.at(id)->changeId(id);
    }
}

/**
 * @brief Change the current LedGroupItem.
 * @param state How did the state of the LedGroupItem change? False: unselected, True: selected
 * @param group The LedGroupItem, which state changed
 *
 *
 * A LedGroupItem signalled, that its selection state has changed. Calling this
 * function causes LedScene to update the current selected group (my_selected_group).
 * If the new group is not the current selected group, LedScene emits the signals
 * selectedGroupChanged and selectedGroupNameChanged.
 */
void LedScene::changeSelectedGroup(bool state, LedGroupItem *group)
{
    if(state == false){
        this->my_selected_group = NULL;
        emit selectedGroupChanged(NULL);
        emit selectedGroupNameChanged("");
    } else {
        if(this->my_selected_group != group){
            if(this->my_selected_group != NULL){
                this->my_selected_group->setSelected(false);
            }
            this->my_selected_group = group;
            emit selectedGroupChanged(group);
            emit selectedGroupNameChanged(group->name());
        }
    }
}

/**
 * @brief Update the Name for a group
 * @param name The new Name for the Group
 * @param group The LedGroupItem, that changed its name
 *
 *
 * There was a name change in a LedGroupItem. If it is the current selected
 * LedGroupItem, propagate the name change via emmiting selectedGroupNameChanged
 */
void LedScene::updateGroupName(QString name, LedGroupItem* group)
{
    if( this->my_selected_group == group){
        emit selectedGroupNameChanged(name);
    }
}

/**
 * @brief Select all LedItem and LedGroupItem, that are in the LedScene.
 */
void LedScene::selectAll()
{
    foreach(QGraphicsItem *itm, items()){
        itm->setSelected(true);
    }
}

/**
 * @brief Get the LedItem with the given ID
 * @param id The ID to the wanted LedItem
 * @return The LedItem corresponding to the given ID
 */
LedItem *LedScene::getLed(int id)
{
    return (this->leds)[id];
}

/**
 * @brief Adds a new LedGroupItem to the LedScene
 * @return The new LedGroupItem
 *
 *
 * Creates a new LedGroupItem, including all connections (escpecially the connections
 * between the LedGroupItem and the LedScene. Emits newGroupAdded.
 */
LedGroupItem* LedScene::newGroup()
{
    LedGroupItem *grp = new LedGroupItem(this->groups.length(), NULL);
    this->groups.append(grp);
    connect(grp, SIGNAL(groupEmpty(LedGroupItem*)), this, SLOT(removeGroup(LedGroupItem*)));
    connect(grp, SIGNAL(selectionChanged(bool,LedGroupItem*)), this, SLOT(changeSelectedGroup(bool,LedGroupItem*)));
    addItem(grp);
    connect(grp, SIGNAL(nameChanged(QString,LedGroupItem*)), this, SLOT(updateGroupName(QString,LedGroupItem*)));
    emit newGroupAdded(grp);
    return grp;
}

/**
 * @brief Handles Mouse-button-press event
 * @param event The Event passed by Qt.
 *
 *
 * If a MouseButton gets pressed on the LedScene, check wheter the mouse is
 * currently over any item(LedItem, LedGroupItem) on the scene. If so, propagate
 * the event to the item. If not, start drawing a rectangle to create a selection.
 */
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

/**
 * @brief Handle Mouse movement on LedScene.
 * @param event The Event passed by Qt.
 *
 *
 * Check, if we are currently drawing a selection (selection_start is not set to
 * settings::ledscene::invalid_pos). If we do, then update the
 * selection-rectangle. \n
 * After drawing the rectangle, don't forge to propagate the Event to superclass.
 */
void LedScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->selection_start != settings::ledscene::invalid_pos){ //selection_start is set to setting::lesdcene::invalid_pos, if there is no selection to draw
        QPointF diff = event->scenePos() - this->selection_start;
        QRectF r;
        if(diff.x() >= 0){
            if(diff.y() >= 0){
                r = QRectF(this->selection_start, event->scenePos());
            } else {
                r = QRectF(this->selection_start.x(), event->scenePos().y(),
                           diff.x(), -diff.y());
            }
        } else {
            if(diff.y() >= 0){
                r = QRectF(event->scenePos().x(), this->selection_start.y(),
                           -diff.x(), diff.y());
            } else {
                r = QRectF(event->scenePos(), this->selection_start);
            }
        }
        this->selection_rect_item->setRect(r);
    }
    QGraphicsScene::mouseMoveEvent(event);
}

/**
 * @brief Handle Mouse-button-release on LedScene.
 * @param event The Event passed by Qt.
 *
 *
 * Finish the Selection (if we have any). The drawn rect is passed to Qt-libary
 * by calling setSelectionArea(), which then automatically selects all items
 * inside the rect and unselects all others.
 * Finally propagate the Event to Superclass.
 */
void LedScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->selection_start != settings::ledscene::invalid_pos){
        QPainterPath sel_path = QPainterPath();
        sel_path.addRect(this->selection_rect_item->rect());
        setSelectionArea(sel_path, Qt::ContainsItemShape);
        //selection finished, now reset rectangle and selection_start position
        this->selection_rect_item->setRect(QRectF(0, 0, 0, 0));
        this->selection_start = settings::ledscene::invalid_pos;
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

/**
 * @brief Handles drag events.
 * @param event
 *
 *
 * At the moment there are only LedItems, that need to be handeled by
 * drag/drop mechanism. LedGroupItems are movable, but need no drag/drop,
 * because they don't interact with other items. \n
 * The dragEnterEvent occurs on both starting a drag inside the scene and
 * moving the item from outside the scene to inside.
 */
void LedScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    //* only handle drag-events for LedItems
    if(LedItem::unpackDragData(event->mimeData()) == NULL){
        event->ignore();
        return;
    }
    //qDebug() << "scene drag enter";
    QGraphicsItem *itm = itemAt(event->scenePos(), QTransform());
    if(itm != NULL){
        //* There is any item (Group or Led) under the current dragging item. Get the Group!
        while(!itm->acceptDrops()){
            if(itm->parentItem() == NULL){
                break;
            }
            itm = itm->parentItem();
        }
        this->current_drag_item = itm;
        event->setPos(event->scenePos());
        //* Propagate the drag event to the LedGroupItem
        sendEvent(itm, event);
    }
}

/**
 * @brief Handles drag events
 * @param event
 *
 *
 * See also LedScene::dragEnterEvent().
 * There are different options, how the LedItem can move from group to group:
 *  -# from group to outside a group
 *  -# from outside a group to a group
 *  -# from a group directly to another group
 *  -# inside a group (don't leaving the group)
 *  -# outside any group
 * These options are differentiated by current_drag_item, which holds the last
 * LedGroupItem (or NULL, if there is no group) and itm, which hold the LedGroupItem
 * currently located unter the mouse.
 */
void LedScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    //* only handle drag-events for LedItems
    if(LedItem::unpackDragData(event->mimeData()) == NULL){
        event->ignore();
        return;
    }
    //qDebug() << "scene drag move";
    QGraphicsItem *itm = itemAt(event->scenePos(), QTransform());
    if(itm != NULL){
        //* Item under mouse -> get the LedGroupItem
        while(!itm->acceptDrops()){
            if(itm->parentItem() == NULL){
                break;
            }
            itm = itm->parentItem();
        }
        event->setPos(itm->mapFromScene(event->scenePos()));
        if(itm != this->current_drag_item){
            //* We got a new LedGroupItem -> remove Led from old group and add to new group
            if(this->current_drag_item != NULL){
                //* Send DragLeave to the old LedGroupItem
                sendEvent(this->current_drag_item, copyEvent(event, QEvent::GraphicsSceneDragLeave));
            }
            //* Send DragEnter to the new LedGroupItem
            sendEvent(itm, copyEvent(event, QEvent::GraphicsSceneDragEnter));
            this->current_drag_item = itm;
        }
        //* Propagate the move event to the group
        sendEvent(itm, event);
        return;
    } else {
        //*no item under mouse
        if(this->current_drag_item != NULL){
            //*send dragLeaveEvent to current_drag_item
            event->setPos(this->current_drag_item->mapFromScene(event->scenePos()));
            sendEvent(this->current_drag_item, copyEvent(event, QEvent::GraphicsSceneDragLeave));
            this->current_drag_item = NULL;
        }
        event->accept();
    }
}

/**
 * @brief LedScene::dragLeaveEvent
 * @param event
 *
 * \todo ensure, that the LED is not removed from the scene
 */
void LedScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    //* only handle drag-events for LedItems
    if(LedItem::unpackDragData(event->mimeData()) == NULL){
        event->ignore();
        return;
    }
    //qDebug() << "scene drag leave";
    QGraphicsItem *itm = itemAt(event->scenePos(), QTransform());
    if(itm != NULL){
        //* This happens, if any QGraphicsItem is at the border of the scene and
        //* the dragging item leaves the scene hovering the item at the border.
        //* In our case the item at the border will be a LedGroupItem (maybe
        //* also a LedItem, but this is inside the LedGroupItem, so we don't
        //* care...
        //* Anyway, we have to tell the ledGroupItem, that the dragging item
        //* has left the group
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
            //* Note: i don't know, in which case this occurs, but i'll leave it here...
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
        LedGroupItem* grp = newGroup();
        grp->addLed(led);
        QPointF grp_pos(event->scenePos());
        grp->setX(grp->x() - (settings::leditem::width+settings::ledgroupitem::border) );
        grp->setY(grp->y() - (settings::leditem::height+settings::ledgroupitem::border) );
        grp->setPos(grp_pos);
    }
    if(mouseGrabberItem() != NULL){
        //qDebug() << "mousegrabber:" << mouseGrabberItem();
        QGraphicsScene::mouseReleaseEvent(new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMouseRelease));
    }
    event->accept();
}

/*void LedScene::selectedItemChanged(LuiItem *item)
{
    if(item != this->selected_item){
        this->selected_item = item;
        if(item == NULL){
            emit selectedItemStatusChanged(false);
            emit selectedGroupChanged("");
        } else {
            emit selectedItemStatusChanged(true);
            emit selectedItemColorChanged(item->color());
            LedGroupItem* group = qgraphicsitem_cast<LedGroupItem*>(item);
            if(group != NULL){
                emit selectedGroupChanged(group->name());
            } else {
                emit selectedGroupChanged("");
            }
        }
    }
}*/

/**
 * @brief Copies an QGraphicsSceneDragDropEvent
 * @param old_event The event to copy
 * @param type Type of the event to copy
 * @return The copy of the event
 * This copies an QGraphicsSceneDragDropEvent with the ability to change the
 * event type. This adds the functionality of passing the current event to
 * another slot.
 */
QGraphicsSceneDragDropEvent *LedScene::copyEvent(QGraphicsSceneDragDropEvent *old_event, QEvent::Type type)
{
    QGraphicsSceneDragDropEvent *new_event = new QGraphicsSceneDragDropEvent(type);
    new_event->setMimeData(old_event->mimeData());
    new_event->setPos(old_event->pos());
    return new_event;
}

QDataStream &operator<<(QDataStream &stream, const LedScene &scene){
    stream << (quint16)scene.groups.size();
    foreach(LedGroupItem* group, scene.groups){
        stream << *group;
    }
    return stream;
}

QDataStream &operator>>(QDataStream &stream, LedScene &scene){
    quint16 group_count;
    stream >> group_count;
    LedGroupItem *group;
    for(quint16 i=0;i<group_count;i++){
        group = scene.newGroup();
        stream >> *group;
    }
    return stream;
}
























