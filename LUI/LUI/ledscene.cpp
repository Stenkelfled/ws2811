#include <global.h>
#include <leditem.h>
#include "ledscene.h"

LedScene::LedScene(QObject *parent) :
    QGraphicsScene(parent)
{
    for(int i=0; i<GLOBAL_LED_COUNT; i++){
        LedItem *led = new LedItem(i, i*(settings::leditem::width + settings::leditem::spacing), 0);
        addItem(led);
    }
}

LedScene::~LedScene(){
    foreach( QGraphicsItem *item, items()){
        delete item;
    }
}
