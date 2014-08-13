#include <QComboBox>
#include <QObject>
#include <QtDebug>

#include <global.h>
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
    this->ui->transmitPushButton->setEnabled(true);
}

void Lui::colorDisplayEnable(bool status)
{
    this->ui->color_display_group->setEnabled(status);
}

void Lui::on_transmitPushButton_clicked()
{
    QString cmd = "Led"; //TODO: build command to send...
    if(this->serial->openAndWrite(cmd.toLocal8Bit())){
        this->ui->transmitPushButton->setEnabled(false);
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
