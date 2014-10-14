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
    explicit LuiItem(const qint16 id, QGraphicsItem *parent = 0);
    explicit LuiItem(const qint16 id, int rect_width, int rect_height, QGraphicsItem *parent = 0);

    virtual QColor color();
    void changeId(int id);
    qint16 id();
    virtual void setColor(const QColor color);
    virtual QByteArray getUsbCmd();

    static bool positionLowerThan(const QGraphicsItem *itm1, const QGraphicsItem *itm2);

public slots:

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

signals:

private:
    void initFlags();
    qint16 my_id;

    friend QDataStream &operator<<(QDataStream &, const LuiItem &);
    friend QDataStream &operator>>(QDataStream &, LuiItem &);
};

#endif // LUIITEM_H
