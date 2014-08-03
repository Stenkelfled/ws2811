#include <QComboBox>
#include <QObject>

#include "lui.h"
#include "ui_lui.h"

Lui::Lui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Lui)
{
    this->ui->setupUi(this);
    this->serial = new Serial(this);
    QObject::connect(this->ui->refreshPushButton, &QPushButton::clicked, this->serial, &Serial::refreshPortData);
    QObject::connect(this->serial, &Serial::updatedPortDescription, this, &Lui::updatePortComboBox);
    QObject::connect(this->ui->portComboBox, SIGNAL(currentIndexChanged(int)), this->serial, SLOT(setCurrentPort(int)));
    QObject::connect(this->serial, &Serial::currentPortChanged, this->ui->portComboBox, &QComboBox::setCurrentIndex);
    QObject::connect(this->serial, SIGNAL(bytesWritten(qint64)), this, SLOT(enableTransmitPushButton()));
    this->serial->refreshPortData();
}

Lui::~Lui()
{
    delete this->ui;
    delete this->serial;
}

void Lui::enableTransmitPushButton(){
    this->ui->transmitPushButton->setEnabled(true);
}

void Lui::updatePortComboBox(QStringList &items){
    this->ui->portComboBox->clear();
    this->ui->portComboBox->addItems(items);
}

void Lui::on_transmitPushButton_clicked()
{
    QString cmd = "Led"; //TODO: build command to send...
    if(this->serial->openAndWrite(cmd.toLocal8Bit())){
        this->ui->transmitPushButton->setEnabled(false);
    }
}
