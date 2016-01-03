#include <QApplication>
#include <QtDebug>

#include "leditem.h"
#include <utils.h>



LedItem::LedItem(qint16 id, QGraphicsItem *parent):
    LuiItem(id, settings::leditem::width, settings::leditem::height, parent),
    is_dragging(false),
    my_color_from_group(true)//,
    //group_index(QPoint(-1,-1))
{
    setAcceptedMouseButtons(Qt::LeftButton);
    //setFlag(QGraphicsItem::ItemIsFocusable, true);
}

QRectF LedItem::boundingRect() const
{
    return QRectF(0, 0, //-settings::leditem::width/2-0.5, -settings::leditem::height/2-0.5,
                  settings::leditem::width, settings::leditem::height);
}

QColor LedItem::color()
{
    return this->my_color;
}

bool LedItem::hasColorFromGroup()
{
    return this->my_color_from_group;
}

void LedItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    if(this->parentItem() == NULL){
        return;
    }
    QColor display_color = utils::toDisplayColor(this->color());
    if(this->is_dragging){
        painter->setPen(QPen(QBrush(display_color), 2, Qt::SolidLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(2, 2, settings::leditem::height-4, settings::leditem::width-4);
    } else {
        painter->setPen(Qt::NoPen);
        painter->setBrush(display_color);
        painter->drawRect(0.5, 0.5, settings::leditem::height, settings::leditem::width);
        QColor text_color(Qt::white);
        if( utils::isBrightBackground(display_color) ){
            text_color = Qt::black;
        }
        painter->setPen(text_color);
        painter->setFont(QFont("MS Shell Dlg 2", settings::leditem::height/2, QFont::DemiBold));
        painter->drawText(this->boundingRect(), Qt::AlignCenter, QString::number(this->id()));

        if(option->state & QStyle::State_Selected){
            painter->setPen(QPen(QBrush(text_color), 1, Qt::DashLine));
            painter->setBrush(Qt::NoBrush);
            painter->drawRect(0, 0, settings::leditem::height-1, settings::leditem::width-1);
        }
    }
}

void LedItem::setColor(QColor color)
{
    this->setColor(color, false);
}

void LedItem::setColor(QColor color, bool from_group)
{
    this->my_color = color;
    update();
    this->my_color_from_group = from_group;
}

LedItem *LedItem::unpackDragData(const QMimeData *data)
{
    if(!data->hasText()){
        return NULL;
    }
    if(data->text().compare("Led") != 0){
        return NULL;
    }
    bool ok;
    LedItem* led = (LedItem*)(data->data(settings::leditem::mimetype).toULongLong(&ok));
    if(!ok){
        return NULL;
    }
    return led;
}

/*void LedItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "led mouse press";
    //QPixmap pixmap(10,10);
    //pixmap.fill(Qt::white);
    LuiItem::mousePressEvent(event);
}*/

void LedItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length() < QApplication::startDragDistance()) {
        event->ignore();
        return;
    }

    event->accept();
    QDrag *drag = new QDrag(event->widget());
    QMimeData *mime = new QMimeData;
    drag->setMimeData(mime);
    mime->setText("Led");
    qintptr my_address = (qintptr)this;
    mime->setData(settings::leditem::mimetype, QByteArray(QByteArray::number(my_address)));

    QPixmap pixmap(settings::leditem::height, settings::leditem::width);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(this->color());
    painter.drawRect(0,0,settings::leditem::height-1, settings::leditem::width-1);
    painter.end();
    pixmap.setMask(pixmap.createHeuristicMask());
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(settings::leditem::height/2, settings::leditem::width/2));
#if 0
    qDebug() << "my recursion:";
    QObject *target = this;
    while(target){
        qDebug() << "next:" << target;
        target = target->parent();
    }
    qDebug() << "recursion done.";
    qDebug() << "scene:" << this->scene();
    qDebug() << "views:";
    QList<QGraphicsView *> my_views = this->scene()->views();
    foreach(QGraphicsView *view, my_views){
        qDebug() << view;
    }
    qDebug() << "views end";
#endif
    this->is_dragging = true;
    update();

    drag->exec();
/*    if(drag->exec(Qt::MoveAction) != Qt::MoveAction){
        qDebug() << "led dropped outside. Start:" << event->buttonDownPos(Qt::LeftButton);
        //emit ledDroppedOutside();
        this->setPos(event->buttonDownPos(Qt::LeftButton));
        this->show();
    }
    target = drag->target();
    while(target){
        qDebug() << "drop to:" << target;
        target = target->parent();
    }
    qDebug() << "recursion done.";
*/

    /* Drag finished. Now check, if one of the target widgets is the view,
     * that contains the scene of the LED.*/
    bool my_view_found = false;
    QList<QGraphicsView *> my_views = this->scene()->views();
    QObject* target = drag->target();
    while(target && !my_view_found){
        foreach(QGraphicsView *view, my_views){
            if(target == view){
                my_view_found = true;
            }
        }
        target = target->parent();
    }
    if(!my_view_found){
        emit ledDroppedOutside(mime);
        //qDebug() << "Led" << (QObject*)this << "dropped outside its view(s).";
    }
    this->is_dragging = false;
    update();

    //QGraphicsRectItem::mouseMoveEvent(event);
}

/*void LedItem::focusInEvent(QFocusEvent *event)
{
    qDebug() << "led focus in";
    LuiItem::focusInEvent(event);
}

void LedItem::focusOutEvent(QFocusEvent *event)
{
    qDebug() << "led focus out";
    LuiItem::focusOutEvent(event);
}*/

/*void LedItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "led release";
    //this->is_dragging = false;
    LuiItem::mouseReleaseEvent(event);
}*/

/*QVariant LedItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == QGraphicsItem::ItemPositionHasChanged){
        qDebug() << "led pos changed" << this->pos();
    }
    return QGraphicsRectItem::itemChange(change, value);
}*/

QDataStream &operator<<(QDataStream &stream, const LedItem &led)
{
    stream << (LuiItem&)(led);
    stream << led.my_color;
    stream << led.my_color_from_group;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, LedItem &led)
{
    stream >> (LuiItem&)(led);
    stream >> led.my_color;
    stream >> led.my_color_from_group;
    return stream;
}

































