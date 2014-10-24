#ifndef SEQUENCEGROUPITEM_H
#define SEQUENCEGROUPITEM_H
#include <QtWidgets>

#include <ledgroupitem.h>
#include <sequenceitem.h>
#include <settings.h>

class SequenceGroupItem : public QGraphicsObject
{
    Q_OBJECT
public:
    enum { Type = UserType + usertype::sequencegroupitem };
    enum colortype {fixed, gradient};
    int type() const {return Type;}
    explicit SequenceGroupItem(LedGroupItem *led_group, colortype col_type = SequenceGroupItem::fixed, QGraphicsItem *parent = 0);
    ~SequenceGroupItem();
    LedGroupItem* led_group();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void initItems(); //this has to be done AFTER the group was added to the scene
    void setVPos(int pos);
    void refreshGroupColor(QColor color);

signals:

public slots:
    void changeName(QString new_name);
    void addSequenceItem(SequenceItem *item);

private:
    LedGroupItem *my_led_group;
    colortype my_colortype;
    QRectF *my_rect;

};

#endif // SEQUENCEGROUPITEM_H
