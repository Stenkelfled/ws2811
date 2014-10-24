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

public slots:
    void newGroup(LedGroupItem *led_group);
    void removeGroup(LedGroupItem *led_group);
    void refreshGroupColors();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);

private:
    int vpos(int idx);
    QList<SequenceGroupItem*> *my_sequences;
    SequenceCursorItem *my_cursor;
    int my_pixels_per_10ms;

};

#endif // SEQUENCESCENE_H
