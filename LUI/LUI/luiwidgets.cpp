#include <QColorDialog>
#include <QtDebug>

#include <luiwidgets.h>
#include <settings.h>
#include <utils.h>

LuiColorLabel::LuiColorLabel(QWidget *parent) :
    QLabel(parent)
{
}

void LuiColorLabel::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    QPalette p = this->palette();
    //qDebug() << "color:" << p.color(QPalette::Window);
    emit clickedColor(p.color(QPalette::Window).toHsv());
}


LuiColorLabelCustom::LuiColorLabelCustom(QWidget *parent) :
    QLabel(parent)
{
}

void LuiColorLabelCustom::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    //const QColorDialog::ColorDialogOptions options = QFlag(colorDialogOptionsWidget->value());
    const QColor c = QColorDialog::getColor(Qt::green, this, "Farbe auswählen");//, options);

    if (c.isValid()) {
        emit clickedColor(c.toHsv());
    }
}


LuiColorDisplay::LuiColorDisplay(QWidget *parent):
    QLabel(parent)
{
    QColor c(Qt::black);
    this->my_color = c.toHsv();
}

QColor LuiColorDisplay::color()
{
    return this->my_color;
}

void LuiColorDisplay::setColor(QColor color)
{
    this->my_color = color;
    QPalette p = palette();
    p.setBrush(QPalette::Window, utils::toDisplayColor(color));
    setPalette(p);
}
