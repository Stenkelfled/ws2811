#include <QApplication>
#include <QBrush>
#include <QFont>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPen>
#include <QtDebug>
#include <QtWidgets>

#include "leditem.h"



LedItem::LedItem(int id, QGraphicsItem *parent):
    LuiItem(id, settings::leditem::width, settings::leditem::height, parent),
    is_moving(false),
    color_from_group(true),
    group_index(QPoint(-1,-1))
{
    setAcceptedMouseButtons(Qt::LeftButton);
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

QPoint LedItem::groupIndex()
{
    return this->group_index;
}

bool LedItem::hasColorFromGroup()
{
    return this->color_from_group;
}

void LedItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    painter->setPen(Qt::NoPen);
    painter->setBrush(this->color());
    painter->drawRect(0.5, 0.5, settings::leditem::height, settings::leditem::width);
    QColor text_color(Qt::white);
    if( (this->color().redF()*0.229 + this->color().greenF()*0.587 + this->color().blueF()*0.114) > 186.0/360.0){
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

void LedItem::setColor(QColor color)
{
    this->setColor(color, false);
}

void LedItem::setColor(QColor color, bool from_group)
{
    this->my_color = color;
    if(color.value() != 0){
        color.setHsv(color.hue(), color.saturation(), DISPLAY_HSV(color.value()));
    }
    update();
    this->color_from_group = from_group;
}

void LedItem::setGroupIndex(QPoint pos)
{
    this->group_index = pos;
}

void LedItem::setGroupIndex(int x, int y)
{
    this->group_index.setX(x);
    this->group_index.setY(y);
}

/*void LedItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPixmap pixmap(10,10);
    pixmap.fill(Qt::white);
    LuiItem::mousePressEvent(event);
}*/

void LedItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length() < QApplication::startDragDistance()) {
        return;
    }
    QDrag *drag = new QDrag(event->widget());
    QMimeData *mime = new QMimeData;
    drag->setMimeData(mime);
    mime->setText("Led"+this->id());

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

    drag->exec();

    QGraphicsRectItem::mouseMoveEvent(event);
}

/*void LedItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->is_moving){
        this->is_moving = false;
        emit itemMoves(false);
    }
    QGraphicsRectItem::mouseReleaseEvent(event);
}

*/QVariant LedItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    /*if(change==QGraphicsItem::ItemSelectedHasChanged || change==QGraphicsItem::ItemVisibleHasChanged){
        this->is_moving = false;
    }*/
    if(change == QGraphicsItem::ItemPositionHasChanged){
        qDebug() << "led pos changed" << this->pos();
        /*if(this->is_moving == false){
            this->is_moving = true;
            emit itemMoves(true);
        }*/
    }
    return QGraphicsRectItem::itemChange(change, value);
}

















