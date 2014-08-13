#include <QColorDialog>
#include <QtDebug>

#include <luiwidgets.h>

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


LuiColorLabelCustom::LuiColorLabelCustom(QWidget *parent) :
    QLabel(parent)
{
}

void LuiColorLabelCustom::mousePressEvent(QMouseEvent *event)
{
    //const QColorDialog::ColorDialogOptions options = QFlag(colorDialogOptionsWidget->value());
    const QColor c = QColorDialog::getColor(Qt::green, this, "Farbe auswählen");//, options);

    if (c.isValid()) {
        emit clickedColor(c);
    }
}
