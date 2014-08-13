#ifndef LUIITEM_H
#define LUIITEM_H

#include <QGraphicsRectItem>
#include <QObject>

enum class LuiItemType: char {none, led, group};

class LuiItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    explicit LuiItem(int id, QGraphicsItem *parent = 0);
    explicit LuiItem(int id, qreal x, qreal y, int rect_width, int rect_height, QGraphicsItem *parent = 0);
    QColor color();
    int id();
    virtual LuiItemType luitype();
    void setColor(QColor color);

public slots:

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

signals:

private:
    void initFlags();
    int my_id;

};

#endif // LUIITEM_H
