#ifndef SEQUENCECURSORITEM_H
#define SEQUENCECURSORITEM_H

#include <QtWidgets>

#include <settings.h>

class SequenceCursorItem : public QGraphicsItem
{
public:
    enum { Type = UserType + usertype::sequencecursoritem };
    int type() const {return Type;}
    explicit SequenceCursorItem(QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setHeight(int groups);

private:
    QRect my_rect;

};

#endif // SEQUENCECURSORITEM_H
