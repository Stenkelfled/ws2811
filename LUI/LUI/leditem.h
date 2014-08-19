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
    QPoint groupIndex();
    bool hasColorFromGroup();
    void setColor(QColor color);
    void setColor(QColor color, bool from_group);
    void setGroupIndex(QPoint pos);
    void setGroupIndex(int x, int y);
    void updateTextColor();

protected:
    //virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);

signals:
    void itemMoves(bool state); //is emitted if the item begins or stops moving; true, on begin
    //void itemMoved();

private:
    QGraphicsTextItem *id_text;
    bool is_moving;
    bool color_from_group;
    QPoint group_index;
    QColor my_color;
};

#endif // LEDITEM_H
