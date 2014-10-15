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
    led_count(GLOBAL_LED_COUNT),
    file_name("")
{
    this->ui->setupUi(this);
    colorDisplayEnable(false);
    this->serial = new Serial(this);
    QObject::connect(this->ui->refreshPushButton, &QPushButton::clicked, this->serial, &Serial::refreshPortData);
    QObject::connect(this->serial, &Serial::updatedPortDescription, this, &Lui::updatePortComboBox);
    QObject::connect(this->ui->portComboBox, SIGNAL(currentIndexChanged(int)), this->serial, SLOT(setCurrentPort(int)));
    QObject::connect(this->serial, &Serial::currentPortChanged, this->ui->portComboBox, &QComboBox::setCurrentIndex);
    QObject::connect(this->serial, SIGNAL(bytesWritten(qint64)), this, SLOT(enableTransmitPushButton()));
    this->serial->refreshPortData();

    this->sequence_scene = new SequenceScene(this->ui->sequenceView);
    this->ui->sequenceView->setScene(this->sequence_scene);

    this->led_scene = new LedScene(this->ui->ledView);
    connect(this->led_scene, SIGNAL(newGroupAdded(LedGroupItem*)), this->sequence_scene, SLOT(newGroup(LedGroupItem*)));
    connect(this->led_scene, SIGNAL(groupRemoved(LedGroupItem*)), this->sequence_scene, SLOT(removeGroup(LedGroupItem*)));
    this->led_scene->fillDefault();
    this->ui->ledView->setScene(this->led_scene);
    connect(this->led_scene, SIGNAL(selectedGroupNameChanged(QString)), this, SLOT(updateGroupLabel(QString)));

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

    this->group_name_label = new QLabel();
    this->statusBar()->addWidget(this->group_name_label, 200);
    updateGroupLabel();;
}

Lui::~Lui()
{
    delete this->led_scene;
    delete this->sequence_scene;
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
       //qDebug() << "display color" << this->ui->color_display->color();
       this->ui->brightness_slider->setSliderPosition(color.value());
   }
}

void Lui::updateGroupLabel(QString name)
{
    if(name.isEmpty()){
        name = "<keine Gruppe>";
    }
    this->group_name_label->setText("Gruppe: '" + name + "'");
}

void Lui::on_transmitPushButton_clicked()
{
    /*QByteArray cmd = QByteArray(USB_PREAMBLE, USB_PREAMBLE_LEN); //TODO: build command to send...
    if(this->serial->openAndWrite(cmd)){
        this->ui->transmitPushButton->setEnabled(false);
    }*/
    QByteArray cmd = QByteArray();
    foreach(QGraphicsItem *itm, this->led_scene->items()){
        QGraphicsItem::GraphicsItemFlags flags = itm->flags();
        if(flags & QGraphicsItem::ItemIsSelectable){
            //LuiItem *group = (LuiItem*)itm;
            LedGroupItem *group = qgraphicsitem_cast<LedGroupItem*>(itm);
            if(group != NULL){
                cmd = group->getUsbCmd();
                qDebug() << cmd.toHex();
            }
        }
    }
}

void Lui::on_actionClose_triggered()
{
    qApp->quit();
}

void Lui::on_actionGroup_triggered()
{
    this->led_scene->group();
}

void Lui::on_actionUngroup_triggered()
{
    //this->scene->ungroup();
}

void Lui::on_actionSelectAll_triggered()
{
    this->led_scene->selectAll();
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

void Lui::newScene()
{
    colorDisplayEnable(false);
    LedScene *old = this->led_scene;
    this->led_scene = new LedScene(this->ui->ledView);
    this->ui->ledView->setScene(this->led_scene);
    connect(this->led_scene, SIGNAL(selectedItemStatusChanged(bool)), this, SLOT(colorDisplayEnable(bool)));
    connect(this->led_scene, SIGNAL(selectedItemColorChanged(QColor)), this, SLOT(colorDisplayChange(QColor)));
    connect(this->led_scene, SIGNAL(selectedGroupChanged(QString)), this, SLOT(updateGroupLabel(QString)));
    connect(this, SIGNAL(colorChanged(QColor)), this->led_scene, SLOT(updateColor(QColor)));
    connect(this->led_scene, SIGNAL(newGroupAdded(LedGroupItem*)), this->sequence_scene, SLOT(newGroup(LedGroupItem*)));
    connect(this->led_scene, SIGNAL(groupRemoved(LedGroupItem*)), this->sequence_scene, SLOT(removeGroup(LedGroupItem*)));
    disconnect(old, SIGNAL(newGroupAdded(LedGroupItem*)), this->sequence_scene, SLOT(newGroup(LedGroupItem*)));
    disconnect(old, SIGNAL(groupRemoved(LedGroupItem*)), this->sequence_scene, SLOT(removeGroup(LedGroupItem*)));
    disconnect(old, SIGNAL(selectedItemStatusChanged(bool)), this, SLOT(colorDisplayEnable(bool)));
    disconnect(old, SIGNAL(selectedItemColorChanged(QColor)), this, SLOT(colorDisplayChange(QColor)));
    disconnect(old, SIGNAL(selectedGroupChanged(QString)), this, SLOT(updateGroupLabel(QString)));
    disconnect(this, SIGNAL(colorChanged(QColor)), old, SLOT(updateColor(QColor)));
    updateGroupLabel();
    delete old;
}

void Lui::on_brightness_slider_valueChanged(int position)
{
    QColor c = this->ui->color_display->color();
    c.setHsv(c.hue(), c.saturation(), position);
    c = c.toHsv();
    if(c != this->ui->color_display->color()){
        //qDebug() << "slider  color" << c;
        colorDisplayChange(c);
        emit colorChanged(c);
    }
}

void Lui::on_testButton_toggled(bool checked)
{
    this->ui->ledView->setEnabled(!checked);
    colorDisplayEnable(checked);
    if(checked){
        disconnect(this, SIGNAL(colorChanged(QColor)), this->led_scene, SLOT(updateColor(QColor)));
        connect(this, SIGNAL(colorChanged(QColor)), this, SLOT(testColor(QColor)));
    } else {
        connect(this, SIGNAL(colorChanged(QColor)), this->led_scene, SLOT(updateColor(QColor)));
        disconnect(this, SIGNAL(colorChanged(QColor)), this, SLOT(testColor(QColor)));
    }
}

void Lui::on_actionNew_triggered()
{
    newScene();
    this->sequence_scene->clear();
    this->led_scene->fillDefault();
}

void Lui::on_actionSave_triggered()
{
    if(this->file_name.isEmpty()){
        //first get a valid filename
        Lui::on_actionSaveAs_triggered();
        return;
    }
    qDebug() << "file:" << this->file_name;
    QFile file(this->file_name);
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::critical(this, tr("Error"), tr("Datei konnte nicht geöffnet werden!"));
        return;
    }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_3);
    out << *(this->led_scene);
    file.close();

}

void Lui::on_actionSaveAs_triggered()
{
    QString new_file_name = QFileDialog::getSaveFileName(this, tr("Speichern unter..."), QString(),
                tr("Lui-Dateien (*.dat)"));

    if(!new_file_name.isEmpty()){
        this->file_name = new_file_name;
        Lui::on_actionSave_triggered();
    }


}

void Lui::on_actionOpen_triggered()
{
    newScene();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Öffnen"), QString(),
                tr("Lui-Dateien (*.dat)"));

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr("Error"), tr("Datei konnte nicht geöffnet werden!"));
            return;
        }
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_5_3);
        in >> *(this->led_scene);
        file.close();
    }

}
