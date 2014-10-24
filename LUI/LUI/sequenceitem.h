#ifndef SEQUENCEITEM_H
#define SEQUENCEITEM_H

#include <QtWidgets>

#include <settings.h>

class SequenceItem : public QGraphicsObject
{
    Q_OBJECT
public:
    enum { Type = UserType + usertype::sequenceitem };
    int type() const {return Type;}
    enum sequencetype {singlecolor, gradient};
    explicit SequenceItem(sequencetype type = singlecolor, QColor color = Qt::black, QColor gradient = Qt::black, QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QColor color() const;
    qreal width() const;
    void refreshGroupColor(int pos);

public slots:
    void setSingleColor(QColor color);

protected:
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void needs_color_selector();

private:
    sequencetype my_type;
    QColor my_color;
    QColor my_end_color;
    quint16 my_length;
};

#endif // SEQUENCEITEM_H
