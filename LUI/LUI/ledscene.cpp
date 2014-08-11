#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QtDebug>

#include <global.h>
#include <groupitem.h>
#include <leditem.h>
#include <luiitem.h>
#include "ledscene.h"

LedScene::LedScene(QObject *parent) :
    QGraphicsScene(parent)
{
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
        }
    }
}

void LedScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug() << "Mouse press on scene: " << event->scenePos();
    if(itemAt(event->scenePos(), QTransform()) != NULL){
        QGraphicsScene::mousePressEvent(event);
    } else {
        foreach(QGraphicsItem *itm, selectedItems()){
            itm->setSelected(false);
        }
    }
}

