#include <lui.h>
#include "sequencescene.h"
#include <settings.h>

SequenceScene::SequenceScene(QObject *parent) :
    QGraphicsScene(parent),
    my_sequence_groups(new QList<SequenceGroupItem*>),
    my_cursor(new SequenceCursorItem),
    my_pixels_per_10ms(settings::sequenceitem::pixels_per_10ms)
{
    this->addItem(my_cursor);
    my_cursor->setPos(settings::sequencecursoritem::xmin, -settings::sequencecursoritem::additional_height);
    my_cursor->setZValue(1000);
}

SequenceScene::~SequenceScene()
{
    this->clear();
    delete this->my_sequence_groups;
}

void SequenceScene::clear()
{
    foreach(SequenceGroupItem* item, *(this->my_sequence_groups)){
        delete item;
    }
    this->my_sequence_groups->clear();
}

int SequenceScene::pixelsPer10ms() const
{
    return (int const)(this->my_pixels_per_10ms);
}

void SequenceScene::newGroup(LedGroupItem *led_group)
{
    SequenceGroupItem *grp = new SequenceGroupItem(led_group);
    grp->setVPos( vpos(this->my_sequence_groups->length()) );
    this->my_sequence_groups->append(grp);
    addItem(grp);
    grp->refreshItemPotitions();
    this->my_cursor->setHeight(this->my_sequence_groups->length());
    //qDebug() << "new Group. total:" << this->my_sequences->length();
}

void SequenceScene::removeGroup(LedGroupItem *led_group)
{
    SequenceGroupItem *sequence_group;
    foreach(SequenceGroupItem *item, *(this->my_sequence_groups)){
        if(item->led_group() == led_group){
            sequence_group = item;
            break;
        }
    }
    this->my_sequence_groups->removeAll(sequence_group);
    delete sequence_group;
    //qDebug() << "remove Group. total:" << this->my_sequences->length();
    for(int i=0; i<this->my_sequence_groups->length(); i++){
        this->my_sequence_groups->at(i)->setVPos( vpos(i) );
    }
    this->my_cursor->setHeight(this->my_sequence_groups->length());
}

void SequenceScene::refreshGroupColors()
{
    this->my_cursor->refreshColors();
}

void SequenceScene::newSequence()
{
    this->my_selected_group_item->newSequence();
}

void SequenceScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mousePressEvent(mouseEvent);
    QList<QGraphicsItem*> sel_items = selectedItems();
    if(sel_items.length() == 1){
        SequenceItem *sequence_item = qgraphicsitem_cast<SequenceItem*>(sel_items.at(0));
        if(sequence_item != NULL){
            //SequenceItem selected
            emit sequenceItemSelected(sequence_item);
        } else {
            emit sequenceItemSelected(NULL);
        }

        SequenceGroupItem *sequence_group_item = qgraphicsitem_cast<SequenceGroupItem*>(sel_items.at(0));
        if(sequence_group_item != NULL){
            //SequenceGroupItem selected
            emit sequenceGroupItemSelected(true);
            this->my_selected_group_item = sequence_group_item;
        } else {
            emit sequenceGroupItemSelected(false);
            this->my_selected_group_item = NULL;
        }
    } else {
        emit sequenceItemSelected(NULL);
        emit sequenceGroupItemSelected(false);
    }
}

int SequenceScene::vpos(int idx)
{
    return idx*(settings::sequencegroupitem::height + settings::sequencegroupitem::group_space);
}
