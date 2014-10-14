#ifndef SEQUENCEITEM_H
#define SEQUENCEITEM_H

#include <QGraphicsRectItem>

class SequenceItem : public QGraphicsRectItem
{
public:
    enum { Type = UserType + 5 };
    explicit SequenceItem(QGraphicsItem *parent = 0);


};

#endif // SEQUENCEITEM_H
