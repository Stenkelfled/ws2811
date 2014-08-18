#include <QComboBox>
#include <QGraphicsItem>
#include <QObject>
#include <QPalette>
#include <QtDebug>

#include <global.h>
#include <protocoll.h>
#include "lui.h"
#include "ui_lui.h"

Lui::Lui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Lui),
    led_count(GLOBAL_LED_COUNT)
{
    this->ui->setupUi(this);
    this->serial = new Serial(this);
    QObject::connect(this->ui->refreshPushButton, &QPushButton::clicked, this->serial, &Serial::refreshPortData);
    QObject::connect(this->serial, &Serial::updatedPortDescription, this, &Lui::updatePortComboBox);
    QObject::connect(this->ui->portComboBox, SIGNAL(currentIndexChanged(int)), this->serial, SLOT(setCurrentPort(int)));
    QObject::connect(this->serial, &Serial::currentPortChanged, this->ui->portComboBox, &QComboBox::setCurrentIndex);
    QObject::connect(this->serial, SIGNAL(bytesWritten(qint64)), this, SLOT(enableTransmitPushButton()));
    this->serial->refreshPortData();

    this->scene = new LedScene(this->ui->ledView);
    this->ui->ledView->setScene(this->scene);
    connect(this->scene, SIGNAL(selectedItemStatusChanged(bool)), this, SLOT(colorDisplayEnable(bool)));
    connect(this->scene, SIGNAL(selectedItemColorChanged(QColor)), this, SLOT(colorDisplayChange(QColor)));
    connect(this, SIGNAL(colorChanged(QColor)), this->scene, SLOT(updateColor(QColor)));

    connect(this->ui->color_white, SIGNAL(clickedColor(QColor)), this, SLOT(newLedColor(QColor)));
    connect(this->ui->color_off, SIGNAL(clickedColor(QColor)), this, SLOT(newLedColor(QColor)));
    connect(this->ui->color_red, SIGNAL(clickedColor(QColor)), this, SLOT(newLedColor(QColor)));
    connect(this->ui->color_blue, SIGNAL(clickedColor(QColor)), this, SLOT(newLedColor(QColor)));
    connect(this->ui->color_green, SIGNAL(clickedColor(QColor)), this, SLOT(newLedColor(QColor)));
    connect(this->ui->color_cyan, SIGNAL(clickedColor(QColor)), this, SLOT(newLedColor(QColor)));
    connect(this->ui->color_yellow, SIGNAL(clickedColor(QColor)), this, SLOT(newLedColor(QColor)));
    connect(this->ui->color_orange, SIGNAL(clickedColor(QColor)), this, SLOT(newLedColor(QColor)));
    connect(this->ui->color_magenta, SIGNAL(clickedColor(QColor)), this, SLOT(newLedColor(QColor)));
    connect(this->ui->color_custom, SIGNAL(clickedColor(QColor)), this, SLOT(newLedColor(QColor)));
}

Lui::~Lui()
{
    delete this->scene;
    delete this->serial;
    delete this->ui;
}




/////////////////////////////////////////////////////////////////////////////
/// Stuff for communicate with uC
///

void Lui::updatePortComboBox(QStringList &items){
    this->ui->portComboBox->clear();
    this->ui->portComboBox->addItems(items);
    for(int i=0; i<items.length(); i++){
        this->ui->portComboBox->setItemData(i, items[i], Qt::ToolTipRole);
    }
}

void Lui::enableTransmitPushButton(){
    if(!this->serial->isOpen()){
        this->ui->transmitPushButton->setEnabled(true);
    }
}

void Lui::colorDisplayEnable(bool status)
{
    this->ui->color->setEnabled(status);
    /*this->ui->color_display_group->setEnabled(status);
    this->ui->brightness->setEnabled(status);*/
    this->ui->brightness_slider->setEnabled(status);
}

