#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QtDebug>

#include <global.h>
#include <groupitem.h>
#include <leditem.h>
#include <luiitem.h>
#include <ledscene.h>
#include <settings.h>

LedScene::LedScene(QObject *parent) :
    QGraphicsScene(parent),
    groups(new QList<GroupItem*>),
    selection_start(settings::ledscene::invalid_pos),
    selected_item(NULL)
{
    this->selection_rect_item = addRect(0, 0, 0, 0, QPen(Qt::black), QBrush(Qt::NoBrush));
    GroupItem* grp = newGroup();
    for(int i=0; i<GLOBAL_LED_COUNT; i++){
        LedItem *led = new LedItem(i, i*(settings::leditem::width + settings::leditem::spacing), 0); //items will be removed automatically on scene deletion
        grp->addLed(led);
    }
}

void LedScene::group()
{
    GroupItem *new_grp = newGroup();
    foreach(QGraphicsItem *itm, selectedItems()){
        LuiItem *lui = (LuiItem*)itm;
        if(lui->luitype() == LuiItemType::led){
            LedItem *led = (LedItem*)lui;
            GroupItem *old_grp = (GroupItem*) led->parentItem();
            if(old_grp != NULL){
                old_grp->removeLed(led);
            }
            new_grp->addLed(led);
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

void LedScene::ungroup()
{
    foreach(QGraphicsItem *itm, selectedItems()){
        LuiItem *lui = ((LuiItem*)itm);
        switch(lui->luitype()){
            case LuiItemType::led:
                {
                GroupItem *grp = (GroupItem*)lui->parentItem();
                if(grp != NULL){
                    grp->removeLed((LedItem*)lui);
                }
                break;
                }
            case LuiItemType::group:
                ((GroupItem*)lui)->makeEmpty();
            case LuiItemType::none:
                //do nothing
                break;
        }
    }
}

void LedScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
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

void LedScene::selectedItemChanged(LuiItem *item)
{
    if(item != this->selected_item){
        if(item == NULL){
            emit selectedItemStatusChanged(false);
        } else {
            emit selectedItemStatusChanged(true);
            emit selectedItemColorChanged(item->color());
        }
        this->selected_item = item;
    }
}

