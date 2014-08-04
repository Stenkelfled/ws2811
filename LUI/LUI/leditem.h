#ifndef LEDITEM_H
#define LEDITEM_H

#include <QGraphicsRectItem>

#include <settings.h>

class LedItem : public QGraphicsRectItem
{
public:
    explicit LedItem(int id, qreal x, qreal y);
    int id();

private:
    int my_id;

};

#endif // LEDITEM_H
