#ifndef LEDITEM_H
#define LEDITEM_H

#include <QVariant>
#include <QtWidgets>

#include <luiitem.h>
#include <settings.h>

class LedItem : public LuiItem
{
    Q_OBJECT
public:
    enum { Type = UserType + usertype::leditem };
    explicit LedItem(qint16 id, QGraphicsItem *parent = 0);
    int type() const {return Type;}

    QRectF boundingRect() const;
    QColor color();
    bool hasColorFromGroup();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setColor(QColor color);
    void setColor(QColor color, bool from_group);
    static LedItem* unpackDragData(const QMimeData *data);

protected:
    //virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    //virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    //virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    //virtual void focusInEvent(QFocusEvent * event);
    //virtual void focusOutEvent(QFocusEvent * event);

signals:
    //void itemMoves(bool state); //is emitted if the item begins or stops moving; true, on begin
    void ledDroppedOutside(QMimeData *led_mime_data);

private:
    bool is_dragging;
    bool my_color_from_group;
    QColor my_color;

    friend QDataStream &operator<<(QDataStream &, const LedItem &);
    friend QDataStream &operator>>(QDataStream &, LedItem &);
};

#endif // LEDITEM_H
