#ifndef GROUPITEM_H
#define GROUPITEM_H
#include <QtWidgets>

#include <leditem.h>
#include <luiitem.h>
#include <sequenceitem.h>
#include <settings.h>

class LedGroupItem : public LuiItem
{
    Q_OBJECT
public:
    enum groupAlignment {horizontal, vertical};
    enum { Type = UserType + usertype::ledgroupitem };
    int type() const {return Type;}
    explicit LedGroupItem(qint16 id, QGraphicsItem *parent = 0);
    ~LedGroupItem();

    quint16 rows() const;
    void addLed(LedItem *led, qint16 row = 0);
    void removeLed(LedItem *led);
    QPoint indexOf(LedItem *led);
    void makeEmpty();
    QColor color();
    void setColor(QColor color);
    QString name();
    void setName(QString const name);
    QList<SequenceItem*>* sequences();
    void newSequence();

    virtual QByteArray getUsbCmd();
    //LedItem* unpackDragData(const QMimeData *data);

public slots:
    void refreshArea(bool plus_extra_space=false);

protected:
    //virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent * event);
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent * event);
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
    virtual void dropEvent(QGraphicsSceneDragDropEvent * event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);

signals:
    void groupEmpty(LedGroupItem* group);
    void nameChanged(QString name, LedGroupItem *group);
    void selectionChanged(bool state, LedGroupItem *group);
    void addedSequenceItem(SequenceItem* item);

private slots:
    void horAlign();
    void verAlign();
    void getName();

private:

    QList<QList<LedItem*>*> *leds; //list(pointer) of row(pointer)s of led(pointer)s
    QList<SequenceItem*> *my_sequences;
    QColor my_color;
    groupAlignment my_alignment;
    QString my_name;

    QAction *horAlignAct;
    QAction *verAlignAct;
    QActionGroup *alignActGroup;
    QAction *nameAct;

    friend QDataStream &operator<<(QDataStream &, const LedGroupItem &);
    friend QDataStream &operator>>(QDataStream &, LedGroupItem &);
    friend QDataStream &operator<<(QDataStream &, const LedGroupItem::groupAlignment &);
    friend QDataStream &operator>>(QDataStream &, LedGroupItem::groupAlignment &);
};

#endif // GROUPITEM_H
