#include <QBrush>
#include <QFont>
#include <QPen>
#include <QtDebug>

#include "leditem.h"



LedItem::LedItem(int id, qreal x, qreal y, QGraphicsItem *parent):
    LuiItem(id, x, y, settings::leditem::width, settings::leditem::height, parent)
{
    setBrush(QBrush(settings::leditem::color));
    setPen(QPen(Qt::NoPen));
    this->id_text = new QGraphicsTextItem(QString::number(this->id()), this); //items will be removed automtically on led deletion
    this->id_text->setFont(QFont("MS Shell Dlg 2", settings::leditem::height/2, QFont::DemiBold));
    this->id_text->setPos(this->rect().center() - this->id_text->boundingRect().center());
    updateTextColor();
    //setAcceptedMouseButtons(Qt::LeftButton);
}

void LedItem::updateTextColor(){
    QColor col = this->brush().color();
    QColor color(Qt::white);
    if( (col.redF()*0.229 + col.greenF()*0.587 + col.blueF()*0.114) > 186.0/360.0){
        color = Qt::black;
    }
    this->id_text->setDefaultTextColor(color);
}

void LedItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Mouse press on Led" << this->id();
    setSelected(true);
}
