#ifndef SEQUENCEITEM_H
#define SEQUENCEITEM_H

#include <QGraphicsRectItem>
#include <QtWidgets>

class SequenceItem : public QGraphicsItem
{
public:
    enum { Type = UserType + 5 };
    enum sequencetype {singlecolor, gradient};
    explicit SequenceItem(sequencetype type = singlecolor, QColor color = Qt::black, QColor gradient = Qt::black, QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    qreal width() const;

private:
    sequencetype my_type;
    QColor my_color;
    QColor my_end_color;
    quint16 my_length;
};

#endif // SEQUENCEITEM_H
