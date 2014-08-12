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
    selection_start(settings::ledscene::invalid_pos)
{
    this->selection_rect_item = addRect(0, 0, 0, 0, QPen(Qt::black), QBrush(Qt::NoBrush));
    GroupItem *grp = new GroupItem(0, NULL);
    connect(grp, SIGNAL(groupEmpty(GroupItem*)), this, SLOT(removeGroup(GroupItem*)));
    addItem(grp);
    for(int i=0; i<GLOBAL_LED_COUNT; i++){
        LedItem *led = new LedItem(i, i*(settings::leditem::width + settings::leditem::spacing), 0); //items will be removed automatically on scene deletion
        grp->addLed(led);
    }
}

void LedScene::group()
{
    GroupItem *new_grp = new GroupItem(0, NULL);
    connect(new_grp, SIGNAL(groupEmpty(GroupItem*)), this, SLOT(removeGroup(GroupItem*)));
    addItem(new_grp);
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
    removeItem((QGraphicsItem*)group);
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
    if(itemAt(event->scenePos(), QTransform()) != NULL){
        QGraphicsScene::mousePressEvent(event);
    } else {
        this->selection_rect_item->setRect(QRectF(event->scenePos(),QSizeF(0.1, 0.1)));
        this->selection_start = event->scenePos();
        /*foreach(QGraphicsItem *itm, selectedItems()){
            itm->setSelected(false);
        }*/
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
    QPainterPath sel_path = QPainterPath();
    sel_path.addRect(this->selection_rect_item->rect());
    setSelectionArea(sel_path, Qt::ContainsItemShape);
    this->selection_rect_item->setRect(QRectF(0, 0, 0, 0));
    this->selection_start = settings::ledscene::invalid_pos;
    QGraphicsScene::mouseReleaseEvent(event);
}

