#ifndef LUIITEM_H
#define LUIITEM_H

#include <QGraphicsRectItem>

class LuiItem : public QGraphicsRectItem
{
public:
    explicit LuiItem(int id, QGraphicsItem *parent = 0);
    explicit LuiItem(int id, qreal x, qreal y, int rect_width, int rect_height, QGraphicsItem *parent = 0);
    int id();
    //void setSelected(bool selected);

protected:

private:
    int my_id;

};

#endif // LUIITEM_H
