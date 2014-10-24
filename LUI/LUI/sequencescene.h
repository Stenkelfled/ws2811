#ifndef SEQUENCESCENE_H
#define SEQUENCESCENE_H

#include <QGraphicsScene>

#include <ledgroupitem.h>
#include <sequencecursoritem.h>
#include <sequenceitem.h>
#include <sequencegroupitem.h>

class SequenceScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit SequenceScene(QObject *parent = 0);
    ~SequenceScene();
    void clear();
    int pixelsPer10ms() const;

signals:
    void sequenceItemSelected(SequenceItem* item);
    void sequenceGroupItemSelected(bool status);

public slots:
    void newGroup(LedGroupItem *led_group);
    void removeGroup(LedGroupItem *led_group);
    void refreshGroupColors();
    void newSequence();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);

private:
    int vpos(int idx);
    QList<SequenceGroupItem*> *my_sequence_groups;
    SequenceCursorItem *my_cursor;
    SequenceGroupItem *my_selected_group_item;
    int my_pixels_per_10ms;

};

#endif // SEQUENCESCENE_H
