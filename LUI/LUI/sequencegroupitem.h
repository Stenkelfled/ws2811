#ifndef SEQUENCEGROUPITEM_H
#define SEQUENCEGROUPITEM_H
#include <QtWidgets>

#include <ledgroupitem.h>

class SequenceGroupItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit SequenceGroupItem(LedGroupItem *led_group, QGraphicsItem *parent = 0);
    ~SequenceGroupItem();
    LedGroupItem* led_group();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setVPos(int pos);

signals:

public slots:
    void changeName(QString new_name);

private:
    LedGroupItem *my_led_group;
    QRectF *my_rect;

};

#endif // SEQUENCEGROUPITEM_H