void Lui::colorDisplayChange(QColor color)
{
   if(this->ui->color->isEnabled()){
       this->ui->color_display->setColor(color);
       qDebug() << "display color" << this->ui->color_display->color();
       this->ui->brightness_slider->setSliderPosition(color.value());
   }
}

void Lui::on_transmitPushButton_clicked()
{
    /*QByteArray cmd = QByteArray(USB_PREAMBLE, USB_PREAMBLE_LEN); //TODO: build command to send...
    if(this->serial->openAndWrite(cmd)){
        this->ui->transmitPushButton->setEnabled(false);
    }*/
    QByteArray cmd = QByteArray();
    foreach(QGraphicsItem *itm, this->scene->items()){
        QGraphicsItem::GraphicsItemFlags flags = itm->flags();
        if(flags & QGraphicsItem::ItemIsSelectable){
            LuiItem *group = (LuiItem*)itm;
            cmd = group->getUsbCmd();
            qDebug() << cmd.toHex();
        }
    }
}

void Lui::on_actionClose_triggered()
{
    qApp->quit();
}

void Lui::on_actionGroup_triggered()
{
    this->scene->group();
}

void Lui::on_actionUngroup_triggered()
{
    this->scene->ungroup();
}

void Lui::on_actionSelectAll_triggered()
{
    this->scene->selectAll();
}

void Lui::newLedColor(QColor color)
{
    colorDisplayChange(color);
    emit colorChanged(color);
}

void Lui::testColor(QColor color)
{
    //QString cmd = USB_PREAMBLE;
    QByteArray cmd = QByteArray(USB_PREAMBLE, USB_PREAMBLE_LEN);
    cmd.append(PR_MSG_TYPE_TEST);
    cmd.append( (char)(color.red()) );
    cmd.append( (char)(color.green()) );
    cmd.append( (char)(color.blue()) );
    if(this->serial->openAndWrite(cmd)){
        this->ui->transmitPushButton->setEnabled(false);
    }
}

void Lui::on_brightness_slider_valueChanged(int position)
{
    QColor c = this->ui->color_display->color();
    c.setHsv(c.hue(), c.saturation(), position);
    c = c.toHsv();
    if(c != this->ui->color_display->color()){
        qDebug() << "slider  color" << c;
        colorDisplayChange(c);
        emit colorChanged(c);
    }
}

void Lui::on_testButton_toggled(bool checked)
{
    this->ui->ledView->setEnabled(!checked);
    colorDisplayEnable(checked);
    if(checked){
        disconnect(this, SIGNAL(colorChanged(QColor)), this->scene, SLOT(updateColor(QColor)));
        connect(this, SIGNAL(colorChanged(QColor)), this, SLOT(testColor(QColor)));
    } else {
        connect(this, SIGNAL(colorChanged(QColor)), this->scene, SLOT(updateColor(QColor)));
        disconnect(this, SIGNAL(colorChanged(QColor)), this, SLOT(testColor(QColor)));
    }
}

void Lui::on_actionNew_triggered()
{
    colorDisplayEnable(false);
    LedScene *old = this->scene;
    this->scene = new LedScene(this->ui->ledView);
    this->ui->ledView->setScene(this->scene);
    connect(this->scene, SIGNAL(selectedItemStatusChanged(bool)), this, SLOT(colorDisplayEnable(bool)));
    connect(this->scene, SIGNAL(selectedItemColorChanged(QColor)), this, SLOT(colorDisplayChange(QColor)));
    connect(this, SIGNAL(colorChanged(QColor)), this->scene, SLOT(updateColor(QColor)));
    disconnect(old, SIGNAL(selectedItemStatusChanged(bool)), this, SLOT(colorDisplayEnable(bool)));
    disconnect(old, SIGNAL(selectedItemColorChanged(QColor)), this, SLOT(colorDisplayChange(QColor)));
    disconnect(this, SIGNAL(colorChanged(QColor)), old, SLOT(updateColor(QColor)));
    delete old;
}
