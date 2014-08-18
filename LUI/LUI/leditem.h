#ifndef LEDITEM_H
#define LEDITEM_H

#include <QVariant>

#include <luiitem.h>
#include <settings.h>

class LedItem : public LuiItem
{
    Q_OBJECT
public:
    enum { Type = UserType + 3 };
    int type() const {return Type;}

    explicit LedItem(int id, QGraphicsItem *parent = 0);
    QColor color();
    bool hasColorFromGroup();
    void setColor(QColor color);
    void setColor(QColor color, bool from_group);
    void updateTextColor();

protected:
    //void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);

signals:
    void itemMoved();

private:
    QGraphicsTextItem *id_text;
    bool is_moving;
    bool color_from_group;
    QColor my_color;
};

#endif // LEDITEM_H
