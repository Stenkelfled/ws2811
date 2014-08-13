#include <QtDebug>

#include "luicolorlabel.h"

LuiColorLabel::LuiColorLabel(QWidget *parent) :
    QLabel(parent)
{
}

void LuiColorLabel::mousePressEvent(QMouseEvent *event)
{
    QPalette p = this->palette();
    //qDebug() << "color:" << p.color(QPalette::Window);
    emit clickedColor(p.color(QPalette::Window));
}
