#ifndef LUIITEM_H
#define LUIITEM_H

#include <QGraphicsRectItem>
#include <QObject>

class LuiItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 1 };
    int type() const {return Type;}

    explicit LuiItem(int id, QGraphicsItem *parent = 0);
    explicit LuiItem(int id, qreal x, qreal y, int rect_width, int rect_height, QGraphicsItem *parent = 0);
    virtual QColor color();
    void changeId(int id);
    int id();
    virtual void setColor(QColor color);
    virtual QByteArray getUsbCmd();

public slots:

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

signals:

private:
    void initFlags();
    int my_id;

};

#endif // LUIITEM_H
