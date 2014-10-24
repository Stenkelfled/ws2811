#include "sequencescene.h"
#include <settings.h>

SequenceScene::SequenceScene(QObject *parent) :
    QGraphicsScene(parent),
    my_sequences(new QList<SequenceGroupItem*>),
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
    delete this->my_sequences;
}

void SequenceScene::clear()
{
    foreach(SequenceGroupItem* item, *(this->my_sequences)){
        delete item;
    }
    this->my_sequences->clear();
}

int SequenceScene::pixelsPer10ms() const
{
    return (int const)(this->my_pixels_per_10ms);
}

void SequenceScene::newGroup(LedGroupItem *led_group)
{
    SequenceGroupItem *seq = new SequenceGroupItem(led_group);
    seq->setVPos( vpos(this->my_sequences->length()) );
    this->my_sequences->append(seq);
    addItem(seq);
    seq->initItems();
    this->my_cursor->setHeight(this->my_sequences->length());
    //qDebug() << "new Group. total:" << this->my_sequences->length();
}

void SequenceScene::removeGroup(LedGroupItem *led_group)
{
    SequenceGroupItem *sequence_group;
    foreach(SequenceGroupItem *item, *(this->my_sequences)){
        if(item->led_group() == led_group){
            sequence_group = item;
            break;
        }
    }
    this->my_sequences->removeAll(sequence_group);
    delete sequence_group;
    //qDebug() << "remove Group. total:" << this->my_sequences->length();
    for(int i=0; i<this->my_sequences->length(); i++){
        this->my_sequences->at(i)->setVPos( vpos(i) );
    }
    this->my_cursor->setHeight(this->my_sequences->length());
}

void SequenceScene::refreshGroupColors()
{
    this->my_cursor->refreshColors();
}

void SequenceScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mousePressEvent(mouseEvent);
    QList<QGraphicsItem*> sel_items = selectedItems();
    if(sel_items.length() == 1){
        SequenceItem *item = qgraphicsitem_cast<SequenceItem*>(sel_items.at(0));
        if(item != NULL){
            emit sequenceItemSelected(item);
            //qDebug() << "scene: item selected";
        } else {
            emit sequenceItemSelected(NULL);
            //qDebug() << "scene: wrong item selected";
        }
    } else {
        emit sequenceItemSelected(NULL);
        //qDebug() << "scene: no item selected";
    }
}

int SequenceScene::vpos(int idx)
{
    return idx*(settings::sequencegroupitem::height + settings::sequencegroupitem::group_space);
}
