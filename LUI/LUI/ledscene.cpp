#include <global.h>
#include <groupitem.h>
#include <leditem.h>
#include "ledscene.h"

LedScene::LedScene(QObject *parent) :
    QGraphicsScene(parent)
{
    GroupItem *grp = new GroupItem(0, NULL);
    addItem(grp);
    for(int i=0; i<GLOBAL_LED_COUNT; i++){
        LedItem *led = new LedItem(i, i*(settings::leditem::width + settings::leditem::spacing), 0); //items will be removed automatically on scene deletion
        grp->addLed(led);
    }
}
